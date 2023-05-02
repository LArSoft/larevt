#include "DBFolder.h"
#include "WebDBIConstants.h"
#include "WebError.h"
#include "larevt/CalibrationDBI/IOVData/TimeStampDecoder.h"

#include "cetlib/search_path.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "sqlite3.h"
#include "wda.h"
#include <cstring>
#include <sstream>
#include <stdlib.h>

namespace lariov {

  // Constructor.

  DBFolder::DBFolder(const std::string& name,
                     const std::string& url,
                     const std::string& url2,
                     const std::string& tag,
                     bool usesqlite,
                     bool testmode)
  {
    fFolderName = name;
    fURL = url;
    fURL2 = url2;
    fTag = tag;
    fUseSQLite = usesqlite;
    fTestMode = testmode;
    if (fURL[fURL.length() - 1] == '/') { fURL = fURL.substr(0, fURL.length() - 1); }
    fMaximumTimeout = 4 * 60; //4 minutes

    // If UsqSQLite is true, hunt for sqlite database file.
    // It is an error if this file can't be found.

    if (fUseSQLite) {
      std::string dbname = fFolderName + ".db";
      cet::search_path sp("FW_SEARCH_PATH");
      fSQLitePath = sp.find_file(dbname); // Throws exception if not found.
    }

    if (fTestMode && fURL2 != "") {
      mf::LogInfo log("DBFolder");
      log << "\nDBFolder test mode, will compare the following urls data."
          << "\n";
      log << fURL << "\n";
      log << fURL2 << "\n"
          << "\n";
    }
    if (fTestMode && fUseSQLite) {
      mf::LogInfo log("DBFolder");
      log << "\nDBFolder test mode, will compare the following url and sqlite data."
          << "\n";
      log << fURL << "\n";
      log << fSQLitePath << "\n"
          << "\n";
    }
  }

  DBFolder::DBFolder(fhicl::ParameterSet const& p)
    : DBFolder{p.get<std::string>("DBFolderName"),
               p.get<std::string>("DBUrl"),
               p.get<std::string>("DBUrl2", ""),
               p.get<std::string>("DBTag", ""),
               p.get<bool>("UseSQLite", false),
               p.get<bool>("TestMode", false)}
  {}

  DBDataset DBFolder::GetDataset(DBTimeStamp_t raw_time) const
  {
    //convert to IOVTimeStamp
    IOVTimeStamp ts = TimeStampDecoder::DecodeTimeStamp(raw_time);

    DBDataset dbdataset;

    //get full url string
    std::stringstream fullurl;
    fullurl << fURL << "/data?f=" << fFolderName << "&t=" << ts.DBStamp();
    if (fTag.length() > 0) fullurl << "&tag=" << fTag;

    //get new dataset
    if (fSQLitePath != "" && !fTestMode) { GetSQLiteData(raw_time / 1000000000, dbdataset); }
    else {
      if (fTestMode) {
        mf::LogInfo log("DBFolder");
        log << "Accessing primary calibration data from http conditions database server."
            << "\n";
        log << "Folder = " << fFolderName << "\n";
      }
      int err = 0;
      Dataset data = getDataWithTimeout(fullurl.str().c_str(), NULL, fMaximumTimeout, &err);
      int status = getHTTPstatus(data);
      if (status != 200) {
        std::string msg = "HTTP error from " + fullurl.str() +
                          ": status: " + std::to_string(status) + ": " +
                          std::string(getHTTPmessage(data));
        throw WebError(msg);
      }
      dbdataset = DBDataset(data, true);
    }

    // If test mode is selected, get comparison data.

    if (fTestMode) {
      if (fSQLitePath != "") {
        DBDataset compare1;
        mf::LogInfo("DBFolder") << "Accessing comparison data from sqlite database " << fSQLitePath
                                << "\n";
        GetSQLiteData(raw_time / 1000000000, compare1);
        CompareDataset(dbdataset, compare1);
      }
      if (fURL2 != "") {
        mf::LogInfo("DBFolder") << "Accessing comparison data from second database url."
                                << "\n";
        std::stringstream fullurl2;
        fullurl2 << fURL2 << "/data?f=" << fFolderName << "&t=" << ts.DBStamp();
        if (fTag.length() > 0) fullurl2 << "&tag=" << fTag;
        mf::LogInfo("DBFolder") << "Full url = " << fullurl2.str() << "\n";
        int err = 0;
        Dataset data = getDataWithTimeout(fullurl2.str().c_str(), NULL, fMaximumTimeout, &err);
        int status = getHTTPstatus(data);
        if (status != 200) {
          std::string msg = "HTTP error from " + fullurl2.str() +
                            ": status: " + std::to_string(status) + ": " +
                            std::string(getHTTPmessage(data));
          throw WebError(msg);
        }
        DBDataset compare2(data, true);
        CompareDataset(dbdataset, compare2);
      }
    }
    return dbdataset;
  }

  // Query data from sqlite database.
  // The return value of type Dataset (aka void*), is partially opaque type HttpResponse*
  // (defined in wda.c and copied above).

  void DBFolder::GetSQLiteData(int t, DBDataset& data) const
  {
    if (fSQLitePath == "") return;

    // DBDataset data to be filled.

    IOVTimeStamp begin_ts(0, 0);               // IOV begin time.
    IOVTimeStamp end_ts(0, 0);                 // IOV end time.
    std::vector<std::string> column_names;     // Column names.
    std::vector<std::string> column_types;     // Column types.
    std::vector<DBChannelID_t> channels;       // Channels.
    std::vector<DBDataset::value_type> values; // Calibration data (length nchan*ncols).

    // Open sqlite database.

    sqlite3* db;
    int rc = sqlite3_open(fSQLitePath.c_str(), &db);
    if (rc != SQLITE_OK) {
      mf::LogError("DBFolder") << "Failed to open sqlite database " << fSQLitePath << "\n";
      throw cet::exception("DBFolder") << "Failed to open sqlite database " << fSQLitePath;
    }

    // Query begin time of IOV.

    std::string table_iovs = fFolderName + "_iovs";
    std::string table_tag_iovs = fFolderName + "_tag_iovs";
    std::ostringstream sql;
    sql << "SELECT " << table_iovs << ".iov_id," << table_iovs << ".begin_time"
        << " FROM " << table_tag_iovs << "," << table_iovs << " WHERE " << table_tag_iovs
        << ".tag='" << fTag << "'"
        << " AND " << table_tag_iovs << ".iov_id=" << table_iovs << ".iov_id"
        << " AND " << table_iovs << ".begin_time <= " << t << " ORDER BY " << table_iovs
        << ".begin_time desc";

    // Prepare query.

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
      mf::LogError log("DBFolder");
      log << "sqlite3_prepare_v2 failed." << fSQLitePath << "\n";
      log << "Failed sql = " << sql.str() << "\n";
      throw cet::exception("DBFolder") << "sqlite3_prepare_v2 error.";
    }

    // Execute query.
    // Just retrieve first row.
    // It is an error if we don't get at least one row.

    rc = sqlite3_step(stmt);
    int begin_time = 0;
    if (rc == SQLITE_ROW) { begin_time = sqlite3_column_int(stmt, 1); }
    else {
      mf::LogError("DBFolder") << "sqlite3_step returned error result = " << rc << "\n";
      throw cet::exception("DBFolder") << "sqlite3_step error.";
    }

    // Delete query.

    sqlite3_finalize(stmt);

    // Query end time of IOV.

    sql.str("");
    sql << "SELECT " << table_iovs << ".begin_time"
        << " FROM " << table_tag_iovs << "," << table_iovs << " WHERE " << table_tag_iovs
        << ".tag='" << fTag << "'"
        << " AND " << table_tag_iovs << ".iov_id=" << table_iovs << ".iov_id"
        << " AND " << table_iovs << ".begin_time > " << t << " ORDER BY " << table_iovs
        << ".begin_time";

    // Prepare query.

    rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
      mf::LogError log("DBFolder");
      log << "sqlite3_prepare_v2 failed." << fSQLitePath << "\n";
      log << "Failed sql = " << sql.str() << "\n";
      throw cet::exception("DBFolder") << "sqlite3_prepare_v2 error.";
    }

    // Execute query.
    // Just retrieve first row.
    // If we don't get any rows, then end time is infinite.

    rc = sqlite3_step(stmt);
    int end_time = 0;
    if (rc == SQLITE_ROW) { end_time = sqlite3_column_int(stmt, 0); }
    else if (rc != SQLITE_DONE) {
      mf::LogError("DBFolder") << "sqlite3_step returned error result = " << rc << "\n";
      throw cet::exception("DBFolder") << "sqlite3_step error.";
    }

    // Delete query.

    sqlite3_finalize(stmt);

    // Query count of channels.
    // We do this so that we know how much memory to allocate.

    std::string table_data = fFolderName + "_data";
    sql.str("");
    sql << "SELECT COUNT(DISTINCT channel)"
        << " FROM " << table_data << "," << table_iovs << "," << table_tag_iovs << " WHERE "
        << table_tag_iovs << ".tag='" << fTag << "'"
        << " AND " << table_iovs << ".iov_id=" << table_tag_iovs << ".iov_id"
        << " AND " << table_data << ".__iov_id=" << table_tag_iovs << ".iov_id"
        << " AND " << table_iovs << ".begin_time <= " << t;
    // Prepare query.

    rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
      mf::LogError log("DBFolder");
      log << "sqlite3_prepare_v2 failed." << fSQLitePath << "\n";
      log << "Failed sql = " << sql.str() << "\n";
      throw cet::exception("DBFolder") << "sqlite3_prepare_v2 error.";
    }

    // Execute query.
    // Retrieve one row.
    // It is an error if we don't get at least one row.

    rc = sqlite3_step(stmt);
    unsigned int nrows = 0;
    if (rc == SQLITE_ROW) { nrows = sqlite3_column_int(stmt, 0); }
    else {
      mf::LogError("DBFolder") << "sqlite3_step returned error result = " << rc << "\n";
      throw cet::exception("DBFolder") << "sqlite3_step error.";
    }

    // Reserve collections that depend on number of rows (only).

    channels.reserve(nrows);

    // Delete query.

    sqlite3_finalize(stmt);

    // Stash begin time.

    begin_ts = IOVTimeStamp(begin_time, 0);

    // Stash end time.

    if (end_time == 0)
      end_ts = IOVTimeStamp::MaxTimeStamp();
    else
      end_ts = IOVTimeStamp(end_time, 0);

    // Main data query.

    sql.str("");
    sql << "SELECT " << table_data << ".*,MAX(begin_time)"
        << " FROM " << table_data << "," << table_iovs << "," << table_tag_iovs << " WHERE "
        << table_tag_iovs << ".tag='" << fTag << "'"
        << " AND " << table_iovs << ".iov_id=" << table_tag_iovs << ".iov_id"
        << " AND " << table_data << ".__iov_id=" << table_tag_iovs << ".iov_id"
        << " AND " << table_iovs << ".begin_time <= " << t << " GROUP BY channel"
        << " ORDER BY channel";

    // Prepare query.

    rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
      mf::LogError log("DBFolder");
      log << "sqlite3_prepare_v2 failed." << fSQLitePath << "\n";
      log << "Failed sql = " << sql.str() << "\n";
      throw cet::exception("DBFolder") << "sqlite3_prepare_v2 error.";
    }

    // Execute the query and retrieve one row.
    // We do this to extract the number, names, and types of relevant columns.

    int ncols = sqlite3_column_count(stmt);
    column_names.reserve(ncols);
    column_types.reserve(ncols);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {

      // Loop over columns.

      for (int col = 0; col < ncols; ++col) {
        std::string colname = sqlite3_column_name(stmt, col);

        // Ignore columns that begin with "_".
        // Also ignore utility column MAX(begin_time).

        if (colname[0] != '_' && colname.substr(0, 3) != "MAX") {
          column_names.push_back(colname);
          int dtype = sqlite3_column_type(stmt, col);
          if (dtype == SQLITE_INTEGER)
            column_types.push_back("integer");
          else if (dtype == SQLITE_FLOAT)
            column_types.push_back("real");
          else if (dtype == SQLITE_TEXT)
            column_types.push_back("text");
          else if (dtype == SQLITE_NULL)
            column_types.push_back("NULL");
          else {
            mf::LogError("DBFolder") << "Unknown type " << dtype << "\n";
            throw cet::exception("DBFolder") << "Unknown type " << dtype;
          }
        }
      }
    }
    else {
      mf::LogError("DBFolder") << "No data rows."
                               << "\n";
      throw cet::exception("DBFolder") << "No data rows.";
    }

    // Remember the number of relevant columns.

    size_t nrelcols = column_names.size();
    values.reserve(nrows * nrelcols);

    // Re-execute query.
    // Retrieve all data rows and stash in result.

    rc = sqlite3_reset(stmt);
    if (rc != SQLITE_OK) {
      mf::LogError("DBFolder") << "sqlite3_reset failed."
                               << "\n";
      throw cet::exception("DBFolder") << "sqlite3_failed.";
    }
    size_t irow = 0;
    while (rc != SQLITE_DONE) {
      rc = sqlite3_step(stmt);
      if (rc == SQLITE_ROW) {
        ++irow;
        if (irow > nrows) {
          mf::LogError("DBFolder") << "Too many data rows " << irow << "\n";
          throw cet::exception("DBFolder") << "Too many data rows " << irow;
        }

        // Loop over columns.
        // Remember that ncols is the number of columns returned by the query,
        // not the number of columns that get stored (some columns are ignored).

        bool firstcol = true;
        for (int col = 0; col < ncols; ++col) {
          std::string colname = sqlite3_column_name(stmt, col);

          // Ignore columns that begin with "_".
          // Also ignore utility column MAX(begin_time).

          if (colname[0] != '_' && colname.substr(0, 3) != "MAX") {
            int dtype = sqlite3_column_type(stmt, col);

            if (dtype == SQLITE_INTEGER) {
              long value = sqlite3_column_int(stmt, col);
              values.push_back(DBDataset::value_type(value));
              if (firstcol) channels.push_back(value);
            }
            else if (dtype == SQLITE_FLOAT) {
              double value = sqlite3_column_double(stmt, col);
              values.push_back(DBDataset::value_type(value));
              if (firstcol) {
                mf::LogError("DBFolder") << "First column has wrong type float."
                                         << "\n";
                throw cet::exception("DBFolder") << "First column has wrong type float.";
              }
            }
            else if (dtype == SQLITE_TEXT) {
              const char* s = (const char*)sqlite3_column_text(stmt, col);
              values.emplace_back(std::make_unique<std::string>(s));
              if (firstcol) {
                mf::LogError("DBFolder") << "First column has wrong type text."
                                         << "\n";
                throw cet::exception("DBFolder") << "First column has wrong type text.";
              }
            }
            else if (dtype == SQLITE_NULL) {
              values.push_back(DBDataset::value_type());
              if (firstcol) {
                mf::LogError("DBFolder") << "First column has wrong type null."
                                         << "\n";
                throw cet::exception("DBFolder") << "First column has wrong type null.";
              }
            }
            else {
              mf::LogError("DBFolder") << "Unrecognized sqlite data type"
                                       << "\n";
              throw cet::exception("DBFolder") << "Unrecognized sqlite data type.";
            }
            firstcol = false;
          }
        }
      }
      else if (rc != SQLITE_DONE) {
        mf::LogError("DBFolder") << "sqlite3_step returned error result = " << rc << "\n";
        throw cet::exception("DBFolder") << "sqlite3_step error.";
      }
    }
    if (irow != nrows) {
      mf::LogError("DBFolder") << "Wrong number of data rows " << irow << "," << nrows << "\n";
      throw cet::exception("DBFolder")
        << "Wrong number of data rows " << irow << "," << nrows << "\n";
    }
    if (values.size() != nrows * nrelcols) {
      mf::LogError("DBFolder") << "Wrong number of values " << values.size() << "," << nrows << ","
                               << nrelcols << "\n";
      throw cet::exception("DBFolder")
        << "Wrong number of values " << values.size() << "," << nrows << "," << nrelcols << "\n";
    }

    // Delete statement.

    sqlite3_finalize(stmt);

    // Close database.

    sqlite3_close(db);

    // Fill result.

    data = DBDataset(begin_ts,
                     end_ts,
                     std::move(column_names),
                     std::move(column_types),
                     std::move(channels),
                     std::move(values));

    // Done.

    return;
  }

  // Dump dataset by rows.

  void DBFolder::DumpDataset(const DBDataset& data) const
  {
    size_t nrows = data.nrows();
    size_t ncols = data.ncols();
    mf::LogInfo log("DBFolder");
    log << "Dataset contains " << nrows << " rows and " << ncols << " columns."
        << "\n";

    // Begin time.

    log << "IOV start time = " << data.beginTime().DBStamp() << "\n";

    // End time.

    log << "IOV end time = " << data.endTime().DBStamp() << "\n";

    // Columnn names.

    const std::vector<std::string>& names = data.colNames();
    for (size_t c = 0; c < ncols; ++c)
      log << "Column " << c << ", name = " << names[c] << "\n";

    // Row 3 - column types.

    const std::vector<std::string>& types = data.colTypes();
    for (size_t c = 0; c < ncols; ++c)
      log << "Column " << c << ", type = " << types[c] << "\n";

    // Data rows.

    for (size_t row = 0; row < nrows; ++row) {
      log << "\nRow " << row << "\n";
      DBDataset::DBRow dbrow = data.getRow(row);

      // Loop over columns.

      for (size_t col = 0; col < ncols; ++col) {
        if (types[col] == "bigint" || types[col] == "integer" || types[col] == "boolean") {
          long value = dbrow.getLongData(col);
          log << names[col] << " = " << value << "\n";
        }
        else if (types[col] == "real") {
          double value = dbrow.getDoubleData(col);
          log << names[col] << " = " << value << "\n";
        }
        else if (types[col] == "text" or types[col] == "boolean") {
          std::string value = dbrow.getStringData(col);
          log << names[col] << " = " << value << "\n";
        }
        else {
          mf::LogError("DBFolder") << "Unknown type " << types[col] << "\n";
          throw cet::exception("DBFolder") << "Unknown type.";
        }
      }
    }
  }

  bool DBFolder::CompareDataset(const DBDataset& data1, const DBDataset& data2) const
  {
    bool compare_ok = true;
    mf::LogInfo("DBFolder") << "\nComparing datasets."
                            << "\n";

    size_t nrows1 = data1.nrows();
    size_t nrows2 = data2.nrows();
    if (nrows1 != nrows2) compare_ok = false;

    // Compare begin time.

    std::string begin1 = data1.beginTime().DBStamp();
    std::string begin2 = data2.beginTime().DBStamp();
    if (begin1 != begin2) compare_ok = false;

    // Compare end time.

    std::string end1 = data1.beginTime().DBStamp();
    std::string end2 = data2.beginTime().DBStamp();
    if (end1 != end2) compare_ok = false;

    // Compare column names.

    size_t ncols1 = data1.ncols();
    size_t ncols2 = data2.ncols();
    const std::vector<std::string>& names1 = data1.colNames();
    const std::vector<std::string>& names2 = data2.colNames();
    if (ncols1 != ncols2 || ncols1 != names1.size() || ncols2 != names2.size()) {
      mf::LogWarning("DBFolder") << "Columns names size mismatch " << ncols1 << " vs. " << ncols2
                                 << " vs. " << names1.size() << " vs. " << names2.size() << "\n";
      compare_ok = false;
    }
    if (compare_ok) {
      for (size_t c = 0; c < ncols1; ++c) {
        if (names1[c] != names2[c]) {
          mf::LogWarning("DBFolder")
            << "Name mismatch " << names1[c] << " vs. " << names2[c] << "\n";
          compare_ok = false;
        }
      }
    }

    // Compare column types.

    const std::vector<std::string>& types1 = data1.colTypes();
    const std::vector<std::string>& types2 = data2.colTypes();
    if (ncols1 != ncols2 || ncols1 != types1.size() || ncols2 != types2.size()) {
      mf::LogWarning("DBFolder") << "Column types ize mismatch " << ncols1 << " vs. " << ncols2
                                 << " vs. " << types1.size() << " vs. " << types2.size() << "\n";
      compare_ok = false;
    }
    if (compare_ok) {
      for (size_t c = 0; c < ncols2; ++c) {

        // Type "bigint" matches "integer."
        // Type "boolean" matches "integer."

        std::string type1 = types1[c];
        std::string type2 = types2[c];
        if (type1 == "bigint" || type1 == "boolean") type1 = "integer";
        if (type2 == "bigint" || type2 == "boolean") type2 = "integer";
        if (type1 != type2) {
          mf::LogWarning("DBFolder") << "Type mismatch " << type1 << " vs. " << type2 << "\n";
          compare_ok = false;
        }
      }
    }

    // Compare channels.

    const std::vector<DBChannelID_t>& channels1 = data1.channels();
    const std::vector<DBChannelID_t>& channels2 = data2.channels();
    if (nrows1 != nrows2 || nrows1 != channels1.size() || nrows2 != channels2.size()) {
      mf::LogWarning("DBFolder") << "Channels size mismatch " << nrows1 << " vs. " << nrows2
                                 << " vs. " << channels1.size() << " vs. " << channels2.size()
                                 << "\n";
      compare_ok = false;
    }
    if (compare_ok) {
      for (size_t r = 0; r < nrows1; ++r) {
        if (channels1[r] != channels2[r]) {
          mf::LogWarning("DBFolder")
            << "Channel mismatch " << channels1[r] << " vs. " << channels2[r] << "\n";
          compare_ok = false;
        }
      }
    }

    // Compare number of values.

    if (data1.data().size() != data2.data().size()) {
      mf::LogWarning("DBFolder") << "Values size mismatch " << data1.data().size() << " vs. "
                                 << data2.data().size() << "\n";
      compare_ok = false;
    }

    // Data rows.
    if (compare_ok) {
      for (size_t row = 0; row < nrows1; ++row) {

        DBDataset::DBRow dbrow1 = data1.getRow(row);
        DBDataset::DBRow dbrow2 = data2.getRow(row);

        // Loop over columns.

        for (size_t col = 0; col < ncols1; ++col) {
          if (types1[col] == "integer" || types1[col] == "bigint" || types1[col] == "boolean") {
            long value1 = dbrow1.getLongData(col);
            long value2 = dbrow2.getLongData(col);
            if (value1 != value2) {
              mf::LogWarning("DBFolder")
                << "Value mismatch " << value1 << " vs. " << value2 << "\n";
              compare_ok = false;
            }
          }
          else if (types1[col] == "real") {
            double value1 = dbrow1.getDoubleData(col);
            double value2 = dbrow2.getDoubleData(col);
            if (value1 != value2) {
              mf::LogWarning("DBFolder")
                << "Value mismatch " << value1 << " vs. " << value2 << "\n";
              compare_ok = false;
            }
          }
          else if (types1[col] == "text") {
            std::string value1 = dbrow2.getStringData(col);
            std::string value2 = dbrow2.getStringData(col);
            if (value1 != value2) {
              mf::LogWarning("DBFolder")
                << "Value mismatch " << value1 << " vs. " << value2 << "\n";
              compare_ok = false;
            }
          }
          else {
            mf::LogError("DBFolder") << "Unknown type " << types1[col] << "\n";
            throw cet::exception("DBFolder") << "Unknown type.";
          }
        }
      }
    }

    if (compare_ok) {
      mf::LogInfo("DBFolder") << "Comparison OK.\n"
                              << "\n";
    }
    else {
      mf::LogError("DBFolder") << "Comparison fail."
                               << "\n";
      throw cet::exception("DBFolder") << "Comparison fail.";
    }
    return compare_ok;
  }

} //end namespace lariov
