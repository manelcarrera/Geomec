// WellDefinitionPointList.cpp: implementation of the CWellDefinitionPointList class.
//
//////////////////////////////////////////////////////////////////////
#include "WellDefinitionPointList.h"
#include "WellPoint.h"
#include "WellPointList.h"
#include "wellpathbase.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well {

// ##ModelId=3F7AA15701DC
CWellDefinitionPointList::CWellDefinitionPointList(CWellPathBase &WellPath) : CWellPointList(WellPath, true) {
  m_Description = "Definition points";
}

// ##ModelId=3F7AA15701DE
CWellDefinitionPointList::~CWellDefinitionPointList() {}

long CWellDefinitionPointList::RemoveRedundantPoints() {
  CWellPointList points_to_remove(WellPath(), false);

  Iterator it = begin();

  CWellPoint *p1 = 0;
  CWellPoint *p2 = 0;
  CWellPoint *p3 = 0;

  if (it != end())
    p1 = *it++;
  if (it != end())
    p2 = *it++;
  if (it != end())
    p3 = *it;

  const double eps = 0.1; // EPS;
  while (p1 && p2 && p3) {
    double eas, nor, tvd, azi, inc;

    CWellPathBase::GetBetweenPosition(*p1, *p3, p2->TMD().Value(), eas, nor, tvd, azi, inc);

    if (fabs(eas - p2->Easting().Value()) < eps && fabs(nor - p2->Northing().Value()) < eps &&
        fabs(tvd - p2->TVD().Value()) < eps)

    {
      points_to_remove.AddPoint(p2);
      p2 = p3;
      ++it;
      if (it != end())
        p3 = *(it);
      else
        p3 = 0;
    } else {
      p1 = p3;
      ++it;
      if (it != end())
        p2 = *(it);
      else
        p2 = 0;

      ++it;
      if (it != end())
        p3 = *(it);
      else
        p3 = 0;
    }
  }

  RemoveDuplicatePoints(points_to_remove);
  return points_to_remove.NrOfPoints();
}

// long CWellDefinitionPointList::RemoveRedundantPoints()
//{
//	CWellPointList points_to_remove(WellPath(),false);
//	CWellPointList all_points_to_remove(WellPath(),false);
//
//	Iterator it = GetIterator();
//
//	CWellPoint* p1 = it.current();
//	CWellPoint* p2 = ++it;
//	CWellPoint* p3 = ++it;
//	bool point_added=false;
//	const double eps = EPS;
//	while(p1 && p2 && p3)
//	{
//
//		point_added=false;
//		double eas,nor,tvd,azi,inc;
//
//		CWellPathBase::GetBetweenPosition(	*p1,
//										*p3,
//										p2->TMD().Value(),
//										eas,
//										nor,
//										tvd,
//										azi,
//										inc);
//
//		if(	fabs(eas - p2->Easting().Value()) < eps &&
//			fabs(nor - p2->Northing().Value()) < eps &&
//			fabs(tvd - p2->TVD().Value()) < eps )
//
//		{
//			point_added=true;
//			points_to_remove.AddPoint(p2);
//			Iterator it2 = points_to_remove.GetIterator();
//			CWellPoint* p_check = it2;
//
//			while(p_check != p2)
//			{
//
//				CWellPathBase::GetBetweenPosition(	*p1,
//												*p3,
//												p_check->TMD().Value(),
//												eas,
//												nor,
//												tvd,
//												azi,
//												inc);
//
//				if(	fabs(eas - p_check->Easting().Value()) < eps &&
//					fabs(nor - p_check->Northing().Value()) < eps &&
//					fabs(tvd - p_check->TVD().Value()) < eps)
//				{
//
//
//				}
//				else
//				{
//					points_to_remove.RemovePoint(p2);
//					point_added=false;
//					break;
//				}
//				p_check=++it2;
//			}
//
//		}
//
//		if(point_added)
//		{
//			p2 = p3;
//			p3 = ++it;
//		}
//		else
//		{
//			p1 = p3;
//			p2 = ++it;
//			p3 = ++it;
//			all_points_to_remove.InsertNonDuplicatePoints(points_to_remove);
//			points_to_remove.RemoveAllPoints();
//		}
//
//
//	}
//
//	RemoveDuplicatePoints(all_points_to_remove);
//	return all_points_to_remove.NrOfPoints();
// }

} // namespace well
