// WellPointValueMap.cpp: implementation of the CWellPointValueMap class.
//
//////////////////////////////////////////////////////////////////////

#include "WellPointMapper.h"
#include "WellSchemeInclude.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well {

IWellPointMapper::~IWellPointMapper() {}

CWellPointValueMap::CWellPointValueMap() : CWellPointMapper<geo::CValue>(QString(), CWellPointList()) {}

CWellPointValueMap::CWellPointValueMap(const QString &description, const CWellPointList &List,
                                       CDoubleQuantity *pQuantity)
    : CWellPointMapper<geo::CValue>(description, List), m_pQuantity(pQuantity) {}

CWellPointValueMap::~CWellPointValueMap() {}

//////////////////////////////////////////////////////////////////////

const CDoubleQuantity &CWellPointValueMap::QuantityIndicator() { return *m_pQuantity; }

//////////////////////////////////////////////////////////////////////

geo::CValue CWellPointValueMap::GetItemByTMD(const double &TMD) const {
  CWellPoint *StartPoint = NULL;
  CWellPoint *EndPoint = NULL;

  double Value = 0;

  QList<CWellPoint *> PointList = m_pList->GetUpperLower(TMD);
  QList<CWellPoint *>::const_iterator it = PointList.begin();

  StartPoint = (*it);
  it++;
  EndPoint = (*it);

  if (!StartPoint || !EndPoint)
    return geo::CValue();

  if (!GetItem(EndPoint).Valid())
    return geo::CValue();

  if (StartPoint == EndPoint)
    return geo::CValue(GetItem(EndPoint).Value());

  double DeltaY = GetItem(EndPoint).Value() - GetItem(StartPoint).Value();
  double DeltaX = EndPoint->TMD().Value() - StartPoint->TMD().Value();

  Value = (DeltaY / DeltaX) * (TMD - StartPoint->TMD().Value()) + GetItem(StartPoint).Value();

  return geo::CValue(Value);
}

CWellPointValueMap &CWellPointValueMap::operator=(const CWellPointValueMap &rhs) {
  CWellPointMapper<geo::CValue>::operator=(rhs);
  return *this;
}

} // namespace well
