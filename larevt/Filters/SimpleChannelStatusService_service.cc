/**
 * @file   SimpleChannelStatusService_service.cc
 * @brief  Service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    SimpleChannelStatusService.h
 */

// Our header
#include "larevt/Filters/SimpleChannelStatusService.h"

// LArSoft libraries
#include "larcore/Geometry/Geometry.h"

// Framework libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace lariov {

  //----------------------------------------------------------------------------
  SimpleChannelStatusService::SimpleChannelStatusService(fhicl::ParameterSet const& pset) :
  fMaxChannel{art::ServiceHandle<geo::Geometry const>()->Nchannels() - 1},
  fProvider{pset, fMaxChannel, fMaxChannel}
  {
    mf::LogInfo("SimpleChannelStatusService") << "Loaded from configuration:"
      << "\n  - " << fProvider.BadChannels().size() << " bad channels"
      << "\n  - " << fProvider.NoisyChannels().size() << " noisy channels"
      << "\n  - largest channel ID: " << fProvider.MaxChannel()
        << ", largest present: " << fProvider.MaxChannelPresent()
      ;

  } // SimpleChannelStatusService::SimpleChannelStatusService()


  //----------------------------------------------------------------------------
  DEFINE_ART_SERVICE_INTERFACE_IMPL
    (lariov::SimpleChannelStatusService, lariov::ChannelStatusService)

} // namespace filter
