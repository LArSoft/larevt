/**
 * \file Snapshot.h
 *
 * \ingroup Snapshot
 *
 * \brief Class def header for a class Snapshot
 *
 * @author kterao
 */

/** \addtogroup Snapshot

    @{*/
#ifndef IOVDATA_SNAPSHOT_H
#define IOVDATA_SNAPSHOT_H

#include "ChData.h"
#include "IOVDataConstants.h"
#include "IOVDataError.h"
#include "IOVTimeStamp.h"
#include <algorithm>
#include <sstream>
#include <vector>

namespace lariov {

  /**
     \class Snapshot
  */
  template <class T>
  class Snapshot {

  public:
    /// Default constructor
    Snapshot() : fStart(0, 0), fEnd(0, 0) {}

    /// Default destructor
    ~Snapshot() {}

    void Clear();

    const IOVTimeStamp& Start() const { return fStart; }
    const IOVTimeStamp& End() const { return fEnd; }
    void SetIoV(const IOVTimeStamp& start, const IOVTimeStamp& end);

    bool IsValid(const IOVTimeStamp& ts) const;

    size_t NChannels() const { return fData.size(); }

    const std::vector<T>& Data() const { return fData; }

    /// Only included with class if T has base class ChData
    template <class U = T,
              typename std::enable_if<std::is_base_of<ChData, U>::value, int>::type = 0>
    bool HasChannel(unsigned int ch) const
    {

      typename std::vector<T>::const_iterator it = std::lower_bound(fData.begin(), fData.end(), ch);
      if (it == fData.end() || it->Channel() != ch) { return false; }
      else
        return true;
    }

    template <class U = T,
              typename std::enable_if<std::is_base_of<ChData, U>::value, int>::type = 0>
    const T& GetRow(unsigned int ch) const
    {

      typename std::vector<T>::const_iterator it = std::lower_bound(fData.begin(), fData.end(), ch);

      if (it == fData.end() || it->Channel() != ch) {
        std::string msg("Channel not found: ");
        msg += std::to_string(ch);
        throw IOVDataError(msg);
      }

      return *it;
    }

    template <class U = T,
              typename std::enable_if<std::is_base_of<ChData, U>::value, int>::type = 0>
    void AddOrReplaceRow(const T& data)
    {
      typename std::vector<T>::iterator it =
        std::lower_bound(fData.begin(), fData.end(), data.Channel());
      if (it == fData.end() || data.Channel() != it->Channel()) {
        bool sort = (!(fData.empty()) && data < fData.back());
        fData.push_back(data);
        if (sort) std::sort(fData.begin(), fData.end());
      }
      else {
        *it = data;
      }
    }

  private:
    IOVTimeStamp fStart;
    IOVTimeStamp fEnd;
    std::vector<T> fData;
  };

  //=============================================
  // Class implementation
  //=============================================
  template <class T>
  void Snapshot<T>::Clear()
  {
    fData.clear();
    fStart = fEnd = IOVTimeStamp::MaxTimeStamp();
    fStart.SetStamp(fStart.Stamp() - 1, fStart.SubStamp());
  }

  template <class T>
  void Snapshot<T>::SetIoV(const IOVTimeStamp& start, const IOVTimeStamp& end)
  {
    if (start >= end) {
      throw IOVDataError("Called Snapshot::SetIoV with start timestamp >= end timestamp!");
    }

    fStart = start;
    fEnd = end;
  }

  template <class T>
  bool Snapshot<T>::IsValid(const IOVTimeStamp& ts) const
  {
    return (ts >= fStart && ts < fEnd);
  }

} //end namespace lariov
#endif
/** @} */ // end of doxygen group
