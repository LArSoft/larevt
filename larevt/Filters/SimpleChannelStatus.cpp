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

namespace lariov {

  //----------------------------------------------------------------------------
  SimpleChannelStatus::SimpleChannelStatus(fhicl::ParameterSet const& pset)
    : fMaxChannel(raw::InvalidChannelID), fMaxPresentChannel(raw::InvalidChannelID)
  {
    using chan_vect_t = std::vector<raw::ChannelID_t>;

    // Read the bad channels as a vector, then convert it into a set
    auto BadChannels = pset.get<chan_vect_t>("BadChannels", {});
    cet::copy_all(BadChannels, std::inserter(fBadChannels, fBadChannels.begin()));

    // Read the noise channels as a vector, then convert it into a set
    auto NoisyChannels = pset.get<chan_vect_t>("NoisyChannels", {});
    cet::copy_all(NoisyChannels, std::inserter(fNoisyChannels, fNoisyChannels.begin()));

  } // SimpleChannelStatus::SimpleChannelStatus()

  //----------------------------------------------------------------------------
  void SimpleChannelStatus::Setup(raw::ChannelID_t MaxChannel, raw::ChannelID_t MaxGoodChannel)
  {

    fMaxChannel = MaxChannel;
    fMaxPresentChannel = MaxGoodChannel;

    // clear the caches, if any
    fGoodChannels.reset();

  } // SimpleChannelStatus::Setup()

  //----------------------------------------------------------------------------
  bool SimpleChannelStatus::IsPresent(raw::ChannelID_t channel) const
  {
    return raw::isValidChannelID(fMaxPresentChannel) ?
             raw::isValidChannelID(channel) && (channel <= fMaxPresentChannel) :
             true;
  } // SimpleChannelStatus::isPresent()

  //----------------------------------------------------------------------------
  SimpleChannelStatus::ChannelSet_t SimpleChannelStatus::GoodChannels() const
  {

    if (!fGoodChannels) FillGoodChannels();
    return *fGoodChannels;

  } // SimpleChannelStatus::GoodChannels()

  //----------------------------------------------------------------------------
  void SimpleChannelStatus::FillGoodChannels() const
  {

    if (!fGoodChannels) fGoodChannels.reset(new ChannelSet_t);

    ChannelSet_t& GoodChannels = *fGoodChannels;
    GoodChannels.clear();

    std::vector<std::pair<ChannelSet_t::const_iterator, ChannelSet_t::const_iterator>> VetoedIDs;

    VetoedIDs.emplace_back(fBadChannels.cbegin(), fBadChannels.cend());
    VetoedIDs.emplace_back(fNoisyChannels.cbegin(), fNoisyChannels.cend());

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
      for (auto iter : VetoedIDs) {

        // check all the remaining vetoed channels in this list
        while (iter.first != iter.second) {
          // if the first vetoed channel is larger than the tested channel,
          // we are done with this list
          if (*(iter.first) > channel) break;

          if (*(iter.first) == channel) { // vetoed!
            bGood = false;
            ++(iter.first); // we have found this already
            break;
          }

          // the last vetoed channel is smaller than the tested channel;
          // maybe the next vetoed channel will be it!
          ++(iter.first);
        } // while

        if (!bGood) break; // already known bad, we are done
      }                    // for

      // add the channel
      if (bGood) GoodChannels.insert(channel);
      ++channel;
    } // while

  } // SimpleChannelStatus::GoodChannels()

  //----------------------------------------------------------------------------

} // namespace filter
