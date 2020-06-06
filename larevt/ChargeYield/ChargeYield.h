////////////////////////////////////////////////////////////////////////
// \file ChargeYield.h
//
// \brief pure virtual base interface for charge yield. Allowing to include modifications of number of drifting electrons depending on spatial position to account for such as (in a dual phase detector) liquid->gas extraction, Charge ReadOut plane transparency, GEM charge amplification and anode collection.
//
// \author jvdawson@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef CHARGEYIELD_CHARGEYIELD_H
#define CHARGEYIELD_CHARGEYIELD_H

// C/C++ standard libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"


namespace chargeyield{

  class ChargeYield {
    public:

      ChargeYield(const ChargeYield &) = delete;
      ChargeYield(ChargeYield &&) = delete;
      ChargeYield& operator = (const ChargeYield &) = delete;
      ChargeYield& operator = (ChargeYield &&) = delete;
      virtual ~ChargeYield() = default;

     
      virtual bool EnableChargeYield() const = 0;

      virtual double GetRemainingElectrons(double NeleCluster, double NelePlane) const =0;     
      virtual double GetNElectrons( unsigned short int cryostat, unsigned short int tpc,  geo::Point_t const& xyz, double tdrift, double Nelec_in) const = 0;
      //type for number of electrons in a cluster
      virtual double GetNElectronsPlane(unsigned short int cryostat, unsigned short int tpc, int p,  double Nelec_in)const =0;
    protected:

      ChargeYield() = default;

    }; // class ChargeYield
} //namespace chargeyield

#endif // CHARGEYIELD_CHARGEYIELD_H
