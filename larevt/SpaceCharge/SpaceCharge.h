////////////////////////////////////////////////////////////////////////
// \file SpaceCharge.h
//
// \brief pure virtual base interface for space charge distortions
//
// \author mrmooney@bnl.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef SPACECHARGE_SPACECHARGE_H
#define SPACECHARGE_SPACECHARGE_H

// C/C++ standard libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"

namespace spacecharge {

  class SpaceCharge {
  public:
    SpaceCharge(const SpaceCharge&) = delete;
    SpaceCharge(SpaceCharge&&) = delete;
    SpaceCharge& operator=(const SpaceCharge&) = delete;
    SpaceCharge& operator=(SpaceCharge&&) = delete;
    virtual ~SpaceCharge() = default;

    virtual bool EnableSimSpatialSCE() const = 0;
    virtual bool EnableSimEfieldSCE() const = 0;
    virtual bool EnableCorrSCE() const = 0;
    virtual bool EnableCalSpatialSCE() const = 0;
    virtual bool EnableCalEfieldSCE() const = 0;

    virtual geo::Vector_t GetPosOffsets(geo::Point_t const& point) const = 0;
    virtual geo::Vector_t GetEfieldOffsets(geo::Point_t const& point) const = 0;
    virtual geo::Vector_t GetCalPosOffsets(geo::Point_t const& point, int const& TPCid) const = 0;
    virtual geo::Vector_t GetCalEfieldOffsets(geo::Point_t const& point,
                                              int const& TPCid) const = 0;

  protected:
    SpaceCharge() = default;

  }; // class SpaceCharge
} //namespace spacecharge

#endif // SPACECHARGE_SPACECHARGE_H
