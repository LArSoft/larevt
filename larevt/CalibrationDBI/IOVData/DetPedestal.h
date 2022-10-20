/**
 * \file DetPedestal.h
 *
 * \ingroup IOVData
 *
 * \brief Class def header for a class DetPedestal
 *
 * @author eberly@slac.stanford.edu
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_DETPEDESTAL_H
#define IOVDATA_DETPEDESTAL_H 1

#include "ChData.h"

namespace lariov {
  /**
     \class DetPedestal
  */
  class DetPedestal : public ChData {

  public:
    /// Constructor
    DetPedestal(unsigned int ch) : ChData(ch) {}

    /// Default destructor
    ~DetPedestal() {}

    float PedMean() const { return fPedMean; }
    float PedRms() const { return fPedRms; }
    float PedMeanErr() const { return fPedMeanErr; }
    float PedRmsErr() const { return fPedRmsErr; }

    void SetPedMean(float pedMean) { fPedMean = pedMean; }
    void SetPedRms(float pedRms) { fPedRms = pedRms; }
    void SetPedMeanErr(float pedMeanErr) { fPedMeanErr = pedMeanErr; }
    void SetPedRmsErr(float pedRmsErr) { fPedRmsErr = pedRmsErr; }

  private:
    float fPedMean;
    float fPedRms;
    float fPedMeanErr;
    float fPedRmsErr;

  }; // end class
} // end namespace lariov

#endif
/** @} */ // end of doxygen group
