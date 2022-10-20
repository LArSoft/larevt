#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "art/Persistency/Provenance/ScheduleContext.h"
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
    SIOVElectronicsCalibService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    ~SIOVElectronicsCalibService() {}

    void PreProcessEvent(const art::Event& evt, art::ScheduleContext)
    {
      fProvider.UpdateTimeStamp(evt.time().value());
    }

  private:
    ElectronicsCalibProvider const& DoGetProvider() const override { return fProvider; }

    ElectronicsCalibProvider const* DoGetProviderPtr() const override { return &fProvider; }

    SIOVElectronicsCalibProvider fProvider;
  };
} //end namespace lariov

DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVElectronicsCalibService,
                                   lariov::ElectronicsCalibService,
                                   LEGACY)

namespace lariov {

  SIOVElectronicsCalibService::SIOVElectronicsCalibService(fhicl::ParameterSet const& pset,
                                                           art::ActivityRegistry& reg)
    : fProvider(pset.get<fhicl::ParameterSet>("ElectronicsCalibProvider"))
  {
    //register callback to update local database cache before each event is processed
    reg.sPreProcessEvent.watch(this, &SIOVElectronicsCalibService::PreProcessEvent);
  }

} //end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVElectronicsCalibService,
                                  lariov::ElectronicsCalibService)
