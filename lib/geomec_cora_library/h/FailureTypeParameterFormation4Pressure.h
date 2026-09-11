#ifndef _cora_FailureTypeParameterFormation4Pressure_h_
#define _cora_FailureTypeParameterFormation4Pressure_h_

#include "FailureTypeParameterFormation.h"

namespace cora
{

class CFailureTypeParameterFormation4Pressure :
  public CFailureTypeParameterFormation
{
  public:
  CFailureTypeParameterFormation4Pressure(
      CSummaryResultFile& summaryResultFile, const std::string& object,
      const std::string& option, const std::string& parameter,
      std::vector <double>& value, const CGetModelInfo& modelInfo);
  virtual ~CFailureTypeParameterFormation4Pressure();

  virtual void modify(CModelBase* modelBase);

  private:
  CFailureTypeParameterFormation4Pressure(
      const CFailureTypeParameterFormation4Pressure& rhs);
  CFailureTypeParameterFormation4Pressure& operator = (
      CFailureTypeParameterFormation4Pressure rhs);
};

} // namespace cora

#endif  // _cora_FailureTypeParameterFormation4Pressure_h_
