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
    using ChannelSet_t = lariov::ChannelStatusProvider::ChannelSet_t;

    /// Configuration
    explicit SimpleChannelStatus(fhicl::ParameterSet const& pset);

    //
    // interface methods
    //

    /// @name Single channel queries
    /// @{
    /// Returns whether the specified channel is physical and connected to wire
    virtual bool IsPresent(raw::ChannelID_t channel) const override;

    /// Returns whether the specified channel is physical and good
    virtual bool IsGood(raw::ChannelID_t channel) const override
    {
      return IsPresent(channel) && !IsBad(channel) && !IsNoisy(channel);
    }

    /// Returns whether the specified channel is bad in the current run
    virtual bool IsBad(raw::ChannelID_t channel) const override
    {
      return fBadChannels.count(channel) > 0;
    }

    /// Returns whether the specified channel is noisy in the current run
    virtual bool IsNoisy(raw::ChannelID_t channel) const override
    {
      return fNoisyChannels.count(channel) > 0;
    }
    /// @}

    /// @name Global channel queries
    /// @{
    /// Returns a copy of set of good channel IDs for the current run
    virtual ChannelSet_t GoodChannels() const override;

    /// Returns a copy of set of bad channel IDs for the current run
    virtual ChannelSet_t BadChannels() const override { return fBadChannels; }

    /// Returns a copy of set of noisy channel IDs for the current run
    virtual ChannelSet_t NoisyChannels() const override { return fNoisyChannels; }
    /// @}

    //
    // non-interface methods and configuration methods
    //

    /// Returns the ID of the largest known channel
    raw::ChannelID_t MaxChannel() const { return fMaxChannel; }

    /// Returns the ID of the largest present channel
    raw::ChannelID_t MaxChannelPresent() const { return fMaxPresentChannel; }

    /// @name Configuration functions
    /// @{

    /**
     * @brief Sets the service provider up
     * @param MaxChannel ID of the last channel
     * @param MaxGoodChannel ID of the last good channel
     *
     * Sets the largest ID of a physically present channel.
     * All valid IDs smaller than this one are also considered present.
     * If MaxGoodChannel is invalid, all channels are considered present.
     */
    void Setup(raw::ChannelID_t MaxChannel, raw::ChannelID_t MaxGoodChannel);

    /**
     * @brief Sets the service provider up
     * @param MaxChannel ID of the last channel
     *
     * As Setup(raw::ChannelID_t, raw::ChannelID_t), but assumes all channels
     * to be present.
     */
    void Setup(raw::ChannelID_t MaxChannel) { Setup(MaxChannel, MaxChannel); }

    /* TODO DELME
    /// Prepares the object to provide information about the specified time
    /// @return always true
    virtual bool Update(std::uint64_t) override { return true; }
    */

    ///@}

  protected:
    ChannelSet_t fBadChannels;   ///< set of bad channels
    ChannelSet_t fNoisyChannels; ///< set of noisy channels

    raw::ChannelID_t fMaxChannel;        ///< largest ID among existing channels
    raw::ChannelID_t fMaxPresentChannel; ///< largest ID among present channels

    /// cached set of good channels (lazy evaluation)
    mutable std::unique_ptr<ChannelSet_t> fGoodChannels;

    /// Fills the collection of good channels
    void FillGoodChannels() const;

  }; // class SimpleChannelStatus

} // namespace lariov

#endif // SIMPLECHANNELFILTER_H
