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
#include "larcore/Geometry/WireReadout.h"

// Framework libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace lariov {

  //----------------------------------------------------------------------------
  SimpleChannelStatusService::SimpleChannelStatusService(fhicl::ParameterSet const& pset)
  {
    raw::ChannelID_t MaxChannel =
      art::ServiceHandle<geo::WireReadout const>()->Get().Nchannels() - 1;

    SimpleChannelStatus* simple_filter = new SimpleChannelStatus(pset);

    simple_filter->Setup(MaxChannel);

    fProvider.reset(simple_filter);

    mf::LogInfo("SimpleChannelStatusService")
      << "Loaded from configuration:"
      << "\n  - " << fProvider->BadChannels().size() << " bad channels"
      << "\n  - " << fProvider->NoisyChannels().size() << " noisy channels"
      << "\n  - largest channel ID: " << fProvider->MaxChannel()
      << ", largest present: " << fProvider->MaxChannelPresent();

  } // SimpleChannelStatusService::SimpleChannelStatusService()

}
