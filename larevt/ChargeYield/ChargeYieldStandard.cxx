////////////////////////////////////////////////////////////////////////
// \file ChargeYieldStandard.cxx
//
// \brief implementation of class for storing/accessing charge yield (exists as an example, by default this does nothing, instead requiring experiment-specific implementation)
//
// \author jvdawson@fnal.gov
//
////////////////////////////////////////////////////////////////////////

// C++ language includes
#include <fstream>

// LArSoft includes
#include "larevt/ChargeYield/ChargeYieldStandard.h"

// Framework includes
#include "canvas/Utilities/Exception.h"
#include "fhiclcpp/ParameterSet.h"

// ROOT includes
#include "TFile.h"
#include "TString.h"

//-----------------------------------------------
chargeyield::ChargeYieldStandard::ChargeYieldStandard(
  fhicl::ParameterSet const& pset
)
{
  Configure(pset);
}

//------------------------------------------------
bool chargeyield::ChargeYieldStandard::Configure(fhicl::ParameterSet const& pset)
{
  fEnableChargeYield = pset.get<bool>("EnableChargeYield");

  if(fEnableChargeYield == true)
    {
      //do something..
    }


  return true;
}

//------------------------------------------------
bool chargeyield::ChargeYieldStandard::Update(uint64_t ts)
{
  if (ts == 0) return false;

  return true;
}

//----------------------------------------------------------------------------
/// Return boolean indicating whether or not to turn simulation of SCE on for
/// spatial distortions
bool chargeyield::ChargeYieldStandard::EnableChargeYield() const
{
  return fEnableChargeYield;
}
double chargeyield::ChargeYieldStandard::GetRemainingElectrons(double NeleCluster, double NelePlane) const 
{
  return NeleCluster; //return number in cluster (no subtraction)
}
double chargeyield::ChargeYieldStandard::GetNElectrons( unsigned short int cryostat, unsigned short int tpc,  geo::Point_t const& point, double tdrift, double Nelec_in) const
{
  return Nelec_in;
}
double chargeyield::ChargeYieldStandard::GetNElectronsPlane(unsigned short int cryostat, unsigned short int tpc, int p, double Nelec_in) const
{
  return Nelec_in;
}
    
//----------------------------------------------------------------------------

