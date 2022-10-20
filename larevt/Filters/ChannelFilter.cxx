///////////////////////////////////////////////////////
//
// ChannelFilter Class
//
// This class has been obsoleted and is now a deprecated interface for
// ChannelStatusService.
//
// Please update your code to use the service directly.
//
//
// Original class: pagebri3@msu.edu
//
///////////////////////////////////////////////////////

// Our header
#include "larevt/Filters/ChannelFilter.h"

// Framework libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Utilities/Exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// LArSoft libraries
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"

//
// The following construct is rarely used:
// I have a C++ reference as a member class, that needs to be constructed in
// the initialization list. The construction could throw an exception,
// and I want to do some special handling if that happens.
// The way to do that is to use a special syntax commonly called
// "function try block".
//
// Note that the "special handling" is not that special as it ends rethrowing
// the exception. The purpose here is just to provide a bit of additional
// information about why the failure could have occurred, since art messages
// often don't have enough.
//
filter::ChannelFilter::ChannelFilter() try
  : provider(art::ServiceHandle<lariov::ChannelStatusService const>()->GetProvider()) {

  MF_LOG_ERROR("ChannelFilter") << "ChannelFilter is now deprecated."
                                   " Replace it with ChannelStatusService";

} // function try
catch (art::Exception& e) { // automatic rethrow happens at end of block
  if (e.categoryCode() == art::errors::ServiceNotFound) {
    MF_LOG_SYSTEM("ChannelFilter")
      << "Failed to obtain an instance of ChannelStatusService service;"
         " you should update your configuration, *and* update the code using"
         " ChannelFilter, that is deprecated."
         " An example are in ChannelFilter class documentation";
  }
} // filter::ChannelFilter::ChannelFilter() (function catch)

///////////////////////////////////////////////////////
bool filter::ChannelFilter::BadChannel(uint32_t channel) const
{
  return provider.IsBad(channel);
}

///////////////////////////////////////////////////////
bool filter::ChannelFilter::NoisyChannel(uint32_t channel) const
{
  return provider.IsNoisy(channel);
}

///////////////////////////////////////////////////////
std::set<uint32_t> filter::ChannelFilter::SetOfBadChannels() const
{
  return provider.BadChannels();
}

///////////////////////////////////////////////////////
std::set<uint32_t> filter::ChannelFilter::SetOfNoisyChannels() const
{
  return provider.NoisyChannels();
}

///////////////////////////////////////////////////////
filter::ChannelFilter::ChannelStatus filter::ChannelFilter::GetChannelStatus(uint32_t channel) const
{

  if (provider.IsGood(channel))
    return GOOD;
  else if (!provider.IsPresent(channel))
    return NOTPHYSICAL;
  else if (provider.IsBad(channel))
    return DEAD;
  else if (provider.IsNoisy(channel))
    return NOISY;
  else
    return DEAD; //assume all other status are equivalent to DEAD
}
