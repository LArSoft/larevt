////////////////////////////////////////////////////////////////////////
// \file ChargeYieldService.h
//
// \brief pure virtual service interface for charge yield
//
// \author jvdawson@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef CHARGEYIELDSERVICE_H
#define CHARGEYIELDSERVICE_H

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "larevt/ChargeYield/ChargeYield.h"

namespace chargeyield {
  class ChargeYieldService {
  public:
    using provider_type = chargeyield::ChargeYield;

    virtual ~ChargeYieldService() = default;
    virtual const chargeyield::ChargeYield* provider() const = 0;
  };
}

DECLARE_ART_SERVICE_INTERFACE(chargeyield::ChargeYieldService, SHARED)

#endif // CHARGEYIELDSERVICE_H
