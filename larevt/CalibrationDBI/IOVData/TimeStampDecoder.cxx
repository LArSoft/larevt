#include "TimeStampDecoder.h"
#include "IOVDataConstants.h"
#include "IOVDataError.h"
#include <string>

namespace lariov {

  //Do NOT change the following code without very good reason!
  //MicroBooNE and other experiments depend on it!
  IOVTimeStamp TimeStampDecoder::DecodeTimeStamp(DBTimeStamp_t ts)
  {

    std::string time = std::to_string(ts);

    //microboone stores timestamp as ns from epoch, so there should be 19 digits.
    if (time.length() == 19) {
      //make timestamp conform to database precision
      time = time.substr(0, 10 + kMAX_SUBSTAMP_LENGTH);

      //insert decimal point
      time.insert(10, ".");

      //finish construction
      IOVTimeStamp tmp = IOVTimeStamp::GetFromString(time);
      return tmp;
    }
    else if (time.length() < kMAX_SUBSTAMP_LENGTH && ts != 0) {
      IOVTimeStamp tmp = IOVTimeStamp::GetFromString(time);
      return tmp;
    }
    else {
      std::string msg = "TimeStampDecoder: I do not know how to convert this timestamp: " + time;
      throw IOVDataError(msg);
    }
  }
} //end namespace lariov
