// WellPoint.cpp: implementation of the WellPoint class.
//
//////////////////////////////////////////////////////////////////////

#include <QList> 
#include <cmath>
#include "WellPoint.h"
#include "IWellSection.h"
#include "WellSectionList.h"
#include "WellPointList.h"
#include "WellDefinitionPointList.h"
#include "wellpathbase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well {

IWellPointFactory::~IWellPointFactory()
{
}


void CWellPoint::Connect()
{
  m_X=0;
  m_Y=0;
  m_Z=0;

  connect(	&m_TMD , 
        SIGNAL(OnChanged(const  CDoubleQuantity&)), 
        this,
        SLOT(OnQuantityChanged(const  CDoubleQuantity&)) );

  connect(	m_pWellPath , 
        SIGNAL(onGeometryChanged()), 
        this,
        SLOT(OnWellDefPointsChanged()));

}


CWellPoint::CWellPoint()
{

}

CWellPoint::CWellPoint(const CWellPathBase& WellPath, double TMD)
{
  m_pWellPath =&WellPath;
  Connect();
  m_TMD.SetValue(TMD);
}

CWellPoint::CWellPoint(const CWellPoint &rhs)
: QObject() // QObject(const QObject&) is private,
      // hence QObject(QObject* parent = 0)
, m_stElements(rhs.m_stElements)
{
  //the rest is done automatic with signals (see OnQuantityChanged)

  bool bDefined = rhs.Defined();

  m_pWellPath=rhs.m_pWellPath;
  Connect();
  m_TMD.SetValue(rhs.TMD().Value());

  if(!bDefined)
    Invalidate();
}

CWellPoint::~CWellPoint()
{
  emit OnDestroy(*this);
}



void CWellPoint::Invalidate()
{
  m_Northing.Invalidate();
  m_Easting.Invalidate();
  m_Azimuth.Invalidate();
  m_Inclination.Invalidate();
  m_TVD.Invalidate();

  //!! do not invalidate TMD because the others can be derived from TMD
  
}

void CWellPoint::OnWellDefPointsChanged()
{
  WellPath().InitWellPoint(*this); //update point
}

void CWellPoint::OnQuantityChanged(const CDoubleQuantity& pQ)
{
  if(&pQ == &m_TMD)
  {
    WellPath().InitWellPoint(*this);
    emit OnTMDChanged();
  }
  else
    assert(false);
}


bool CWellPoint::Defined() const
{
  if(m_Northing.Undefined())
    return false;
  if(m_Easting.Undefined())
    return false;
  if(m_Azimuth.Undefined())
    return false;
  if(m_Inclination.Undefined())
    return false;
  if(m_TMD.Undefined())
    return false;

  return true;
}


const CLengthQuantity& CWellPoint::Northing() const
{
  return m_Northing;
}

const CLengthQuantity& CWellPoint::Easting() const 
{
  return m_Easting;
}

const CAngleQuantity& CWellPoint::Azimuth() const 
{
  return m_Azimuth;
}

const CAngleQuantity& CWellPoint::Inclination() const 
{
  return m_Inclination;
}

CLengthQuantity& CWellPoint::TMD()
{
  return m_TMD;
}

const CLengthQuantity& CWellPoint::TMD() const 
{
  return m_TMD;
}

const CLengthQuantity& CWellPoint::TVD() const 
{
  return m_TVD;
}

CWellPoint& CWellPoint::operator=(const CWellPoint &rhs)
{
  //the rest is done automatic with signals (see OnQuantityChanged)

  assert(m_pWellPath==rhs.m_pWellPath);

  m_stElements = rhs.m_stElements;

  bool bDefined = rhs.Defined();
  m_TMD.SetValue(rhs.TMD().Value());

  if(!bDefined)
    Invalidate();

  return *this;
}


bool CWellPoint::operator==(const CWellPoint &rhs) const
{
  if(*this < rhs)
    return false;

  if(*this > rhs)
    return false;

  return true;
  
}

bool CWellPoint::operator!=(const CWellPoint &rhs) const
{
  return !(*this == rhs);
}


bool CWellPoint::operator <(const CWellPoint &rhs) const
{
  if(m_TMD.Value() - rhs.TMD().Value() < -EPS)
    return true;

  return false;
}

bool CWellPoint::operator >(const CWellPoint &rhs) const
{
  if(m_TMD.Value() - rhs.TMD().Value() > EPS)
    return true;

  return false;

}

geo::CVector CWellPoint::GetDirection() const
{
  
  if(!m_Azimuth.Undefined() && !m_Inclination.Undefined())
  {
    return CWellPathBase::GetDirection(m_Azimuth.Value(),m_Inclination.Value());
  }

  return geo::CVector();
}

void CWellPoint::addElement(const geo::IElement* pElement)
{
  assert(!pElement || pElement->Contains(*this, true));
  m_stElements.insert(pElement);
}

void CWellPoint::setElements(const std::set<const geo::IElement*>& stElements)
{
  m_stElements = stElements;
}

const std::set<const geo::IElement*>& CWellPoint::getElements() const
{
  return m_stElements;
}

/*virtual*/ 
const double &CWellPoint::X() const
{
  m_X = m_pWellPath->GlobalNorthing().Value() + m_Northing.Value();
  return m_X; 
}

/*virtual*/ 
const double &CWellPoint::Y() const
{
  m_Y = m_pWellPath->GlobalEasting().Value() + m_Easting.Value();
  return m_Y;
}

/*virtual*/ 
const double &CWellPoint::Z() const
{
  m_Z = m_pWellPath->GlobalTVD().Value() + m_TVD.Value();
  return m_Z;
}

/*virtual*/ 
void CWellPoint::AssertValid() const
{
  assert(m_pWellPath); 
}

/*virtual*/ 
bool CWellPoint::Empty() const
{
  if(m_pWellPath->GlobalNorthing().Undefined())
    return true;

  if(m_pWellPath->GlobalEasting().Undefined())
    return true;

  if(m_pWellPath->GlobalTVD().Undefined())
    return true;

  if(m_Northing.Undefined())
    return true;

  if(m_Easting.Undefined())
    return true;

  if(m_TVD.Undefined())
    return true;

  if(m_TMD.Undefined())
    return true;

  return false; 
}

const CWellPathBase& CWellPoint::WellPath() const
{
  return *m_pWellPath;
}

CWellPathBase& CWellPoint::WellPath()
{
  return const_cast<CWellPathBase&>(*m_pWellPath);
}


}
