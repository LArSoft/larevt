/**
 * @file   SimpleChannelStatusService.h
 * @brief  Service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    ChannelStatusService.h SimpleChannelStatus.h
 *
 * Service serving a SimpleChannelStatus.
 */

#ifndef SIMPLECHANNELFILTERSERVICE_H
#define SIMPLECHANNELFILTERSERVICE_H

// LArSoft libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/Filters/SimpleChannelStatus.h"

// C/C++ standard libraries
#include <memory> //std::unique_ptr<>

namespace art {
  class Run;
} // namespace art

namespace lariov {

  /**
   * @brief Service providing information about the quality of channels
   *
   * This is a simple implementation of the ChannelStatusService
   * service interface.
   * Channel lists are passed by FHiCL configuration.
   * Note that there is no support for conditions varying with time.
   *
   * @note This implementation requires Geometry service
   *
   * Configuration parameters
   * =========================
   *
   * In addition to the parameters supported by filter::SimpleChannelStatus,
   * this service supports:
   *
   * - *service_type* (string): must be set to "SimpleChannelStatusService"
   *
   */
  class SimpleChannelStatusService : public ChannelStatusService {
  public:
    /// Constructor: reads the channel IDs from the configuration
    SimpleChannelStatusService(fhicl::ParameterSet const& pset);

  private:
    virtual ChannelStatusProvider const& DoGetProvider() const override
    {
      return *DoGetProviderPtr();
    }
    virtual ChannelStatusProvider const* DoGetProviderPtr() const override
    {
      return fProvider.get();
    }

    /// Update valid channel range
    void UpdateChannelRange();

    /// React to a new run
    void postBeginRun(art::Run const& run);

    std::unique_ptr<SimpleChannelStatus> fProvider;

  }; // class SimpleChannelStatusService

} // namespace lariov

DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SimpleChannelStatusService,
                                   lariov::ChannelStatusService,
                                   LEGACY)

#endif // SIMPLECHANNELFILTERSERVICE_H
