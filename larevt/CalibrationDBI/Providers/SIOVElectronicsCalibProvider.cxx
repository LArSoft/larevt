#include "SIOVElectronicsCalibProvider.h"

// art/LArSoft libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib_except/exception.h"
#include "larcore/Geometry/Geometry.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <fstream>

namespace lariov {

  //constructor
  //MT note: Could use a better name instead of "DatabaseRetrievalAlg",
  //but dont want to break existing configurations
  SIOVElectronicsCalibProvider::SIOVElectronicsCalibProvider(fhicl::ParameterSet const& p)
    : fDBFolder(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"))
  {

    Snapshot<ElectronicsCalib> snapshot;
    IOVTimeStamp tmp = IOVTimeStamp::MaxTimeStamp();
    tmp.SetStamp(tmp.Stamp() - 1, tmp.SubStamp());
    snapshot.SetIoV(tmp, IOVTimeStamp::MaxTimeStamp());

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
      std::cout << "Using electronics calibrations from conditions database" << std::endl;
      return;
    }

    if (fDataSource == DataSource::Default) {
      auto const default_gain = p.get<float>("DefaultGain");
      auto const default_gain_err = p.get<float>("DefaultGainErr");
      auto const default_st = p.get<float>("DefaultShapingTime");
      auto const default_st_err = p.get<float>("DefaultShapingTimeErr");

      art::ServiceHandle<geo::Geometry const> geo; // FIXME: Should not use services in provider.
      for (auto const& id : geo->Iterate<geo::WireID>()) {
        ElectronicsCalib defaultCalib{
          geo->PlaneWireToChannel(id), default_gain, default_gain_err, default_st, default_st_err};
        snapshot.AddOrReplaceRow(defaultCalib);
      }
    }
    else if (fDataSource == DataSource::File) {
      cet::search_path sp("FW_SEARCH_PATH");
      std::string abs_fp = sp.find_file(fileName);
      std::cout << "Using electronics calibrations from local file: " << abs_fp << "\n";
      std::ifstream file(abs_fp);
      if (!file) {
        throw cet::exception("SIOVElectronicsCalibProvider")
          << "File " << abs_fp << " is not found.";
      }

      std::string line;
      while (std::getline(file, line)) {
        size_t current_comma = line.find(',');
        DBChannelID_t ch = (DBChannelID_t)std::stoi(line.substr(0, current_comma));
        float gain = std::stof(
          line.substr(current_comma + 1, line.find(',', current_comma + 1) - (current_comma + 1)));

        current_comma = line.find(',', current_comma + 1);
        float gain_err = std::stof(
          line.substr(current_comma + 1, line.find(',', current_comma + 1) - (current_comma + 1)));

        current_comma = line.find(',', current_comma + 1);
        float shaping_time = std::stof(
          line.substr(current_comma + 1, line.find(',', current_comma + 1) - (current_comma + 1)));

        current_comma = line.find(',', current_comma + 1);
        float shaping_time_err = std::stof(line.substr(current_comma + 1));

        ElectronicsCalib dp{ch, gain, gain_err, shaping_time, shaping_time_err};
        snapshot.AddOrReplaceRow(dp);
      }
    }
    fData.emplace(0, std::move(snapshot));
  }

  // Maybe update method cached data (private const version).
  // This is the function that does the actual work of updating data from database.

  SIOVElectronicsCalibProvider::handle_t SIOVElectronicsCalibProvider::DBUpdate(
    DBTimeStamp_t ts) const
  {
    if (fDataSource != DataSource::Database) { return fData.at(0); }
    if (auto h = fData.at(ts)) { return h; }

    mf::LogInfo("SIOVElectronicsCalibProvider")
      << "SIOVElectronicsCalibProvider::DBUpdate called with new timestamp.";

    auto const dataset = fDBFolder.GetDataset(ts);

    Snapshot<ElectronicsCalib> data{dataset.beginTime(), dataset.endTime()};
    for (auto const channel : dataset.channels()) {
      ElectronicsCalib pg{channel,
                          dataset.GetDataAsFloat(channel, "gain"),
                          dataset.GetDataAsFloat(channel, "gain_err"),
                          dataset.GetDataAsFloat(channel, "shaping_time"),
                          dataset.GetDataAsFloat(channel, "shaping_time_err")};
      data.AddOrReplaceRow(pg);
    }
    //MT note: there may be  a better place for this cleanup call
    fData.drop_unused();
    return fData.emplace(ts, data);
  }

  float SIOVElectronicsCalibProvider::Gain(DBTimeStamp_t ts, DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).Gain();
  }

  float SIOVElectronicsCalibProvider::GainErr(DBTimeStamp_t ts, DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).GainErr();
  }

  float SIOVElectronicsCalibProvider::ShapingTime(DBTimeStamp_t ts, DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).ShapingTime();
  }

  float SIOVElectronicsCalibProvider::ShapingTimeErr(DBTimeStamp_t ts, DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).ShapingTimeErr();
  }

  CalibrationExtraInfo const& SIOVElectronicsCalibProvider::ExtraInfo(DBTimeStamp_t ts,
                                                                      DBChannelID_t ch) const
  {
    auto dbHandle = DBUpdate(ts);
    return dbHandle->GetRow(ch).ExtraInfo();
  }

} //end namespace lariov
