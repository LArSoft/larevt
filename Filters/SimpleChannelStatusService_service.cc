/**
 * @file   SimpleChannelStatusService_service.cc
 * @brief  Service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    SimpleChannelStatusService.h
 */

// Our header
#include "Filters/SimpleChannelStatusService.h"

// LArSoft libraries
#include "Geometry/Geometry.h"
#include "Geometry/GeometryCore.h"

// Framework libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"



namespace lariov {

  //----------------------------------------------------------------------------
  SimpleChannelStatusService::SimpleChannelStatusService
    (fhicl::ParameterSet const& pset, art::ActivityRegistry&)
  {
    SimpleChannelStatus* simple_filter = new SimpleChannelStatus(pset);
    
    raw::ChannelID_t MaxChannel
      = raw::ChannelID_t(art::ServiceHandle<geo::Geometry>()->Nchannels() - 1);
    
    simple_filter->Setup(MaxChannel);
    
    fProvider.reset(simple_filter);
    
    mf::LogInfo("SimpleChannelStatusService") << "Loaded from configuration:"
      << "\n  - " << fProvider->BadChannels().size() << " bad channels"
      << "\n  - " << fProvider->NoisyChannels().size() << " noisy channels"
      << "\n  - largest channel ID: " << fProvider->MaxChannel()
        << ", largest present: " << fProvider->MaxChannelPresent()
      ;
    
  } // SimpleChannelStatusService::SimpleChannelStatusService()
  
  
  //----------------------------------------------------------------------------
  DEFINE_ART_SERVICE_INTERFACE_IMPL
    (lariov::SimpleChannelStatusService, lariov::IChannelStatusService)
  
} // namespace filter
