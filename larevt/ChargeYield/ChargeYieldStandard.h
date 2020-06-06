////////////////////////////////////////////////////////////////////////
// \file ChargeYieldStandard.h
//
// \brief header of class for storing/accessing charge yield
//
// \author jvdawson@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef CHARGEYIELD_CHARGEYIELDSTANDARD_H
#define CHARGEYIELD_CHARGEYIELDSTANDARD_H

// LArSoft libraries
#include "larevt/ChargeYield/ChargeYield.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"

// FHiCL libraries
namespace fhicl { class ParameterSet; }

// ROOT includes

// C/C++ standard libraries
#include <stdint.h>
#include <string>
#include <vector>

namespace chargeyield {

  class ChargeYieldStandard : public ChargeYield {

    public:

      explicit ChargeYieldStandard(fhicl::ParameterSet const& pset);
      ChargeYieldStandard(ChargeYieldStandard const&) = delete;
      virtual ~ChargeYieldStandard() = default;

      bool Configure(fhicl::ParameterSet const& pset);
      bool Update(uint64_t ts=0);
 
      bool EnableChargeYield() const override;

      double GetRemainingElectrons(double NeleCluster, double NelePlane) const override;
      double GetNElectrons( unsigned short int cryostat, unsigned short int tpc,  geo::Point_t const& point, double tdrift, double Nelec_in) const override;
      //type for number of electrons in a cluster
     double GetNElectronsPlane(unsigned short int cryostat, unsigned short int tpc, int p, double Nelec_in) const override;


    private:
    protected:
      
      bool fEnableChargeYield;


      std::string fRepresentationType;
      //      std::string fInputFilename;

  }; // class ChargeYieldStandard
} //namespace chargeyield
#endif // CHARGEYIELD_CHARGEYIELDSTANDARD_H
