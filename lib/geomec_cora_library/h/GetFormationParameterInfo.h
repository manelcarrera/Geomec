#ifndef _cora_GetFormationParameterInfo_h_
#define _cora_GetFormationParameterInfo_h_

#include <set>

class CFormationBase;
class CLibraryMaterialParameter;
class CValueType;

#include "Value.h"

namespace cora
{

class CGetFormationParameterInfo
{
  public:
    CGetFormationParameterInfo(const CFormationBase* formationBase);

    double getMean(const CLibraryMaterialParameter& libraryMaterialParameter,
      const std::set <CValueType*>& valueTypes) const;
    std::pair <geo::CValue, geo::CValue> getRange(
      const CLibraryMaterialParameter& libraryMaterialParameter,
      const std::set <CValueType*>& valueTypes) const;

  private:
    CGetFormationParameterInfo(const CGetFormationParameterInfo& rhs);
    CGetFormationParameterInfo& operator = (CGetFormationParameterInfo rhs);

    static double calculateVolume(const CFormationBase* formationBase);

    double getMean(const CLibraryMaterialParameter& libraryMaterialParameter,
      const CValueType* valueType) const;
    std::pair <geo::CValue, geo::CValue> getRange(
      const CLibraryMaterialParameter& libraryMaterialParameter,
      const CValueType* valueType) const;

    const CFormationBase* m_formationBase;
    double m_totalVolume;
};

} // namespace cora

#endif  // _cora_GetFormationParameterInfo_h_
