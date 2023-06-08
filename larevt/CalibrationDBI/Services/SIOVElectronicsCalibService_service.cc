#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "larevt/CalibrationDBI/Interface/ElectronicsCalibService.h"
#include "larevt/CalibrationDBI/Providers/SIOVElectronicsCalibProvider.h"

namespace lariov {

  /**
     \class SIOVElectronicsCalibService
     art service implementation of ElectronicsCalibService.  Implements
     an electronics calibration retrieval service for database scheme in which
     all elements in a database folder share a common interval of validity
  */
  class SIOVElectronicsCalibService : public ElectronicsCalibService {

  public:
    SIOVElectronicsCalibService(fhicl::ParameterSet const& pset);

  private:
    ElectronicsCalibProvider const& DoGetProvider() const override { return fProvider; }

    ElectronicsCalibProvider const* DoGetProviderPtr() const override { return &fProvider; }

    SIOVElectronicsCalibProvider fProvider;
  };
} //end namespace lariov

DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVElectronicsCalibService,
                                   lariov::ElectronicsCalibService,
                                   SHARED)

namespace lariov {

  SIOVElectronicsCalibService::SIOVElectronicsCalibService(fhicl::ParameterSet const& pset)
    : fProvider(pset.get<fhicl::ParameterSet>("ElectronicsCalibProvider"))
  {}

} //end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVElectronicsCalibService,
                                  lariov::ElectronicsCalibService)
