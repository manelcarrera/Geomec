#include "GetValueTypeInfo.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "LibraryMaterialParameter.h"

namespace cora
{

// static

CGetValueTypeInfo& CGetValueTypeInfo::instance(CModelBase* modelBase)
{
  if (m_getValueTypeInfo == 0)
  {
    m_getValueTypeInfo = new CGetValueTypeInfo(modelBase);
  }

  return *m_getValueTypeInfo;
}

// non-static

QString CGetValueTypeInfo::getImportTag(unsigned int valueType) const
{
  return CValueTypeFactory::instance()->getImportTag(valueType);
}

std::pair <geo::CValue, geo::CValue> CGetValueTypeInfo::getRange(
  unsigned int valueTypeID, const QString& valueTypeName) const
{
  CPointSet pointSet(*m_modelBase);
  QSharedPointer <CValueType> valueType = QSharedPointer <CValueType> (
    CValueTypeFactory::instance()->BuildValueType(pointSet, valueTypeID, valueTypeName));
  IValueComponentBase& valueComponent = valueType->Component();
  CValueComponent& component =
    dynamic_cast <CValueComponent&> (valueComponent);
  geo::CValue rangeMin = component.RangeMin();
  geo::CValue rangeMax = component.RangeMax();

  return std::make_pair(rangeMin, rangeMax);
}

std::pair <geo::CValue, geo::CValue> CGetValueTypeInfo::getRange(
  const CLibraryMaterialParameter& libraryMaterialParameter) const
{
  return getRange(libraryMaterialParameter.ValueTypeID(),
    libraryMaterialParameter.Name());
}

// private

CGetValueTypeInfo::CGetValueTypeInfo(CModelBase* modelBase)
: m_modelBase(modelBase)
{
  atexit(&cleanup);
}

CGetValueTypeInfo::~CGetValueTypeInfo()
{
}

void CGetValueTypeInfo::cleanup()
{
  delete m_getValueTypeInfo;
  m_getValueTypeInfo = 0;
}

CGetValueTypeInfo* CGetValueTypeInfo::m_getValueTypeInfo = 0;

} // namespace cora
