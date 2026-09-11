#ifndef _cora_FailureTypeParameterFormation_h_
#define _cora_FailureTypeParameterFormation_h_

namespace cora
{

class CSummaryResultFile;
class CGetModelInfo;

} // namespace cora

#include "FailureTypeParameterBase.h"

namespace cora
{

class CFailureTypeParameterFormation : public CFailureTypeParameterBase
{
  public:
  static const QString FORMATION;

  CFailureTypeParameterFormation(CSummaryResultFile& summaryResultFile,
      const std::string& object, const std::string& option,
      const std::string& parameter, std::vector <double>& value,
      const CGetModelInfo& modelInfo);
  virtual ~CFailureTypeParameterFormation();

  private:
  CFailureTypeParameterFormation(const CFailureTypeParameterFormation& rhs);
  CFailureTypeParameterFormation& operator = (
      CFailureTypeParameterFormation rhs);
};

} // namespace cora

#endif  // _cora_FailureTypeParameterFormation_h_
