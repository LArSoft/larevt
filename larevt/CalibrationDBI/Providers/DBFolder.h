#ifndef DBFOLDER_H
#define DBFOLDER_H

#include "larevt/CalibrationDBI/IOVData/IOVTimeStamp.h"
#include "larevt/CalibrationDBI/Interface/CalibrationDBIFwd.h"
#include "larevt/CalibrationDBI/Providers/DBDataset.h"

#include "fhiclcpp/fwd.h"

#include <string>
#include <vector>

namespace lariov {

  using Dataset = void*;
  using Tuple = void*;

  class DBFolder {
  public:
    DBFolder(const std::string& name,
             const std::string& url,
             const std::string& url2,
             const std::string& tag = "",
             bool useqlite = false,
             bool testmode = false);
    explicit DBFolder(fhicl::ParameterSet const& p);
    const std::string& URL() const { return fURL; }
    const std::string& FolderName() const { return fFolderName; }
    const std::string& Tag() const { return fTag; }

    DBDataset GetDataset(DBTimeStamp_t raw_time) const;
    void GetSQLiteData(int t, DBDataset& data) const;

    void DumpDataset(const DBDataset& data) const;

    bool CompareDataset(const DBDataset& data1, const DBDataset& data2) const;

  private:
    std::string fURL;
    std::string fURL2;
    std::string fFolderName;
    std::string fTag;
    bool fUseSQLite;
    bool fTestMode;
    std::string fSQLitePath;
    int fMaximumTimeout;
  };
}

#endif
