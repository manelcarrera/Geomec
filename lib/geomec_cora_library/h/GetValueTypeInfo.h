#ifndef _cora_GetValueTypeInfo_h_
#define _cora_GetValueTypeInfo_h_

class CModelBase;
class CLibraryMaterialParameter;

#include "ValueTypeFactory.h"

namespace cora
{

class CGetValueTypeInfo
{
  public:
  static CGetValueTypeInfo& instance(CModelBase* modelBase);

  QString getImportTag(unsigned int valueType) const;

  std::pair <geo::CValue, geo::CValue>
      getRange(unsigned int valueTypeID, const QString& valueTypeName) const;
  std::pair <geo::CValue, geo::CValue>
      getRange(const CLibraryMaterialParameter& libraryMaterialParameter) const;

  private:
  CGetValueTypeInfo(CModelBase* modelBase);
  ~CGetValueTypeInfo();

  CGetValueTypeInfo(const CGetValueTypeInfo& rhs);
  CGetValueTypeInfo& operator = (CGetValueTypeInfo rhs);

  static void cleanup();

  static CGetValueTypeInfo* m_getValueTypeInfo;
  CModelBase* m_modelBase;
};

} // namespace cora

#endif  // _cora_GetValueTypeInfo_h_
