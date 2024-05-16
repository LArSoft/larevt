/**
 * \file ChannelStatus.h
 *
 * \ingroup IOVData
 *
 * \brief Class def header for a class ChannelStatus
 *
 * @author eberly@slac.stanford.edu
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_CHANNELSTATUS_H
#define IOVDATA_CHANNELSTATUS_H 1

#include "ChData.h"

namespace lariov {

  enum chStatus {
    kDISCONNECTED = 0,
    kDEAD = 1,
    kLOWNOISE = 2,
    kNOISY = 3,
    kGOOD = 4,
    kUNKNOWN = 5,
    kBAD = 6
  };

  /**
     \class ChannelStatus
  */
  class ChannelStatus : public ChData {

  public:
    explicit ChannelStatus(unsigned int const ch, int const status)
      : ChData(ch), fStatus{GetStatusFromInt(status)}
    {}

    bool IsDead() const { return fStatus == kDEAD; }
    bool IsLowNoise() const { return fStatus == kLOWNOISE; }
    bool IsNoisy() const { return fStatus == kNOISY; }
    bool IsPresent() const { return fStatus != kDISCONNECTED; }
    bool IsGood() const { return fStatus == kGOOD; }
    chStatus Status() const { return fStatus; }

    static chStatus GetStatusFromInt(int status)
    {
      switch (status) {
      case kDISCONNECTED: return kDISCONNECTED;
      case kDEAD: return kDEAD;
      case kLOWNOISE: return kLOWNOISE;
      case kNOISY: return kNOISY;
      case kGOOD: return kGOOD;
      default: return kUNKNOWN;
      };
    }

  private:
    chStatus fStatus;
  }; //end class
} //end namespace lariov

#endif
/** @} */ //end doxygen group
