/**
 * \file IOVDataError.h
 *
 * \ingroup IOVData
 *
 * \brief Collection of exception classes for IOVData
 *
 * @author kterao
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_IOVDATAERROR_H
#define IOVDATA_IOVDATAERROR_H

#include <exception>
#include <iostream>

namespace lariov {

  /**
     \class IOVDataError
  */
  class IOVDataError : public std::exception {

  public:
    IOVDataError(std::string msg = "") : std::exception()
    {
      fMsg = "\033[93m";
      fMsg += msg;
      fMsg += "\033[00m";
    }

    virtual ~IOVDataError() throw(){};
    virtual const char* what() const throw() { return fMsg.c_str(); }

  private:
    std::string fMsg;
  };

}

#endif
/** @} */ // end of doxygen group
