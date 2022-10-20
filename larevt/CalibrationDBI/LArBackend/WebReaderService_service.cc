#include "WebReaderService.h"

namespace lariov {

  WebReaderService::WebReaderService(fhicl::ParameterSet const& pset)
    : _server(pset.get<std::string>("WDA_SERVER"))
    , _port(pset.get<std::string>("WDA_PORT"))
    , _db(pset.get<std::string>("WDA_DBNAME"))
    , _timeout(pset.get<unsigned int>("WDA_TIMEOUT"))
  {}

  DEFINE_ART_SERVICE(WebReaderService)

}
