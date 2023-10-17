/**
 * @file   ChannelStatusService.h
 * @brief  Interface for experiment-specific service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 *
 * The schema is the same as for ExptGeoHelperInterface in Geometry library
 * (larcore repository).
 * The implementations of this interface can be directly used as art services.
 */

#ifndef CHANNELSTATUSSERVICE_H
#define CHANNELSTATUSSERVICE_H

// LArSoft libraries
#include "larevt/CalibrationDBI/IOVData/ChannelStatusData.h"

// Framework libraries
#include "art/Framework/Principal/fwd.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

//forward declarations
namespace lariov {
  class ChannelStatusProvider;
}

namespace lariov {

  /**
   * @brief Service providing information about the quality of channels
   *
   * This service provides only a simple interface.
   * Experiments need to implement and configure their own service implementing
   * this interface.
   * The simplest implementation is provided in LArSoft:
   * SimpleChannelStatusService.
   *
   * Currently, the service provides interface for the following information:
   * - goodness of the channel: good or bad (dead or unusable)
   * - noisiness of the channel: good or noisy (or compromised in some way)
   * - presence of the channel: connected to a wire or not
   *
   * The use of this service replaces the deprecated ChannelFilter class.
   * An algorithm that used to use ChannelFilter class can be updated. From:
   *
   *      filter::ChannelFilter* chanFilt = new filter::ChannelFilter();
   *
   * to
   *
   *      art::ServiceHandle<lariov::ChannelStatusService const> chanFilt;
   *
   * (include files CalibrationDBI/Interface/ChannelStatusService.h instead of
   * Filters/ChannelFilter.h) or
   *
   *      lariov::ChannelStatusProvider const& chanFilt
   *        = art::ServiceHandle<lariov::ChannelStatusService const>()
   *          ->GetProvider();
   *
   * (include files CalibrationDBI/Interface/ChannelStatusService.h and
   * CalibrationDBI/Interface/ChannelStatusProvider.h instead of
   * Filters/ChannelFilter.h).
   * The latter object can in principle be passed to algorithms that are not
   * art-aware.
   *
   */
  class ChannelStatusService {
  public:
    virtual ~ChannelStatusService() = default;
    virtual ChannelStatusDataPtr DataFor(art::Event const& evt) const = 0;
  }; // class ChannelStatusService

} // namespace lariov

DECLARE_ART_SERVICE_INTERFACE(lariov::ChannelStatusService, SHARED)

#endif // CHANNELSTATUSSERVICE_H
