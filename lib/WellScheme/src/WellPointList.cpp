// CWellPointList.cpp: implementation of the CWellPointList class.
//
//////////////////////////////////////////////////////////////////////

#include <QList> 
#include <QVector>
#include <cmath>
#include "WellPoint.h"
#include "WellPointList.h"
#include "WellDefinitionPointList.h"
#include "wellpathbase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well {


CWellPointList::CWellPointList()
{
	m_Description = "No Description";
	m_pWellPath = 0;
	AutoDelete(true);
	m_bAllowDuplicateTMD = false;
}
//##ModelId=3F7AA15603B6
CWellPointList::CWellPointList(const CWellPathBase& WellPath, bool bAutoDelete /*=true*/, bool bAllowDuplicateTMD /* = false */ )
{
	m_Description="No Description";
	m_pWellPath= &WellPath;
	AutoDelete(bAutoDelete);
	m_bAllowDuplicateTMD = bAllowDuplicateTMD;
}

//##ModelId=3F7AA15603B9
CWellPointList::~CWellPointList()
{
	clear();
}

CWellPointList::CWellPointList(const CWellPointList& rhs)
: QObject(),  // QObject(const QObject&) is private,
              // hence QObject(QObject* parent = 0)
  m_MaxPoint(rhs.m_MaxPoint),
  m_MinPoint(rhs.m_MinPoint),
  m_pWellPath(rhs.m_pWellPath),
  m_lstPoints(rhs.m_lstPoints),
  m_bAutoDelete(rhs.m_bAutoDelete),
  m_bAllowDuplicateTMD(rhs.m_bAllowDuplicateTMD)
{
}



CWellPointList& CWellPointList::operator=(const CWellPointList& rhs)
{
	m_Description=rhs.m_Description;
	m_pWellPath= rhs.m_pWellPath;
	m_MaxPoint=rhs.m_MaxPoint;
	m_MinPoint=rhs.m_MinPoint;
  m_lstPoints = rhs.m_lstPoints;
  m_bAutoDelete = rhs.m_bAutoDelete;
  m_bAllowDuplicateTMD = rhs.m_bAllowDuplicateTMD;
	return *this;
}


//##ModelId=3F7AA15603D1
bool CWellPointList::AutoDelete() const
{
	return m_bAutoDelete;
}

void CWellPointList::AutoDelete(bool autodelete)
{
	m_bAutoDelete = autodelete;
}

bool CWellPointList::AllowDuplicateTMD() const
{
	return m_bAllowDuplicateTMD;
}

void CWellPointList::AllowDuplicateTMD(bool allowduplicate)
{
	m_bAllowDuplicateTMD = allowduplicate;
}

//##ModelId=3F7AA15603C0
int CWellPointList::NrOfPoints() const
{
	return m_lstPoints.size();
}

//##ModelId=3F7AA15603D9
bool CWellPointList::AllPointsDefined() const
{
  ConstIterator it;
  for(it = m_lstPoints.begin(); it != m_lstPoints.end(); ++it)
  {
    if(!(*it)->Defined())
      return false;
  }

  return true;
}

void CWellPointList::CalculateMaxMin() const
{
  ConstIterator it;
  for(it = m_lstPoints.begin(); it != m_lstPoints.end(); ++it)
  {
    m_MaxPoint = (*it)->Max(m_MaxPoint);
    m_MinPoint = (*it)->Min(m_MinPoint);
  }
}


geo::CPoint CWellPointList::MaxPoint() const
{
	if(m_MaxPoint.Empty())
		CalculateMaxMin();

	return m_MaxPoint;
		
}

geo::CPoint CWellPointList::MinPoint() const
{
	if(m_MinPoint.Empty())
		CalculateMaxMin();

	return m_MinPoint;

}

QList<CWellPoint*> CWellPointList::GetUpperLower(const double & TMD) const 
{
	CWellPoint * MinPoint = NULL;
	CWellPoint * MaxPoint = NULL;
	QList<CWellPoint*> List;
	double StartTMD = -DBL_MAX;
	double EndTMD = DBL_MAX;
	
	ConstIterator it = m_lstPoints.begin();
  CWellPoint *CurrentPoint = 0;
	
	//calculate the min max
	if(it != m_lstPoints.end()) 
	{
		StartTMD = (*it)->TMD().Value();
    CurrentPoint = *it;
	}

	while(it != m_lstPoints.end())
	{
    CurrentPoint = *it;
		if (fabs(CurrentPoint->TMD().Value() - TMD) < EPS)
		{
			List.insert(0,CurrentPoint);
			List.insert(1,CurrentPoint);
			return List;
		}
		
		//calculate the maximum point under this TMD
		if (CurrentPoint->TMD().Value() < TMD &&  CurrentPoint->TMD().Value() >= StartTMD)
		{
			MinPoint = CurrentPoint;
			StartTMD = CurrentPoint ->TMD().Value();
		}
	
		if (CurrentPoint->TMD().Value() > TMD &&  CurrentPoint->TMD().Value() <= EndTMD)
		{
			MaxPoint = CurrentPoint;
			EndTMD = CurrentPoint ->TMD().Value();
		}

		if(MaxPoint && MinPoint)
			break;

		++it;	
	}
	
	List.insert(0,MinPoint);
	List.insert(1,MaxPoint);

	return List;
}
	
void CWellPointList::clear()
{
  if(m_bAutoDelete)
  {
    Iterator it;
    for(it = m_lstPoints.begin(); it != m_lstPoints.end(); ++it)
      delete *it;
  }

  m_lstPoints.clear();
}

void CWellPointList::RemoveUnDefinedPoints()
{
	QVector<well::CWellPoint*> points_to_remove;

	Iterator it = m_lstPoints.begin();
  CWellPoint *point;
  while(it != m_lstPoints.end())
	{
    point = *it;
		if(!point->Defined())
		{
			points_to_remove.resize(points_to_remove.size()+1);
			points_to_remove.insert(points_to_remove.count(),point);
		}
		++it;
	}

	for(int i = 0; i < points_to_remove.count(); i++)
	{
		RemovePoint(points_to_remove[i]);
	}
}

//##ModelId=3F7AA15603E5
CWellPointList* CWellPointList::This() const
{
	return const_cast<CWellPointList*>(this);
}

//##ModelId=3F7AA15603D5
int CWellPointList::GetIndex(const CWellPoint* point) const
{
  return m_lstPoints.indexOf(const_cast<CWellPoint*>(point));
}

bool CWellPointList::EqualPointExist(const CWellPoint& point) const
{
  return (GetIndex(&point) != -1);
}

bool CWellPointList::Exist(const double& tmd) const
{
	return EqualPointExist(well::CWellPoint(WellPath(),tmd)); 
}

bool CWellPointList::Exist(const CWellPoint* point) const
{
	return !(GetIndex(point)==-1);
}


CWellPointList::Iterator CWellPointList::GetIteratorAt(const CWellPoint* point) const
{
	Iterator ret;
  for(ret = This()->m_lstPoints.begin(); ret != This()->m_lstPoints.end(); ++ret)
  {
    if(*ret == point)
      return ret;
  }

	return This()->m_lstPoints.end();
}

CWellPointList::Iterator CWellPointList::begin() const
{
	return This()->m_lstPoints.begin();
}

CWellPointList::Iterator CWellPointList::end() const
{
  return This()->m_lstPoints.end();
}

CWellPoint* CWellPointList::First() const
{
  if(m_lstPoints.empty())
    return 0;

	return m_lstPoints.first();
}

CWellPoint* CWellPointList::Last() const
{
  if(m_lstPoints.empty())
    return 0;

	return m_lstPoints.last();
}

CWellPoint* CWellPointList::At(int index)
{
  if(index >= 0 && index < m_lstPoints.size())
	  return m_lstPoints.at(index);

  return 0;
}

const CWellPoint* CWellPointList::At(int index) const
{
  if(index >= 0 && index < m_lstPoints.size())
  	return This()->m_lstPoints.at(index);

  return 0;
}

int CWellPointList::AddPoint(const CWellPoint* point)
{

	if(m_lstPoints.indexOf(const_cast<CWellPoint*>(point)) != -1 && !m_bAllowDuplicateTMD)
	{
		return -1;
	}

	m_MaxPoint=geo::CPoint();
	m_MinPoint=geo::CPoint();

  Iterator it = m_lstPoints.end();
  for(it = m_lstPoints.begin(); it != m_lstPoints.end(); ++it)
  {
    int comp = compareItems(*it, const_cast<CWellPoint*>(point));
    if(comp == 0)
      return -1;
    if(comp > 0)
      break;
  }

  m_lstPoints.insert(it, const_cast<CWellPoint*>(point));

	return m_lstPoints.indexOf(const_cast<CWellPoint*>(point));
}

//##ModelId=3F7AA15603D8
void CWellPointList::RemoveAllPoints()
{
	m_MaxPoint=geo::CPoint();
	m_MinPoint=geo::CPoint();

	clear();
}

void CWellPointList::RemovePoint(int index)
{
	m_MaxPoint=geo::CPoint();
	m_MinPoint=geo::CPoint();

	m_lstPoints.removeAt(index); 
}

void CWellPointList::RemovePoint(const CWellPoint* point)
{
  int idx = m_lstPoints.indexOf(const_cast<CWellPoint*>(point));
  if(idx >= 0)
  {
	  m_MaxPoint=geo::CPoint();
	  m_MinPoint=geo::CPoint();

	  m_lstPoints.removeAt(idx);
  }
}


//##ModelId=3F7AA15603E7
int CWellPointList::compareItems(CWellPoint* item1, CWellPoint* item2 )
{

	CWellPoint* p1 = (CWellPoint*)(item1);
	CWellPoint* p2 = (CWellPoint*)(item2);

	if(*p1 == *p2)
		return 0;

	if(*p1 > *p2)
		return 1;

	return -1;

}


//##ModelId=3F7AA15603DB
//CWellPathBase& CWellPointList::WellPath()
//{
//	return *m_pWellPath;
//}


const CWellPathBase& CWellPointList::WellPath() const
{
	return *m_pWellPath;
}

void CWellPointList::WellPath(CWellPathBase *pWellPath) 
{
	m_pWellPath = pWellPath;
}

void CWellPointList::InsertNonDuplicatePoints(const CWellPointList& list)
{
	Iterator it=list.begin();
  while ( it != list.end() )
	{
    CWellPoint *point=*it;
        
		if(!EqualPointExist(*point))
		{
			AddPoint(const_cast<well::CWellPoint*>(point));
		}
		++it;
  }
}

void CWellPointList::RemoveNonDuplicatePoints(const CWellPointList& list)
{

	QVector<well::CWellPoint*> points_to_remove;

	Iterator it = begin();
  while(it != end())
	{
    CWellPoint *point = *it;
		if(!list.EqualPointExist(*point))
		{
			points_to_remove.resize(points_to_remove.size()+1);
			points_to_remove.insert(points_to_remove.count(),point);
		}
		++it;
	}

	for(int i=0;i<points_to_remove.count();i++)
	{
		RemovePoint(points_to_remove[i]);
	}
}


void CWellPointList::RemoveDuplicatePoints(const CWellPointList& list)
{

	QVector<well::CWellPoint*> points_to_remove;

	Iterator it = begin();
  while(it != end())
	{
    CWellPoint *point = *it;
		if(list.EqualPointExist(*point))
		{
			points_to_remove.resize(points_to_remove.size()+1);
			points_to_remove.insert(points_to_remove.count(),point);
		}
		++it;
	}

	for(int i=0;i<points_to_remove.count();i++)
	{
		RemovePoint(points_to_remove[i]);
	}
}

QString CWellPointList::GetDescription() const
{
	return m_Description;
}

void CWellPointList::SetDescription(QString description) 
{
	m_Description=description;
}


}//end namespace
