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

#include "DBFolder.h"
#include "hep_concurrency/cache.h"
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
  class SIOVElectronicsCalibProvider : public ElectronicsCalibProvider {

  public:
    /// Constructors
    SIOVElectronicsCalibProvider(fhicl::ParameterSet const& p);

    /// Retrieve electronics calibration information
    float Gain(DBTimeStamp_t ts, DBChannelID_t ch) const override;
    float GainErr(DBTimeStamp_t ts, DBChannelID_t ch) const override;
    float ShapingTime(DBTimeStamp_t ts, DBChannelID_t ch) const override;
    float ShapingTimeErr(DBTimeStamp_t ts, DBChannelID_t ch) const override;
    CalibrationExtraInfo const& ExtraInfo(DBTimeStamp_t ts, DBChannelID_t ch) const override;

    using cache_t = hep::concurrency::cache<DBTimeStamp_t, Snapshot<ElectronicsCalib>>;
    using handle_t = cache_t::handle;

  private:
    /// Do actual database updates.

    handle_t DBUpdate(DBTimeStamp_t ts) const;

    DBFolder fDBFolder;

    // Time stamps.

    DataSource::ds fDataSource;
    mutable cache_t fData;
  };
} //end namespace lariov

#endif
