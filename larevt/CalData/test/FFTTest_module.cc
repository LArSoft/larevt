//
// Name:  FFTTest.h
//
// Purpose: FFTTest module.  Test convolution/deconvolution.
//
// Created:  29-Aug-2011  H. Greenlee

#include <algorithm>
#include <iostream>

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art_root_io/TFileService.h"
#include "cetlib/search_path.h"

#include "lardata/Utilities/LArFFT.h"

#include "TComplex.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

// Local functions.

namespace {

  // Fill vector from histogram (ignore underflow/overflow bins).

  void hist_to_vector(const TH1D* h, std::vector<double>& v)
  {
    assert(h != 0);
    int n = h->GetNbinsX();
    v.resize(n);
    for (int i = 0; i < n; ++i)
      v[i] = h->GetBinContent(i + 1);
  }

  // Fill histogram from vector (set underflow/overflow bins to zero).

  void vector_to_hist(const std::vector<double>& v, TH1D* h)
  {
    assert(h != 0);
    int nvec = v.size();
    int nbins = h->GetNbinsX();
    int nfill = std::min(nvec, nbins);
    h->SetBinContent(0, 0.);
    for (int i = 0; i < nfill; ++i)
      h->SetBinContent(i + 1, v[i]);
    for (int i = nfill + 1; i <= nbins + 1; ++i)
      h->SetBinContent(i, 0.);
  }

  // Fill vector with initial delta-function at bin d.

  void fill_delta(std::vector<double>& v, int d)
  {
    int n = v.size();
    assert(d >= 0 && d < n);
    for (int i = 0; i < n; ++i)
      v[i] = 0.;
    v[d] = 1.;
  }
}

namespace caldata {
  class FFTTest : public art::EDAnalyzer {
  public:
    // Constructor, destructor.

    explicit FFTTest(fhicl::ParameterSet const& pset);
    virtual ~FFTTest();

    // Overrides.

    void analyze(const art::Event& evt);

  private:
    // Attributes.

    std::string fSimFile; // SimWire response file name.
    std::string fCalFile; // CalWire response file name.
    int fNTicks;          // Number of ticks.

    // Time domain response functions.

    std::vector<double> fSimElect;    // response function for the electronics
    std::vector<double> fSimColField; // response function for the field @ collection plane
    std::vector<double> fSimIndField; // response function for the field @ induction plane
    std::vector<double> fSimColConv;  // Collection plane convoluted response function.
    std::vector<double> fSimIndConv;  // Induction plane convoluted response function.

    // Frequency domain response functions.

    std::vector<TComplex> fSimElectF;    // response function for the electronics
    std::vector<TComplex> fSimColFieldF; // response function for the field @ collection plane
    std::vector<TComplex> fSimIndFieldF; // response function for the field @ induction plane
    std::vector<TComplex> fSimColConvF;  // Collection plane convoluted response function.
    std::vector<TComplex> fSimIndConvF;  // Induction plane convoluted response function.
    std::vector<TComplex> fColDeconvF;   // Collection plane deconvolution.
    std::vector<TComplex> fIndDeconvF;   // Collection plane deconvolution.
  };

  DEFINE_ART_MODULE(FFTTest)

  FFTTest::FFTTest(const fhicl::ParameterSet& pset) : EDAnalyzer(pset)
  {
    // Get file service.

    art::ServiceHandle<art::TFileService> tfs;

    // Get FFT service.

    art::ServiceHandle<util::LArFFT> fFFT;
    fNTicks = fFFT->FFTSize();
    std::cout << "Number of ticks = " << fNTicks << std::endl;

    // Get simulation (convolution) response functions.

    fSimFile = pset.get<std::string>("simwire_file");
    std::cout << "SimWire file = " << fSimFile << std::endl;

    TFile fsim(fSimFile.c_str());

    TH1D* hSimElect = dynamic_cast<TH1D*>(fsim.Get("daq/ElectronicsResponse"));
    hist_to_vector(hSimElect, fSimElect);
    fSimElect.resize(fNTicks, 0.);
    fSimElectF.resize(fNTicks / 2 + 1);
    fFFT->DoFFT(fSimElect, fSimElectF);

    TH1D* hSimColField = dynamic_cast<TH1D*>(fsim.Get("daq/CollectionFieldResponse"));
    hist_to_vector(hSimColField, fSimColField);
    fSimColField.resize(fNTicks, 0.);
    fSimColFieldF.resize(fNTicks / 2 + 1);
    fFFT->DoFFT(fSimColField, fSimColFieldF);

    TH1D* hSimIndField = dynamic_cast<TH1D*>(fsim.Get("daq/InductionFieldResponse"));
    hist_to_vector(hSimIndField, fSimIndField);
    fSimIndField.resize(fNTicks, 0.);
    fSimIndFieldF.resize(fNTicks / 2 + 1);
    fFFT->DoFFT(fSimIndField, fSimIndFieldF);

    TH1D* hSimColConv = dynamic_cast<TH1D*>(fsim.Get("daq/ConvolutedCollection"));
    hist_to_vector(hSimColConv, fSimColConv);
    fSimColConv.resize(fNTicks, 0.);
    fSimColConvF.resize(fNTicks / 2 + 1);
    fFFT->DoFFT(fSimColConv, fSimColConvF);

    TH1D* hSimIndConv = dynamic_cast<TH1D*>(fsim.Get("daq/ConvolutedInduction"));
    hist_to_vector(hSimIndConv, fSimIndConv);
    fSimIndConv.resize(fNTicks, 0.);
    fSimIndConvF.resize(fNTicks / 2 + 1);
    fFFT->DoFFT(fSimIndConv, fSimIndConvF);

    // Get reco (deconvolution) response function.

    fhicl::ParameterSet calwire_pset = pset.get<fhicl::ParameterSet>("calwire");
    cet::search_path sp("FW_SEARCH_PATH");
    sp.find_file(calwire_pset.get<std::string>("ResponseFile"), fCalFile);
    std::cout << "CalWire file = " << fCalFile << std::endl;

    TFile fcal(fCalFile.c_str());

    TH2D* respRe = dynamic_cast<TH2D*>(fcal.Get("sim/RespRe"));
    TH2D* respIm = dynamic_cast<TH2D*>(fcal.Get("sim/RespIm"));
    int nx = respRe->GetNbinsX();
    int ny = respRe->GetNbinsY();
    assert(nx == respIm->GetNbinsX());
    assert(ny == respIm->GetNbinsY());
    assert(nx == 2); // 1=induction, 2=collection.

    fColDeconvF.resize(ny);
    fIndDeconvF.resize(ny);

    for (int i = 0; i < ny; ++i) {
      double ac = respRe->GetBinContent(2, i + 1);
      double bc = respIm->GetBinContent(2, i + 1);
      TComplex zc(ac, bc);
      fColDeconvF[i] = zc;

      double ai = respRe->GetBinContent(1, i + 1);
      double bi = respIm->GetBinContent(1, i + 1);
      TComplex zi(ai, bi);
      fIndDeconvF[i] = zi;
    }

    // Calculate response of delta function to collection field + electronics.

    art::TFileDirectory dirc = tfs->mkdir("Collection", "Collection");
    int nhist = std::min(200, fNTicks);

    // Input signal (delta function).

    std::vector<double> tinc(fNTicks, 0.);
    fill_delta(tinc, nhist / 2);
    TH1D* hinc = dirc.make<TH1D>("input", "Collection Input", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tinc, hinc);

    // Electronics response.

    std::vector<double> telectc(tinc);
    fFFT->Convolute(telectc, fSimElectF);
    TH1D* helectc =
      dirc.make<TH1D>("elect", "Collection Electronics", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(telectc, helectc);

    // Field response.

    std::vector<double> tfieldc(tinc);
    fill_delta(tfieldc, nhist / 2);
    fFFT->Convolute(tfieldc, fSimColFieldF);
    TH1D* hfieldc = dirc.make<TH1D>("field", "Collection Field", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tfieldc, hfieldc);

    // Convolution of electronics and field response.

    std::vector<double> tbothc(tfieldc);
    fFFT->Convolute(tbothc, fSimElectF);
    TH1D* hbothc =
      dirc.make<TH1D>("both", "Collection Field+Electronics", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tbothc, hbothc);

    // Shifted convolution of electronics and field response.

    double shift = fFFT->PeakCorrelation(tbothc, tinc);
    std::cout << "Collection shift = " << shift << std::endl;
    std::vector<double> tshiftc(tbothc);
    fFFT->ShiftData(tshiftc, shift);
    TH1D* hshiftc =
      dirc.make<TH1D>("shift", "Collection Field+Electronics+Shift", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tshiftc, hshiftc);

    // Convolution response function read from file.

    std::vector<double> tconvc(tinc);
    fFFT->Convolute(tconvc, fSimColConvF);
    TH1D* hconvc = dirc.make<TH1D>("conv", "Collection Response", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tconvc, hconvc);

    // Deconvolution.

    std::vector<double> tdeconvc(tconvc);
    fFFT->Convolute(tdeconvc, fColDeconvF);
    TH1D* hdeconvc =
      dirc.make<TH1D>("deconv", "Collection Deconvoluted", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tdeconvc, hdeconvc);

    // Calculate response of delta function to induction field + electronics.

    art::TFileDirectory diri = tfs->mkdir("Induction", "Induction");

    // Input signal (delta function).

    std::vector<double> tini(fNTicks, 0.);
    fill_delta(tini, nhist / 2);
    TH1D* hini = diri.make<TH1D>("input", "Induction Input", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tini, hini);

    // Electronics response.

    std::vector<double> telecti(tini);
    fFFT->Convolute(telecti, fSimElectF);
    TH1D* helecti = diri.make<TH1D>("elect", "Induction Electronics", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(telecti, helecti);

    // Field response.

    std::vector<double> tfieldi(tini);
    fFFT->Convolute(tfieldi, fSimIndFieldF);
    TH1D* hfieldi = diri.make<TH1D>("field", "Induction Field", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tfieldi, hfieldi);

    // Convolution of electronics and field response.

    std::vector<double> tbothi(tfieldi);
    fFFT->Convolute(tbothi, fSimElectF);
    TH1D* hbothi =
      diri.make<TH1D>("both", "Induction Field+Electronics", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tbothi, hbothi);

    // Shifted convolution of electronics and field response.

    shift = fFFT->PeakCorrelation(tbothi, tini);
    std::cout << "Induction shift = " << shift << std::endl;
    std::vector<double> tshifti(tbothi);
    fFFT->ShiftData(tshifti, shift);
    TH1D* hshifti =
      diri.make<TH1D>("shift", "Induction Field+Electronics+Shift", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tshifti, hshifti);

    // Convolution response function read from file.

    std::vector<double> tconvi(tini);
    fFFT->Convolute(tconvi, fSimIndConvF);
    TH1D* hconvi = diri.make<TH1D>("conv", "Induction Response", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tconvi, hconvi);

    // Deconvolution.

    std::vector<double> tdeconvi(tconvi);
    fFFT->Convolute(tdeconvi, fIndDeconvF);
    TH1D* hdeconvi =
      diri.make<TH1D>("deconv", "Induction Deconvoluted", nhist + 1, -0.5, nhist + 0.5);
    vector_to_hist(tdeconvi, hdeconvi);
  }

  FFTTest::~FFTTest() {}

  void FFTTest::analyze(const art::Event& /* evt */) {}
}
