#ifndef CALIBRATIONEXTRAINFO_H
#define CALIBRATIONEXTRAINFO_H

#include <map>
#include <string>
#include <vector>

namespace lariov {

  class CalibrationExtraInfo {

  public:
    CalibrationExtraInfo(std::string const& name) : fName(name) {}

    virtual ~CalibrationExtraInfo() = default;

    //Get the name of the calibration
    std::string const& GetName() const { return fName; }

    void AddOrReplaceBoolData(std::string const& label, bool const data);
    void AddOrReplaceIntData(std::string const& label, int const data);
    void AddOrReplaceVecIntData(std::string const& label, std::vector<int> const& data);
    void AddOrReplaceFloatData(std::string const& label, float const data);
    void AddOrReplaceVecFloatData(std::string const& label, std::vector<float> const& data);
    void AddOrReplaceStringData(std::string const& label, std::string const& data);

    bool GetBoolData(std::string const& label) const;
    int GetIntData(std::string const& label) const;
    std::vector<int> const& GetVecIntData(std::string const& label) const;
    float GetFloatData(std::string const& label) const;
    std::vector<float> const& GetVecFloatData(std::string const& label) const;
    std::string const& GetStringData(std::string const& label) const;

    void ClearDataByLabel(std::string const& label);
    void ClearAllData();

  private:
    std::string fName;

    std::map<std::string, bool> fBoolData;

    std::map<std::string, int> fIntData;
    std::map<std::string, std::vector<int>> fVecIntData;

    std::map<std::string, float> fFloatData;
    std::map<std::string, std::vector<float>> fVecFloatData;

    std::map<std::string, std::string> fStringData;
  };
}

#endif
