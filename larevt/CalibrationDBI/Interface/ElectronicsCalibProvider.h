/**
 * \file ElectronicsCalibProvider
 *
 * \brief Class def header for a class ElectronicsCalibProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef ELECTRONICSCALIBPROVIDER_H
#define ELECTRONICSCALIBPROVIDER_H

#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larevt/CalibrationDBI/IOVData/CalibrationExtraInfo.h"

namespace lariov {

  /**
   \class ElectronicsCalibProvider
   * Currently, the class provides interface for the following information:
   * - electronics gain and its error
   * - electronics shaping time and its error
   * - electronics extra info, related to procedure that determines the gain and shaping time
   */
  class ElectronicsCalibProvider : private lar::UncopiableAndUnmovableClass {

  public:
    virtual ~ElectronicsCalibProvider() = default;

    /// Retrieve pmt gain information
    virtual float Gain(raw::ChannelID_t ch) const = 0;
    virtual float GainErr(raw::ChannelID_t ch) const = 0;
    virtual float ShapingTime(raw::ChannelID_t ch) const = 0;
    virtual float ShapingTimeErr(raw::ChannelID_t ch) const = 0;

    virtual CalibrationExtraInfo const& ExtraInfo(raw::ChannelID_t ch) const = 0;
  };
} //end namespace lariov

#endif
