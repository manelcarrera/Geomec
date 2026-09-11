/********************************************************************

  RescueWellboreSurface.cpp

  The record of an intersect between a wellbore and a surface.

  Rod Hanks,  January 1997

*********************************************************************/
#include "RescueWellboreSurface.h"
#include "RescueWellbore.h"
#include "RescueIJSurface.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueWellboreSurface::RescueWellboreSurface(RescueWellbore *wellboreIn, 
                                     RescueIJSurface *surfaceIn,
                                     RESCUEFLOAT uIn,
                                     RESCUEFLOAT vIn,
                                     RESCUEFLOAT mdIn)
                                     :RescueObject(wellboreIn->ParentModel()->Context())
                                     ,wellbore(wellboreIn)
                                     ,surface(surfaceIn)
                                     ,u(uIn)
                                     ,v(vIn)
                                     ,md(mdIn)
{
  isA = R_RescueWellboreSurface;
  (*wellbore->surfaceIntersections) += this;
  surface->AddWellboreSurface(this);
}

RescueWellboreSurface::RescueWellboreSurface(RescueContext *context, FILE *archiveFile)
                      :RescueObject(context)
                      ,wellbore(0)
                      ,surface(0)
{
  myfscanf(context, archiveFile, &surfaceID);
  myfscanf(context, archiveFile, &u);
  myfscanf(context, archiveFile, &v);
  myfscanf(context, archiveFile, &md);
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
  }
  }
}

void RescueWellboreSurface::Relink(RescueObject *object)
{
  wellbore = (RescueWellbore *) object;
  if (wellbore->ParentModel()->Context()->ReadFileVersion() < 23)
  {
  RescueSurface *parentSurface = ((RescueWellbore *) object)->ParentModel()->SurfaceIdentifiedBy(surfaceID);
  surface = parentSurface->NthIJSurface(0);
  }
  else
  {
  surface = ((RescueWellbore *) object)->ParentModel()->IJSurfaceIdentifiedBy(surfaceID);
  }
  if (surface != 0)
  {
  surface->AddWellboreSurface(this);
  }
}

void RescueWellboreSurface::Archive(FILE *archiveFile)
{
  RescueContext *context = wellbore->ParentModel()->Context();
  if (context->FileVersion() >= 23)
  {
  myfprintf(context, archiveFile, surface->Identifier());
  }
  else
  {
  myfprintf(context, archiveFile, surface->ParentSurface()->Identifier());
  }
  myfprintf(context, archiveFile, u);
  myfprintf(context, archiveFile, v);
  myfprintf(context, archiveFile, md);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueWellboreSurface::Swap(bool swapI, RescueGridAxis *iAxis, bool swapJ, RescueGridAxis *jAxis)
{
  if (swapI)
  {
  RESCUEFLOAT axisBegin = (RESCUEFLOAT) iAxis->LowBound64();
  RESCUEFLOAT axisEnd = (RESCUEFLOAT) iAxis->LowBound64() + (iAxis->Count64() - 1);
  u = axisEnd - (u - axisBegin);
  }
  if (swapJ)
  {
  RESCUEFLOAT axisBegin = (RESCUEFLOAT) jAxis->LowBound64();
  RESCUEFLOAT axisEnd = (RESCUEFLOAT) jAxis->LowBound64() + (jAxis->Count64() - 1);
  v = axisEnd - (v - axisBegin);
  }
}

RescueWellboreSurface::~RescueWellboreSurface()
{
/*
  Note we can drop ourselves from the surface automatically, it
  has a cBag relationship to us.  Don't try that with wellbore,
  it has a cSet relationship, so you will get into a hard loop.
*/
  surface->DropWellboreSurface(this);
}

RESCUEBOOL RescueWellboreSurface::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueWellboreSurface)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}



