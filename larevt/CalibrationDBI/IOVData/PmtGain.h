/**
 * \file PmtGain.h
 *
 * \ingroup IOVData
 *
 * \brief Class def header for a class PmtGain
 *
 * @author eberly@slac.stanford.edu
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_PMTGAIN_H
#define IOVDATA_PMTGAIN_H

#include "CalibrationExtraInfo.h"
#include "ChData.h"

namespace lariov {
  /**
     \class PmtGain
  */
  class PmtGain : public ChData {

  public:
    /// Constructor
    PmtGain(unsigned int ch) : ChData(ch), fExtraInfo("PmtGain") {}

    /// Default destructor
    ~PmtGain() {}

    float Gain() const { return fGain; }
    float GainErr() const { return fGainErr; }
    CalibrationExtraInfo const& ExtraInfo() const { return fExtraInfo; }

    void SetGain(float v) { fGain = v; }
    void SetGainErr(float v) { fGainErr = v; }
    void SetExtraInfo(CalibrationExtraInfo const& info) { fExtraInfo = info; }

  private:
    float fGain;
    float fGainErr;
    CalibrationExtraInfo fExtraInfo;

  }; // end class
} // end namespace lariov

#endif
/** @} */ // end of doxygen group
