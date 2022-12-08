#include "SIOVPmtGainProvider.h"

// art/LArSoft libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib_except/exception.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/WireReadout.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <fstream>

namespace lariov {

  //constructor
  SIOVPmtGainProvider::SIOVPmtGainProvider(fhicl::ParameterSet const& p)
    : DatabaseRetrievalAlg(p.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"))
    , fEventTimeStamp(0)
    , fCurrentTimeStamp(0)
  {
    this->Reconfigure(p);
  }

  void SIOVPmtGainProvider::Reconfigure(fhicl::ParameterSet const& p)
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

      PmtGain defaultGain(0);

      defaultGain.SetGain(default_gain);
      defaultGain.SetGainErr(default_gain_err);
      defaultGain.SetExtraInfo(CalibrationExtraInfo("PmtGain"));

      art::ServiceHandle<geo::Geometry const> geo;
      auto const& wireReadoutGeom = art::ServiceHandle<geo::WireReadout const>()->Get();
      for (unsigned int od = 0; od != geo->NOpDets(); ++od) {
        if (wireReadoutGeom.IsValidOpChannel(od)) {
          defaultGain.SetChannel(od);
          fData.AddOrReplaceRow(defaultGain);
        }
      }
    }
    else if (fDataSource == DataSource::File) {
      cet::search_path sp("FW_SEARCH_PATH");
      std::string abs_fp = sp.find_file(fileName);
      std::cout << "Using pmt gains from local file: " << abs_fp << "\n";
      std::ifstream file(abs_fp);
      if (!file) {
        throw cet::exception("SIOVPmtGainProvider") << "File " << abs_fp << " is not found.";
      }

      std::string line;
      PmtGain dp(0);
      while (std::getline(file, line)) {
        if (line[0] == '#') continue;
        size_t current_comma = line.find(',');
        DBChannelID_t ch = (DBChannelID_t)std::stoi(line.substr(0, current_comma));
        float gain = std::stof(
          line.substr(current_comma + 1, line.find(',', current_comma + 1) - (current_comma + 1)));

        current_comma = line.find(',', current_comma + 1);
        float gain_err = std::stof(line.substr(current_comma + 1));

        CalibrationExtraInfo info("PmtGain");

        dp.SetChannel(ch);
        dp.SetGain(gain);
        dp.SetGainErr(gain_err);
        dp.SetExtraInfo(info);

        fData.AddOrReplaceRow(dp);
      }
    }
    else {
      std::cout << "Using pmt gains from conditions database" << std::endl;
    }
  }

  // This method saves the time stamp of the latest event.

  void SIOVPmtGainProvider::UpdateTimeStamp(DBTimeStamp_t ts)
  {
    mf::LogInfo("SIOVPmtGainProvider") << "SIOVPmtGainProvider::UpdateTimeStamp called.";
    fEventTimeStamp = ts;
  }

  // Maybe update method cached data (public non-const version).

  bool SIOVPmtGainProvider::Update(DBTimeStamp_t ts)
  {

    fEventTimeStamp = ts;
    return DBUpdate(ts);
  }

  // Maybe update method cached data (private const version using current event time).

  bool SIOVPmtGainProvider::DBUpdate() const { return DBUpdate(fEventTimeStamp); }

  // Maybe update method cached data (private const version).
  // This is the function that does the actual work of updating data from database.

  bool SIOVPmtGainProvider::DBUpdate(DBTimeStamp_t ts) const
  {

    bool result = false;
    if (fDataSource == DataSource::Database && ts != fCurrentTimeStamp) {

      mf::LogInfo("SIOVPmtGainProvider")
        << "SIOVPmtGainProvider::DBUpdate called with new timestamp.";

      fCurrentTimeStamp = ts;

      // Call non-const base class method.

      result = const_cast<SIOVPmtGainProvider*>(this)->UpdateFolder(ts);
      if (result) {
        //DBFolder was updated, so now update the Snapshot
        fData.Clear();
        fData.SetIoV(this->Begin(), this->End());

        std::vector<DBChannelID_t> channels;
        fFolder->GetChannelList(channels);
        for (auto it = channels.begin(); it != channels.end(); ++it) {

          double gain, gain_err;
          fFolder->GetNamedChannelData(*it, "gain", gain);
          fFolder->GetNamedChannelData(*it, "gain_sigma", gain_err);

          PmtGain pg(*it);
          pg.SetGain((float)gain);
          pg.SetGainErr((float)gain_err);
          pg.SetExtraInfo(CalibrationExtraInfo("PmtGain"));

          fData.AddOrReplaceRow(pg);
        }
      }
    }

    return result;
  }

  const PmtGain& SIOVPmtGainProvider::PmtGainObject(DBChannelID_t ch) const
  {
    DBUpdate();
    return fData.GetRow(ch);
  }

  float SIOVPmtGainProvider::Gain(DBChannelID_t ch) const { return this->PmtGainObject(ch).Gain(); }

  float SIOVPmtGainProvider::GainErr(DBChannelID_t ch) const
  {
    return this->PmtGainObject(ch).GainErr();
  }

  CalibrationExtraInfo const& SIOVPmtGainProvider::ExtraInfo(DBChannelID_t ch) const
  {
    return this->PmtGainObject(ch).ExtraInfo();
  }

} //end namespace lariov
