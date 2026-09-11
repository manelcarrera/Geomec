/********************************************************************

  RescueSplineCoef.cpp

  Spline Coefficients to support RescuePillar.

  Rod Hanks,  October 2003

*********************************************************************/
#include "RescueSplineCoef.h"
#include "cSetRescuePoint.h"
#include "RescuePoint.h"
#include <limits.h>

RESCUEFLOAT RescueSplineCoef::interpolate(RESCUEFLOAT zmin, RESCUEFLOAT dz) const
{
  return zmin + (b + (c + d * dz) * dz) * dz;
}

RESCUEFLOAT RescueSplineCoef::derive(RESCUEFLOAT dz) const
{
  return b + (RESCUEFLOAT(2) * c + RESCUEFLOAT(3) * d * dz) * dz;
}

RESCUEINT64 RescueSplineCoef::getNextBarrierIndex(cSetRescuePoint *points, RESCUEINT64 prev_index)
{
  RESCUEINT64 size = points->Count();
  if (prev_index == INT_MAX) return INT_MAX;
  if (prev_index >= size - 1) return INT_MAX;
  RESCUEINT64 loop;
  for (loop = prev_index + 1; loop < size - 1; loop++)
  {
  RescuePoint *point = points->NthObject(loop);
  RescuePoint *prevPoint = points->NthObject(loop - 1);
  RescuePoint *nextPoint = points->NthObject(loop + 1);
  RESCUEFLOAT z = point->z;
  if (z == prevPoint->z || z == nextPoint->z)
  {
      return loop;
  }
  }
  return size - 1;
}

void RescueSplineCoef::rebuild(RescueSplineCoef coeffs[], 
                               cSetRescuePoint *points,
                               RESCUEINT64 index)
{
  RESCUEFLOAT h[4], b[4], z[5], u[4], v[4];
  RESCUEINT64 imin = 0;
  RESCUEINT64 imax = getNextBarrierIndex(points, imin);

  while (imin != INT_MAX && imax != INT_MAX)
  {
  RescuePoint *minPoint = points->NthObject(imin);
  RescuePoint *maxPoint = points->NthObject(imax);
  if (minPoint->z == maxPoint->z)
  {
      RESCUEINT64 loop;
      for (loop = imin; loop < imax; loop++)
      {
    coeffs[loop].b = 0;
    coeffs[loop].c = 0;
    coeffs[loop].d = 0;
      }
  }
  else
  {
      RescuePoint *thisPoint = points->NthObject(imin);
      RESCUEINT64 loop;
      for (loop = imin; loop < imax; loop++)
      {
    RescuePoint *nextPoint = points->NthObject(loop + 1);
    h[loop] = 1;
    RESCUEFLOAT aP = (*nextPoint)[index];
    RESCUEFLOAT bP = (*thisPoint)[index];
    b[loop] = (aP - bP)/h[loop];
    thisPoint = nextPoint;
      }
      u[imin] = 0;
      v[imin] = 0;
      if (imin+1 < imax)
      {
    u[imin+1] = 2 * (h[imin]+h[imin+1]);
    v[imin+1] = 6 * (b[imin+1]-b[imin]);
      }
      for (loop = imin + 2; loop < imax; loop++)
      {
    u[loop] = 2 * (h[loop] + h[loop - 1]) - h[loop - 1] * h[loop - 1] / u[loop - 1];
    v[loop] = 6 * (b[loop] - b[loop - 1]) - h[loop - 1] * v[loop - 1] / u[loop - 1];
      }
      z[imin] = 0;
      z[imax] = 0;
      for (loop = imax - 1; loop > imin; loop--)
      {
    z[loop] = (v[loop] - h[loop] * z[loop + 1]) / u[loop];
      }
      for (loop = imin; loop < imax; loop++)
      {
    coeffs[loop].b = -h[loop] * (z[loop + 1] / RESCUEFLOAT(6) + z[loop] / RESCUEFLOAT(3)) + b[loop];
    coeffs[loop].c = z[loop] / 2;
    coeffs[loop].d = (z[loop + 1] - z[loop]) / (RESCUEFLOAT(6) * h[loop]);
      }
  }
  imin = imax;
  imax = getNextBarrierIndex(points, imax);
  }
}

void RescueSplineCoef::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, b);
  myfprintf(context, archiveFile, c);
  myfprintf(context, archiveFile, d);
}

void RescueSplineCoef::UnArchive(RescueContext *context, FILE *archiveFile)
{
  myfscanf(context, archiveFile, &b);
  myfscanf(context, archiveFile, &c);
  myfscanf(context, archiveFile, &d);
}


