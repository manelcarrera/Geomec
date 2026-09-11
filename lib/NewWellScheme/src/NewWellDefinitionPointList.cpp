
#include "NewWellDefinitionPointList.h"
#include "NewWellPathBase.h"

CNewWellDefinitionPointList::CNewWellDefinitionPointList(
  CNewWellPathBase & WellPath)
: CNewWellPointList(WellPath,true)
{
  m_Description = "Definition points";
}

CNewWellDefinitionPointList::~CNewWellDefinitionPointList()
{
}

long CNewWellDefinitionPointList::RemoveRedundantPoints()
{
  CNewWellPointList points_to_remove(WellPath(), false);

  Iterator it = begin();

  CNewWellPoint* p1 = 0;
  CNewWellPoint* p2 = 0;
  CNewWellPoint* p3 = 0;

  if (it != end())
  p1 = *it++;
  if (it != end())
  p2 = *it++;
  if (it != end())
  p3 = *it;

  const double eps = 0.1;

  while (p1 && p2 && p3)
  {
  double eas, nor, tvd, azi, inc;
    
  CNewWellPathBase::GetBetweenPosition(*p1, *p3, p2->TMD(), eas, nor, tvd,
      azi, inc);

  if (fabs(eas - p2->Easting()) < eps &&
      fabs(nor - p2->Northing()) < eps &&
      fabs(tvd - p2->TVD()) < eps )

  {
      points_to_remove.AddPoint(p2);
      p2 = p3;
      ++it;
      if (it != end())
    p3 = *(it);
      else
    p3 = 0;
  }
  else
  {
      p1 = p3;
      ++it;
      if (it != end())
    p2 = *(it);
      else
    p2 = 0;

      ++it;
      if(it != end())
    p3 = *(it);
      else
    p3 = 0;
  }
  }

  RemoveDuplicatePoints(points_to_remove);

  return points_to_remove.NrOfPoints();
}
