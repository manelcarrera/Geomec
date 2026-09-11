#ifndef _cora_FilterValueTypes_h_
#define _cora_FilterValueTypes_h_

#include <map>
#include <set>

#include <stddef.h>

namespace cora
{

class CFilterValueTypes
{
  public:
  CFilterValueTypes(int materialModel);

  bool isValueTypeAllowed(size_t valueType) const;
  bool isValueTypeExcluded(size_t valueType) const;

  bool isValueTypeLinear(size_t valueType) const;

  private:
  CFilterValueTypes(const CFilterValueTypes& rhs);
  CFilterValueTypes& operator = (CFilterValueTypes rhs);

  typedef std::map <size_t, bool> TFilterMap;
  typedef std::set <size_t> TFilterSet;

  static TFilterSet fillExcludedValueTypes(int materialModel);
  static TFilterSet fillAllLinearValueTypes();
  static void addLinearModelValueTypes(TFilterSet& allLinearValueTypes);

  TFilterSet m_excludedValueTypes;
  TFilterSet m_allLinearValueTypes;
  int m_materialModel;
};

} // namespace cora

#endif  // _cora_FilterValueTypes_h_
