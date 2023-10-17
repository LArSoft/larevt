/**
 * @file   SimpleChannelStatus.cpp
 * @brief  Channel quality provider with information from configuration file
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 25th, 2014
 * @see    SimpleChannelStatus.h
 */

// Our header
#include "larevt/Filters/SimpleChannelStatus.h"

// LArSoft library
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::isValidChannelID()

// Framework libraries
#include "cetlib/container_algorithms.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"

// C/C++ standard libraries
#include <iterator> // std::inserter()
#include <utility>  // std::pair<>

lariov::ChannelSet_t vectoset(fhicl::ParameterSet const& pset, std::string const name)
{
  auto vec = pset.get<std::vector<raw::ChannelID_t>>(name, {});
  return {vec.begin(), vec.end()};
}

namespace lariov {

  //----------------------------------------------------------------------------
  SimpleChannelStatus::SimpleChannelStatus(fhicl::ParameterSet const& pset,
                                           raw::ChannelID_t maxchannel,
                                           raw::ChannelID_t maxgoodchannel)
    : fMaxChannel(maxchannel)
    , fMaxPresentChannel(maxgoodchannel)
    , fBadChannels(vectoset(pset, "BadChannels"))
    , fNoisyChannels(vectoset(pset, "NoisyChannels"))
  {} // SimpleChannelStatus::SimpleChannelStatus()

  //----------------------------------------------------------------------------
  bool SimpleChannelStatus::IsPresent(DBTimeStamp_t, raw::ChannelID_t channel) const
  {
    bool allchannelspresent = raw::isValidChannelID(fMaxPresentChannel);
    return allchannelspresent ? raw::isValidChannelID(channel) && (channel <= fMaxPresentChannel) :
                                true;
  } // SimpleChannelStatus::isPresent()

  //----------------------------------------------------------------------------
  ChannelSet_t SimpleChannelStatus::GoodChannels(DBTimeStamp_t) const
  {

    ChannelSet_t GoodChannels;
    // go for the first (lowest) channel ID...
    raw::ChannelID_t channel = 0;
    while (!raw::isValidChannelID(channel))
      ++channel;

    // ... to the last present one
    raw::ChannelID_t last_channel = fMaxChannel;
    if (raw::isValidChannelID(fMaxPresentChannel) && (fMaxPresentChannel < last_channel))
      last_channel = fMaxPresentChannel;

    // if we don't know how many channels
    if (!raw::isValidChannelID(last_channel)) {
      // this exception means that the Setup() function was not called
      // or it was called with an invalid value
      throw cet::exception("SimpleChannelStatus")
        << "Can't fill good channel list since no largest channel was set up\n";
    } // if
    // add the channels to the set one by one
    while (channel <= last_channel) {
      bool bGood = true;

      // check if this channel is in any of the vetoed lists
      for (auto bad : fBadChannels) {
        if (bad > channel) break;
        if (bad == channel) { // vetoed!
          bGood = false;
          break;
        }
      }
      if (bGood) {
        for (auto noisy : fNoisyChannels) {
          if (noisy > channel) break;
          if (noisy == channel) { // vetoed!
            bGood = false;
            break;
          }
        }
      }
      // add the channel
      if (bGood) GoodChannels.insert(channel);
      ++channel;
    } // while

    return GoodChannels;
  } // SimpleChannelStatus::GoodChannels()

  //----------------------------------------------------------------------------

  chStatus SimpleChannelStatus::Status(DBTimeStamp_t ts, raw::ChannelID_t ch) const
  {
    if (!IsPresent(ts, ch)) return kUNKNOWN;
    if (IsGood(ts, ch)) return kGOOD;
    if (IsNoisy(ts, ch)) return kNOISY;
    return kUNKNOWN;
  }
} // namespace filter
