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
#include <limits> // std::numeric_limits<>
#include <set>

// LArSoft libraries
#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t

/// Filters for channels, events, etc
namespace lariov {

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
  class ChannelStatusProvider : private lar::UncopiableAndUnmovableClass {

  public:
    using Status_t = unsigned short; ///< type representing channel status

    /// Type of set of channel IDs
    using ChannelSet_t = std::set<raw::ChannelID_t>;

    /// Value or invalid status
    static constexpr Status_t InvalidStatus = std::numeric_limits<Status_t>::max();

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
    virtual bool IsPresent(raw::ChannelID_t channel) const = 0;

    /// Returns whether the specified channel is bad in the current run
    virtual bool IsBad(raw::ChannelID_t channel) const = 0;

    /// Returns whether the specified channel is noisy in the current run
    virtual bool IsNoisy(raw::ChannelID_t channel) const = 0;

    /// Returns whether the specified channel is physical and good
    virtual bool IsGood(raw::ChannelID_t channel) const
    {
      return IsPresent(channel) && !IsBad(channel) && !IsNoisy(channel);
    }

    /// Returns a status integer with arbitrary meaning
    virtual Status_t Status(raw::ChannelID_t channel) const { return InvalidStatus; }

    /// Returns whether the specified status is a valid one
    virtual bool HasStatus(raw::ChannelID_t channel) const
    {
      return IsValidStatus(Status(channel));
    }

    /// Returns a copy of set of good channel IDs for the current run
    virtual ChannelSet_t GoodChannels() const = 0;

    /// Returns a copy of set of bad channel IDs for the current run
    virtual ChannelSet_t BadChannels() const = 0;

    /// Returns a copy of set of noisy channel IDs for the current run
    virtual ChannelSet_t NoisyChannels() const = 0;

    /* TODO DELME
      /// Prepares the object to provide information about the specified time
      /// @return whether information is available for the specified time
      virtual bool Update(DBTimeStamp_t ts) = 0;
      */

    /// Returns whether the specified status is a valid one
    static bool IsValidStatus(Status_t status) { return status != InvalidStatus; }

  }; // class ChannelStatusProvider

} // namespace lariov

#endif // CHANNELSTATUSPROVIDER_H
