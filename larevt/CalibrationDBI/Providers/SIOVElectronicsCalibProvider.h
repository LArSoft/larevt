/**
 * \file SIOVElectronicsCalibProvider.h
 *
 * \ingroup WebDBI
 *
 * \brief Class def header for a class SIOVElectronicsCalibProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef SIOVELECTRONICSCALIBPROVIDER_H
#define SIOVELECTRONICSCALIBPROVIDER_H

#include "DatabaseRetrievalAlg.h"
#include "larevt/CalibrationDBI/IOVData/ElectronicsCalib.h"
#include "larevt/CalibrationDBI/IOVData/IOVDataConstants.h"
#include "larevt/CalibrationDBI/IOVData/Snapshot.h"
#include "larevt/CalibrationDBI/Interface/ElectronicsCalibProvider.h"

namespace lariov {

  /**
   * @brief Retrieves information: electronics calibrations, specifically gain and shaping time
   *
   * Configuration parameters
   * =========================
   *
   * - *DatabaseRetrievalAlg* (parameter set, mandatory): configuration for the
   *   database; see lariov::DatabaseRetrievalAlg
   * - *UseDB* (boolean, default: false): retrieve information from the database
   * - *UseFile* (boolean, default: false): retrieve information from a file;
   *   not implemented yet
   * - *DefaultGain* (real, default: ): Gain returned
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultGainErr* (real, default: ): Gain uncertainty returned
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultShapingTime* (real, default: ): Shaping Time returned
   *   when /UseDB/ and /UseFile/ parameters are false
   * - *DefaultShapingTimeErr* (real, default: ): Shaping Time uncertainty returned
   *   when /UseDB/ and /UseFile/ parameters are false
   */
  class SIOVElectronicsCalibProvider : public DatabaseRetrievalAlg,
                                       public ElectronicsCalibProvider {

  public:
    /// Constructors
    SIOVElectronicsCalibProvider(fhicl::ParameterSet const& p);

    /// Reconfigure function called by fhicl constructor
    void Reconfigure(fhicl::ParameterSet const& p) override;

    /// Update event time stamp.
    void UpdateTimeStamp(DBTimeStamp_t ts);

    /// Update Snapshot and inherited DBFolder if using database.  Return true if updated
    bool Update(DBTimeStamp_t ts);

    /// Retrieve electronics calibration information
    const ElectronicsCalib& ElectronicsCalibObject(DBChannelID_t ch) const;
    float Gain(DBChannelID_t ch) const override;
    float GainErr(DBChannelID_t ch) const override;
    float ShapingTime(DBChannelID_t ch) const override;
    float ShapingTimeErr(DBChannelID_t ch) const override;
    CalibrationExtraInfo const& ExtraInfo(DBChannelID_t ch) const override;

  private:
    /// Do actual database updates.

    bool DBUpdate() const; // Uses current event time.
    bool DBUpdate(DBTimeStamp_t ts) const;

    // Time stamps.

    DBTimeStamp_t fEventTimeStamp;           // Most recently seen time stamp.
    mutable DBTimeStamp_t fCurrentTimeStamp; // Time stamp of cached data.

    DataSource::ds fDataSource;

    mutable Snapshot<ElectronicsCalib> fData;
  };
} //end namespace lariov

#endif
