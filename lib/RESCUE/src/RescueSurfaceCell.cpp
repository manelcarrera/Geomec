/********************************************************************

  RescueSurfaceCell.cpp

  The record of an intersect between a grid cell and a surface.

  Rod Hanks,  January 1997

*********************************************************************/
#include "RescueSurfaceCell.h"
#include "RescueGeometry.h"
#include "RescueIJSurface.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueSurfaceCell::RescueSurfaceCell(RescueGeometry *geometryIn, 
                                     RescueIJSurface *surfaceIn,
                                     RESCUEINT64 cellNumberIn,
                                     RESCUEINT64 cellFaceNumberIn,
                                     SurfaceSide sideIn, 
                                     RESCUEFLOAT *uvsIn)
                                     :RescueObject(geometryIn->ParentModel()->Context())
                                     ,geometry(geometryIn)
                                     ,surface(surfaceIn)
                                     ,cellNumber(cellNumberIn)
                                     ,cellFaceNumber(cellFaceNumberIn)
                                     ,side(sideIn)
                                     ,uvs(uvsIn)
{
  isA = R_RescueSurfaceCell;
  if (surface != 0)
  {
    surface->AddSurfaceCell(this);
  }
}


RescueSurfaceCell::RescueSurfaceCell(RescueContext *context, FILE *archiveFile)
                      :RescueObject(context)
                      ,surface(0)
                      ,uvs(0)
{
  RESCUEINT64 looseInteger;

  myfscanf(context, archiveFile, &surfaceID);
  myfscanf(context, archiveFile, &cellNumber);
  myfscanf(context, archiveFile, &cellFaceNumber);
  myfscanf(context, archiveFile, &looseInteger);
  side = (SurfaceSide) looseInteger;
  myfscanf(context, archiveFile, &looseInteger);
  uvs = new RESCUEFLOAT[8];
  if (looseInteger == 8)
  {
    myfscanf(context, archiveFile, uvs, 8, FALSE);
  }
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

void RescueSurfaceCell::Relink(RescueObject *object)
{
  geometry = (RescueGeometry *) object;
  RescueContext *context = geometry->ParentModel()->Context();
  if (context->ReadFileVersion() < 23)
  {
    RescueSurface *parentSurface = geometry->ParentModel()->SurfaceIdentifiedBy(surfaceID);
    surface = parentSurface->NthIJSurface(0);
  }
  else
  {
    surface = geometry->ParentModel()->IJSurfaceIdentifiedBy(surfaceID);
  }
  surface->AddSurfaceCell(this);
}

void RescueSurfaceCell::Archive(FILE *archiveFile)
{
  RescueContext *context = this->Surface()->ParentModel()->Context();
  myfprintf(context, archiveFile, "; Surface cell");
  if (context->FileVersion() >= 23)
  {
    myfprintf(context, archiveFile, surface->Identifier());
  }
  else
  {
    myfprintf(context, archiveFile, surface->ParentSurface()->Identifier());
  }
  myfprintf(context, archiveFile, cellNumber);
  myfprintf(context, archiveFile, cellFaceNumber);
  myfprintf(context, archiveFile, (RESCUEINT64) side);
  if (uvs == 0)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
    myfprintf(context, archiveFile, (RESCUEINT64) 8);
    myfprintf(context, archiveFile, uvs, 8, FALSE);
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueSurfaceCell::~RescueSurfaceCell()
{
  if (uvs != 0)
  {
    delete [] uvs;
  }
  if (surface != 0)
  {
    surface->DropSurfaceCell(this);
  }
}

void RescueSurfaceCell::CellIndex(RescueGeometry *geometryIn, RESCUEINT32 *returnArray, RESCUEINT32 arraySize)
{
  RESCUEINT64 i;
  RESCUEINT64 j;
  RESCUEINT64 k;
  CellIndex(geometryIn, i, j, k);
  if (arraySize >= 1)
  {
    returnArray[0] = (RESCUEINT32) i;
  }
  if (arraySize >= 2)
  {
    returnArray[1] = (RESCUEINT32) j;
  }
  if (arraySize >= 3)
  {
    returnArray[2] = (RESCUEINT32) k;
  }
}

void RescueSurfaceCell::CellIndex(RescueGeometry *geometryIn, RESCUEINT64 *returnArray, RESCUEINT64 arraySize)
{
  RESCUEINT64 i;
  RESCUEINT64 j;
  RESCUEINT64 k;
  CellIndex(geometryIn, i, j, k);
  if (arraySize >= 1)
  {
    returnArray[0] = i;
  }
  if (arraySize >= 2)
  {
    returnArray[1] = j;
  }
  if (arraySize >= 3)
  {
    returnArray[2] = k;
  }
}

void RescueSurfaceCell::CellIndex(RescueGeometry *geometryIn, RESCUEINT32 &i, RESCUEINT32 &j, RESCUEINT32 &k)
{
  RESCUEINT64 maxI = geometryIn->Grid()->Axis(0)->Count64();
  RESCUEINT64 maxJ = geometryIn->Grid()->Axis(1)->Count64();
  k = (RESCUEINT32) (cellNumber / (maxI * maxJ));
  RESCUEINT64 remainder = cellNumber - (k * maxI * maxJ);
  j = (RESCUEINT32) (remainder / maxI);
  i = (RESCUEINT32) (remainder - (j * maxI));
}

void RescueSurfaceCell::CellIndex(RescueGeometry *geometryIn, RESCUEINT64 &i, RESCUEINT64 &j, RESCUEINT64 &k)
{
  RESCUEINT64 maxI = geometryIn->Grid()->Axis(0)->Count64();
  RESCUEINT64 maxJ = geometryIn->Grid()->Axis(1)->Count64();
  k = cellNumber / (maxI * maxJ);
  RESCUEINT64 remainder = cellNumber - (k * maxI * maxJ);
  j = remainder / maxI;
  i = remainder - (j * maxI);
}

void RescueSurfaceCell::Swap(bool swapI, RescueGridAxis *iAxis, bool swapJ, RescueGridAxis *jAxis)
{
  if (uvs != 0)
  {
    if (swapI)
    {
      RESCUEFLOAT axisBegin = (RESCUEFLOAT) iAxis->LowBound64();
      RESCUEFLOAT axisEnd = (RESCUEFLOAT) iAxis->LowBound64() + (iAxis->Count64() - 1);
      RESCUEINT64 loop;
      for (loop = 0; loop < 4; loop++)
      {
        uvs[loop] = axisEnd - (uvs[loop] - axisBegin);
      }
    }
    if (swapJ)
    {
      RESCUEFLOAT axisBegin = (RESCUEFLOAT) jAxis->LowBound64();
      RESCUEFLOAT axisEnd = (RESCUEFLOAT) jAxis->LowBound64() + (jAxis->Count64() - 1);
      RESCUEINT64 loop;
      for (loop = 0; loop < 4; loop++)
      {
        uvs[loop + 4] = axisEnd - (uvs[loop + 4] - axisBegin);
      }
    }
  }
}

RESCUEBOOL RescueSurfaceCell::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueSurfaceCell)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}





