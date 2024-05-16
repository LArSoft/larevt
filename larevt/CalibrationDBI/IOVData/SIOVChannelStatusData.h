#ifndef SIOVCHANNELSTATUSDATA_H
#define SIOVCHANNELSTATUSDATA_H

#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "larevt/CalibrationDBI/IOVData/ChannelStatusData.h"
#include "larevt/CalibrationDBI/Providers/SIOVChannelStatusProvider.h"

namespace lariov {
  template <typename Provider> 
  class ChannelStatusDataT : public ChannelStatusData {
  public:
    ChannelStatusDataT(Provider const* p, DBTimeStamp_t t)
      : fProviderPtr(p), fTimestamp(t)
    {}
    ChannelSet_t BadChannels() const override { return fProviderPtr->BadChannels(fTimestamp); }
    bool IsGood(raw::ChannelID_t ch) const override { return fProviderPtr->IsGood(fTimestamp, ch); }
    bool IsBad(raw::ChannelID_t ch) const override { return fProviderPtr->IsBad(fTimestamp, ch); }
    bool IsNoisy(raw::ChannelID_t ch) const override
    {
      return fProviderPtr->IsNoisy(fTimestamp, ch);
    }
    bool IsPresent(raw::ChannelID_t ch) const override { return fProviderPtr->IsPresent(fTimestamp, ch); }
    ChannelSet_t NoisyChannels() const override { return fProviderPtr->NoisyChannels(fTimestamp); }
    chStatus Status(raw::ChannelID_t ch) const override
    {
      return fProviderPtr->Status(fTimestamp, ch);
    }

  private:
    Provider const* fProviderPtr;
    DBTimeStamp_t fTimestamp;
  };
  using SIOVChannelStatusData = ChannelStatusDataT<SIOVChannelStatusProvider>;
}
#endif

