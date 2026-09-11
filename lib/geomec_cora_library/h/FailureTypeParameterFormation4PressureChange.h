#ifndef _cora_FailureTypeParameterFormation4PressureChange_h_
#define _cora_FailureTypeParameterFormation4PressureChange_h_

#include "FailureTypeParameterFormation.h"

namespace cora
{

class CFailureTypeParameterFormation4PressureChange :
  public CFailureTypeParameterFormation
{
  public:
    CFailureTypeParameterFormation4PressureChange(
      CSummaryResultFile& summaryResultFile, const std::string& object,
      const std::string& option, const std::string& parameter,
      std::vector <double>& value, const CGetModelInfo& modelInfo);
    virtual ~CFailureTypeParameterFormation4PressureChange();

    virtual void modify(CModelBase* modelBase);

  private:
    CFailureTypeParameterFormation4PressureChange(
      const CFailureTypeParameterFormation4PressureChange& rhs);
    CFailureTypeParameterFormation4PressureChange& operator = (
      CFailureTypeParameterFormation4PressureChange rhs);
};

} // namespace cora

#endif  // _cora_FailureTypeParameterFormation4PressureChange_h_
