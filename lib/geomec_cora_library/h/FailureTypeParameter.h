#ifndef _cora_FailureTypeParameter_h_
#define _cora_FailureTypeParameter_h_

class CModelBase;

namespace cora
{

class CSummaryResultFile;
class CGetModelInfo;

} // namespace cora

#include "FailureTypeParameterBase.h"

namespace cora
{

class CFailureTypeParameter
{
  public:
    CFailureTypeParameter(CSummaryResultFile& summaryResultFile,
      std::istream& stream, const CGetModelInfo& modelInfo);

    void modify(CModelBase* modelBase);

    const TParameter getActualParameter() const;

  private:
    CFailureTypeParameter(const CFailureTypeParameter& rhs);
    CFailureTypeParameter& operator = (CFailureTypeParameter rhs);

    static TFailureTypeParameterBase selectFailureType(
      CSummaryResultFile& summaryResultFile, std::istream& stream,
      const CGetModelInfo& modelInfo);
    static TFailureTypeParameterBase selectFailureType(
      CSummaryResultFile& summaryResultFile, const std::string& object,
      const std::string& option, const std::string& parameter,
      std::vector <double>& value, const CGetModelInfo& modelInfo);

    TFailureTypeParameterBase m_failureTypeParameterBase;
};

typedef QSharedPointer <CFailureTypeParameter> TFailureTypeParameter;

} // namespace cora

#endif  // _cora_FailureTypeParameter_h_
