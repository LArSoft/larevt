//=================================================================================
//
// Name: DBDataset.cpp
//
// Purpose: Implementation for class DBDataset.
//
// Created: 26-Oct-2020 - H. Greenlee
//
//=================================================================================

#include "DBDataset.h"
#include "WebDBIConstants.h"
#include "cetlib_except/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "wda.h"
#include <cstring>

// Default constructor.

lariov::DBDataset::DBDataset() : fBeginTime(0, 0), fEndTime(0, 0) {}

// Libwda initializing constructor.

lariov::DBDataset::DBDataset(void* dataset, bool release) : fBeginTime(0, 0), fEndTime(0, 0)
{
  // Parse dataset and get number of rows.

  size_t nrows = getNtuples(dataset) - kNUMBER_HEADER_ROWS;
  //mf::LogInfo("DBDataset") << "DBDataset: Number of rows = " << nrows << "\n";
  fChannels.reserve(nrows);

  // Process header rows.

  int err = 0;
  char buf[kBUFFER_SIZE];
  Tuple tup;

  // Extract IOV begin time.

  tup = getTuple(dataset, 0);
  getStringValue(tup, 0, buf, kBUFFER_SIZE, &err);
  fBeginTime = IOVTimeStamp::GetFromString(std::string(buf));
  //mf::LogInfo log("DBDataset");
  //log << "DBDataset: Begin time stamp input = " << buf << "\n";
  //log << "DBDataset: Begin time stamp = " << fBeginTime.DBStamp() << "\n";
  releaseTuple(tup);

  // Extract IOV end time.

  tup = getTuple(dataset, 1);
  getStringValue(tup, 0, buf, kBUFFER_SIZE, &err);
  if (0 == strcmp(buf, "-"))
    fEndTime = IOVTimeStamp::MaxTimeStamp();
  else
    fEndTime = IOVTimeStamp::GetFromString(std::string(buf));
  //mf::LogInfo log("DBDataset");
  //log << "DBDataset: End time stamp input = " << buf << "\n";
  //log << "DBDataset: End time stamp = " << fEndTime.DBStamp() << "\n";
  releaseTuple(tup);

  // Extract column names.

  tup = getTuple(dataset, 2);
  size_t ncols = getNfields(tup);
  //mf::LogInfo("DBDataset") << "DBDataset: Number of columns = " << ncols << "\n";
  fColNames.reserve(ncols);
  for (size_t col = 0; col < ncols; ++col) {
    getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
    //mf::LogInfo("DBDataset") << "DBDataset: Column name = " << buf << "\n";
    fColNames.push_back(std::string(buf));
  }
  releaseTuple(tup);

  // Extract column types.

  tup = getTuple(dataset, 3);
  fColTypes.reserve(ncols);
  for (size_t col = 0; col < ncols; ++col) {
    getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
    //mf::LogInfo("DBDataset") << "DBDataset: Column type = " << buf << "\n";
    fColTypes.push_back(std::string(buf));
  }
  releaseTuple(tup);

  // Extract data.  Loop over rows.

  fData.reserve(nrows * ncols);
  for (size_t row = 0; row < nrows; ++row) {
    //mf::LogInfo("DBDataset") << "\nRow " << row << "\n";
    tup = getTuple(dataset, row + kNUMBER_HEADER_ROWS);

    // Loop over columns.

    for (size_t col = 0; col < ncols; ++col) {
      getStringValue(tup, col, buf, kBUFFER_SIZE, &err);

      // Convert string value to DBDataset::value_type (std::variant).

      if (fColTypes[col] == "integer" || fColTypes[col] == "bigint") {
        long value = strtol(buf, 0, 10);
        fData.push_back(value_type(value));
        //mf::LogInfo("DBDataset") << "DBDataset: row=" << row << ", column=" << col << ", value=" << fData.back()
        //	<< "\n";
        if (col == 0) {
          fChannels.push_back(value);
          //mf::LogInfo("DBDataset") << "DBDataset: channel=" << fChannels.back() << "\n";
        }
      }
      else if (fColTypes[col] == "real") {
        double value = strtod(buf, 0);
        fData.push_back(value_type(value));
        //mf::LogInfo("DBDataset") << "DBDataset: row=" << row << ", column=" << col << ", value=" << fData.back()
        //	<< "\n";
        if (col == 0) {
          mf::LogError("DBDataset") << "First column has wrong type real."
                                    << "\n";
          throw cet::exception("DBDataset") << "First column has wrong type real.";
        }
      }
      else if (fColTypes[col] == "text") {
        fData.emplace_back(std::make_unique<std::string>(buf));
        //mf::LogInfo("DBDataset") << "DBDataset: row=" << row << ", column=" << col << ", value=" << fData.back()
        //	<< "\n";
        if (col == 0) {
          mf::LogError("DBDataset") << "First column has wrong type text."
                                    << "\n";
          throw cet::exception("DBDataset") << "First column has wrong type text.";
        }
      }
      else if (fColTypes[col] == "boolean") {
        long value = 0;
        std::string s = std::string(buf);
        if (s == "true" || s == "True" || s == "TRUE" || s == "1")
          value = 1;
        else if (s == "false" || s == "False" || s == "FALSE" || s == "0")
          value = 0;
        else {
          mf::LogError("DBDataset") << "Unknown string representation of boolean " << s << "\n";
          throw cet::exception("DBDataset")
            << "Unknown string representation of boolean " << s << "\n";
        }
        fData.push_back(value_type(value));
        //mf::LogInfo("DBDataset") << "DBDataset: row=" << row << ", column=" << col << ", value=" << fData.back()
        //	<< "\n";
        if (col == 0) {
          mf::LogError("DBDataset") << "First column has wrong type boolean."
                                    << "\n";
          throw cet::exception("DBDataset") << "First column has wrong type boolean.";
        }
      }
      else {
        mf::LogError("DBDataset") << "Unknown datatype = " << fColTypes[col] << "\n";
        throw cet::exception("DBDataset")
          << "Unknown datatype = " << fColTypes[col] << ": " << buf << "\n";
      }
    }
    releaseTuple(tup);
  }

  // Maybe release dataset memory.

  if (release) releaseDataset(dataset);
}

// SQLite initializing move constructor.

lariov::DBDataset::DBDataset(const IOVTimeStamp& begin_time,        // IOV begin time.
                             const IOVTimeStamp& end_time,          // IOV end time.
                             std::vector<std::string>&& col_names,  // Column names.
                             std::vector<std::string>&& col_types,  // Column types.
                             std::vector<DBChannelID_t>&& channels, // Channels.
                             std::vector<value_type>&& data)
  : // Calibration data.
  fBeginTime(begin_time)
  , fEndTime(end_time)
  , fColNames(std::move(col_names))
  , fColTypes(std::move(col_types))
  , fChannels(std::move(channels))
  , fData(std::move(data))
{}

// Get row number by channel number.
// Return -1 if not found.

int lariov::DBDataset::getRowNumber(DBChannelID_t ch) const
{
  int result = -1;

  // Do a binary search on channel numbers.

  int low = 0;
  int high = fChannels.size() - 1;
  while (high - low > 1) {
    int mid = (low + high) / 2;
    if (fChannels[mid] < ch)
      low = mid;
    else if (fChannels[mid] > ch)
      high = mid;
    else {

      // Found an exact match.  Break out of loop.

      result = mid;
      break;
    }
  }

  // If we fell out of loop without finding a match...

  if (result < 0) {
    if (fChannels[low] == ch)
      result = low;
    else if (fChannels[high] == ch)
      result = high;
  }

  // Done.

  return result;
}

// Get column number by column name.
// Return -1 if not found.

int lariov::DBDataset::getColNumber(const std::string& name) const
{
  int result = -1;

  // Scan column names.

  for (size_t i = 0; i < fColNames.size(); ++i) {
    if (fColNames[i] == name) {
      result = i;
      break;
    }
  }

  return result;
}
