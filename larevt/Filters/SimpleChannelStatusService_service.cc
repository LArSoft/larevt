/**
 * @file   SimpleChannelStatusService_service.cc
 * @brief  Service Registration for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    SimpleChannelStatusService.h
 */

// Our header
#include "larevt/Filters/SimpleChannelStatusService.h"

// Framework libraries
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"

namespace lariov {

  DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SimpleChannelStatusService,
                                    lariov::ChannelStatusService)

}
