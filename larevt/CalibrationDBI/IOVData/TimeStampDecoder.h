#ifndef TIMESTAMPDECODER_H
#define TIMESTAMPDECODER_H

#include "IOVTimeStamp.h"
#include "larevt/CalibrationDBI/Interface/CalibrationDBIFwd.h"

namespace lariov {

  class TimeStampDecoder {

  public:
    TimeStampDecoder() {}
    virtual ~TimeStampDecoder();

    static IOVTimeStamp DecodeTimeStamp(DBTimeStamp_t ts);
  };
}

#endif
