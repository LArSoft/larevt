////////////////////////////////////////////////////////////////////////
// \file SpaceChargeStandard.cxx
//
// \brief implementation of class for storing/accessing space charge distortions (exists as an example, by default this does nothing, instead requiring experiment-specific implementation)
//
// \author mrmooney@bnl.gov
//
////////////////////////////////////////////////////////////////////////

// C++ language includes
#include <fstream>

// LArSoft includes
#include "larevt/SpaceCharge/SpaceChargeStandard.h"

// Framework includes
#include "canvas/Utilities/Exception.h"
#include "fhiclcpp/ParameterSet.h"

// ROOT includes
#include "TFile.h"
#include "TGraph.h"
#include "TString.h"

//-----------------------------------------------
spacecharge::SpaceChargeStandard::SpaceChargeStandard(fhicl::ParameterSet const& pset)
{
  Configure(pset);
}

//------------------------------------------------
bool spacecharge::SpaceChargeStandard::Configure(fhicl::ParameterSet const& pset)
{
  fEnableSimSpatialSCE = pset.get<bool>("EnableSimSpatialSCE");
  fEnableSimEfieldSCE = pset.get<bool>("EnableSimEfieldSCE");
  fEnableCalSpatialSCE = pset.get<bool>("EnableCalSpatialSCE");
  fEnableCalEfieldSCE = pset.get<bool>("EnableCalEfieldSCE");
  fEnableCorrSCE = pset.get<bool>("EnableCorrSCE");

  // check that the old obsoleted parameter is not in configuration:
  if (pset.has_key("EnableSimulationSCE")) {
    throw art::Exception(art::errors::Configuration)
      << "Configuration parameter 'EnableSimulationSCE' has been replaced by "
         "'EnableSimSpatialSCE'.\n";
  }

  if ((fEnableSimSpatialSCE == true) | (fEnableSimEfieldSCE == true)) {
    fRepresentationType = pset.get<std::string>("RepresentationType");
    fInputFilename = pset.get<std::string>("InputFilename");

    std::string fname;
    cet::search_path sp("FW_SEARCH_PATH");
    sp.find_file(fInputFilename, fname);

    auto infile = std::make_unique<TFile>(fname.c_str(), "READ");
    if (!infile->IsOpen())
      throw art::Exception(art::errors::Configuration)
        << "Could not find the space charge effect file '" << fname << "'!\n";

    if (fRepresentationType == "Parametric") {
      for (int i = 0; i < 5; i++) {
        g1_x[i] = (TGraph*)infile->Get(Form("deltaX/g1_%d", i));
        g2_x[i] = (TGraph*)infile->Get(Form("deltaX/g2_%d", i));
        g3_x[i] = (TGraph*)infile->Get(Form("deltaX/g3_%d", i));
        g4_x[i] = (TGraph*)infile->Get(Form("deltaX/g4_%d", i));
        g5_x[i] = (TGraph*)infile->Get(Form("deltaX/g5_%d", i));

        g1_y[i] = (TGraph*)infile->Get(Form("deltaY/g1_%d", i));
        g2_y[i] = (TGraph*)infile->Get(Form("deltaY/g2_%d", i));
        g3_y[i] = (TGraph*)infile->Get(Form("deltaY/g3_%d", i));
        g4_y[i] = (TGraph*)infile->Get(Form("deltaY/g4_%d", i));
        g5_y[i] = (TGraph*)infile->Get(Form("deltaY/g5_%d", i));
        g6_y[i] = (TGraph*)infile->Get(Form("deltaY/g6_%d", i));

        g1_z[i] = (TGraph*)infile->Get(Form("deltaZ/g1_%d", i));
        g2_z[i] = (TGraph*)infile->Get(Form("deltaZ/g2_%d", i));
        g3_z[i] = (TGraph*)infile->Get(Form("deltaZ/g3_%d", i));
        g4_z[i] = (TGraph*)infile->Get(Form("deltaZ/g4_%d", i));

        g1_Ex[i] = (TGraph*)infile->Get(Form("deltaExOverE/g1_%d", i));
        g2_Ex[i] = (TGraph*)infile->Get(Form("deltaExOverE/g2_%d", i));
        g3_Ex[i] = (TGraph*)infile->Get(Form("deltaExOverE/g3_%d", i));
        g4_Ex[i] = (TGraph*)infile->Get(Form("deltaExOverE/g4_%d", i));
        g5_Ex[i] = (TGraph*)infile->Get(Form("deltaExOverE/g5_%d", i));

        g1_Ey[i] = (TGraph*)infile->Get(Form("deltaEyOverE/g1_%d", i));
        g2_Ey[i] = (TGraph*)infile->Get(Form("deltaEyOverE/g2_%d", i));
        g3_Ey[i] = (TGraph*)infile->Get(Form("deltaEyOverE/g3_%d", i));
        g4_Ey[i] = (TGraph*)infile->Get(Form("deltaEyOverE/g4_%d", i));
        g5_Ey[i] = (TGraph*)infile->Get(Form("deltaEyOverE/g5_%d", i));
        g6_Ey[i] = (TGraph*)infile->Get(Form("deltaEyOverE/g6_%d", i));

        g1_Ez[i] = (TGraph*)infile->Get(Form("deltaEzOverE/g1_%d", i));
        g2_Ez[i] = (TGraph*)infile->Get(Form("deltaEzOverE/g2_%d", i));
        g3_Ez[i] = (TGraph*)infile->Get(Form("deltaEzOverE/g3_%d", i));
        g4_Ez[i] = (TGraph*)infile->Get(Form("deltaEzOverE/g4_%d", i));
      }

      g1_x[5] = (TGraph*)infile->Get("deltaX/g1_5");
      g2_x[5] = (TGraph*)infile->Get("deltaX/g2_5");
      g3_x[5] = (TGraph*)infile->Get("deltaX/g3_5");
      g4_x[5] = (TGraph*)infile->Get("deltaX/g4_5");
      g5_x[5] = (TGraph*)infile->Get("deltaX/g5_5");

      g1_y[5] = (TGraph*)infile->Get("deltaY/g1_5");
      g2_y[5] = (TGraph*)infile->Get("deltaY/g2_5");
      g3_y[5] = (TGraph*)infile->Get("deltaY/g3_5");
      g4_y[5] = (TGraph*)infile->Get("deltaY/g4_5");
      g5_y[5] = (TGraph*)infile->Get("deltaY/g5_5");
      g6_y[5] = (TGraph*)infile->Get("deltaY/g6_5");

      g1_x[6] = (TGraph*)infile->Get("deltaX/g1_6");
      g2_x[6] = (TGraph*)infile->Get("deltaX/g2_6");
      g3_x[6] = (TGraph*)infile->Get("deltaX/g3_6");
      g4_x[6] = (TGraph*)infile->Get("deltaX/g4_6");
      g5_x[6] = (TGraph*)infile->Get("deltaX/g5_6");

      g1_Ex[5] = (TGraph*)infile->Get("deltaExOverE/g1_5");
      g2_Ex[5] = (TGraph*)infile->Get("deltaExOverE/g2_5");
      g3_Ex[5] = (TGraph*)infile->Get("deltaExOverE/g3_5");
      g4_Ex[5] = (TGraph*)infile->Get("deltaExOverE/g4_5");
      g5_Ex[5] = (TGraph*)infile->Get("deltaExOverE/g5_5");

      g1_Ey[5] = (TGraph*)infile->Get("deltaEyOverE/g1_5");
      g2_Ey[5] = (TGraph*)infile->Get("deltaEyOverE/g2_5");
      g3_Ey[5] = (TGraph*)infile->Get("deltaEyOverE/g3_5");
      g4_Ey[5] = (TGraph*)infile->Get("deltaEyOverE/g4_5");
      g5_Ey[5] = (TGraph*)infile->Get("deltaEyOverE/g5_5");
      g6_Ey[5] = (TGraph*)infile->Get("deltaEyOverE/g6_5");

      g1_Ex[6] = (TGraph*)infile->Get("deltaExOverE/g1_6");
      g2_Ex[6] = (TGraph*)infile->Get("deltaExOverE/g2_6");
      g3_Ex[6] = (TGraph*)infile->Get("deltaExOverE/g3_6");
      g4_Ex[6] = (TGraph*)infile->Get("deltaExOverE/g4_6");
      g5_Ex[6] = (TGraph*)infile->Get("deltaExOverE/g5_6");
    }

    infile->Close();
  }

  if (fEnableCorrSCE == true) {
    // Grab other parameters from pset
  }

  return true;
}

//------------------------------------------------
bool spacecharge::SpaceChargeStandard::Update(uint64_t ts)
{
  if (ts == 0) return false;

  return true;
}

//----------------------------------------------------------------------------
/// Return boolean indicating whether or not to turn simulation of SCE on for
/// spatial distortions
bool spacecharge::SpaceChargeStandard::EnableSimSpatialSCE() const
{
  return fEnableSimSpatialSCE;
}

//----------------------------------------------------------------------------
/// Return boolean indicating whether or not to turn simulation of SCE on for
/// E field distortions
bool spacecharge::SpaceChargeStandard::EnableSimEfieldSCE() const
{
  return fEnableSimEfieldSCE;
}

//----------------------------------------------------------------------------
/// Return boolean indicating whether or not to apply SCE corrections
bool spacecharge::SpaceChargeStandard::EnableCorrSCE() const
{
  return fEnableCorrSCE;
}

/// Return boolean indicating whether or not to apply SCE corrections
bool spacecharge::SpaceChargeStandard::EnableCalSpatialSCE() const
{
  return fEnableCalSpatialSCE;
}

/// Return boolean indicating whether or not to apply SCE corrections
bool spacecharge::SpaceChargeStandard::EnableCalEfieldSCE() const
{
  return fEnableCalEfieldSCE;
}

//----------------------------------------------------------------------------
/// Primary working method of service that provides position offsets to be
/// used in ionization electron drift
geo::Vector_t spacecharge::SpaceChargeStandard::GetPosOffsets(geo::Point_t const& point) const
{
  std::vector<double> thePosOffsets;

  if (IsInsideBoundaries(point.X(), point.Y(), point.Z()) == false) {
    thePosOffsets.resize(3, 0.0);
  }
  else {
    if (fRepresentationType == "Parametric")
      thePosOffsets = GetPosOffsetsParametric(point.X(), point.Y(), point.Z());
    else
      thePosOffsets.resize(3, 0.0);
  }

  return {thePosOffsets[0], thePosOffsets[1], thePosOffsets[2]};
}

geo::Vector_t spacecharge::SpaceChargeStandard::GetCalPosOffsets(geo::Point_t const& point,
                                                                 int const& TPCid) const
{

  return {0., 0., 0.};
}

//----------------------------------------------------------------------------
/// Provides position offsets using a parametric representation
std::vector<double> spacecharge::SpaceChargeStandard::GetPosOffsetsParametric(double xVal,
                                                                              double yVal,
                                                                              double zVal) const
{
  std::vector<double> thePosOffsetsParametric;

  double xValNew = TransformX(xVal);
  double yValNew = TransformY(yVal);
  double zValNew = TransformZ(zVal);

  thePosOffsetsParametric.push_back(GetOnePosOffsetParametric(xValNew, yValNew, zValNew, "X"));
  thePosOffsetsParametric.push_back(GetOnePosOffsetParametric(xValNew, yValNew, zValNew, "Y"));
  thePosOffsetsParametric.push_back(GetOnePosOffsetParametric(xValNew, yValNew, zValNew, "Z"));

  return thePosOffsetsParametric;
}

//----------------------------------------------------------------------------
/// Provides one position offset using a parametric representation, for a given
/// axis
double spacecharge::SpaceChargeStandard::GetOnePosOffsetParametric(double xValNew,
                                                                   double yValNew,
                                                                   double zValNew,
                                                                   std::string axis) const
{
  double parA[6][7];
  double parB[6];

  for (int j = 0; j < 6; j++) {
    for (int i = 0; i < 7; i++)
      parA[j][i] = 0.0;

    parB[j] = 0.0;
  }

  if (axis == "X") {
    for (int j = 0; j < 7; j++) {
      parA[0][j] = g1_x[j]->Eval(zValNew);
      parA[1][j] = g2_x[j]->Eval(zValNew);
      parA[2][j] = g3_x[j]->Eval(zValNew);
      parA[3][j] = g4_x[j]->Eval(zValNew);
      parA[4][j] = g5_x[j]->Eval(zValNew);
    }

    f1_x->SetParameters(parA[0]);
    f2_x->SetParameters(parA[1]);
    f3_x->SetParameters(parA[2]);
    f4_x->SetParameters(parA[3]);
    f5_x->SetParameters(parA[4]);
  }
  else if (axis == "Y") {
    for (int j = 0; j < 6; j++) {
      parA[0][j] = g1_y[j]->Eval(zValNew);
      parA[1][j] = g2_y[j]->Eval(zValNew);
      parA[2][j] = g3_y[j]->Eval(zValNew);
      parA[3][j] = g4_y[j]->Eval(zValNew);
      parA[4][j] = g5_y[j]->Eval(zValNew);
      parA[5][j] = g6_y[j]->Eval(zValNew);
    }

    f1_y->SetParameters(parA[0]);
    f2_y->SetParameters(parA[1]);
    f3_y->SetParameters(parA[2]);
    f4_y->SetParameters(parA[3]);
    f5_y->SetParameters(parA[4]);
    f6_y->SetParameters(parA[5]);
  }
  else if (axis == "Z") {
    for (int j = 0; j < 5; j++) {
      parA[0][j] = g1_z[j]->Eval(zValNew);
      parA[1][j] = g2_z[j]->Eval(zValNew);
      parA[2][j] = g3_z[j]->Eval(zValNew);
      parA[3][j] = g4_z[j]->Eval(zValNew);
    }

    f1_z->SetParameters(parA[0]);
    f2_z->SetParameters(parA[1]);
    f3_z->SetParameters(parA[2]);
    f4_z->SetParameters(parA[3]);
  }

  double aValNew;
  double bValNew;

  if (axis == "Y") {
    aValNew = xValNew;
    bValNew = yValNew;
  }
  else {
    aValNew = yValNew;
    bValNew = xValNew;
  }

  double offsetValNew = 0.0;
  if (axis == "X") {
    parB[0] = f1_x->Eval(aValNew);
    parB[1] = f2_x->Eval(aValNew);
    parB[2] = f3_x->Eval(aValNew);
    parB[3] = f4_x->Eval(aValNew);
    parB[4] = f5_x->Eval(aValNew);

    fFinal_x->SetParameters(parB);
    offsetValNew = 100.0 * fFinal_x->Eval(bValNew);
  }
  else if (axis == "Y") {
    parB[0] = f1_y->Eval(aValNew);
    parB[1] = f2_y->Eval(aValNew);
    parB[2] = f3_y->Eval(aValNew);
    parB[3] = f4_y->Eval(aValNew);
    parB[4] = f5_y->Eval(aValNew);
    parB[5] = f6_y->Eval(aValNew);

    fFinal_y->SetParameters(parB);
    offsetValNew = 100.0 * fFinal_y->Eval(bValNew);
  }
  else if (axis == "Z") {
    parB[0] = f1_z->Eval(aValNew);
    parB[1] = f2_z->Eval(aValNew);
    parB[2] = f3_z->Eval(aValNew);
    parB[3] = f4_z->Eval(aValNew);

    fFinal_z->SetParameters(parB);
    offsetValNew = 100.0 * fFinal_z->Eval(bValNew);
  }

  return offsetValNew;
}

//----------------------------------------------------------------------------
/// Primary working method of service that provides E field offsets to be
/// used in charge/light yield calculation (e.g.)
geo::Vector_t spacecharge::SpaceChargeStandard::GetEfieldOffsets(geo::Point_t const& point) const
{
  std::vector<double> theEfieldOffsets;

  if (fRepresentationType == "Parametric")
    theEfieldOffsets = GetEfieldOffsetsParametric(point.X(), point.Y(), point.Z());
  else
    theEfieldOffsets.resize(3, 0.0);

  return {-theEfieldOffsets[0], -theEfieldOffsets[1], -theEfieldOffsets[2]};
}

geo::Vector_t spacecharge::SpaceChargeStandard::GetCalEfieldOffsets(geo::Point_t const& point,
                                                                    int const& TPCid) const
{

  return {0., 0., 0.};
}

//----------------------------------------------------------------------------
/// Provides E field offsets using a parametric representation
std::vector<double> spacecharge::SpaceChargeStandard::GetEfieldOffsetsParametric(double xVal,
                                                                                 double yVal,
                                                                                 double zVal) const
{
  std::vector<double> theEfieldOffsetsParametric;

  double xValNew = TransformX(xVal);
  double yValNew = TransformY(yVal);
  double zValNew = TransformZ(zVal);

  theEfieldOffsetsParametric.push_back(
    GetOneEfieldOffsetParametric(xValNew, yValNew, zValNew, "X"));
  theEfieldOffsetsParametric.push_back(
    GetOneEfieldOffsetParametric(xValNew, yValNew, zValNew, "Y"));
  theEfieldOffsetsParametric.push_back(
    GetOneEfieldOffsetParametric(xValNew, yValNew, zValNew, "Z"));

  return theEfieldOffsetsParametric;
}

//----------------------------------------------------------------------------
/// Provides one E field offset using a parametric representation, for a given
/// axis, with returned E field offsets normalized to nominal drift E field
double spacecharge::SpaceChargeStandard::GetOneEfieldOffsetParametric(double xValNew,
                                                                      double yValNew,
                                                                      double zValNew,
                                                                      std::string axis) const
{
  double parA[6][7];
  double parB[6];

  for (int j = 0; j < 6; j++) {
    for (int i = 0; i < 7; i++)
      parA[j][i] = 0.0;

    parB[j] = 0.0;
  }

  if (axis == "X") {
    for (int j = 0; j < 7; j++) {
      parA[0][j] = g1_Ex[j]->Eval(zValNew);
      parA[1][j] = g2_Ex[j]->Eval(zValNew);
      parA[2][j] = g3_Ex[j]->Eval(zValNew);
      parA[3][j] = g4_Ex[j]->Eval(zValNew);
      parA[4][j] = g5_Ex[j]->Eval(zValNew);
    }

    f1_Ex->SetParameters(parA[0]);
    f2_Ex->SetParameters(parA[1]);
    f3_Ex->SetParameters(parA[2]);
    f4_Ex->SetParameters(parA[3]);
    f5_Ex->SetParameters(parA[4]);
  }
  else if (axis == "Y") {
    for (int j = 0; j < 6; j++) {
      parA[0][j] = g1_Ey[j]->Eval(zValNew);
      parA[1][j] = g2_Ey[j]->Eval(zValNew);
      parA[2][j] = g3_Ey[j]->Eval(zValNew);
      parA[3][j] = g4_Ey[j]->Eval(zValNew);
      parA[4][j] = g5_Ey[j]->Eval(zValNew);
      parA[5][j] = g6_Ey[j]->Eval(zValNew);
    }

    f1_Ey->SetParameters(parA[0]);
    f2_Ey->SetParameters(parA[1]);
    f3_Ey->SetParameters(parA[2]);
    f4_Ey->SetParameters(parA[3]);
    f5_Ey->SetParameters(parA[4]);
    f6_Ey->SetParameters(parA[5]);
  }
  else if (axis == "Z") {
    for (int j = 0; j < 5; j++) {
      parA[0][j] = g1_Ez[j]->Eval(zValNew);
      parA[1][j] = g2_Ez[j]->Eval(zValNew);
      parA[2][j] = g3_Ez[j]->Eval(zValNew);
      parA[3][j] = g4_Ez[j]->Eval(zValNew);
    }

    f1_Ez->SetParameters(parA[0]);
    f2_Ez->SetParameters(parA[1]);
    f3_Ez->SetParameters(parA[2]);
    f4_Ez->SetParameters(parA[3]);
  }

  double aValNew;
  double bValNew;

  if (axis == "Y") {
    aValNew = xValNew;
    bValNew = yValNew;
  }
  else {
    aValNew = yValNew;
    bValNew = xValNew;
  }

  double offsetValNew = 0.0;
  if (axis == "X") {
    parB[0] = f1_Ex->Eval(aValNew);
    parB[1] = f2_Ex->Eval(aValNew);
    parB[2] = f3_Ex->Eval(aValNew);
    parB[3] = f4_Ex->Eval(aValNew);
    parB[4] = f5_Ex->Eval(aValNew);

    fFinal_Ex->SetParameters(parB);
    offsetValNew = fFinal_Ex->Eval(bValNew);
  }
  else if (axis == "Y") {
    parB[0] = f1_Ey->Eval(aValNew);
    parB[1] = f2_Ey->Eval(aValNew);
    parB[2] = f3_Ey->Eval(aValNew);
    parB[3] = f4_Ey->Eval(aValNew);
    parB[4] = f5_Ey->Eval(aValNew);
    parB[5] = f6_Ey->Eval(aValNew);

    fFinal_Ey->SetParameters(parB);
    offsetValNew = fFinal_Ey->Eval(bValNew);
  }
  else if (axis == "Z") {
    parB[0] = f1_Ez->Eval(aValNew);
    parB[1] = f2_Ez->Eval(aValNew);
    parB[2] = f3_Ez->Eval(aValNew);
    parB[3] = f4_Ez->Eval(aValNew);

    fFinal_Ez->SetParameters(parB);
    offsetValNew = fFinal_Ez->Eval(bValNew);
  }

  return offsetValNew;
}

//----------------------------------------------------------------------------
/// Transform X to SCE X coordinate - redefine this in experiment-specific implementation!
double spacecharge::SpaceChargeStandard::TransformX(double xVal) const
{
  double xValNew;
  xValNew = xVal / 100.0;

  return xValNew;
}

//----------------------------------------------------------------------------
/// Transform Y to SCE Y coordinate - redefine this in experiment-specific implementation!
double spacecharge::SpaceChargeStandard::TransformY(double yVal) const
{
  double yValNew;
  yValNew = yVal / 100.0;

  return yValNew;
}

//----------------------------------------------------------------------------
/// Transform Z to SCE Z coordinate - redefine this in experiment-specific implementation!
double spacecharge::SpaceChargeStandard::TransformZ(double zVal) const
{
  double zValNew;
  zValNew = zVal / 100.0;

  return zValNew;
}

//----------------------------------------------------------------------------
/// Check to see if point is inside boundaries of map - redefine this in experiment-specific implementation!
bool spacecharge::SpaceChargeStandard::IsInsideBoundaries(double xVal,
                                                          double yVal,
                                                          double zVal) const
{
  bool isInside = false;

  return isInside;
}
