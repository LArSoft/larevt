/**
 * \file ElectronicsCalib.h
 *
 * \ingroup IOVData
 *
 * \brief Class def header for a class ElectronicsCalib
 *
 * @author eberly@slac.stanford.edu
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_ELECTRONICSCALIB_H
#define IOVDATA_ELECTRONICSCALIB_H

#include "CalibrationExtraInfo.h"
#include "ChData.h"

namespace lariov {
  /**
     \class ElectronicsCalib
  */
  class ElectronicsCalib : public ChData {

  public:
    /// Constructor
    ElectronicsCalib(unsigned int ch) : ChData(ch), fExtraInfo("ElectronicsCalib") {}

    /// Default destructor
    ~ElectronicsCalib() {}

    float Gain() const { return fGain; }
    float GainErr() const { return fGainErr; }
    float ShapingTime() const { return fShapingTime; }
    float ShapingTimeErr() const { return fShapingTimeErr; }
    CalibrationExtraInfo const& ExtraInfo() const { return fExtraInfo; }

    void SetGain(float v) { fGain = v; }
    void SetGainErr(float v) { fGainErr = v; }
    void SetShapingTime(float v) { fShapingTime = v; }
    void SetShapingTimeErr(float v) { fShapingTimeErr = v; }
    void SetExtraInfo(CalibrationExtraInfo const& info) { fExtraInfo = info; }

  private:
    float fGain;
    float fGainErr;
    float fShapingTime;
    float fShapingTimeErr;
    CalibrationExtraInfo fExtraInfo;

  }; // end class
} // end namespace lariov

#endif
/** @} */ // end of doxygen group
