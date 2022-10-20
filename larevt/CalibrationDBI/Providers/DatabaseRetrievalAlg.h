/**
 * \file DatabaseRetrievalAlg.h
 *
 * \ingroup WebDBI
 *
 * \brief Class def header for a class DatabaseRetrievalAlg
 *
 * @author kterao, eberly@slac.stanford.edu
 */

/** \addtogroup WebDBI

    @{*/
#ifndef DATABASERETRIEVALALG_H
#define DATABASERETRIEVALALG_H

#include "DBFolder.h"
#include <memory>

namespace fhicl {
  class ParameterSet;
}

namespace lariov {

  class IOVTimeStamp;

  /**
     \class DatabaseRetrievalAlg
     User defined class DatabaseRetrievalAlg ... these comments are used to generate
     doxygen documentation!
  */
  class DatabaseRetrievalAlg {

  public:
    /// Constructors
    DatabaseRetrievalAlg(const std::string& foldername,
                         const std::string& url,
                         const std::string& url2 = "",
                         const std::string& tag = "",
                         bool usesqlite = false,
                         bool testmode = false)
      : fFolder(new DBFolder(foldername, url, url2, tag, usesqlite, testmode))
    {}

    DatabaseRetrievalAlg(fhicl::ParameterSet const& p) { this->Reconfigure(p); }

    /// Default destructor
    virtual ~DatabaseRetrievalAlg() {}

    /// Configure using fhicl::ParameterSet
    virtual void Reconfigure(fhicl::ParameterSet const& p);

    /// Return true if fFolder is successfully updated
    bool UpdateFolder(DBTimeStamp_t ts) { return fFolder->UpdateData(ts); }

    /// Get connection information
    const std::string& URL() const { return fFolder->URL(); }
    const std::string& FolderName() const { return fFolder->FolderName(); }
    const std::string& Tag() const { return fFolder->Tag(); }

    /// Get Timestamp information
    const IOVTimeStamp& Begin() const { return fFolder->CachedStart(); }
    const IOVTimeStamp& End() const { return fFolder->CachedEnd(); }

  protected:
    std::unique_ptr<DBFolder> fFolder;
  };
}

#endif
/** @} */ // end of doxygen group
