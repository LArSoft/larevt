/**
 * @file   SIOVChannelStatusProvider.h
 * @brief  Channel quality provider with information from configuration file
 * @author Brandon Eberly (eberly@fnal.gov)
 * @date   August 24, 2015
 * @see    ChannelStatusService.h SIOVChannelStatusProvider.cpp
 */

#ifndef SIOVCHANNELSTATUSPROVIDER_H
#define SIOVCHANNELSTATUSPROVIDER_H 1

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larevt/CalibrationDBI/IOVData/ChannelStatus.h"
#include "larevt/CalibrationDBI/IOVData/IOVDataConstants.h"
#include "larevt/CalibrationDBI/IOVData/Snapshot.h"
#include "larevt/CalibrationDBI/Interface/CalibrationDBIFwd.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"
#include "larevt/CalibrationDBI/Providers/DBFolder.h"

#include "hep_concurrency/cache.h"

// Utility libraries
namespace fhicl {
  class ParameterSet;
}

/// Filters for channels, events, etc
namespace lariov {
  class SIOVChannelStatusForTimestamp;
  using SIOVChannelStatusForTimestampPtr = std::shared_ptr<SIOVChannelStatusForTimestamp>;

  /** **************************************************************************
   * @brief Class providing information about the quality of channels
   *
   * This class serves information read from a FHiCL configuration file and/or a database.
   *
   * LArSoft interface to this class is through the service
   * SIOVChannelStatusService.
   */
  class SIOVChannelStatusProvider : public ChannelStatusProvider {

  public:
    /// Constructor
    SIOVChannelStatusProvider(fhicl::ParameterSet const& pset);
    //
    // interface methods
    //

    /// @name Single channel queries
    /// @{
    /// Returns whether the specified channel is physical and connected to wire
    bool IsPresent(DBTimeStamp_t ts, raw::ChannelID_t channel) const override;

    /// Returns whether the specified channel is bad in the current run
    bool IsBad(DBTimeStamp_t ts, raw::ChannelID_t channel) const override;

    /// Returns whether the specified channel is noisy in the current run
    bool IsNoisy(DBTimeStamp_t ts, raw::ChannelID_t channel) const override;

    /// Returns whether the specified channel is physical and good
    bool IsGood(DBTimeStamp_t ts, raw::ChannelID_t channel) const override;
    /// @}

    /// @name Global channel queries
    /// @{
    /// Returns a copy of set of good channel IDs for the current run
    ChannelSet_t GoodChannels(DBTimeStamp_t ts) const override;

    /// Returns a copy of set of bad channel IDs for the current run
    ChannelSet_t BadChannels(DBTimeStamp_t ts) const override;

    /// Returns a copy of set of noisy channel IDs for the current run
    ChannelSet_t NoisyChannels(DBTimeStamp_t ts) const override;
    /// @}

    /// Allows a service to add to the list of noisy channels
    void AddNoisyChannels(DBTimeStamp_t ts, std::vector<raw::ChannelID_t> ch);
    chStatus Status(DBTimeStamp_t ts, raw::ChannelID_t ch) const;
    ///@}
    using cache_t = hep::concurrency::cache<DBTimeStamp_t, Snapshot<ChannelStatus>>;
    using handle_t = cache_t::handle;

  private:
    /// Do actual database updates.

    handle_t DBUpdate(DBTimeStamp_t ts) const;
    handle_t GetNoisyData(DBTimeStamp_t ts) const;
    SIOVChannelStatusForTimestampPtr GetData(DBTimeStamp_t ts) const;

    DBFolder fDBFolder;
    DataSource::ds fDataSource;
    mutable cache_t fData; //using concurrent caching
    mutable cache_t fNewNoisy;

  }; // class SIOVChannelStatusProvider

} // namespace lariov

#endif // SIOVCHANNELSTATUSPROVIDER_H
