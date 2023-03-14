////////////////////////////////////////////////////////////////////////
// \file SpaceChargeStandard.cxx
//
// \brief implementation of class for storing/accessing space charge distortions
//
// \author mrmooney@bnl.gov
//
////////////////////////////////////////////////////////////////////////

// C++ language includes

// LArSoft includes
#include "larevt/SpaceChargeServices/SpaceChargeServiceStandard.h"

// Framework includes
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "canvas/Persistency/Provenance/RunID.h"

//-----------------------------------------------
spacecharge::SpaceChargeServiceStandard::SpaceChargeServiceStandard(fhicl::ParameterSet const& pset,
                                                                    art::ActivityRegistry& reg)
  : fProp{pset}
{
  reg.sPreBeginRun.watch(this, &SpaceChargeServiceStandard::preBeginRun);
}

//----------------------------------------------
void spacecharge::SpaceChargeServiceStandard::preBeginRun(const art::Run& run)
{
  fProp.Update(run.run());
}

//------------------------------------------------
void spacecharge::SpaceChargeServiceStandard::reconfigure(fhicl::ParameterSet const& pset)
{
  fProp.Configure(pset);
}

//------------------------------------------------
DEFINE_ART_SERVICE_INTERFACE_IMPL(spacecharge::SpaceChargeServiceStandard,
                                  spacecharge::SpaceChargeService)
