/**
 * \file PmtGainProvider
 *
 * \brief Class def header for a class PmtGainProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef PMTGAINPROVIDER_H
#define PMTGAINPROVIDER_H

#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larevt/CalibrationDBI/IOVData/CalibrationExtraInfo.h"
#include "larevt/CalibrationDBI/Interface/CalibrationDBIFwd.h"

namespace lariov {

  /**
   \class PmtGainProvider
   * Currently, the class provides interface for the following information:
   * - pmt gain and its error
   * - pmt extra info, related to procedure that determines gain
   */
  class PmtGainProvider : private lar::UncopiableAndUnmovableClass {

  public:
    virtual ~PmtGainProvider() = default;

    /// Retrieve pmt gain information
    virtual float Gain(DBTimeStamp_t ts, raw::ChannelID_t ch) const = 0;
    virtual float GainErr(DBTimeStamp_t ts, raw::ChannelID_t ch) const = 0;

    virtual CalibrationExtraInfo const& ExtraInfo(DBTimeStamp_t ts, raw::ChannelID_t ch) const = 0;
  };
} //end namespace lariov

#endif
