////////////////////////////////////////////////////////////////////////
// \file ChargeYieldServiceStandard.h
//
// \brief header of service for storing/accessing charge yield
//
// \author jvdawson@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef CHARGEYIELDSERVICESTANDARD_H
#define CHARGEYIELDSERVICESTANDARD_H

#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "larevt/ChargeYield/ChargeYieldStandard.h"
#include "larevt/ChargeYieldServices/ChargeYieldService.h"

namespace chargeyield {
  class ChargeYieldServiceStandard : public ChargeYieldService {
  public:
    // this enables art to print the configuration help:
    // using Parameters =
    // art::ServiceTable<chargeyield::ChargeYieldStandard::ConfigurationParameters_t>;

    ChargeYieldServiceStandard(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

  private:
    void reconfigure(fhicl::ParameterSet const& pset);
    void preBeginRun(const art::Run& run);

    const provider_type*
    provider() const override
    {
      return &fProp;
    }

    chargeyield::ChargeYieldStandard fProp;

  }; // class ChargeYieldServiceStandard
} // namespace chargeyield

DECLARE_ART_SERVICE_INTERFACE_IMPL(chargeyield::ChargeYieldServiceStandard,
                                   chargeyield::ChargeYieldService,
                                   SHARED)

#endif // CHARGEYIELDSERVICESTANDARD_H
