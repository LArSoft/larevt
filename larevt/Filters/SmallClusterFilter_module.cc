////////////////////////////////////////////////////////////////////////
//
// \file SmallClusterFilter_module.cc
//
// \author corey.adams@yale.edu
//
// \brief Filter out events and only keep those with a few hits.
/*
        This filter is meant to search for radioactive decay products in "empty" frames.

        It will run on microboone, I claim, though it's really meant for argoneut.

        The algorithm is quite simple, it just makes a cut on the total number of hits (all
        planes combined) and also a hit on the number of hits in each individual plane.  Both
        of those numbers are parameters from the .fcl file filters.fcl.
        -Corey
*/
//
//
///////////////////////////////////////////////////////////////////////

// include the proper bit of the framework
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "larcore/Geometry/Geometry.h"
#include "lardata/Utilities/GeometryUtilities.h"
#include "lardataobj/RecoBase/Hit.h"

namespace cluster {

  class SmallClusterFilter : public art::EDFilter {
  public:
    explicit SmallClusterFilter(fhicl::ParameterSet const& pset);

  private:
    bool filter(art::Event& evt) override;

    std::string fHitFinderModuleLabel;        ///< label of module making hits
    std::vector<std::size_t> fMaxHitsByPlane; ///< maximum hits on each plane
    std::size_t fMaxTotalHits;                ///< maximum number of hits allowed
    std::size_t fNPlanes;                     ///< number of planes
  };

}

cluster::SmallClusterFilter::SmallClusterFilter(fhicl::ParameterSet const& pset)
  : EDFilter{pset}
  , fHitFinderModuleLabel{pset.get<std::string>("HitFinderModuleLabel")}
  , fMaxHitsByPlane{pset.get<std::vector<std::size_t>>("MaxHitsByPlane")}
  , fMaxTotalHits{pset.get<std::size_t>("MaxTotalHits")}
  , fNPlanes{art::ServiceHandle<geo::Geometry const>()->Nplanes()}
{
  if (size(fMaxHitsByPlane) != fNPlanes) {
    throw art::Exception{art::errors::Configuration}
      << "Mismatch in number of planes specified in 'MaxHitsByPlane' (" << size(fMaxHitsByPlane)
      << ") and the number of planes (" << fNPlanes << ")\n";
  }
}

bool cluster::SmallClusterFilter::filter(art::Event& evt)
{
  auto const& hits = *evt.getValidHandle<std::vector<recob::Hit>>(fHitFinderModuleLabel);

  mf::LogVerbatim("SmallClusterFilter")
    << " ++++ Hitsreceived received " << size(hits) << " +++++ ";

  if (empty(hits)) {
    mf::LogWarning("SmallClusterFilter") << " no hits received! exiting ";
    return false;
  }

  if (size(hits) > fMaxTotalHits) {
    mf::LogWarning("SmallClusterFinder") << "Not an empty event, exiting.";
    return false;
  }

  bool collFound = false;

  std::map<unsigned int, std::size_t> hitsPerPlane;

  for (auto const& hit : hits) {

    // Skip crazy hits:
    if (hit.Integral() > 500) continue;

    ++hitsPerPlane[hit.WireID().Plane];

    if (hit.SignalType() == geo::kCollection) collFound = true;
  }

  for (unsigned int i = 0; i < fNPlanes; i++) {
    if (hitsPerPlane[i] > fMaxHitsByPlane[i]) return false;
  }

  //check that there is at least 1 hit on collection:
  return collFound;
}

DEFINE_ART_MODULE(cluster::SmallClusterFilter)
