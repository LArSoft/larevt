/**
 * \file ElectronLifetimeProvider
 *
 * \brief Class def header for a class ElectronLifetimeProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef ELECTRONLIFETIMEPROVIDER_H
#define ELECTRONLIFETIMEPROVIDER_H

#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"

namespace lariov {

  /**
     \class ElectronLifetimeProvider
     Pure abstract interface class for retrieving electron lifetimes.

  */
  class ElectronLifetimeProvider : private lar::UncopiableAndUnmovableClass {

  public:
    virtual ~ElectronLifetimeProvider() = default;

    /// Retrieve pedestal information
    virtual float Lifetime(float t) const = 0;
    virtual float Purity() const = 0;
    virtual float LifetimeErr(float t) const = 0;
    virtual float PurityErr() const = 0;
  };
} //end namespace lariov

#endif
