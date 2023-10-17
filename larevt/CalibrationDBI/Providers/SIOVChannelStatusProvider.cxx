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
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <fstream>

namespace lariov {
  using handle_t = SIOVChannelStatusProvider::handle_t;
  /// Converts LArSoft channel ID in the one proper for the DB
  static DBChannelID_t rawToDBChannel(raw::ChannelID_t channel) { return DBChannelID_t(channel); }

  class SIOVChannelStatusForTimestamp {
  public:
    SIOVChannelStatusForTimestamp() = default;
    SIOVChannelStatusForTimestamp(SIOVChannelStatusProvider::handle_t noisy,
                                  SIOVChannelStatusProvider::handle_t dbdata)
      : fNoisyData(noisy), fDBdata(dbdata), fUseDefault(!fNoisyData || !fDBdata)
    {}

    ChannelStatus GetChannelStatus(raw::ChannelID_t ch) const
    {
      if (fUseDefault) { return ChannelStatus{0, kGOOD}; }
      if (fNoisyData && fNoisyData->HasChannel(rawToDBChannel(ch))) {
        return fNoisyData->GetRow(rawToDBChannel(ch));
      }
      return fDBdata->GetRow(rawToDBChannel(ch));
    }

    ChannelSet_t GoodChannels() const { return GetChannelsWithStatus(kGOOD); }

    ChannelSet_t BadChannels() const
    {
      ChannelSet_t dead = GetChannelsWithStatus(kDEAD);
      ChannelSet_t ln = GetChannelsWithStatus(kLOWNOISE);
      dead.insert(ln.begin(), ln.end());
      return dead;
    }

    ChannelSet_t NoisyChannels() const { return GetChannelsWithStatus(kNOISY); }

    ChannelSet_t GetChannelsWithStatus(chStatus status) const
    {
      if (fUseDefault && status != kGOOD) { return {}; }
      ChannelSet_t retSet;
      DBChannelID_t maxChannel = art::ServiceHandle<geo::Geometry const>()->Nchannels() - 1;
      for (DBChannelID_t ch = 0; ch != maxChannel; ++ch) {
        if (fUseDefault || this->GetChannelStatus(ch).Status() == status) { retSet.insert(ch); }
      }
      return retSet;
    }

  private:
    SIOVChannelStatusProvider::handle_t fNoisyData{handle_t::invalid()};
    SIOVChannelStatusProvider::handle_t fDBdata = handle_t::invalid();
    bool fUseDefault = false;
  };
  //----------------------------------------------------------------------------
  SIOVChannelStatusProvider::SIOVChannelStatusProvider(fhicl::ParameterSet const& pset)
    : fDBFolder(pset.get<fhicl::ParameterSet>("DatabaseRetrievalAlg"))
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

    Snapshot<ChannelStatus> snapshot;
    IOVTimeStamp tmp = IOVTimeStamp::MaxTimeStamp();
    tmp.SetStamp(tmp.Stamp() - 1, tmp.SubStamp());
    snapshot.SetIoV(tmp, IOVTimeStamp::MaxTimeStamp());

    if (fDataSource == DataSource::Default) {
      std::cout << "Using default channel status value: " << kGOOD << "\n";
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
      while (std::getline(file, line)) {
        DBChannelID_t ch = std::stoi(line.substr(0, line.find(',')));
        int status = std::stoi(line.substr(line.find(',') + 1));
        ChannelStatus cs{ch, status};
        snapshot.AddOrReplaceRow(cs);
      }
      fData.emplace(0, std::move(snapshot));
    } // if source from file
    else {
      std::cout << "Using channel statuses from conditions database\n";
    }
  }

  bool SIOVChannelStatusProvider::IsPresent(DBTimeStamp_t ts, raw::ChannelID_t channel) const
  {
    auto data = GetData(ts);
    return data->GetChannelStatus(channel).IsPresent();
  }

  /// Returns whether the specified channel is bad in the current run
  bool SIOVChannelStatusProvider::IsBad(DBTimeStamp_t ts, raw::ChannelID_t channel) const
  {
    auto data = GetData(ts);
    auto status = data->GetChannelStatus(channel);
    return status.IsDead() || status.IsLowNoise() || !status.IsPresent();
  }

  /// Returns whether the specified channel is noisy in the current run
  bool SIOVChannelStatusProvider::IsNoisy(DBTimeStamp_t ts, raw::ChannelID_t channel) const
  {
    auto data = GetData(ts);
    return data->GetChannelStatus(channel).IsNoisy();
  }

  /// Returns whether the specified channel is physical and good
  bool SIOVChannelStatusProvider::IsGood(DBTimeStamp_t ts, raw::ChannelID_t channel) const
  {
    auto data = GetData(ts);
    return data->GetChannelStatus(channel).IsGood();
  }

  // This is the function that does the actual work of updating data from database.
  SIOVChannelStatusProvider::handle_t SIOVChannelStatusProvider::DBUpdate(DBTimeStamp_t ts) const
  {
    if (fDataSource != DataSource::Database) { return fData.at(0); }
    if (auto h = fData.at(ts)) { return h; }

    mf::LogInfo("SIOVChannelStatusProvider")
      << "SIOVChannelStatusProvider::DBUpdate called with new timestamp.";

    auto const dataset = fDBFolder.GetDataset(ts);

    Snapshot<ChannelStatus> data{dataset.beginTime(), dataset.endTime()};
    for (auto const channel : dataset.channels()) {
      ChannelStatus cs{channel, static_cast<int>(dataset.GetDataAsLong(channel, "status"))};
      data.AddOrReplaceRow(cs);
    }
    // SS: we will be entering a new entery into the cache
    // and return it
    fData.drop_unused();
    return fData.emplace(ts, data);
  }

  // Get noisy data
  SIOVChannelStatusProvider::handle_t SIOVChannelStatusProvider::GetNoisyData(
    DBTimeStamp_t ts) const
  {
    return fNewNoisy.at(ts);
  }

  SIOVChannelStatusForTimestampPtr SIOVChannelStatusProvider::GetData(DBTimeStamp_t ts) const
  {
    if (fDataSource == DataSource::Default)
      return std::make_shared<SIOVChannelStatusForTimestamp>();
    return std::make_shared<SIOVChannelStatusForTimestamp>(GetNoisyData(ts), DBUpdate(ts));
  }

  //----------------------------------------------------------------------------
  ChannelSet_t SIOVChannelStatusProvider::GoodChannels(DBTimeStamp_t ts) const
  {
    auto data = GetData(ts);
    return data->GoodChannels();
  }

  //----------------------------------------------------------------------------
  ChannelSet_t SIOVChannelStatusProvider::BadChannels(DBTimeStamp_t ts) const
  {
    auto data = GetData(ts);
    return data->BadChannels();
  }

  //----------------------------------------------------------------------------
  ChannelSet_t SIOVChannelStatusProvider::NoisyChannels(DBTimeStamp_t ts) const
  {
    auto data = GetData(ts);
    return data->NoisyChannels();
  }

  //----------------------------------------------------------------------------
  void SIOVChannelStatusProvider::AddNoisyChannels(DBTimeStamp_t ts,
                                                   std::vector<raw::ChannelID_t> channels)
  {
    Snapshot<ChannelStatus> snapshot;
    for (auto const ch : channels) {
      ChannelStatus cs{rawToDBChannel(ch), kNOISY};
      snapshot.AddOrReplaceRow(cs);
    }
    fNewNoisy.drop_unused();
    fNewNoisy.emplace(ts, std::move(snapshot));
  }

  //----------------------------------------------------------------------------

  chStatus SIOVChannelStatusProvider::Status(DBTimeStamp_t ts, raw::ChannelID_t ch) const
  {
    auto data = GetData(ts);
    return data->GetChannelStatus(ch).Status();
  }
} // namespace lariov
