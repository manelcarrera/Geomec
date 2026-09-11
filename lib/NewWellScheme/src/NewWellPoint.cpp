
#include "NewWellPoint.h"
#include "NewWellPathBase.h"
#include "IBody.h"
#include "Vector.h"

const double CNewWellPoint::m_dEpsilon = 1e-4;

CNewWellPoint::CNewWellPoint
( const CNewWellPathBase& wellPath
, const double& tmd
)
: m_pFormation(0)
, m_pWellPath(&wellPath)
, m_dTMD( tmd)
, m_dX(-1)
, m_dY(-1)
, m_dZ(-1)
{
  m_pWellPath->InitWellPoint(*this);
}

void CNewWellPoint::TMD(const double &d)
{
  m_dTMD= d;
  m_pWellPath->InitWellPoint(*this);
}

CNewWellPoint::CNewWellPoint
( const CNewWellPathBase& wellPath
, const double& northing
, const double& easting
, const double& tvd
, const double& tmd
)
: m_pFormation(0)
, m_pWellPath(&wellPath)
, m_dNorthing( northing)
, m_dEasting( easting)
, m_dTVD( tvd)
, m_dTMD( tmd)
, m_dX(-1)
, m_dY(-1)
, m_dZ(-1)
{
}

CNewWellPoint::CNewWellPoint
( const CNewWellPathBase& wellPath
, const geo::IPoint &pt
, const double& TMD
)
: m_pFormation(0)
, m_pWellPath( &wellPath)
, m_dNorthing( pt.X() - wellPath.GlobalNorthing() )
, m_dEasting ( pt.Y() - wellPath.GlobalEasting() )
, m_dTVD     ( pt.Z() - wellPath.GlobalTVD() )
, m_dTMD(TMD)
, m_dX(-1)
, m_dY(-1)
, m_dZ(-1)
{
}

CNewWellPoint::CNewWellPoint(const CNewWellPoint &rhs)
: m_pFormation(rhs.m_pFormation)
, m_pWellPath( rhs.m_pWellPath)
, m_dNorthing( rhs.Northing())
, m_dEasting( rhs.Easting())
, m_dTVD( rhs.TVD())
, m_dTMD( rhs.TMD())
, m_dX(-1)
, m_dY(-1)
, m_dZ(-1)
, m_stElements(rhs.m_stElements)
{
}

CNewWellPoint::~CNewWellPoint()
{
}

bool CNewWellPoint::Defined() const
{
   return true;
}

CNewWellPoint& CNewWellPoint::operator=(const CNewWellPoint &rhs)
{
  m_pWellPath= rhs.m_pWellPath;
  m_dNorthing=   rhs.Northing();
  m_dEasting=   rhs.Easting();
  m_dTVD=   rhs.TVD();
  m_dTMD= rhs.TMD();
  m_dX= -1;
  m_dY= -1;
  m_dZ= -1;
  m_stElements= rhs.m_stElements;
  m_pFormation= rhs.m_pFormation;
  return *this;
}

bool CNewWellPoint::operator==(const CNewWellPoint &rhs) const
{
  if ( &WellPath() !=  &rhs.WellPath() ) return false;

  if(*this < rhs)
  return false;

  if(*this > rhs)
  return false;

  return true;
  
}

bool CNewWellPoint::operator!=(const CNewWellPoint &rhs) const
{
  return !(*this == rhs);
}


bool CNewWellPoint::operator <(const CNewWellPoint &rhs) const
{
  if(m_dTMD - rhs.TMD() < -m_dEpsilon)
  return true;

  return false;
}

bool CNewWellPoint::operator >(const CNewWellPoint &rhs) const
{
  if(m_dTMD - rhs.TMD() > m_dEpsilon)
  return true;

  return false;

}

void CNewWellPoint::addElement(const geo::IElement* pElement) const
{
  m_stElements.insert(pElement);
}

void CNewWellPoint::setElements(const std::set<const geo::IElement*>& stElements) const
{
  m_stElements = stElements;
}

const std::set<const geo::IElement*>& CNewWellPoint::getElements() const
{
  return m_stElements;
}

const double &CNewWellPoint::X() const
{
  return m_dX= m_dNorthing + m_pWellPath->GlobalNorthing(); 
}

const double &CNewWellPoint::Y() const
{
  return m_dY= m_dEasting + m_pWellPath->GlobalEasting();
}

const double &CNewWellPoint::Z() const
{
  return m_dZ= m_dTVD + m_pWellPath->GlobalTVD();
}

const CNewWellPathBase& CNewWellPoint::WellPath() const
{
  return *m_pWellPath;
}

CNewWellPathBase& CNewWellPoint::WellPath()
{
  return const_cast<CNewWellPathBase&>(*m_pWellPath);
}

void CNewWellPoint::GetUpperLower
( const std::list<CNewWellPoint> &lstPoints
, const double & TMD
, const CNewWellPoint *MinPoint
, const CNewWellPoint *MaxPoint
)
{
  MinPoint = NULL;
  MaxPoint = NULL;
  std::list<CNewWellPoint> List;
  double StartTMD = -DBL_MAX;
  double EndTMD = DBL_MAX;

  std::list<CNewWellPoint>:: const_iterator it = lstPoints.begin();
  const CNewWellPoint *CurrentPoint = 0;

  //calculate the min max
  if(it != lstPoints.end()) 
  {
  StartTMD = (*it).TMD();
  CurrentPoint = &(*it);
  }

  while(it != lstPoints.end())
  {
  CurrentPoint = &(*it);
  if (fabs(CurrentPoint->TMD() - TMD) < EPS)
  {
      MinPoint= CurrentPoint;
      MaxPoint= CurrentPoint;
      return;
  }

  //calculate the maximum point under this TMD
  if (CurrentPoint->TMD() < TMD &&  CurrentPoint->TMD() >= StartTMD)
  {
      MinPoint = CurrentPoint;
      StartTMD = CurrentPoint ->TMD();
  }

  if (CurrentPoint->TMD() > TMD &&  CurrentPoint->TMD() <= EndTMD)
  {
      MaxPoint = CurrentPoint;
      EndTMD = CurrentPoint ->TMD();
  }

  if(MaxPoint && MinPoint)
  break;

  ++it;	
  }

  return;
}

bool CNewWellPoint::EqualPointExist(const std::list<CNewWellPoint> &list) const
{
  for ( std::list<CNewWellPoint>::const_iterator it= list.begin()
      ; it != list.end()
      ; ++it
      )
  {
  if ( *it == *this ) return true;
  }

  return false;
}

void CNewWellPoint::AddPoint( std::list<CNewWellPoint> &list) const
{
  list.push_back(*this);
  list.sort(CNewWellPoint::less);
}

const CFormationBase* CNewWellPoint::Formation() const
{
  return m_pFormation;
}

void CNewWellPoint::Formation(CFormationBase *formation) const
{
  m_pFormation = formation;
}

bool CNewWellPoint::less(const CNewWellPoint & first, const CNewWellPoint & second)
{
  return first < second;
}

bool CNewWellPoint::equal(const CNewWellPoint & first, const CNewWellPoint & second)
{
  return first == second;
}

void  CNewWellPoint::AzimuthInclination
( CAngleQuantity &Azimuth
, CAngleQuantity &Inclination
) const
{
  size_t index= m_pWellPath->AngleIndex(*this);
  Azimuth= m_pWellPath->Azimuth(index);
  Inclination= m_pWellPath->Inclination(index);
}

void  CNewWellPoint::AzimuthInclination
( double Azimuth
, double Inclination
)
{
  // when this 'const_cast' is not used a cascading effect regarding
  // 'const' members and functions can be observed (and has to be implemented)

  (const_cast <CNewWellPathBase*> (m_pWellPath))->AzimuthInclination(Azimuth, Inclination);
}

// waij TFS 74145
//
geo::CVector CNewWellPoint::GetDirection() const
{
  CAngleQuantity Azimuth, Inclination;
  AzimuthInclination(Azimuth, Inclination);

  double sin_i = sin(DEG2RAD(Inclination.Value()));
  double cos_i = cos(DEG2RAD(Inclination.Value()));
  double cos_a = cos(DEG2RAD(Azimuth.Value()));
  double sin_a = sin(DEG2RAD(Azimuth.Value()));

  return geo::CVector(cos_a*sin_i,sin_a*sin_i,cos_i);
}
