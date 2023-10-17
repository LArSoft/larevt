/**
 * @file   ChannelStatusProvider.h
 * @brief  Interface for experiment-specific channel quality info provider
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    ChannelStatusService.h
 *
 * This is the interface of ChannelStatus service provider core object.
 * A ChannelStatus service provider object (with the interface of ChannelStatus
 * service provider core object) provides the actual information about channels.
 * It can be instanciated by a art service (an implementation of
 * ChannelStatusService) or from whatever system needs it.
 */

#ifndef CHANNELSTATUSPROVIDER_H
#define CHANNELSTATUSPROVIDER_H 1

// C/C++ standard libraries
#include <set>

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "larevt/CalibrationDBI/IOVData/ChannelStatus.h"
#include "larevt/CalibrationDBI/Interface/CalibrationDBIFwd.h"

/// Filters for channels, events, etc
namespace lariov {

  using ChannelSet_t = std::set<raw::ChannelID_t>;
  /** **************************************************************************
   * @brief Class providing information about the quality of channels
   *
   * This class provides only a simple interface.
   * Experiments need to implement and configure their own class implementing
   * this interface.
   * The simplest implementation is provided in LArSoft:
   * SimpleChannelStatus.
   *
   * Currently, the class provides interface for the following information:
   * - goodness of the channel: good or bad (dead or unusable)
   * - noisiness of the channel: good or noisy (or compromised in some way)
   * - physical channel: physically connected to a wire or not
   *
   * It also has a stub interface to inform the object of which time we are
   * interested in.
   *
   */
  class ChannelStatusProvider {

  public:
    /// Default constructor
    ChannelStatusProvider() = default;

    // do not allow for copies or moves of this class
    ChannelStatusProvider(ChannelStatusProvider const&) = delete;
    ChannelStatusProvider(ChannelStatusProvider&&) = delete;
    ChannelStatusProvider& operator=(ChannelStatusProvider const&) = delete;
    ChannelStatusProvider& operator=(ChannelStatusProvider&&) = delete;

    /// Virtual destructor; destructs nothing
    virtual ~ChannelStatusProvider() = default;

    /// Returns whether the specified channel is physical and connected to wire
    virtual bool IsPresent(DBTimeStamp_t ts, raw::ChannelID_t channel) const = 0;

    /// Returns whether the specified channel is bad in the current run
    virtual bool IsBad(DBTimeStamp_t ts, raw::ChannelID_t channel) const = 0;

    /// Returns whether the specified channel is noisy in the current run
    virtual bool IsNoisy(DBTimeStamp_t ts, raw::ChannelID_t channel) const = 0;

    /// Returns whether the specified channel is physical and good
    virtual bool IsGood(DBTimeStamp_t ts, raw::ChannelID_t channel) const
    {
      return IsPresent(ts, channel) && !IsBad(ts, channel) && !IsNoisy(ts, channel);
    }

    /// Returns a status integer with arbitrary meaning
    virtual chStatus Status(DBTimeStamp_t ts, raw::ChannelID_t channel) const { return kUNKNOWN; }

    /// Returns whether the specified status is a valid one
    virtual bool HasStatus(DBTimeStamp_t ts, raw::ChannelID_t channel) const
    {
      return IsValidStatus(Status(ts, channel));
    }

    /// Returns a copy of set of good channel IDs for the current run
    virtual ChannelSet_t GoodChannels(DBTimeStamp_t ts) const = 0;

    /// Returns a copy of set of bad channel IDs for the current run
    virtual ChannelSet_t BadChannels(DBTimeStamp_t ts) const = 0;

    /// Returns a copy of set of noisy channel IDs for the current run
    virtual ChannelSet_t NoisyChannels(DBTimeStamp_t ts) const = 0;

    /// Returns whether the specified status is a valid one
    static bool IsValidStatus(chStatus status) { return status != kUNKNOWN; }

  }; // class ChannelStatusProvider

} // namespace lariov

#endif // CHANNELSTATUSPROVIDER_H
