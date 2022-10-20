/**
 * @file   SIOVChannelStatusProvider.cxx
 * @brief  Channel quality provider with information from configuration file
 * @author Brandon Eberly (eberly@fnal.gov)
 * @date   August 24, 2015
 * @see    SIOVChannelStatusProvider.h
 */

// Our header
#include "SIOVChannelStatusProvider.h"

// LArSoft libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"
#include "larcore/Geometry/Geometry.h"
#include "larevt/CalibrationDBI/IOVData/IOVDataConstants.h"
#include "larevt/CalibrationDBI/Providers/DBFolder.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <fstream>

namespace lariov {

  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::SIOVChannelStatusProvider(fhicl::ParameterSet const& pset)
    : DatabaseRetrievalAlg(pset.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"))
    , fEventTimeStamp(0)
    , fCurrentTimeStamp(0)
    , fDefault(0)
  {

    bool UseDB = pset.get<bool>("UseDB", false);
    bool UseFile = pset.get<bool>("UseFile", false);
    std::string fileName = pset.get<std::string>("FileName", "");

    //priority:  (1) use db, (2) use table, (3) use defaults
    //If none are specified, use defaults
    if (UseDB)
      fDataSource = DataSource::Database;
    else if (UseFile)
      fDataSource = DataSource::File;
    else
      fDataSource = DataSource::Default;

    if (fDataSource == DataSource::Default) {
      std::cout << "Using default channel status value: " << kGOOD << "\n";
      fDefault.SetStatus(kGOOD);
    }
    else if (fDataSource == DataSource::File) {
      cet::search_path sp("FW_SEARCH_PATH");
      std::string abs_fp = sp.find_file(fileName);
      std::cout << "Using channel statuses from local file: " << abs_fp << "\n";
      std::ifstream file(abs_fp);
      if (!file) {
        throw cet::exception("SIOVChannelStatusProvider") << "File " << abs_fp << " is not found.";
      }

      std::string line;
      ChannelStatus cs(0);
      while (std::getline(file, line)) {
        DBChannelID_t ch = (DBChannelID_t)std::stoi(line.substr(0, line.find(',')));
        int status = std::stoi(line.substr(line.find(',') + 1));

        cs.SetChannel(ch);
        cs.SetStatus(ChannelStatus::GetStatusFromInt(status));
        fData.AddOrReplaceRow(cs);
      }
    } // if source from file
    else {
      std::cout << "Using channel statuses from conditions database\n";
    }
  }

  // This method saves the time stamp of the latest event.

  void SIOVChannelStatusProvider::UpdateTimeStamp(DBTimeStamp_t ts)
  {
    mf::LogInfo("SIOVChannelStatusProvider")
      << "SIOVChannelStatusProvider::UpdateTimeStamp called.";
    fNewNoisy.Clear();
    fEventTimeStamp = ts;
  }

  // Maybe update method cached data (public non-const version).

  bool SIOVChannelStatusProvider::Update(DBTimeStamp_t ts)
  {

    fEventTimeStamp = ts;
    fNewNoisy.Clear();
    return DBUpdate(ts);
  }

  // Maybe update method cached data (private const version using current event time).

  bool SIOVChannelStatusProvider::DBUpdate() const { return DBUpdate(fEventTimeStamp); }

  // Maybe update method cached data (private const version).
  // This is the function that does the actual work of updating data from database.

  bool SIOVChannelStatusProvider::DBUpdate(DBTimeStamp_t ts) const
  {

    bool result = false;
    if (fDataSource == DataSource::Database && ts != fCurrentTimeStamp) {

      mf::LogInfo("SIOVChannelStatusProvider")
        << "SIOVChannelStatusProvider::DBUpdate called with new timestamp.";

      fCurrentTimeStamp = ts;

      // Call non-const base class method.

      result = const_cast<SIOVChannelStatusProvider*>(this)->UpdateFolder(ts);
      if (result) {
        //DBFolder was updated, so now update the Snapshot
        fData.Clear();
        fData.SetIoV(this->Begin(), this->End());

        std::vector<DBChannelID_t> channels;
        fFolder->GetChannelList(channels);
        for (auto it = channels.begin(); it != channels.end(); ++it) {

          long status;
          fFolder->GetNamedChannelData(*it, "status", status);

          ChannelStatus cs(*it);
          cs.SetStatus(ChannelStatus::GetStatusFromInt((int)status));

          fData.AddOrReplaceRow(cs);
        }
      }
    }
    return result;
  }

  //----------------------------------------------------------------------------
  const ChannelStatus& SIOVChannelStatusProvider::GetChannelStatus(raw::ChannelID_t ch) const
  {
    if (fDataSource == DataSource::Default) { return fDefault; }
    DBUpdate();
    if (fNewNoisy.HasChannel(rawToDBChannel(ch))) { return fNewNoisy.GetRow(rawToDBChannel(ch)); }
    else {
      return fData.GetRow(rawToDBChannel(ch));
    }
  }

  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::ChannelSet_t SIOVChannelStatusProvider::GetChannelsWithStatus(
    chStatus status) const
  {

    ChannelSet_t retSet;
    retSet.clear();
    DBChannelID_t maxChannel = art::ServiceHandle<geo::Geometry const>()->Nchannels() - 1;
    if (fDataSource == DataSource::Default) {
      if (fDefault.Status() == status) {
        std::vector<DBChannelID_t> chs;
        for (DBChannelID_t ch = 0; ch != maxChannel; ++ch) {
          chs.push_back(ch);
        }
        retSet.insert(chs.begin(), chs.end());
      }
    }
    else {
      std::vector<DBChannelID_t> chs;
      for (DBChannelID_t ch = 0; ch != maxChannel; ++ch) {
        if (this->GetChannelStatus(ch).Status() == status) chs.push_back(ch);
      }

      retSet.insert(chs.begin(), chs.end());
    }
    return retSet;
  }

  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::ChannelSet_t SIOVChannelStatusProvider::GoodChannels() const
  {
    return GetChannelsWithStatus(kGOOD);
  }

  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::ChannelSet_t SIOVChannelStatusProvider::BadChannels() const
  {
    ChannelSet_t dead = GetChannelsWithStatus(kDEAD);
    ChannelSet_t ln = GetChannelsWithStatus(kLOWNOISE);
    dead.insert(ln.begin(), ln.end());
    return dead;
  }

  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::ChannelSet_t SIOVChannelStatusProvider::NoisyChannels() const
  {
    return GetChannelsWithStatus(kNOISY);
  }

  //----------------------------------------------------------------------------
  void SIOVChannelStatusProvider::AddNoisyChannel(raw::ChannelID_t ch)
  {

    // for c2: ISO C++17 does not allow 'register' storage class specifier
    //register DBChannelID_t const dbch = rawToDBChannel(ch);
    DBChannelID_t const dbch = rawToDBChannel(ch);
    if (!this->IsBad(dbch) && this->IsPresent(dbch)) {
      ChannelStatus cs(dbch);
      cs.SetStatus(kNOISY);
      fNewNoisy.AddOrReplaceRow(cs);
    }
  }

  //----------------------------------------------------------------------------

} // namespace lariov
