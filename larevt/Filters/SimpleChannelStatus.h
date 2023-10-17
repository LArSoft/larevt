/**
 * @file   SimpleChannelStatus.h
 * @brief  Channel quality provider with information from configuration file
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 25th, 2014
 * @see    ChannelStatusService.h SimpleChannelStatus.cpp
 */

#ifndef SIMPLECHANNELFILTER_H
#define SIMPLECHANNELFILTER_H 1

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "larevt/CalibrationDBI/Interface/CalibrationDBIFwd.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"

// Utility libraries
namespace fhicl {
  class ParameterSet;
}

// C/C++ standard library
#include <memory> // std::unique_ptr<>

namespace lariov {

  /** **************************************************************************
   * @brief Class providing information about the quality of channels
   *
   * This class serves information read from a FHiCL configuration file.
   * Time-dependent (or, run-dependent) information is not supported.
   *
   * All and only the channels up to the one specified in the Setup() call (that
   * one included) are considered present. If no valid ID is specified, all
   * channels are supposed present.
   *
   * LArSoft interface to this class is through the service
   * SimpleChannelStatusService.
   *
   * Configuration parameters
   * =========================
   *
   * - *BadChannels* (list of integers, default: empty) list of ID of bad
   *   channels
   * - *NoisyChannels* (list of integers, default: empty) list of ID of noisy
   *   channels
   *
   */
  class SimpleChannelStatus : public lariov::ChannelStatusProvider {
  public:
    /// Configuration
    explicit SimpleChannelStatus(fhicl::ParameterSet const& pset,
                                 raw::ChannelID_t maxchannel,
                                 raw::ChannelID_t maxgoodchannel);

    //
    // interface methods
    //

    /// @name Single channel queries
    /// @{
    /// Returns whether the specified channel is physical and connected to wire
    virtual bool IsPresent(DBTimeStamp_t ts, raw::ChannelID_t channel) const override;

    /// Returns whether the specified channel is physical and good
    virtual bool IsGood(DBTimeStamp_t ts, raw::ChannelID_t channel) const override
    {
      return IsPresent(ts, channel) && !IsBad(ts, channel) && !IsNoisy(ts, channel);
    }

    /// Returns whether the specified channel is bad in the current run
    virtual bool IsBad(DBTimeStamp_t, raw::ChannelID_t channel) const override
    {
      return fBadChannels.count(channel) > 0;
    }

    /// Returns whether the specified channel is noisy in the current run
    virtual bool IsNoisy(DBTimeStamp_t, raw::ChannelID_t channel) const override
    {
      return fNoisyChannels.count(channel) > 0;
    }
    /// @}

    /// @name Global channel queries
    /// @{
    /// Returns a copy of set of good channel IDs for the current run
    virtual ChannelSet_t GoodChannels(DBTimeStamp_t ts) const override;

    /// Returns a copy of set of bad channel IDs for the current run
    virtual ChannelSet_t BadChannels(DBTimeStamp_t ts) const override { return fBadChannels; }

    /// Returns a copy of set of noisy channel IDs for the current run
    virtual ChannelSet_t NoisyChannels(DBTimeStamp_t ts) const override { return fNoisyChannels; }
    /// @}

    //
    // non-interface methods and configuration methods

    /// Returns the ID of the largest known channel
    raw::ChannelID_t MaxChannel() const { return fMaxChannel; }

    /// Returns the ID of the largest present channel
    raw::ChannelID_t MaxChannelPresent() const { return fMaxPresentChannel; }

    chStatus Status(DBTimeStamp_t ts, raw::ChannelID_t channel) const override;

  protected:
    const raw::ChannelID_t fMaxChannel;        ///< largest ID among existing channels
    const raw::ChannelID_t fMaxPresentChannel; ///< largest ID among present channels
    const ChannelSet_t fBadChannels;           ///< set of bad channels
    const ChannelSet_t fNoisyChannels;         ///< set of noisy channels

  }; // class SimpleChannelStatus

} // namespace lariov

#endif // SIMPLECHANNELFILTER_H
