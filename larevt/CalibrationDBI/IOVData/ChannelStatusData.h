#ifndef CHANNELSTATUSDATA_H
#define CHANNELSTATUSDATA_H

#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "larevt/CalibrationDBI/IOVData/ChannelStatus.h"
#include "larevt/CalibrationDBI/IOVData/Snapshot.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"
#include <memory>
#include <set>

namespace lariov {

  /// Type of set of channel IDs
  //  using ChannelSet_t = std::set<raw::ChannelID_t>;

  class ChannelStatusData {
  public:
    virtual ~ChannelStatusData() = default;
    virtual ChannelSet_t BadChannels() const = 0;
    virtual bool IsGood(raw::ChannelID_t ch) const = 0;
    virtual bool IsBad(raw::ChannelID_t ch) const = 0;
    virtual bool IsNoisy(raw::ChannelID_t ch) const = 0;
    virtual bool IsPresent(raw::ChannelID_t ch) const = 0;
    virtual ChannelSet_t NoisyChannels() const = 0;
    virtual chStatus Status(raw::ChannelID_t ch) const = 0;
  };

  using ChannelStatusDataPtr = std::shared_ptr<ChannelStatusData>;
  //Mt note: this should go to SIOVProvider eventually
}
#endif
