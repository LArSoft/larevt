////////////////////////////////////////////////////////////////////////
// \file ChargeYieldStandard.cxx
//
// \brief implementation of class for storing/accessing charge yield
//
// \author jvdawson@fnal.gov
//
////////////////////////////////////////////////////////////////////////

// C++ language includes

// LArSoft includes
#include "larevt/ChargeYield/ChargeYieldStandard.h"
#include "larevt/ChargeYieldServices/ChargeYieldServiceStandard.h"

// Framework includes
#include "art/Framework/Principal/Run.h" // for Run
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/GlobalSignal.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "canvas/Persistency/Provenance/RunID.h"

//-----------------------------------------------
chargeyield::ChargeYieldServiceStandard::ChargeYieldServiceStandard(fhicl::ParameterSet const& pset,
                                                                    art::ActivityRegistry& reg)
  : fProp{pset}
{
  reg.sPreBeginRun.watch(this, &ChargeYieldServiceStandard::preBeginRun);
}

//----------------------------------------------
void
chargeyield::ChargeYieldServiceStandard::preBeginRun(const art::Run& run)
{
  fProp.Update(run.run());
}

//------------------------------------------------
void
chargeyield::ChargeYieldServiceStandard::reconfigure(fhicl::ParameterSet const& pset)
{
  fProp.Configure(pset);
}

//------------------------------------------------
DEFINE_ART_SERVICE_INTERFACE_IMPL(chargeyield::ChargeYieldServiceStandard,
                                  chargeyield::ChargeYieldService)
