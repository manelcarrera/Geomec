// WellSymbolBase.cpp: implementation of the CWellSymbolBase class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "WellSymbolBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




CWellSymbolBase::CWellSymbolBase(const geo::CPoint& pos, const QString& description,bool autoDelete )
{
  m_autoDelete=autoDelete;
  m_Description=description;
  m_PivotPoint=geo::CPoint::NullPoint;
  m_Position=pos;
}


/*virtual*/ CWellSymbolBase::~CWellSymbolBase()
{
  if(m_autoDelete)
    m_Array.ClearAndDelete();
}


/*virtual*/ int CWellSymbolBase::NrOfPositions() const
{
  return 1; 
}

/*virtual*/ const geo::IPoint& CWellSymbolBase::Position(int /*Index*/) const
{
  return m_Position;
}

/*virtual*/ const geo::IArray& CWellSymbolBase::Geometry() const
{
  return m_Array; 
}

/*virtual*/ const geo::IPoint& CWellSymbolBase::PivotPoint() const
{
  return m_PivotPoint;
}

/*virtual*/ void CWellSymbolBase::Rotate(const geo::IVector &vec,const double &dAngleDeg)
{
  m_Position.Rotate( vec,dAngleDeg);
}

/*virtual*/ void CWellSymbolBase::Move(const geo::IVector &vec)
{
  m_Position.Move( vec );
}

/*virtual*/ void CWellSymbolBase::Transform(const geo::IMatrix &matrix)
{
  m_Position.Transform( matrix);
}

/*virtual*/ void CWellSymbolBase::AssertValid() const
{
  // 
}

/*virtual*/ bool CWellSymbolBase::Empty() const
{
  return false;
}

/*virtual*/ geo::CPoint CWellSymbolBase::Min() const
{
  return m_Position; 
}

/*virtual*/ geo::CPoint CWellSymbolBase::Max() const
{
  return m_Position;
}

