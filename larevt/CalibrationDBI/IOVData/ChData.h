/**
 * \file ChData.h
 *
 * \ingroup IOVData
 *
 * \brief Class def header for a class ChData
 *
 * @author kterao
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_CHDATA_H
#define IOVDATA_CHDATA_H

#include <functional>

namespace lariov {
  /**
     \class ChData
  */
  class ChData {
  public:
    explicit ChData(unsigned int ch) : fChannel(ch) {}
    unsigned int Channel() const { return fChannel; }
    bool operator<(unsigned int rhs) const { return fChannel < rhs; }
    bool operator<(const ChData& ch) const { return fChannel < ch.Channel(); }

  private:
    unsigned int fChannel;
  };
}

namespace std {
  template <>
  class less<lariov::ChData*> {
  public:
    bool operator()(const lariov::ChData* lhs, const lariov::ChData* rhs) const
    {
      return *lhs < *rhs;
    }
  };
}

#endif
/** @} */ // end of doxygen group
