#include "SIOVElectronicsCalibProvider.h"

// art/LArSoft libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib_except/exception.h"
#include "larcore/Geometry/Geometry.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <fstream>

namespace lariov {

  //constructor
  SIOVElectronicsCalibProvider::SIOVElectronicsCalibProvider(fhicl::ParameterSet const& p)
    : DatabaseRetrievalAlg(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"))
    , fEventTimeStamp(0)
    , fCurrentTimeStamp(0)
  {

    this->Reconfigure(p);
  }

  void SIOVElectronicsCalibProvider::Reconfigure(fhicl::ParameterSet const& p)
  {

    this->DatabaseRetrievalAlg::Reconfigure(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"));
    fData.Clear();
    IOVTimeStamp tmp = IOVTimeStamp::MaxTimeStamp();
    tmp.SetStamp(tmp.Stamp() - 1, tmp.SubStamp());
    fData.SetIoV(tmp, IOVTimeStamp::MaxTimeStamp());

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

    if (fDataSource == DataSource::Default) {
      float default_gain = p.get<float>("DefaultGain");
      float default_gain_err = p.get<float>("DefaultGainErr");
      float default_st = p.get<float>("DefaultShapingTime");
      float default_st_err = p.get<float>("DefaultShapingTimeErr");

      ElectronicsCalib defaultCalib(0);

      defaultCalib.SetGain(default_gain);
      defaultCalib.SetGainErr(default_gain_err);
      defaultCalib.SetShapingTime(default_st);
      defaultCalib.SetShapingTimeErr(default_st_err);
      defaultCalib.SetExtraInfo(CalibrationExtraInfo("ElectronicsCalib"));

      art::ServiceHandle<geo::Geometry const> geo;
      geo::wire_id_iterator itW = geo->begin_wire_id();
      for (; itW != geo->end_wire_id(); ++itW) {
        DBChannelID_t ch = geo->PlaneWireToChannel(*itW);
        defaultCalib.SetChannel(ch);
        fData.AddOrReplaceRow(defaultCalib);
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
      ElectronicsCalib dp(0);
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

        CalibrationExtraInfo info("ElectronicsCalib");

        dp.SetChannel(ch);
        dp.SetGain(gain);
        dp.SetGainErr(gain_err);
        dp.SetShapingTime(shaping_time);
        dp.SetShapingTimeErr(shaping_time_err);
        dp.SetExtraInfo(info);

        fData.AddOrReplaceRow(dp);
      }
    }
    else {
      std::cout << "Using electronics calibrations from conditions database" << std::endl;
    }
  }

  // This method saves the time stamp of the latest event.

  void SIOVElectronicsCalibProvider::UpdateTimeStamp(DBTimeStamp_t ts)
  {
    mf::LogInfo("SIOVElectronicsCalibProvider")
      << "SIOVElectronicsCalibProvider::UpdateTimeStamp called.";
    fEventTimeStamp = ts;
  }

  // Maybe update method cached data (public non-const version).

  bool SIOVElectronicsCalibProvider::Update(DBTimeStamp_t ts)
  {

    fEventTimeStamp = ts;
    return DBUpdate(ts);
  }

  // Maybe update method cached data (private const version using current event time).

  bool SIOVElectronicsCalibProvider::DBUpdate() const { return DBUpdate(fEventTimeStamp); }

  // Maybe update method cached data (private const version).
  // This is the function that does the actual work of updating data from database.

  bool SIOVElectronicsCalibProvider::DBUpdate(DBTimeStamp_t ts) const
  {

    bool result = false;
    if (fDataSource == DataSource::Database && ts != fCurrentTimeStamp) {

      mf::LogInfo("SIOVElectronicsCalibProvider")
        << "SIOVElectronicsCalibProvider::DBUpdate called with new timestamp.";

      fCurrentTimeStamp = ts;

      // Call non-const base class method.

      result = const_cast<SIOVElectronicsCalibProvider*>(this)->UpdateFolder(ts);
      if (result) {
        //DBFolder was updated, so now update the Snapshot
        fData.Clear();
        fData.SetIoV(this->Begin(), this->End());

        std::vector<DBChannelID_t> channels;
        fFolder->GetChannelList(channels);
        for (auto it = channels.begin(); it != channels.end(); ++it) {

          double gain, gain_err, shaping_time, shaping_time_err;
          fFolder->GetNamedChannelData(*it, "gain", gain);
          fFolder->GetNamedChannelData(*it, "gain_err", gain_err);
          fFolder->GetNamedChannelData(*it, "shaping_time", shaping_time);
          fFolder->GetNamedChannelData(*it, "shaping_time_err", shaping_time_err);

          ElectronicsCalib pg(*it);
          pg.SetGain((float)gain);
          pg.SetGainErr((float)gain_err);
          pg.SetShapingTime((float)shaping_time);
          pg.SetShapingTimeErr((float)shaping_time_err);
          pg.SetExtraInfo(CalibrationExtraInfo("ElectronicsCalib"));

          fData.AddOrReplaceRow(pg);
        }
      }
    }

    return result;
  }

  const ElectronicsCalib& SIOVElectronicsCalibProvider::ElectronicsCalibObject(
    DBChannelID_t ch) const
  {
    DBUpdate();
    return fData.GetRow(ch);
  }

  float SIOVElectronicsCalibProvider::Gain(DBChannelID_t ch) const
  {
    return this->ElectronicsCalibObject(ch).Gain();
  }

  float SIOVElectronicsCalibProvider::GainErr(DBChannelID_t ch) const
  {
    return this->ElectronicsCalibObject(ch).GainErr();
  }

  float SIOVElectronicsCalibProvider::ShapingTime(DBChannelID_t ch) const
  {
    return this->ElectronicsCalibObject(ch).ShapingTime();
  }

  float SIOVElectronicsCalibProvider::ShapingTimeErr(DBChannelID_t ch) const
  {
    return this->ElectronicsCalibObject(ch).ShapingTimeErr();
  }

  CalibrationExtraInfo const& SIOVElectronicsCalibProvider::ExtraInfo(DBChannelID_t ch) const
  {
    return this->ElectronicsCalibObject(ch).ExtraInfo();
  }

} //end namespace lariov
