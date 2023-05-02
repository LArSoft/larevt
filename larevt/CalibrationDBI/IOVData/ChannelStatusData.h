#ifndef CHANNELSTATUSDATA_H
#define CHANNELSTATUSDATA_H

#include <memory>
#include <set>

#include "larevt/CalibrationDBI/IOVData/Snapshot.h"
#include "larevt/CalibrationDBI/IOVData/ChannelStatus.h"

namespace lariov {

  /// Type of set of channel IDs
  using ChannelSet_t = std::set<raw::ChannelID_t>;

  class ChannelStatusData {
  public:
    virtual ~ChannelStatusData() = default;
    bool IsNoisy(raw::ChannelID_t ch) const { return GetChannelStatus(ch).IsNoisy(); };
    bool IsGood(raw::ChannelID_t ch) const { return GetChannelStatus(ch).IsGood(); };
    bool IsPresent(raw::ChannelID_t ch) const { return GetChannelStatus(ch).IsPresent(); };
    bool IsDead(raw::ChannelID_t ch) const { return GetChannelStatus(ch).IsDead(); };
    bool IsLowNoise(raw::ChannelID_t ch) const { return GetChannelStatus(ch).IsLowNoise(); };
    virtual ChannelSet_t GoodChannels() const = 0;
    virtual ChannelSet_t BadChannels() const = 0;
    virtual ChannelSet_t NoisyChannels() const = 0;

  private:
    virtual ChannelStatus GetChannelStatus(raw::ChannelID_t ch) const = 0;
  };

  using ChannelStatusDataPtr = std::shared_ptr<ChannelStatusData>;
  //Mt note: this should go to SIOVProvider eventually
}
#endif
