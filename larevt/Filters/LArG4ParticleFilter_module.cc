//Author: Dom Brailsford
//An ART filter that selects events which contain 'interesting' simb::MCParticles.  Interesting is user-defined.
//The filter parameters are FHICL configurable but can select based on, momentum, particle flavour, TPC trajectory length and whether the particle stops/starts in the TPC.
//The user can ask for events which contain multiple types of interesting particles, each with their own requirements.

//ART
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

//LArSoft
#include "larcore/Geometry/Geometry.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "nusimdata/SimulationBase/MCParticle.h"

// ROOT
#include "TLorentzVector.h"
#include "TVector3.h"

namespace filt {

  class LArG4ParticleFilter : public art::EDFilter {
  public:
    explicit LArG4ParticleFilter(fhicl::ParameterSet const& pset);
    virtual bool filter(art::Event& e);

  private:
    bool IsInterestingParticle(const art::Ptr<simb::MCParticle>& particle,
                               const unsigned int index) const;
    bool PDGCheck(const art::Ptr<simb::MCParticle>& particle, const unsigned int index) const;
    bool IsPrimaryCheck(const art::Ptr<simb::MCParticle>& particle, const unsigned int index) const;
    bool MinMomentumCheck(const art::Ptr<simb::MCParticle>& particle,
                          const unsigned int index) const;
    bool MaxMomentumCheck(const art::Ptr<simb::MCParticle>& particle,
                          const unsigned int index) const;
    bool StartInTPCCheck(const art::Ptr<simb::MCParticle>& particle,
                         const unsigned int index) const;
    bool StopInTPCCheck(const art::Ptr<simb::MCParticle>& particle, const unsigned int index) const;
    bool TPCTrajLengthCheck(const art::Ptr<simb::MCParticle>& particle,
                            const unsigned int index) const;

    double CalculateLength(const std::vector<TVector3>& position_segment) const;

    void VerifyDataMembers() const;
    void VerifyElementRequest(const unsigned int index) const;

    art::ServiceHandle<geo::Geometry const> fGeom;

    const std::string fLArG4ModuleLabel;
    const std::vector<int> fInterestingPDGs;
    const std::vector<int> fIsPrimary;
    const std::vector<double> fParticleMinMomentum;
    const std::vector<double> fParticleMaxMomentum;
    const std::vector<int> fStartInTPC;
    const std::vector<int> fStopInTPC;
    const std::vector<double> fParticleMinTPCLength;
    const bool fRequireAllInterestingParticles;

    std::vector<bool> fFoundInterestingParticles;
  };

  LArG4ParticleFilter::LArG4ParticleFilter::LArG4ParticleFilter(fhicl::ParameterSet const& pset)
    : EDFilter{pset}
    , fLArG4ModuleLabel(pset.get<std::string>("LArG4ModuleLabel"))
    , fInterestingPDGs(pset.get<std::vector<int>>("InterestingPDGs"))
    , fIsPrimary(pset.get<std::vector<int>>("IsPrimary"))
    , fParticleMinMomentum(pset.get<std::vector<double>>("ParticleMinMomentum"))
    , fParticleMaxMomentum(pset.get<std::vector<double>>("ParticleMaxMomentum"))
    , fStartInTPC(pset.get<std::vector<int>>("StartInTPC"))
    , fStopInTPC(pset.get<std::vector<int>>("StopInTPC"))
    , fParticleMinTPCLength(pset.get<std::vector<double>>("ParticleMinTPCLength"))
    , fRequireAllInterestingParticles(pset.get<bool>("RequireAllInterestingParticles"))
    , fFoundInterestingParticles(fInterestingPDGs.size(), false)
  {
    VerifyDataMembers();
  }

  bool LArG4ParticleFilter::filter(art::Event& e)
  {
    //Reset the found vector
    for (unsigned int i = 0; i < fFoundInterestingParticles.size(); i++) {
      fFoundInterestingParticles[i] = false;
    }

    //Get the vector of particles
    art::Handle<std::vector<simb::MCParticle>> particles;
    e.getByLabel(fLArG4ModuleLabel, particles);
    //Loop through the particles
    for (unsigned int part_i = 0; part_i < particles->size(); part_i++) {
      //Get the particle
      const art::Ptr<simb::MCParticle> particle(particles, part_i);
      //Loop over the list of particles we want and compare it with the particle we are looking it
      for (unsigned int interest_i = 0; interest_i < fInterestingPDGs.size(); interest_i++) {
        if (IsInterestingParticle(particle, interest_i)) {
          if (!fRequireAllInterestingParticles)
            return true; //If we only require at least one of the particles be found then we have already done our job
          fFoundInterestingParticles[interest_i] = true;
          bool foundThemAll = true;
          for (unsigned int found_i = 0; found_i < fFoundInterestingParticles.size(); found_i++) {
            if (fFoundInterestingParticles[found_i] == false) {
              foundThemAll = false;
              break;
            }
          }
          if (foundThemAll) { return true; }
        }
      }
    }
    //Assume that the event is not worth saving
    return false;
  }

  bool LArG4ParticleFilter::IsInterestingParticle(const art::Ptr<simb::MCParticle>& particle,
                                                  const unsigned int index) const
  {
    VerifyElementRequest(index);
    //Run the checks
    if (!PDGCheck(particle, index)) return false;
    if (!IsPrimaryCheck(particle, index)) return false;
    if (!MinMomentumCheck(particle, index)) return false;
    if (!MaxMomentumCheck(particle, index)) return false;
    if (!StartInTPCCheck(particle, index)) return false;
    if (!StopInTPCCheck(particle, index)) return false;
    if (!TPCTrajLengthCheck(particle, index)) return false;

    return true;
  }

  bool LArG4ParticleFilter::PDGCheck(const art::Ptr<simb::MCParticle>& particle,
                                     const unsigned int index) const
  {
    int pdg = fInterestingPDGs[index];
    if (pdg == 0) return true; //User does not care what the PDG is
    if (particle->PdgCode() != pdg) return false;
    return true;
  }

  bool LArG4ParticleFilter::IsPrimaryCheck(const art::Ptr<simb::MCParticle>& particle,
                                           const unsigned int index) const
  {
    const int isPrimaryCondition(fIsPrimary[index]);
    if (isPrimaryCondition == -1) return true;
    bool particlePrimaryStatus(particle->Mother() > 0 ? false : true);
    if (particlePrimaryStatus != isPrimaryCondition) return false;
    return true;
  }

  bool LArG4ParticleFilter::MinMomentumCheck(const art::Ptr<simb::MCParticle>& particle,
                                             const unsigned int index) const
  {
    if (fParticleMinMomentum[index] > 0 &&
        particle->Momentum(0).Vect().Mag() < fParticleMinMomentum[index])
      return false;
    return true;
  }

  bool LArG4ParticleFilter::MaxMomentumCheck(const art::Ptr<simb::MCParticle>& particle,
                                             const unsigned int index) const
  {
    if (fParticleMaxMomentum[index] > 0 &&
        particle->Momentum(0).Vect().Mag() > fParticleMaxMomentum[index])
      return false;
    return true;
  }

  bool LArG4ParticleFilter::StartInTPCCheck(const art::Ptr<simb::MCParticle>& particle,
                                            const unsigned int index) const
  {
    //Firstly check if we even care if the particle starts in the TPC or not
    int demand = fStartInTPC[index];
    if (demand == 0)
      return true; //We don't care if the particle starts in the TPC or not so pass the check
    //Get starting position of particle
    TLorentzVector starting_position_4vect = particle->Position(0);
    double starting_position[3];
    starting_position[0] = starting_position_4vect.X();
    starting_position[1] = starting_position_4vect.Y();
    starting_position[2] = starting_position_4vect.Z();

    geo::TPCID tpcid = fGeom->FindTPCAtPosition(starting_position);
    bool validtpc = tpcid.isValid;
    //Now we need to compare if we have a TPC that we started in with whether we wanted to start in a TPC at all
    if (validtpc) {
      //The particle DID start in a TPC.  Now, did we WANT this to happen
      if (demand == 1)
        return true; //We DID want this to happen
      else
        return false;
    }
    else {
      //The particle did NOT start in a TPC.  Did we WANT this to happen?
      if (demand == 2)
        return true; //We DID want this to happen
      else
        return false;
    }

    //Assume true by default
    return true;
  }

  bool LArG4ParticleFilter::StopInTPCCheck(const art::Ptr<simb::MCParticle>& particle,
                                           const unsigned int index) const
  {
    //Firstly check if we even care if the particle stops in the TPC or not
    int demand = fStopInTPC[index];
    if (demand == 0)
      return true; //We don't care if the particle stops in the TPC or not so pass the check
    //Get final position of particle
    TLorentzVector final_position_4vect =
      particle->Position(particle->NumberTrajectoryPoints() - 1);
    double final_position[3];
    final_position[0] = final_position_4vect.X();
    final_position[1] = final_position_4vect.Y();
    final_position[2] = final_position_4vect.Z();

    geo::TPCID tpcid = fGeom->FindTPCAtPosition(final_position);
    bool validtpc = tpcid.isValid;
    //Now we need to compare if we have a TPC that we stopped in with whether we wanted to stop in a TPC at all
    if (validtpc) {
      //The particle DID stop in a TPC.  Now, did we WANT this to happen
      if (demand == 1)
        return true; //We DID want this to happen
      else
        return false;
    }
    else {
      //The particle did NOT stop in a TPC.  Did we WANT this to happen?
      if (demand == 2)
        return true; //We DID want this to happen
      else
        return false;
    }

    //Assume true by default
    return true;
  }

  bool LArG4ParticleFilter::TPCTrajLengthCheck(const art::Ptr<simb::MCParticle>& particle,
                                               const unsigned int index) const
  {
    double min_traj_length = fParticleMinTPCLength[index];

    //Firstly, if we don't care about the TPC trajectory length then pass the check
    if (min_traj_length < 0) return true;

    //Now the hard bit
    //To do this, we need to collect the sequential particle positions which are contained in the TPC into segments.  The reason for doing this is that the particle may enter a TPC, leave it and enter it again and if this isn't taken into account, the length might be grossly overestimated
    //It is easiest to store the positions in a vector of vectors
    bool OK = false;
    std::vector<std::vector<TVector3>> position_segments;
    //We are also going to need an empty vector to store in the above vector
    std::vector<TVector3> position_segment;
    //Loop through the trajectory points
    for (unsigned int i = 0; i < particle->NumberTrajectoryPoints(); i++) {
      //Extract the current position of the particle
      double curr_pos[3];
      curr_pos[0] = particle->Position(i).X();
      curr_pos[1] = particle->Position(i).Y();
      curr_pos[2] = particle->Position(i).Z();
      geo::TPCID curr_tpcid = fGeom->FindTPCAtPosition(curr_pos);
      //There are a couple of things to check here.  If the particle is currently in the TPC, then we need to store that particular position.  If it is NOT in the TPC, then its either exited the TPC or has not yet entered.  If it has just exited, then the position_segment should have some positions stored in it, it which case we now need to store this segment.  If it has not yet entered the TPC, then we don't need to do anything
      //If it is currently in the TPC
      if (curr_tpcid.isValid) position_segment.push_back(particle->Position(i).Vect());
      //It has just exited the TPC
      else if (position_segment.size() > 0) {
        //Store the segment
        position_segments.push_back(position_segment);
        //Now reset the segment
        position_segment.clear();
      }
      //There is nothing to do because the particle has remained outside of the TPC
    }
    //We need to check once more if the position_segment vector has been filled
    if (position_segment.size() > 0) {
      position_segments.push_back(position_segment);
      position_segment.clear();
    }
    //Now lets check the length of each segment
    //Firstly, if we didn't store a segment then the particle fails the check
    if (position_segments.size() == 0) return false;
    //Now loop through the segments and check if they are above threshold
    for (unsigned int i = 0; i < position_segments.size(); i++) {
      double segment_length = CalculateLength(position_segments[i]);
      if (segment_length > min_traj_length) {
        //We found a track segment in the TPC which passes the length threshold so don't flag as bad
        OK = true;
        break;
      }
    }
    if (!OK) return false;

    return true;
  }

  double LArG4ParticleFilter::CalculateLength(const std::vector<TVector3>& position_segment) const
  {
    double length = 0;
    //Check how many points we have in the segment.  If it is one or less, there is nothing to calculate so return 0
    if (position_segment.size() <= 1) return length;

    //Now we need to compare every adjacent pair of positions to work out the length of this segment
    for (unsigned int i = 1; i < position_segment.size(); i++) {
      length += (position_segment[i] - position_segment[i - 1]).Mag();
    }

    return length;
  }

  void LArG4ParticleFilter::VerifyDataMembers() const
  {
    if (fInterestingPDGs.size() != fIsPrimary.size())
      throw art::Exception(art::errors::Configuration)
        << "Config error: InterestingPDGs and IsPrimary FCL vectors are different sizes\n";
    if (fInterestingPDGs.size() != fParticleMinMomentum.size())
      throw art::Exception(art::errors::Configuration)
        << "Config error: InterestingPDGs and ParticleMinMomentum FCL vectors are different "
           "sizes\n";
    if (fInterestingPDGs.size() != fParticleMaxMomentum.size())
      throw art::Exception(art::errors::Configuration)
        << "Config error: InterestingPDGs and ParticleMaxMomentum FCL vectors are different "
           "sizes\n";
    if (fInterestingPDGs.size() != fStartInTPC.size())
      throw art::Exception(art::errors::Configuration)
        << "Config error: InterestingPDGs and StartInTPC FCL vectors are different sizes\n";
    if (fInterestingPDGs.size() != fStopInTPC.size())
      throw art::Exception(art::errors::Configuration)
        << "Config error: InterestingPDGs and StopInTPC FCL vectors are different sizes\n";
    if (fInterestingPDGs.size() != fParticleMinTPCLength.size())
      throw art::Exception(art::errors::Configuration)
        << "Config error: InterestingPDGs and ParticleMinTPCLength FCL vectors are different "
           "sizes\n";

    for (unsigned int iIsPrimary = 0; iIsPrimary < fIsPrimary.size(); iIsPrimary++)
      if (!(-1 == fIsPrimary[iIsPrimary] || 0 == fIsPrimary[iIsPrimary] ||
            1 == fIsPrimary[iIsPrimary]))
        throw art::Exception(art::errors::Configuration)
          << "Config error: Element " << iIsPrimary << " of the IsPrimary vector equals "
          << fIsPrimary[iIsPrimary]
          << ".  Valid options are -1 (not set), 0 (not primary) or 1 (is primary)" << std::endl;

    for (unsigned int iStartInTPC = 0; iStartInTPC < fStartInTPC.size(); iStartInTPC++)
      if (!(0 == fStartInTPC[iStartInTPC] || 1 == fStartInTPC[iStartInTPC] ||
            2 == fStartInTPC[iStartInTPC]))
        throw art::Exception(art::errors::Configuration)
          << "Config error: Element " << iStartInTPC << " of the StartInTPC vector equals "
          << fStartInTPC[iStartInTPC]
          << ".  Valid options are 0 (not set), 1 (start in TPC) or 2 (do not start in TPC)"
          << std::endl;

    for (unsigned int iStopInTPC = 0; iStopInTPC < fStopInTPC.size(); iStopInTPC++)
      if (!(0 == fStopInTPC[iStopInTPC] || 1 == fStopInTPC[iStopInTPC] ||
            2 == fStopInTPC[iStopInTPC]))
        throw art::Exception(art::errors::Configuration)
          << "Config error: Element " << iStopInTPC << " of the StopInTPC vector equals "
          << fStopInTPC[iStopInTPC]
          << ".  Valid options are 0 (not set), 1 (stop in TPC) or 2 (do not stop in TPC)"
          << std::endl;

    return;
  }

  void LArG4ParticleFilter::VerifyElementRequest(const unsigned int index) const
  {
    if (index > fInterestingPDGs.size())
      throw art::Exception(art::errors::InvalidNumber)
        << "Bounds error:  Requested element " << index
        << " from InterestingPDGs vector which is size " << fInterestingPDGs.size() << "\n";
    if (index > fIsPrimary.size())
      throw art::Exception(art::errors::InvalidNumber)
        << "Bounds error:  Requested element " << index << " from IsPrimary vector which is size "
        << fIsPrimary.size() << "\n";
    if (index > fParticleMinMomentum.size())
      throw art::Exception(art::errors::InvalidNumber)
        << "Bounds error:  Requested element " << index
        << " from ParticleMinMomentum vector which is size " << fParticleMinMomentum.size() << "\n";
    if (index > fParticleMaxMomentum.size())
      throw art::Exception(art::errors::InvalidNumber)
        << "Bounds error:  Requested element " << index
        << " from ParticleMaxMomentum vector which is size " << fParticleMaxMomentum.size() << "\n";
    if (index > fStartInTPC.size())
      throw art::Exception(art::errors::InvalidNumber)
        << "Bounds error:  Requested element " << index << " from StartInTPC vector which is size "
        << fStartInTPC.size() << "\n";
    if (index > fStopInTPC.size())
      throw art::Exception(art::errors::InvalidNumber)
        << "Bounds error:  Requested element " << index << " from StopInTPC vector which is size "
        << fStopInTPC.size() << "\n";
    if (index > fParticleMinTPCLength.size())
      throw art::Exception(art::errors::InvalidNumber)
        << "Bounds error:  Requested element " << index
        << " from ParticleMinTPCLength vector which is size " << fParticleMinTPCLength.size()
        << "\n";
    return;
  }
  DEFINE_ART_MODULE(LArG4ParticleFilter)
}
