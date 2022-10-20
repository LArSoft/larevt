/**
 * \file WebError.h
 *
 * \ingroup WebDBI
 *
 * \brief Collection of exception classes for WebDBI
 *
 * @author kterao
 */

/** \addtogroup WebDBI

    @{*/
#ifndef WEBDB_WEBERROR_H
#define WEBDB_WEBERROR_H

#include <exception>
#include <iostream>

namespace lariov {

  /**
     \class WebError
  */
  class WebError : public std::exception {

  public:
    WebError(std::string msg = "") : std::exception()
    {
      _msg = "\033[93m";
      _msg += msg;
      _msg += "\033[00m";
    }

    virtual ~WebError() throw(){};
    virtual const char* what() const throw() { return _msg.c_str(); }

  private:
    std::string _msg;
  };

}

#endif
/** @} */ // end of doxygen group
