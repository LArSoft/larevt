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

DEFINE_ART_SERVICE_INTERFACE_IMPL(spacecharge::SpaceChargeServiceStandard,
                                  spacecharge::SpaceChargeService)
