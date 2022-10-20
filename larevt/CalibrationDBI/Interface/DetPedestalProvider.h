/**
 * \file DetPedestalProvider
 *
 * \brief Class def header for a class DetPedestalProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef DETPEDESTALPROVIDER_H
#define DETPEDESTALPROVIDER_H

// LArSoft libraries
#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t

namespace lariov {

  /**
     \class DetPedestalProvider
     Pure abstract interface class for retrieving detector pedestals.
     Includes a feature to encourage database use: an Update method that can be used to update
     an implementation's local state to ensure that the correct information is retrieved
  */
  class DetPedestalProvider : private lar::UncopiableAndUnmovableClass {

  public:
    virtual ~DetPedestalProvider() = default;

    /// Retrieve pedestal information
    virtual float PedMean(raw::ChannelID_t ch) const = 0;
    virtual float PedRms(raw::ChannelID_t ch) const = 0;
    virtual float PedMeanErr(raw::ChannelID_t ch) const = 0;
    virtual float PedRmsErr(raw::ChannelID_t ch) const = 0;

    /* TODO DELME
      /// Update local state of implementation
      virtual bool Update(DBTimeStamp_t ts) = 0;
    */
  };
} //end namespace lariov

#endif
