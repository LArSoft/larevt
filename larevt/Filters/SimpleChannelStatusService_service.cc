/**
 * @file   SimpleChannelStatusService_service.cc
 * @brief  Service Registration for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    SimpleChannelStatusService.h
 */

// Our header
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "larevt/Filters/SimpleChannelStatusService.h"

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SimpleChannelStatusService, lariov::ChannelStatusService)
