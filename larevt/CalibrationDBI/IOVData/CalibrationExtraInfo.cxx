#include "CalibrationExtraInfo.h"
#include "IOVDataError.h"

namespace lariov {

  void CalibrationExtraInfo::AddOrReplaceBoolData(std::string const& label, bool const data)
  {
    fBoolData[label] = data;
  }

  void CalibrationExtraInfo::AddOrReplaceIntData(std::string const& label, int const data)
  {
    fIntData[label] = data;
  }

  void CalibrationExtraInfo::AddOrReplaceVecIntData(std::string const& label,
                                                    std::vector<int> const& data)
  {
    fVecIntData[label] = data;
  }

  void CalibrationExtraInfo::AddOrReplaceFloatData(std::string const& label, float const data)
  {
    fFloatData[label] = data;
  }

  void CalibrationExtraInfo::AddOrReplaceVecFloatData(std::string const& label,
                                                      std::vector<float> const& data)
  {
    fVecFloatData[label] = data;
  }

  void CalibrationExtraInfo::AddOrReplaceStringData(std::string const& label,
                                                    std::string const& data)
  {
    fStringData[label] = data;
  }

  void CalibrationExtraInfo::ClearDataByLabel(std::string const& label)
  {
    unsigned int n_erased = 0;

    n_erased += fBoolData.erase(label);
    n_erased += fIntData.erase(label);
    n_erased += fVecIntData.erase(label);
    n_erased += fFloatData.erase(label);
    n_erased += fVecFloatData.erase(label);
    n_erased += fStringData.erase(label);

    if (n_erased > 1) {
      std::cout << "INFO(CalibrationExtraInfo): Erased more than one entry with label " << label
                << ".  Recommend that you do not use identical labels" << std::endl;
    }
  }

  void CalibrationExtraInfo::ClearAllData()
  {
    fBoolData.clear();
    fIntData.clear();
    fVecIntData.clear();
    fFloatData.clear();
    fVecFloatData.clear();
    fStringData.clear();
  }

  bool CalibrationExtraInfo::GetBoolData(std::string const& label) const
  {
    if (fBoolData.find(label) != fBoolData.end()) { return fBoolData.at(label); }

    throw IOVDataError("CalibrationExtraInfo: Could not find extra bool data " + label +
                       " for calibration " + fName);
  }

  int CalibrationExtraInfo::GetIntData(std::string const& label) const
  {
    if (fIntData.find(label) != fIntData.end()) { return fIntData.at(label); }

    throw IOVDataError("CalibrationExtraInfo: Could not find extra int data " + label +
                       " for calibration " + fName);
  }

  std::vector<int> const& CalibrationExtraInfo::GetVecIntData(std::string const& label) const
  {
    if (fVecIntData.find(label) != fVecIntData.end()) { return fVecIntData.at(label); }

    throw IOVDataError("CalibrationExtraInfo: Could not find extra vector int data " + label +
                       " for calibration " + fName);
  }

  float CalibrationExtraInfo::GetFloatData(std::string const& label) const
  {
    if (fFloatData.find(label) != fFloatData.end()) { return fFloatData.at(label); }

    throw IOVDataError("CalibrationExtraInfo: Could not find extra float data " + label +
                       " for calibration " + fName);
  }

  std::vector<float> const& CalibrationExtraInfo::GetVecFloatData(std::string const& label) const
  {
    if (fVecFloatData.find(label) != fVecFloatData.end()) { return fVecFloatData.at(label); }

    throw IOVDataError("CalibrationExtraInfo: Could not find extra vector float data " + label +
                       " for calibration " + fName);
  }

  std::string const& CalibrationExtraInfo::GetStringData(std::string const& label) const
  {
    if (fStringData.find(label) != fStringData.end()) { return fStringData.at(label); }

    throw IOVDataError("CalibrationExtraInfo: Could not find extra string data " + label +
                       " for calibration " + fName);
  }
} //end namesplace lariov
