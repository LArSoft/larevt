#ifndef ELECTRONLIFETIMESERVICE_H
#define ELECTRONLIFETIMESERVICE_H

#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "larcore/CoreUtils/ServiceUtil.h"

//forward declarations
namespace lariov {
  class ElectronLifetimeProvider;
}

namespace lariov {

  class ElectronLifetimeService {

  public:
    using provider_type = ElectronLifetimeProvider;

    virtual ~ElectronLifetimeService() = default;

    //retrieve provider
    const ElectronLifetimeProvider& GetProvider() const { return this->DoGetProvider(); }

    ElectronLifetimeProvider const* provider() const { return &DoGetProvider(); }

  private:
    virtual const ElectronLifetimeProvider& DoGetProvider() const = 0;
  };
} //end namespace lariov

DECLARE_ART_SERVICE_INTERFACE(lariov::ElectronLifetimeService, LEGACY)

#endif
