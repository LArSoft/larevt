/**
 * \file IOVTimeStamp.cxx
 *
 * \ingroup IOVData
 *
 * \brief Implementation for class IOVTimeStamp
 *
 * @author eberly@fnal.gov
 */

/** \addtogroup IOVData

    @{*/
#include "IOVTimeStamp.h"
#include "IOVDataConstants.h"
#include "IOVDataError.h"
#include <iomanip>
#include <limits>
#include <sstream>

namespace lariov {

  /**Create unique database timestamp of the form <fStamp>.<fSubStamp>,
     where fSubStamp is prepended with zeroes to ensure six digits
  */
  void IOVTimeStamp::CalcDBStamp()
  {
    if (fSubStamp > kMAX_SUBSTAMP_VALUE) {
      throw IOVDataError("SubStamp of an IOVTimeStamp cannot have more than six digits!");
    }
    std::stringstream stream;
    stream << fStamp << "." << std::setfill('0') << std::setw(kMAX_SUBSTAMP_LENGTH) << fSubStamp;
    fDBStamp = stream.str();
  }

  IOVTimeStamp IOVTimeStamp::GetFromString(const std::string& ts)
  {
    unsigned long stamp;
    std::string substamp_str;
    if (ts.find_first_of(".") == std::string::npos) {
      stamp = std::stoul(ts);
      substamp_str = "0";
    }
    else {
      stamp = std::stoul(ts.substr(0, ts.find_first_of(".")));
      substamp_str = ts.substr(ts.find_first_of(".") + 1);
    }

    if (substamp_str.length() > kMAX_SUBSTAMP_LENGTH) {
      throw IOVDataError("SubStamp of an IOVTimeStamp cannot have more than six digits!");
    }
    while (substamp_str.length() < kMAX_SUBSTAMP_LENGTH)
      substamp_str += "0";
    unsigned int substamp = std::stoi(substamp_str);

    return IOVTimeStamp(stamp, substamp);
  }

  IOVTimeStamp IOVTimeStamp::MinTimeStamp()
  {
    return IOVTimeStamp(0, 0);
  }

  IOVTimeStamp IOVTimeStamp::MaxTimeStamp()
  {
    return IOVTimeStamp(std::numeric_limits<unsigned long>::max(), kMAX_SUBSTAMP_VALUE);
  }

  ///implementation of assignment operator
  IOVTimeStamp& IOVTimeStamp::operator=(const IOVTimeStamp& ts)
  {
    if (this == &ts) return *this;
    fStamp = ts.Stamp();
    fSubStamp = ts.SubStamp();
    fDBStamp = ts.DBStamp();
    return *this;
  }

  ///implementation of operator<
  bool IOVTimeStamp::operator<(const IOVTimeStamp& ts) const
  {
    if (this->Stamp() < ts.Stamp())
      return true;
    else if (this->Stamp() == ts.Stamp() && this->SubStamp() < ts.SubStamp())
      return true;
    else
      return false;
  }

  ///implementation of equality operator
  bool IOVTimeStamp::operator==(const IOVTimeStamp& ts) const
  {
    if (fStamp == ts.Stamp() && fSubStamp == ts.SubStamp()) return true;
    return false;
  }

  ///remaining comparison operators implemented in terms of == and <
  bool IOVTimeStamp::operator!=(const IOVTimeStamp& ts) const
  {
    return !(*this == ts);
  }

  bool IOVTimeStamp::operator<=(const IOVTimeStamp& ts) const
  {
    if (*this < ts || *this == ts)
      return true;
    else
      return false;
  }

  bool IOVTimeStamp::operator>=(const IOVTimeStamp& ts) const
  {
    if (!(*this < ts))
      return true;
    else
      return false;
  }

  bool IOVTimeStamp::operator>(const IOVTimeStamp& ts) const
  {
    if (!(*this <= ts))
      return true;
    else
      return false;
  }
}
