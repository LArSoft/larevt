/**
 * \file SIOVPmtGainProvider.h
 *
 * \ingroup WebDBI
 *
 * \brief Class def header for a class SIOVPmtGainProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef SIOVPMTGAINPROVIDER_H
#define SIOVPMTGAINPROVIDER_H

#include "DBFolder.h"
#include "hep_concurrency/cache.h"
#include "larevt/CalibrationDBI/IOVData/IOVDataConstants.h"
#include "larevt/CalibrationDBI/IOVData/PmtGain.h"
#include "larevt/CalibrationDBI/IOVData/Snapshot.h"
#include "larevt/CalibrationDBI/Interface/PmtGainProvider.h"

namespace lariov {

  /**
   * @brief Retrieves information: pmt gain
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
   */
  class SIOVPmtGainProvider : public PmtGainProvider {

  public:
    /// Constructors
    SIOVPmtGainProvider(fhicl::ParameterSet const& p);

    float Gain(DBTimeStamp_t ts, DBChannelID_t ch) const override;
    float GainErr(DBTimeStamp_t ts, DBChannelID_t ch) const override;
    CalibrationExtraInfo const& ExtraInfo(DBTimeStamp_t ts, DBChannelID_t ch) const override;

    using cache_t = hep::concurrency::cache<DBTimeStamp_t, Snapshot<PmtGain>>;
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
