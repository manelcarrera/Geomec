/********************************************************************

  RescueGeometryUnit.cpp

  This object, owned by RescueGeometry, records an intersect between
  a RescueGeometry which contains a global grid and a RescueUnit which
  is part of that grid.

  Rod Hanks,  June 2001

*********************************************************************/
#include "RescueGeometryUnit.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueGeometryUnit::RescueGeometryUnit(RescueGeometry *parentGeometryIn, 
                                       RescueUnit *relatedUnitIn, 
                                       RESCUEINT64 topLayerNdxIn, 
                                       RESCUEINT64 bottomLayerNdxIn)
                                       :RescueObject(parentGeometryIn->ParentModel()->Context())
                                       ,parentGeometry(parentGeometryIn)
                                       ,relatedUnit(relatedUnitIn)
                                       ,topLayerNdx(topLayerNdxIn)
                                       ,bottomLayerNdx(bottomLayerNdxIn)
{
  isA = R_RescueGeometryUnit;
  (*parentGeometry->geometryUnit) += this;
  if (relatedUnit == 0)
  {
  unitID = 0;
  }
  else
  {
  unitID = relatedUnit->Identifier();
  }
}

void RescueGeometryUnit::SetRelatedUnit(RescueUnit *unitIn)
{
  relatedUnit = unitIn;
  if (relatedUnit == 0)
  {
  unitID = 0;
  }
  else
  {
  unitID = relatedUnit->Identifier();
  }
}

RescueUnit *RescueGeometryUnit::RelatedUnit()
{
  if (relatedUnit == 0 && unitID != 0)
  {
  relatedUnit = parentGeometry->ParentModel()->UnitIdentifiedBy(unitID);
  }
  return relatedUnit;
}

void RescueGeometryUnit::Archive(FILE *archiveFile)
{
  RescueContext *context = parentGeometry->ParentModel()->Context();
  myfprintf(context, archiveFile, "; GeometryUnit");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, unitID);
  myfprintf(context, archiveFile, topLayerNdx);
  myfprintf(context, archiveFile, bottomLayerNdx);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueGeometryUnit::RescueGeometryUnit(RescueContext *context, FILE *archiveFile)
                                       :RescueObject(context)
                                       ,parentGeometry(0)
                                       ,relatedUnit(0)
                                       ,topLayerNdx(0)
                                       ,bottomLayerNdx(0)
{
  isA = R_RescueGeometryUnit;
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &unitID);
  myfscanf(context, archiveFile, &topLayerNdx);
  myfscanf(context, archiveFile, &bottomLayerNdx);
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

void RescueGeometryUnit::Relink(RescueObject *parent)
{
  parentGeometry = (RescueGeometry *) parent;
}

RESCUEBOOL RescueGeometryUnit::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueGeometryUnit)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}


