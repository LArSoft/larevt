/**
 * \file IOVTimeStamp.h
 *
 * \ingroup IOVData
 *
 * \brief Class def header for a class IOVTimeStamp
 *
 * @author eberly@fnal.gov
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_IOVTIMESTAMP_H
#define IOVDATA_IOVTIMESTAMP_H

#include <string>

namespace lariov {
  /**
     \class IOVTimeStamp
  */

  class IOVTimeStamp {

  public:
    ///Constructor
    IOVTimeStamp(unsigned long stamp, unsigned int substamp = 0)
      : fStamp(stamp), fSubStamp(substamp)
    {
      this->CalcDBStamp();
    }

    ///Default destructor
    virtual ~IOVTimeStamp() {}

    unsigned long Stamp() const { return fStamp; }
    unsigned long SubStamp() const { return fSubStamp; }
    const std::string& DBStamp() const { return fDBStamp; }

    void SetStamp(unsigned long stamp, unsigned int substamp = 0)
    {
      fStamp = stamp;
      fSubStamp = substamp;
      this->CalcDBStamp();
    }

    /**
        This function combines the stamp and substamp into a unique string to be used
	as a database timestamp.
      */
    void CalcDBStamp();

    static IOVTimeStamp GetFromString(const std::string& ts);
    static IOVTimeStamp MinTimeStamp();
    static IOVTimeStamp MaxTimeStamp();

    ///comparison operators
    bool operator<(const IOVTimeStamp& ts) const;
    bool operator<=(const IOVTimeStamp& ts) const;
    bool operator>=(const IOVTimeStamp& ts) const;
    bool operator>(const IOVTimeStamp& ts) const;

    bool operator==(const IOVTimeStamp& ts) const;
    bool operator!=(const IOVTimeStamp& ts) const;

    ///assignment operator
    IOVTimeStamp& operator=(const IOVTimeStamp& ts);

  protected:
    unsigned long fStamp;
    unsigned int fSubStamp;

    std::string fDBStamp;
  };
}
#endif
