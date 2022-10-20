/**
 * @file   ElectronicsCalibService.h
 * @brief  Interface for experiment-specific service for pmt gain info
 * @author Brandon Eberly (eberly@fnal.gov)
 * @date   August 30, 2015
 */

#ifndef ELECTRONICSCALIBSERVICE_H
#define ELECTRONICSCALIBSERVICE_H

// Framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

//forward declarations
namespace lariov {
  class ElectronicsCalibProvider;
}

namespace lariov {

  /**
   \class ElectronicsCalibService
   This service provides only a simple interface to a provider class
   */
  class ElectronicsCalibService {

  public:
    using provider_type = ElectronicsCalibProvider;

    /// Destructor
    virtual ~ElectronicsCalibService() = default;

    //retrieve provider
    ElectronicsCalibProvider const& GetProvider() const { return DoGetProvider(); }

    ElectronicsCalibProvider const* GetProviderPtr() const { return DoGetProviderPtr(); }

  private:
    /// Returns a reference to the service provider
    virtual ElectronicsCalibProvider const& DoGetProvider() const = 0;

    virtual ElectronicsCalibProvider const* DoGetProviderPtr() const = 0;

  }; // class ElectronicsCalibService
} // namespace lariov

DECLARE_ART_SERVICE_INTERFACE(lariov::ElectronicsCalibService, LEGACY)

#endif
