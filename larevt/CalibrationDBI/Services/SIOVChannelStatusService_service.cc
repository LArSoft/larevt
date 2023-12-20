#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "art/Persistency/Provenance/ScheduleContext.h"
#include "fhiclcpp/ParameterSet.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "larevt/CalibrationDBI/IOVData/SIOVChannelStatusData.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Providers/SIOVChannelStatusProvider.h"

namespace lariov {
  /**
     \class SIOVChannelStatusService
     art service implementation of ChannelStatusService.  Implements
     a channel status retrieval service for database scheme in which
     all elements in a database folder share a common interval of validity
  */
  class SIOVChannelStatusService : public ChannelStatusService {

  public:
    SIOVChannelStatusService(fhicl::ParameterSet const& pset);

  private:
    ChannelStatusDataPtr DataFor(art::Event const& evt) const override
    {
      return std::make_shared<SIOVChannelStatusData>(&fProvider, evt.time().value());
    }
    SIOVChannelStatusProvider fProvider;
  };
} //end namespace lariov

DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVChannelStatusService,
                                   lariov::ChannelStatusService,
                                   SHARED)

namespace lariov {

  SIOVChannelStatusService::SIOVChannelStatusService(fhicl::ParameterSet const& pset)
    : fProvider(pset.get<fhicl::ParameterSet>("ChannelStatusProvider"))
  {}

} //end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVChannelStatusService, lariov::ChannelStatusService)
