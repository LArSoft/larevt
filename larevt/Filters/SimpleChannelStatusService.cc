/**
 * @file   SimpleChannelStatusService.cc
 * @brief  Service implementation for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    SimpleChannelStatusService.h
 */

// Our header
#include "larevt/Filters/SimpleChannelStatusService.h"

// LArSoft libraries
#include "larcore/Geometry/Geometry.h"

// Framework libraries
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace lariov {
  class SimpleChannelStatusData : public ChannelStatusData {
  public:
    SimpleChannelStatusData(SimpleChannelStatus const* p, DBTimeStamp_t t)
      : fProviderPtr(p), fTimestamp(t)
    {}
    ChannelSet_t BadChannels() const override { return fProviderPtr->BadChannels(fTimestamp); }
    bool IsGood(raw::ChannelID_t ch) const override { return fProviderPtr->IsGood(fTimestamp, ch); }
    bool IsBad(raw::ChannelID_t ch) const override { return fProviderPtr->IsBad(fTimestamp, ch); }
    bool IsNoisy(raw::ChannelID_t ch) const override
    {
      return fProviderPtr->IsNoisy(fTimestamp, ch);
    }
    ChannelSet_t NoisyChannels() const override { return fProviderPtr->NoisyChannels(fTimestamp); }
    chStatus Status(raw::ChannelID_t ch) const override
    {
      return fProviderPtr->Status(fTimestamp, ch);
    }

  private:
    SimpleChannelStatus const* fProviderPtr;
    DBTimeStamp_t fTimestamp;
  };

  //----------------------------------------------------------------------------
  SimpleChannelStatusService::SimpleChannelStatusService(fhicl::ParameterSet const& pset)
    : fMaxChannel{art::ServiceHandle<geo::Geometry const>()->Nchannels() - 1}
    , fProvider{pset, fMaxChannel, fMaxChannel}
  {
    mf::LogInfo("SimpleChannelStatusService")
      << "Loaded from configuration:"
      << "\n  - " << fProvider.BadChannels(-1u).size() << " bad channels"
      << "\n  - " << fProvider.NoisyChannels(-1u).size() << " noisy channels"
      << "\n  - largest channel ID: " << fProvider.MaxChannel()
      << ", largest present: " << fProvider.MaxChannelPresent();

  } // SimpleChannelStatusService::SimpleChannelStatusService()

  ChannelStatusDataPtr SimpleChannelStatusService::DataFor(art::Event const& evt) const
  {
    return std::make_shared<SimpleChannelStatusData>(&fProvider, evt.time().value());
  }

}
