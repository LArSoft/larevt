/**
 * @file   SimpleChannelStatusTest_module.cc
 * @brief  Integration test for SimpleChannelStatusService
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 25th, 2014
 */

// C/C++ standard library

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// LArSoft includes
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"

namespace art {
  class Event;
} // art::Event declaration

///tracking algorithms
namespace lariov {
  /**
   * @brief Tests an instanciation of the ChannelStatusService
   *
   * Configuration parameters
   * =========================
   *
   * - **TestBadChannels** (list of integers, default: empty): test singularly
   *   these channels and verify they are bad
   * - **TestGoodChannels** (list of integers, default: empty): test singularly
   *   these channels and verify they are not bad nor noisy
   * - **TestNoisyChannels** (list of integers, default: empty): test singularly
   *   these channels and verify they are noisy
   */
  class SimpleChannelStatusTest : public art::EDAnalyzer {
  public:
    explicit SimpleChannelStatusTest(fhicl::ParameterSet const& pset);

    virtual void analyze(art::Event const&) override {}
    virtual void beginRun(art::Run const& run) override;

  private:
    std::vector<unsigned int> KnownGoodChannels;
    std::vector<unsigned int> KnownNoisyChannels;
    std::vector<unsigned int> KnownBadChannels;

    template <typename Obj>
    unsigned int testObject(Obj const* pChStatus) const;

    static const std::vector<unsigned int> EmptyVect; ///< for initializations
  };                                                  // class SimpleChannelStatusTest

} // namespace lariov

//------------------------------------------------------------------------------

namespace lariov {

  const std::vector<unsigned int> SimpleChannelStatusTest::EmptyVect;

  //......................................................................
  SimpleChannelStatusTest::SimpleChannelStatusTest(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
    , KnownGoodChannels(pset.get<std::vector<unsigned int>>("TestGoodChannels", EmptyVect))
    , KnownNoisyChannels(pset.get<std::vector<unsigned int>>("TestNoisyChannels", EmptyVect))
    , KnownBadChannels(pset.get<std::vector<unsigned int>>("TestBadChannels", EmptyVect))
  {
    mf::LogInfo log("SimpleChannelStatusTest");

    log << "Channels known as good:  " << KnownGoodChannels.size();
    if (!KnownGoodChannels.empty()) {
      log << " (";
      for (unsigned int chId : KnownGoodChannels)
        log << " " << chId;
      log << ")";
    } // if known good channels

    log << "\nChannels known as noisy: " << KnownNoisyChannels.size();
    if (!KnownNoisyChannels.empty()) {
      log << " (";
      for (unsigned int chId : KnownNoisyChannels)
        log << " " << chId;
      log << ")";
    } // if known noisy channels

    log << "\nChannels known as bad:   " << KnownBadChannels.size();
    if (!KnownBadChannels.empty()) {
      log << " (";
      for (unsigned int chId : KnownBadChannels)
        log << " " << chId;
      log << ")";
    } // if known bad channels

  } // SimpleChannelStatusTest::SimpleChannelStatusTest()

  //......................................................................
  void SimpleChannelStatusTest::beginRun(art::Run const& run)
  {

    mf::LogInfo("SimpleChannelStatusTest") << "New run: " << run.run();

    unsigned int nErrors = 0;

    //---
    mf::LogVerbatim("SimpleChannelStatusTest") << "\nTesting service interface...";
    art::ServiceHandle<lariov::ChannelStatusService const> StatusSrvHandle;
    /* // since the service does not share the interface of the provider,
     // this test can't be
    const lariov::ChannelStatusService* pStatusSrv = &*StatusSrvHandle;
    nErrors = testObject(pStatusSrv);
    if (nErrors > 0) {
      throw art::Exception(art::errors::LogicError)
        << nErrors << " errors while testing ChannelStatusService!";
    } // if errors
  */

    //---
    mf::LogVerbatim("SimpleChannelStatusTest") << "\nTesting base interface...";
    lariov::ChannelStatusProvider const* pChStatus = StatusSrvHandle->GetProviderPtr();
    nErrors = testObject(pChStatus);
    if (nErrors > 0) {
      throw art::Exception(art::errors::LogicError)
        << nErrors << " errors while testing ChannelStatusProvider!";
    } // if errors

  } // SimpleChannelStatusTest::beginRun()

  //......................................................................
  template <typename Obj>
  unsigned int SimpleChannelStatusTest::testObject(Obj const* pChStatus) const
  {
    // 1. print all the channels marked non-good
    {
      mf::LogInfo log("SimpleChannelStatusTest");

      // this is a copy of the list;
      // to avoid creating temporary objects, check channels one by one
      auto BadChannels = pChStatus->BadChannels();
      log << "\nChannels marked as bad:   " << BadChannels.size();
      if (!BadChannels.empty()) {
        log << " (";
        for (unsigned int chId : BadChannels)
          log << " " << chId;
        log << ")";
      } // if bad channels

      auto NoisyChannels = pChStatus->NoisyChannels();
      log << "\nChannels marked as noisy: " << NoisyChannels.size();
      if (!NoisyChannels.empty()) {
        log << " (";
        for (unsigned int chId : NoisyChannels)
          log << " " << chId;
        log << ")";
      } // if noisy channels
    }   // print test

    // 2. test the channels as in the configuration
    unsigned int nErrors = 0;
    for (const auto chId : KnownBadChannels) {
      if (!pChStatus->IsBad(chId)) {
        mf::LogError("SimpleChannelStatusTest")
          << "channel #" << chId << " is not bad as it should";
        ++nErrors;
      }
    } // for knwon bad channels

    for (const auto chId : KnownNoisyChannels) {
      if (!pChStatus->IsNoisy(chId)) {
        mf::LogError("SimpleChannelStatusTest")
          << "channel #" << chId << " is not noisy as it should";
        ++nErrors;
      }
    } // for knwon noisy channels

    for (const auto chId : KnownGoodChannels) {
      if (pChStatus->IsBad(chId)) {
        mf::LogError("SimpleChannelStatusTest")
          << "channel #" << chId << " is bad, while it should not";
        ++nErrors;
      }
      if (pChStatus->IsNoisy(chId)) {
        mf::LogError("SimpleChannelStatusTest")
          << "channel #" << chId << " is noisy, while it should not";
        ++nErrors;
      }
    } // for knwon good channels

    return nErrors;
  } // SimpleChannelStatusTest::testObject()

  DEFINE_ART_MODULE(SimpleChannelStatusTest)

} // namespace lariov
