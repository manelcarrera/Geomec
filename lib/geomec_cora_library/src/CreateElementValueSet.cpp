#include "CreateElementValueSet.h"
#include "MapSingleValue2ElementValues.h"
#include "MapInterfaceElement2ElementValues.h"

namespace cora
{

CCreateElementValueSet::CCreateElementValueSet(
  const TMapSingleValue2ElementValues&)
: m_createElementValueSet(
    TCreateElementValueSetBase(new CMapSingleValue2ElementValues()))
{
}

CCreateElementValueSet::CCreateElementValueSet(
  const TMapInterfaceElement2ElementValues&)
: m_createElementValueSet(
    TCreateElementValueSetBase(new CMapInterfaceElement2ElementValues()))
{
}

void CCreateElementValueSet::createElementValueSet(CModelBase* modelBase,
  const CFailureTypeParameterBase* failureTypeParameterBase)
{
  m_createElementValueSet->
    createElementValueSet(modelBase, failureTypeParameterBase);
}

// static

CCreateElementValueSet::TMapSingleValue2ElementValues
  CCreateElementValueSet::mapSingleValue2ElementValues;
CCreateElementValueSet::TMapInterfaceElement2ElementValues
  CCreateElementValueSet::mapInterfaceElement2ElementValues;

} // namespace cora
