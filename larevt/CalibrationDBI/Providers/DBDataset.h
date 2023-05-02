#ifndef DBDATASET_H
#define DBDATASET_H
//=================================================================================
//
// Name: DBDataset.h
//
// Purpose: Header for class DBDataset.
//          This class represents data extracted from the opaque wda struct Dataset,
//          which struct represents the result of a calibration database query on a
//          particular database table in an IOV database.  Data is extracted and
//          copied from the Dataset struct using the wda api.
//
//          Database data are essentially a rectangular array of values, indexed by
//          (row, column).  Accessors are provided to access data as string, long,
//          or double.
//
//          Rows are labeled by channel number.  Columns are labeled by name and type.
//
//          Binary values, which are stored as strings by wda, are stored as
//          std::variant<long, double, unique_ptr<std::string> > > in this class.
//
//          Columns are labeled by column name and type.
//
// Data members:
//
// fBeginTime - IOV begin validity time.
// fEndTime   - IOV end validity time.
// fColNames  - Names of columns.
// fColTypes  - Data types of columns.
// fChannels  - Channel numbers (indexed by row number).
// fData      - Calibration data.
//
// Normally, the first element of each row is an integer channel number.
// Furthermore, it can be assumed that rows are ordered by increasing channel number.
//
// Calibration data is contained in data member fData, which is a rectangular array
// of strings of dimension # channels x # cols.  For efficiency, calibration data are
// stored using a single std::vector, which array is allocated once at
// construction or update time.  Elements are accessed columnwise as follows.
//
// value = fData[ncols*row + column]
//
// Or use the provided accessors.
//
// Nested class DBRow provides access to data from a single database row.
//
// Created: 26-Oct-2020 - H. Greenlee
//
//=================================================================================

#include "larevt/CalibrationDBI/IOVData/IOVTimeStamp.h"
#include "larevt/CalibrationDBI/Interface/CalibrationDBIFwd.h"
#include "larevt/CalibrationDBI/Providers/WebError.h"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace lariov {
  class DBDataset {
  public:
    using value_type = std::variant<long, double, std::unique_ptr<std::string>>;

    // Nested class representing data from one row.

    class DBRow {
    public:
      DBRow() : fData(nullptr) {}
      explicit DBRow(const value_type* s) : fData(s) {}

      bool isValid() const { return fData != nullptr; }
      const value_type& getData(size_t col) const { return fData[col]; }
      const std::string& getStringData(size_t col) const
      {
        return *std::get<std::unique_ptr<std::string>>(fData[col]);
      }
      long getLongData(size_t col) const { return std::get<long>(fData[col]); }
      double getDoubleData(size_t col) const { return std::get<double>(fData[col]); }

    private:
      const value_type* fData; // Borrowed referenced from enclosing class.
    };

    // Back to main class.

  public:
    DBDataset();

    // Initializing constructor based on libwda struct.

    DBDataset(void* dataset, bool release = false);

    // Initializing move constructor.
    // This constructor is used to initialize sqlite data.

    DBDataset(const IOVTimeStamp& begin_time,        // IOV begin time.
              const IOVTimeStamp& end_time,          // IOV end time.
              std::vector<std::string>&& col_names,  // Column names.
              std::vector<std::string>&& col_types,  // Column types.
              std::vector<DBChannelID_t>&& channels, // Channels.
              std::vector<value_type>&& data);       // Calibration data (length nchan*ncol).

    // Simple accessors.

    const IOVTimeStamp& beginTime() const { return fBeginTime; }
    const IOVTimeStamp& endTime() const { return fEndTime; }
    size_t nrows() const { return fChannels.size(); }
    size_t ncols() const { return fColNames.size(); }
    const std::vector<std::string>& colNames() const { return fColNames; }
    const std::vector<std::string>& colTypes() const { return fColTypes; }
    const std::vector<DBChannelID_t>& channels() const { return fChannels; }
    const std::vector<value_type>& data() const { return fData; }

    // Interpret row and column as specified type

    bool GetDataAsBool(DBChannelID_t channel, const std::string& name) const;
    long GetDataAsLong(DBChannelID_t channel, const std::string& name) const;
    float GetDataAsFloat(DBChannelID_t channel,
                         const std::string& name) const; // Truncates from double to float
    double GetDataAsDouble(DBChannelID_t channel, const std::string& name) const;
    std::string GetDataAsString(DBChannelID_t channel, const std::string& name) const;

    // Access one row.

    DBRow getRow(size_t row) const { return DBRow(&fData[ncols() * row]); }

    // SS: Can move the implementation to its CXX file.
    DBRow getRowForChannel(DBChannelID_t channel) const
    {
      auto const row = getRowNumber(channel);
      if (row < 0) {
        std::string msg = "Channel " + std::to_string(channel) + " is not found in database!";
        throw WebError(msg);
      }
      return getRow(row);
    }

    bool IsValid(const IOVTimeStamp& time) const { return time >= fBeginTime && time < fEndTime; }

  private:
    IOVTimeStamp fBeginTime;              // IOV begin time.
    IOVTimeStamp fEndTime;                // IOV end time.
    std::vector<std::string> fColNames;   // Column names.
    std::vector<std::string> fColTypes;   // Column types.
    std::vector<DBChannelID_t> fChannels; // Channels.
    std::vector<value_type> fData;        // Calibration data (length nchan*ncols).
    // SS: from 12/18/20: we start here next year.
    // and fix binary search in DBDataSet.cxx
    int getRowNumber(DBChannelID_t ch) const;
    size_t getColNumber(const std::string& name) const;
  };
}

#endif
