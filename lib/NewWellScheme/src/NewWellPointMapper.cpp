// NewWellPointMapper.cpp: implementation of the CNewWellPointValueMap class.
//
//////////////////////////////////////////////////////////////////////

#include "WellSchemeInclude.h"
#include "NewWellPointMapper.h"
#include "NewWellPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewWellPointValueMap::CNewWellPointValueMap()
:CNewWellPointMapper(QString(), std::list<CNewWellPoint>())
{}

CNewWellPointValueMap::CNewWellPointValueMap(const QString& description,const std::list<CNewWellPoint>  & List,CDoubleQuantity* pQuantity)
:CNewWellPointMapper<geo::CValue>(description,List),
 m_pQuantity(pQuantity)
{

}

CNewWellPointValueMap::~CNewWellPointValueMap()
{

}

//////////////////////////////////////////////////////////////////////

const CDoubleQuantity& CNewWellPointValueMap::QuantityIndicator()
{
  return *m_pQuantity;
}

//////////////////////////////////////////////////////////////////////


geo::CValue CNewWellPointValueMap::GetItemByTMD(const double & TMD) const
{
  CNewWellPoint * StartPoint = NULL;
  CNewWellPoint * EndPoint = NULL;
  
  double Value = 0;

  CNewWellPoint::GetUpperLower ( *m_pList, TMD, StartPoint, EndPoint);
  
  if (!StartPoint || !EndPoint)
    return geo::CValue();

  if (!GetItem(EndPoint).Valid())
    return geo::CValue();

  if (StartPoint == EndPoint)
    return geo::CValue(GetItem(EndPoint).Value());
  
  double DeltaY = GetItem(EndPoint).Value() - GetItem(StartPoint).Value();
  double DeltaX = EndPoint->TMD() - StartPoint->TMD();

  Value = (DeltaY / DeltaX) * (TMD - StartPoint->TMD()) + GetItem(StartPoint).Value();	

  return geo::CValue(Value);
}

CNewWellPointValueMap& CNewWellPointValueMap::operator=(const CNewWellPointValueMap& rhs)
{	
  CNewWellPointMapper<geo::CValue>::operator =(rhs);
  return *this;
}
