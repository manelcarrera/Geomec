#ifndef _cora_FailureTypeParameterWell_h_
#define _cora_FailureTypeParameterWell_h_

namespace cora
{

class CSummaryResultFile;
class CGetModelInfo;

} // namespace cora

#include "FailureTypeParameterBase.h"

namespace cora
{

class CFailureTypeParameterWell : public CFailureTypeParameterBase
{
  public:
  CFailureTypeParameterWell(CSummaryResultFile& summaryResultFile,
      const std::string& object, const std::string& option,
      const std::string& parameter, std::vector <double>& value,
      const CGetModelInfo& modelInfo);
  virtual ~CFailureTypeParameterWell();

  private:
  CFailureTypeParameterWell(const CFailureTypeParameterWell& rhs);
  CFailureTypeParameterWell& operator = (CFailureTypeParameterWell rhs);
};

} // namespace cora

#endif  // _cora_FailureTypeParameterWell_h_
