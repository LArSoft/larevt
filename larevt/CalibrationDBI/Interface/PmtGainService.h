/**
 * @file   PmtGainService.h
 * @brief  Interface for experiment-specific service for pmt gain info
 * @author Brandon Eberly (eberly@fnal.gov)
 * @date   August 30, 2015
 */

#ifndef PMTGAINSERVICE_H
#define PMTGAINSERVICE_H

// Framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

//forward declarations
namespace lariov {
  class PmtGainProvider;
}

namespace lariov {

  /**
   \class PmtGainService
   This service provides only a simple interface to a provider class
   */
  class PmtGainService {

  public:
    using provider_type = PmtGainProvider;

    /// Destructor
    virtual ~PmtGainService() = default;

    //retrieve provider
    PmtGainProvider const& GetProvider() const { return DoGetProvider(); }

    PmtGainProvider const* GetProviderPtr() const { return DoGetProviderPtr(); }

  private:
    /// Returns a reference to the service provider
    virtual PmtGainProvider const& DoGetProvider() const = 0;

    virtual PmtGainProvider const* DoGetProviderPtr() const = 0;

  }; // class PmtGainService
} // namespace lariov

DECLARE_ART_SERVICE_INTERFACE(lariov::PmtGainService, SHARED)

#endif
