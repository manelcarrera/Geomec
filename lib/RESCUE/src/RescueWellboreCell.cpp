/********************************************************************

  RescueWellboreCell.cpp

  The record of an intersect between a grid cell and a surface.

  Rod Hanks,  January 1997

*********************************************************************/
#include "RescueWellboreCell.h"
#include "RescueGeometry.h"
#include "RescueSurface.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueWellboreCell::RescueWellboreCell(RescueGeometry *geometryIn, 
                                     RescueWellbore *wellboreIn,
                                     RESCUEINT64 cellNumberIn)
                                     :RescueObject(geometryIn->ParentModel()->Context())
                                     ,wellbore(wellboreIn)
                                     ,geometry(geometryIn)
                                     ,cellNumber(cellNumberIn)
{
  isA = R_RescueWellboreCell;
  (*wellbore->cellIntersections) += this;
}

RescueWellboreCell::RescueWellboreCell(RescueContext *context, FILE *archiveFile)
                      :RescueObject(context)
                      ,wellbore(0)
                      ,geometry(0)
{
  myfscanf(context, archiveFile, &wellboreID);
  myfscanf(context, archiveFile, &cellNumber);
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

void RescueWellboreCell::Relink(RescueObject *object)
{
  geometry = (RescueGeometry *) object;
  wellbore = geometry->ParentModel()->WellboreIdentifiedBy(wellboreID);
  (*wellbore->cellIntersections) += this;
}

void RescueWellboreCell::Archive(FILE *archiveFile)
{
  RescueContext *context = geometry->ParentModel()->Context();
  myfprintf(context, archiveFile, wellbore->Identifier());
  myfprintf(context, archiveFile, cellNumber);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueWellboreCell::~RescueWellboreCell()
{
  (*wellbore->cellIntersections) -= this;
}

void RescueWellboreCell::CellIndex(RescueGeometry *geometryIn, RESCUEINT64 *returnArray, RESCUEINT64 arraySize)
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

void RescueWellboreCell::CellIndex(RescueGeometry *geometryIn, RESCUEINT32 &i, RESCUEINT32 &j, 
                                   RESCUEINT32 &k, bool throwIfTooBig)
{
  RESCUEINT64 iPrime;
  RESCUEINT64 jPrime;
  RESCUEINT64 kPrime;
  CellIndex(geometryIn, iPrime, jPrime, kPrime);
  i = RescueContext::Return32For64(iPrime, throwIfTooBig);
  j = RescueContext::Return32For64(jPrime, throwIfTooBig);
  k = RescueContext::Return32For64(kPrime, throwIfTooBig);
}

void RescueWellboreCell::CellIndex(RescueGeometry *geometryIn, RESCUEINT32 *returnArray, 
                                   RESCUEINT32 arraySize, bool throwIfTooBig)
{
  RESCUEINT32 i;
  RESCUEINT32 j;
  RESCUEINT32 k;
  CellIndex(geometryIn, i, j, k, throwIfTooBig);
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

void RescueWellboreCell::CellIndex(RescueGeometry *geometryIn, RESCUEINT64 &i, RESCUEINT64 &j, RESCUEINT64 &k)
{
  RESCUEINT64 maxI = geometryIn->Grid()->Axis(0)->Count64();
  RESCUEINT64 maxJ = geometryIn->Grid()->Axis(1)->Count64();
  k = cellNumber / (maxI * maxJ);
  RESCUEINT64 remainder = cellNumber - (k * maxI * maxJ);
  j = remainder / maxI;
  i = remainder - (j * maxI);
}

RESCUEBOOL RescueWellboreCell::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueWellboreCell)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}




