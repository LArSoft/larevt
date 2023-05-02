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
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace lariov {

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

}
