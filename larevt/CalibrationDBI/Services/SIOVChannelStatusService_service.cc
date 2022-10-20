#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "art/Persistency/Provenance/ScheduleContext.h"
#include "fhiclcpp/ParameterSet.h"
#include "larcore/CoreUtils/EnsureOnlyOneSchedule.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Providers/SIOVChannelStatusProvider.h"

namespace lariov {

  /**
     \class SIOVChannelStatusService
     art service implementation of ChannelStatusService.  Implements
     a channel status retrieval service for database scheme in which
     all elements in a database folder share a common interval of validity
  */
  class SIOVChannelStatusService : public ChannelStatusService,
                                   private lar::EnsureOnlyOneSchedule<SIOVChannelStatusService> {

  public:
    SIOVChannelStatusService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

    void PreProcessEvent(const art::Event& evt, art::ScheduleContext);

  private:
    const ChannelStatusProvider& DoGetProvider() const override { return fProvider; }

    const ChannelStatusProvider* DoGetProviderPtr() const override { return &fProvider; }

    SIOVChannelStatusProvider fProvider;
  };
} //end namespace lariov

DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVChannelStatusService,
                                   lariov::ChannelStatusService,
                                   SHARED)

namespace lariov {

  SIOVChannelStatusService::SIOVChannelStatusService(fhicl::ParameterSet const& pset,
                                                     art::ActivityRegistry& reg)
    : fProvider(pset.get<fhicl::ParameterSet>("ChannelStatusProvider"))
  {

    //register callback to update local database cache before each event is processed
    reg.sPreProcessEvent.watch(this, &SIOVChannelStatusService::PreProcessEvent);
  }

  void SIOVChannelStatusService::PreProcessEvent(const art::Event& evt, art::ScheduleContext)
  {

    //First grab an update from the database
    //fProvider.UpdateTimeStamp(evt.time().value());
    fProvider.Update(evt.time().value());
  }

} //end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVChannelStatusService, lariov::ChannelStatusService)
