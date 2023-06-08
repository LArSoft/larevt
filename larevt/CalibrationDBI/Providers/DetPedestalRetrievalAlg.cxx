#include "DetPedestalRetrievalAlg.h"
#include "larevt/CalibrationDBI/IOVData/IOVDataConstants.h"

// art/LArSoft libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h" // for Paramete...
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/GeometryCore.h"             // for wire_id_...
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h" // for kCollection
#include "larevt/CalibrationDBI/IOVData/IOVDataError.h"   // for IOVDataE...
#include "larevt/CalibrationDBI/IOVData/IOVTimeStamp.h"   // for IOVTimeS...
#include "messagefacility/MessageLogger/MessageLogger.h"

//C/C++
#include <fstream>

namespace lariov {

  //constructors
  DetPedestalRetrievalAlg::DetPedestalRetrievalAlg(const std::string& foldername,
                                                   const std::string& url,
                                                   const std::string& tag /*=""*/)
    : fDBFolder{foldername, url, tag}, fDataSource(DataSource::Database)
  {}

  DetPedestalRetrievalAlg::DetPedestalRetrievalAlg(fhicl::ParameterSet const& p)
    : fDBFolder{p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg")}
  {

    bool UseDB = p.get<bool>("UseDB", false);
    bool UseFile = p.get<bool>("UseFile", false);
    std::string fileName = p.get<std::string>("FileName", "");

    //priority:  (1) use db, (2) use table, (3) use defaults
    //If none are specified, use defaults
    if (UseDB)
      fDataSource = DataSource::Database;
    else if (UseFile)
      fDataSource = DataSource::File;
    else
      fDataSource = DataSource::Default;

    if (fDataSource == DataSource::Database) {
      std::cout << "Using pedestals from conditions database\n";
      return;
    }
    Snapshot<DetPedestal> snapshot;
    IOVTimeStamp tmp = IOVTimeStamp::MaxTimeStamp();
    tmp.SetStamp(tmp.Stamp() - 1, tmp.SubStamp());
    snapshot.SetIoV(tmp, IOVTimeStamp::MaxTimeStamp());
    if (fDataSource == DataSource::Default) {
      std::cout << "Using default pedestal values\n";
      auto const default_collmean = p.get<float>("DefaultCollMean", 400.0);
      auto const default_collrms = p.get<float>("DefaultCollRms", 0.3);
      auto const default_mean_err = p.get<float>("DefaultMeanErr", 0.0);
      auto const default_rms_err = p.get<float>("DefaultRmsErr", 0.0);
      auto const default_indmean = p.get<float>("DefaultIndMean", 2048.0);
      auto const default_indrms = p.get<float>("DefaultIndRms", 0.3);

      art::ServiceHandle<geo::Geometry const> geo;
      for (auto const& id : geo->Iterate<geo::WireID>()) {
        DBChannelID_t ch = geo->PlaneWireToChannel(id);
        if (geo->SignalType(ch) == geo::kCollection) {
          DetPedestal DefaultColl(
            ch, default_collmean, default_mean_err, default_collrms, default_rms_err);
          snapshot.AddOrReplaceRow(DefaultColl);
        }
        else if (geo->SignalType(ch) == geo::kInduction) {
          DetPedestal DefaultInd(
            ch, default_indmean, default_mean_err, default_indrms, default_rms_err);
          snapshot.AddOrReplaceRow(DefaultInd);
        }
        else
          throw IOVDataError("Wire type is not collection or induction!");
      }
    }
    else if (fDataSource == DataSource::File) {
      cet::search_path sp("FW_SEARCH_PATH");
      std::string abs_fp = sp.find_file(fileName);
      std::cout << "Using pedestals from local file: " << abs_fp << "\n";
      std::ifstream file(abs_fp);
      if (!file) {
        throw cet::exception("DetPedestalRetrievalAlg") << "File " << abs_fp << " is not found.";
      }

      std::string line;
      while (std::getline(file, line)) {
        size_t current_comma = line.find(',');
        DBChannelID_t ch = (DBChannelID_t)std::stoi(line.substr(0, current_comma));
        float ped = std::stof(
          line.substr(current_comma + 1, line.find(',', current_comma + 1) - (current_comma + 1)));

        current_comma = line.find(',', current_comma + 1);
        float rms = std::stof(
          line.substr(current_comma + 1, line.find(',', current_comma + 1) - (current_comma + 1)));

        current_comma = line.find(',', current_comma + 1);
        float ped_err = std::stof(
          line.substr(current_comma + 1, line.find(',', current_comma + 1) - (current_comma + 1)));

        current_comma = line.find(',', current_comma + 1);
        float rms_err = std::stof(line.substr(current_comma + 1));

        DetPedestal dp(ch, ped, rms, ped_err, rms_err);
        snapshot.AddOrReplaceRow(dp);
      }
    } // if source from file
    fData.emplace(0, std::move(snapshot));
  }

  // Maybe update method cached data (private const version).
  // This is the function that does the actual work of updating data from database.

  DetPedestalRetrievalAlg::handle_t DetPedestalRetrievalAlg::DBUpdate(DBTimeStamp_t ts) const
  {
    if (fDataSource != DataSource::Database) { return fData.at(0); }
    if (auto h = fData.at(ts)) { return h; }

    mf::LogInfo("DetPedestalRetrievalAlg")
      << "DetPedestalRetrievalAlg::DBUpdate called with new timestamp.";

    auto const dataset = fDBFolder.GetDataset(ts);

    Snapshot<DetPedestal> data{dataset.beginTime(), dataset.endTime()};
    for (auto const channel : dataset.channels()) {
      DetPedestal pd{channel,
                     dataset.GetDataAsFloat(channel, "mean"),
                     dataset.GetDataAsFloat(channel, "rms"),
                     dataset.GetDataAsFloat(channel, "mean_err"),
                     dataset.GetDataAsFloat(channel, "rms_err")};
      data.AddOrReplaceRow(pd);
    }
    //MT note: there may be  a better place for this cleanup call
    fData.drop_unused();
    return fData.emplace(ts, data);
  }

  float DetPedestalRetrievalAlg::PedMean(DBTimeStamp_t ts, DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).PedMean();
  }

  float DetPedestalRetrievalAlg::PedRms(DBTimeStamp_t ts, DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).PedRms();
  }

  float DetPedestalRetrievalAlg::PedMeanErr(DBTimeStamp_t ts, DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).PedMeanErr();
  }

  float DetPedestalRetrievalAlg::PedRmsErr(DBTimeStamp_t ts, DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).PedRmsErr();
  }

} //end namespace lariov
