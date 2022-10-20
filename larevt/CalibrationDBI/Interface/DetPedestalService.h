#ifndef DETPEDESTALSERVICE_H
#define DETPEDESTALSERVICE_H

#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "larcore/CoreUtils/ServiceUtil.h" // unused; for includer's convenience

//forward declarations
namespace lariov {
  class DetPedestalProvider;
}

namespace lariov {

  /**
     \class DetPedestalService
     art service interface for detector pedestal conditions retrieval
  */
  class DetPedestalService {

  public:
    using provider_type = DetPedestalProvider;

    virtual ~DetPedestalService() = default;

    //retrieve pedestal provider
    const DetPedestalProvider& GetPedestalProvider() const { return this->DoGetPedestalProvider(); }

    DetPedestalProvider const* provider() const { return &DoGetPedestalProvider(); }

  private:
    virtual const DetPedestalProvider& DoGetPedestalProvider() const = 0;
  };
} //end namespace lariov

DECLARE_ART_SERVICE_INTERFACE(lariov::DetPedestalService, SHARED)

#endif
