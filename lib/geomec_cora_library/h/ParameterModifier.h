#ifndef _cora_ParameterModifier_h_
#define _cora_ParameterModifier_h_

#include "safeQSharedPointer.h"

namespace cora
{

class CSummaryResultFile;

} // namespace cora

#include "ParameterModifierBase.h"

namespace cora
{

class CParameterModifier
{
  public:
  typedef std::vector <geo::CValue> TValueVec;

  CParameterModifier(CSummaryResultFile& summaryResultFile,
      const QString& option, std::vector <double>& value);

  geo::CValue modify(const geo::CValue& value) const;
  TValueVec modify(TValueVec& valueVec);

  private:
  CParameterModifier(const CParameterModifier& rhs);
  CParameterModifier& operator = (CParameterModifier rhs);

  static TParameterModifierBase selectParameterModifier(
      CSummaryResultFile& summaryResultFile, const QString& option,
      std::vector <double>& value);

  TParameterModifierBase m_parameterModifierBase;
};

typedef QSharedPointer <CParameterModifier> TParameterModifier;

} // namespace cora

#endif  // _cora_ParameterModifier_h_
