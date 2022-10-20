#ifndef IOVDATA_CONSTANTS_H
#define IOVDATA_CONSTANTS_H

#include <math.h>
#include <string>

namespace lariov {

  const std::string kTREE_PREFIX = "iov";

  const unsigned short kMAX_SUBSTAMP_LENGTH = 6;
  const unsigned int kMAX_SUBSTAMP_VALUE = ((unsigned int)pow(10, kMAX_SUBSTAMP_LENGTH)) - 1;

  namespace DataSource {
    enum ds { Database, File, Default };
  }
}
#endif
