#ifndef CALIBRATIONDBIFWD_H
#define CALIBRATIONDBIFWD_H

#include <cstdint>
#include <set>

namespace lariov {

  typedef std::uint64_t DBTimeStamp_t;
  typedef std::uint32_t DBChannelID_t;

  typedef std::set<DBChannelID_t> DBChannelSet_t;
}

#endif
