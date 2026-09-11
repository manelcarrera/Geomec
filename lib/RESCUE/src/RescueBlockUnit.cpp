/********************************************************************

  RescueBlockUnit.cpp

  block unit for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueBlockUnit.h"
#include "RescueBlock.h"
#include "RescueUnit.h"
#include "RescueBlockUnitHorizonSurface.h"
#include "RescueModel.h"
#include "RescueReferenceSurface.h"
#include "RescueMacroVolume.h"
#include "RescueVertex.h"
#include "RescueGeobodyPart.h"
#include "RescueHistory.h"
#include "RescueColor.h"
#include "RescueBuffer.h"
#include <string.h>

void RescueBlockUnit::DropGeometry(RescueGeometry *toDrop)
{
  toDrop->RescueDeleteFile();
  (*gridGeometries) -= toDrop;
}

RescueBlockUnitSide *RescueBlockUnit::BlockUnitSideIdentifiedBy(RESCUEINT64 id)
{
  RescueBlockUnitSide *myReturn = 0;
  RESCUEINT64 ordinal = 0;
  RescueMacroVolume *volume = NthMacroVolume(ordinal++);
  while (volume != 0 && myReturn == 0)
  {
    myReturn = volume->BlockUnitSideIdentifiedBy(id);
    volume = NthMacroVolume(ordinal++);
  }
  return myReturn;
}

RescueProperty *RescueBlockUnit::PropertyIdentifiedBy(RESCUEINT64 id)
{
  RescueProperty *myReturn = 0;
  RESCUEINT64 ordinal = 0;
  RescueGeometry *geometry = GridGeometry(ordinal++);
  while (geometry != 0 && myReturn == 0)
  {
    myReturn = geometry->PropertyIdentifiedBy(id);
    geometry = GridGeometry(ordinal++);
  }
  return myReturn;
}

RESCUEBOOL RescueBlockUnit::DeletePropertyGroup(RescuePropertyGroup *existingPropertyGroup)
{
  unit->ParentModel()->DeletePropertyGroup(existingPropertyGroup);
  return ((*propertyGroups) -= existingPropertyGroup);
}

RescueBlockUnit::RescueBlockUnit(RescueBlock *parentBlock,
                                 RescueUnit *parentUnit)
                                :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                ,block(parentBlock)
                                ,unit(parentUnit)
                                ,surfaceAboveMe(0)
                                ,surfaceBelowMe(0)
                                ,macroVolumes(0)
                                ,propertyGroups(0)
                                ,propertyGroupsID(0)
                                ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueCoordinateSystem::Orientation orientation,
                                 RescueBlock *parentBlock,
                                RescueUnit *parentUnit,
                                RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                                RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                                RESCUEFLOAT k_origin, RESCUEFLOAT k_step,
                                RESCUEINT64 k_lowbound, RESCUEINT64 k_count,
                                RESCUEFLOAT missingValue)
                                :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                ,block(parentBlock)
                                ,unit(parentUnit)
                                ,surfaceAboveMe(0)
                                ,surfaceBelowMe(0)
                                ,macroVolumes(0)
                                ,propertyGroups(0)
                                ,propertyGroupsID(0)
                                ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(parentBlock->ParentModel()->Context(),
                                             orientation,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(0),
                                             i_origin, i_step,
                                             i_lowbound, i_count,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(1),
                                             j_origin, j_step,
                                             j_lowbound, j_count,
                                             k_origin, k_step,
                                             k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  (*gridGeometries) += new RescueGeometry(parentBlock->ParentModel(), blockUnitGrid, missingValue);
  GridGeometry()->parentBlockUnit = this;
  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueCoordinateSystem::Orientation orientation,
                                 RescueBlock *parentBlock,
                                RescueUnit *parentUnit,
                                RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                RESCUEINT32 i_lowbound, RESCUEINT32 i_count,
                                RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                RESCUEINT32 j_lowbound, RESCUEINT32 j_count,
                                RESCUEFLOAT k_origin, RESCUEFLOAT k_step,
                                RESCUEINT32 k_lowbound, RESCUEINT32 k_count,
                                RESCUEFLOAT missingValue)
                                :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                ,block(parentBlock)
                                ,unit(parentUnit)
                                ,surfaceAboveMe(0)
                                ,surfaceBelowMe(0)
                                ,macroVolumes(0)
                                ,propertyGroups(0)
                                ,propertyGroupsID(0)
                                ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(parentBlock->ParentModel()->Context(),
                                             orientation,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(0),
                                             i_origin, i_step,
                                             i_lowbound, i_count,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(1),
                                             j_origin, j_step,
                                             j_lowbound, j_count,
                                             k_origin, k_step,
                                             k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  (*gridGeometries) += new RescueGeometry(parentBlock->ParentModel(), blockUnitGrid, missingValue);
  GridGeometry()->parentBlockUnit = this;
  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueCoordinateSystem::Orientation orientation,
                                 RescueBlock *parentBlock,
                                RescueUnit *parentUnit,
                                RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                                RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                                RESCUEINT64 k_lowbound, RESCUEINT64 k_count,
                                RESCUEFLOAT missingValue)
                                :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                ,block(parentBlock)
                                ,unit(parentUnit)
                                ,surfaceAboveMe(0)
                                ,surfaceBelowMe(0)
                                ,macroVolumes(0)
                                ,propertyGroups(0)
                                ,propertyGroupsID(0)
                                ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(parentBlock->ParentModel()->Context(),
                                             orientation,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(0),
                                             i_origin, i_step,
                                             i_lowbound, i_count,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(1),
                                             j_origin, j_step,
                                             j_lowbound, j_count,
                                             k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  (*gridGeometries) += new RescueGeometry(parentBlock->ParentModel(), blockUnitGrid, missingValue);
  GridGeometry()->parentBlockUnit = this;
  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueCoordinateSystem::Orientation orientation,
                                 RescueBlock *parentBlock,
                                RescueUnit *parentUnit,
                                RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                RESCUEINT32 i_lowbound, RESCUEINT32 i_count,
                                RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                RESCUEINT32 j_lowbound, RESCUEINT32 j_count,
                                RESCUEINT32 k_lowbound, RESCUEINT32 k_count,
                                RESCUEFLOAT missingValue)
                                :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                ,block(parentBlock)
                                ,unit(parentUnit)
                                ,surfaceAboveMe(0)
                                ,surfaceBelowMe(0)
                                ,macroVolumes(0)
                                ,propertyGroups(0)
                                ,propertyGroupsID(0)
                                ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(parentBlock->ParentModel()->Context(),
                                             orientation,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(0),
                                             i_origin, i_step,
                                             i_lowbound, i_count,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(1),
                                             j_origin, j_step,
                                             j_lowbound, j_count,
                                             k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  (*gridGeometries) += new RescueGeometry(parentBlock->ParentModel(), blockUnitGrid, missingValue);
  GridGeometry()->parentBlockUnit = this;
  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueCoordinateSystem::Orientation orientation,
                                 RescueBlock *parentBlock,
                                 RescueUnit *parentUnit,
                                 RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                 RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                                 RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                 RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                                 RESCUEINT64 k_lowbound, RESCUEINT64 k_count,
                                 RESCUEFLOAT missingValue,
                                 RescueReferenceSurface *referenceSurfaceIn, RESCUEFLOAT referenceOffsetIn, 
                                 RESCUEFLOAT thicknessIn, RescueGeometry::RescueLapType onOffLapIn)
                                 :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                 ,block(parentBlock)
                                 ,unit(parentUnit)
                                 ,surfaceAboveMe(0)
                                 ,surfaceBelowMe(0)
                                 ,macroVolumes(0)
                                 ,propertyGroups(0)
                                 ,propertyGroupsID(0)
                                 ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(parentBlock->ParentModel()->Context(),
                                             orientation,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(0),
                                             i_origin, i_step,
                                             i_lowbound, i_count,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(1),
                                             j_origin, j_step,
                                             j_lowbound, j_count,
                                             k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  (*gridGeometries) += new RescueGeometry(parentBlock->ParentModel(), 
                                          blockUnitGrid, missingValue,
                                          referenceSurfaceIn, 
                                          referenceOffsetIn, 
                                          thicknessIn, 
                                          onOffLapIn);
  GridGeometry()->parentBlockUnit = this;
  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueCoordinateSystem::Orientation orientation,
                                 RescueBlock *parentBlock,
                                 RescueUnit *parentUnit,
                                 RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                 RESCUEINT32 i_lowbound, RESCUEINT32 i_count,
                                 RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                 RESCUEINT32 j_lowbound, RESCUEINT32 j_count,
                                 RESCUEINT32 k_lowbound, RESCUEINT32 k_count,
                                 RESCUEFLOAT missingValue,
                                 RescueReferenceSurface *referenceSurfaceIn, RESCUEFLOAT referenceOffsetIn, 
                                 RESCUEFLOAT thicknessIn, RescueGeometry::RescueLapType onOffLapIn)
                                 :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                 ,block(parentBlock)
                                 ,unit(parentUnit)
                                 ,surfaceAboveMe(0)
                                 ,surfaceBelowMe(0)
                                 ,macroVolumes(0)
                                 ,propertyGroups(0)
                                 ,propertyGroupsID(0)
                                 ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(parentBlock->ParentModel()->Context(),
                                             orientation,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(0),
                                             i_origin, i_step,
                                             i_lowbound, i_count,
                                             parentBlock->ParentModel()->CoordinateSystem()->Axis(1),
                                             j_origin, j_step,
                                             j_lowbound, j_count,
                                             k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  (*gridGeometries) += new RescueGeometry(parentBlock->ParentModel(), 
                                          blockUnitGrid, missingValue,
                                          referenceSurfaceIn, 
                                          referenceOffsetIn, 
                                          thicknessIn, 
                                          onOffLapIn);
  GridGeometry()->parentBlockUnit = this;
  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueCoordinateSystem::Orientation orientation,
                                 RescueBlock *parentBlock,
                                 RescueUnit *parentUnit,
                                 RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                 RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                                 RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                 RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                                 RESCUEINT64 k_lowbound, RESCUEINT64 k_count,
                                 RESCUEFLOAT missingValue,
                                 RescueReferenceSurface *topSurfaceIn, RESCUEFLOAT topOffsetIn,
                                 RescueReferenceSurface *bottomSurfaceIn, RESCUEFLOAT  bottomOffsetIn)
                                 :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                 ,block(parentBlock)
                                 ,unit(parentUnit)
                                 ,surfaceAboveMe(0)
                                 ,surfaceBelowMe(0)
                                 ,macroVolumes(0)
                                 ,propertyGroups(0)
                                 ,propertyGroupsID(0)
                                 ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(parentBlock->ParentModel()->Context(),
                                             orientation,
                                 parentBlock->ParentModel()->CoordinateSystem()->Axis(0),
                                 i_origin, i_step,
                                 i_lowbound, i_count,
                                 parentBlock->ParentModel()->CoordinateSystem()->Axis(1),
                                 j_origin, j_step,
                                 j_lowbound, j_count,
                                 k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  (*gridGeometries) += new RescueGeometry(parentBlock->ParentModel(), blockUnitGrid, missingValue,
                                    topSurfaceIn, topOffsetIn, bottomSurfaceIn, bottomOffsetIn);
  GridGeometry()->parentBlockUnit = this;
  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueCoordinateSystem::Orientation orientation,
                                 RescueBlock *parentBlock,
                                 RescueUnit *parentUnit,
                                 RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                 RESCUEINT32 i_lowbound, RESCUEINT32 i_count,
                                 RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                 RESCUEINT32 j_lowbound, RESCUEINT32 j_count,
                                 RESCUEINT32 k_lowbound, RESCUEINT32 k_count,
                                 RESCUEFLOAT missingValue,
                                 RescueReferenceSurface *topSurfaceIn, RESCUEFLOAT topOffsetIn,
                                 RescueReferenceSurface *bottomSurfaceIn, RESCUEFLOAT  bottomOffsetIn)
                                 :RescueWireframeOwner(parentBlock->ParentModel()->Context())
                                 ,block(parentBlock)
                                 ,unit(parentUnit)
                                 ,surfaceAboveMe(0)
                                 ,surfaceBelowMe(0)
                                 ,macroVolumes(0)
                                 ,propertyGroups(0)
                                 ,propertyGroupsID(0)
                                 ,color(0)
{
  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(parentBlock->ParentModel()->Context(),
                                             orientation,
                                 parentBlock->ParentModel()->CoordinateSystem()->Axis(0),
                                 i_origin, i_step,
                                 i_lowbound, i_count,
                                 parentBlock->ParentModel()->CoordinateSystem()->Axis(1),
                                 j_origin, j_step,
                                 j_lowbound, j_count,
                                 k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBlock->blockUnits) += this;
  (*parentUnit->blockUnits) += this;

  (*gridGeometries) += new RescueGeometry(parentBlock->ParentModel(), blockUnitGrid, missingValue,
                                    topSurfaceIn, topOffsetIn, bottomSurfaceIn, bottomOffsetIn);
  GridGeometry()->parentBlockUnit = this;
  isA = R_RescueBlockUnit;
  AddMacroVolume();
}

RescueBlockUnit::RescueBlockUnit(RescueContext *context, FILE *archiveFile)
                                      :RescueWireframeOwner(context)
                                      ,block(0)
                                      ,unit(0)
                                      ,surfaceAboveMe(0)
                                      ,surfaceBelowMe(0)
                                      ,macroVolumes(0)
                                      ,propertyGroups(0)
                                      ,gridGeometries(0)
                                      ,propertyGroupsID(0)
                                      ,color(0)
{
  RESCUEINT64 count;

  geobodyParts = new cBagRescueGeobodyPart();
  macroVolumes = new cSetRescueMacroVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = 0;
  if (context->ReadFileVersion() < 18)
  {
    blockUnitGrid = new RescueGrid(context, archiveFile);
  }
  propertyGroupsID = 0;
  isA = R_RescueBlockUnit;
  context->RescueProgress("Block Unit");
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &blockID);
  myfscanf(context, archiveFile, &unitID);
  myfscanf(context, archiveFile, &surfaceAboveMeID);
  myfscanf(context, archiveFile, &surfaceBelowMeID);
  myfscanf(context, archiveFile, &count);
  if (count > 0)
  {
    propertyGroupsID = new cBagInt();

    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++)
    {
      RESCUEINT64 id;

      myfscanf(context, archiveFile, &id);
      (*propertyGroupsID) += id;
    }
  }
  if (context->ReadFileVersion() < 18)
  {
    RescueGeometry *gridGeometry = new RescueGeometry(context, archiveFile);
    gridGeometry->SetGrid(blockUnitGrid);
    (*gridGeometries) += gridGeometry;
    GridGeometry()->parentBlockUnit = this;
  }
  else
  {
    gridGeometries->UnArchive(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 6)
  {
    (*macroVolumes).UnArchive(context, archiveFile);
  }
  else
  {
    RescueMacroVolume *volOne = AddMacroVolume();
    volOne->PartialRead(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 11 && context->ReadFileVersion() < 28)
  {
    cSetRescueProperty properties; 
    properties.UnArchive(context, archiveFile);
    RESCUEINT64 howMany = properties.Count64();
    RESCUEINT64 geoCount = gridGeometries->Count64();
    RESCUEINT64 pOrdinal = 0;
    while (pOrdinal < howMany)
    {
      RescueProperty *property = properties.NthObject(pOrdinal);
      RESCUEINT64 subloop;
      RESCUEBOOL found = FALSE;
      for (subloop = 0; subloop < geoCount && found == FALSE; subloop++)
      {
        RescueGeometry *geometry = gridGeometries->NthObject(subloop);
        if (context->ReadFileVersion() < 18
        ||  geometry->Identifier() == property->geometryId)
        {
          properties.Relinquish(property);
          (*geometry->properties) += property;
          found = TRUE;
          howMany--;
        }
      }
      if (found == FALSE)
      {
        pOrdinal++;
      }
    }
/*
  Those properties that we did not find a home for will be deleted when
  properties goes out of scope.  Hopefully there are none.  This was made
  easier because gridGeometries is unarchived before properties.
*/
  }
  if (context->ReadFileVersion() >= 22)
  {
    RESCUEINT64 colorFlag = 0;
    myfscanf(context, archiveFile, &colorFlag);
    if (colorFlag == 1)
    {
      color = new RescueColor(context, archiveFile);
    }
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

RescueBlockUnit::~RescueBlockUnit()
{
  if (gridGeometries != 0)
  {
    delete gridGeometries;
  }
  if (macroVolumes != 0)
  {
    delete macroVolumes;
  }
  if (propertyGroups != 0)
  {
    delete propertyGroups;
  }
  if (propertyGroupsID != 0)
  {
    delete propertyGroupsID;
  }
  if (color != 0)
  {
    delete color;
  }
  if (geobodyParts != 0)
  {
    RESCUEINT64 howMany = geobodyParts->Count64();
    RESCUEINT64 loop;
    for (loop = 0; loop < howMany; loop++)
    {
      RescueGeobodyPart *part = geobodyParts->NthObject(loop);
      part->DropBlockUnit(this);
    }
    delete geobodyParts;
  }
}

void RescueBlockUnit::Dispose()
{
  if (gridGeometries != 0)
  {
    gridGeometries->Dispose();
  }
}

void RescueBlockUnit::AddGeometry(RescueGeometry *newGeometry)
{
  (*gridGeometries) += newGeometry;
  newGeometry->parentBlockUnit = this;
}
void RescueBlockUnit::Relink(RescueObject *parent)
{
  block = (RescueBlock *) parent;
  unit = block->ParentModel()->UnitIdentifiedBy(unitID);
  (*unit->blockUnits) += this;
  if (surfaceAboveMeID != 0)
  {
    surfaceAboveMe = block->ParentModel()->HorizonSurfaceIdentifiedBy(surfaceAboveMeID);
  }
  if (surfaceBelowMeID != 0)
  {
    surfaceBelowMe = block->ParentModel()->HorizonSurfaceIdentifiedBy(surfaceBelowMeID);
  }
  if (propertyGroupsID != 0)
  {
    RESCUEINT64 loop;
    for (loop = 0; loop < propertyGroupsID->Count64(); loop++)
    {
      RESCUEINT64 groupID = propertyGroupsID->NthObject(loop);
      RescuePropertyGroup *group = 
        (RescuePropertyGroup *) RescueHistory::FindObject(block->ParentModel(), 
                                                                    R_RescuePropertyGroup, 
                                                                     groupID);
      if (group != 0) 
      {
        (*propertyGroups) += group;
      }
    }
    delete propertyGroupsID;
    propertyGroupsID = 0;
  }
  (*macroVolumes).Relink(this);
  gridGeometries->Relink(this);
  if (block->ParentModel()->Context()->ReadFileVersion() >= 15 
  &&  block->ParentModel()->Context()->ReadFileVersion() <= 21
  &&  GridGeometry() != 0)
  {
    color = GridGeometry()->color;
    GridGeometry()->color = 0;
  }
}

RESCUEBOOL RescueBlockUnit::AnyFileTruncated()
{
  RESCUEBOOL myReturn = gridGeometries->AnyFileTruncated();
  return myReturn;
}

void RescueBlockUnit::Archive(FILE *archiveFile)
{
  RescueContext *context = block->ParentModel()->Context();
  myfprintf(context, archiveFile, "; Block Unit");
  context->RescueProgress("Block Unit");

  if (GridGeometry() == 0 && context->FileVersion() < 23)
  {
    RescueGrid *fakeGrid = new RescueGrid(context, RescueCoordinateSystem::LDF,
                                          0, 10, 0, 10, 0, 10);
    RescueGeometry *fakeGeometry = new RescueGeometry(block->ParentModel(), 
                                                      fakeGrid, (RESCUEFLOAT) -9999.0);
    AddGeometry(fakeGeometry);
  }
/*
  If writing to an older model we must have a BUG of some sort.
*/
  if (context->FileVersion() < 18)
  {
    BlockUnitGrid()->Archive(context, archiveFile);
  }
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, block->Identifier());
  myfprintf(context, archiveFile, unit->Identifier());
  if (surfaceAboveMe == 0)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
    myfprintf(context, archiveFile, surfaceAboveMe->Identifier());
  }
  if (surfaceBelowMe == 0)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
    myfprintf(context, archiveFile, surfaceBelowMe->Identifier());
  }
  myfprintf(context, archiveFile, (*propertyGroups).Count64());
  RESCUEINT64 loop;
  for (loop = 0; loop < (*propertyGroups).Count64(); loop++)
  {
   RescuePropertyGroup *group = (RescuePropertyGroup *) 
                            (*propertyGroups).NthObject(loop);
   myfprintf(context, archiveFile, group->Identifier());
  }
  if (context->FileVersion() >= 15 && context->FileVersion() <= 21)
  {
    RESCUEINT64 ordinal = 0;
    RescueGeometry *geometry = gridGeometries->NthObject(ordinal++);
    while (geometry != 0)
    {
      if (color == 0)
      {
        geometry->SetColor(0);
      }
      else
      {
        geometry->SetColor(*color);
      }
      geometry = gridGeometries->NthObject(ordinal++);
    }
  }
  if (context->FileVersion() < 18)
  {
    GridGeometry()->Archive(archiveFile);
  }
  else
  {
   gridGeometries->Archive(context, archiveFile);
  }
  (*macroVolumes).Archive(context, archiveFile);
  if (context->FileVersion() >= 11 && context->FileVersion() < 28)
  {
    cSetRescueProperty properties;
    RESCUEINT64 gOrdinal = 0;
    RescueGeometry *geometry = gridGeometries->NthObject(gOrdinal++);
    while (geometry != 0)
    {
      RESCUEINT64 pOrdinal = 0;
      RescueProperty *property = geometry->NthRescueProperty(pOrdinal++);
      while (property != 0)
      {
        properties += property;
        property = geometry->NthRescueProperty(pOrdinal++);
      }
      geometry = gridGeometries->NthObject(gOrdinal++);
    }
    properties.Archive(context, archiveFile);
    properties.RelinquishAll();
  }
  if (context->FileVersion() >= 22)
  {
    if (color == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      color->Archive(context, archiveFile);
    }
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueMacroVolume *RescueBlockUnit::AddMacroVolume()
{
  RescueMacroVolume *myVolume = new RescueMacroVolume(this);
  (*macroVolumes) += myVolume;
  return myVolume;
}

void RescueBlockUnit::DropMacroVolume(RescueMacroVolume *existingVolume)
{
  (*macroVolumes) -= existingVolume;
  if ((*macroVolumes).Count64() == 0)
  {
    (*macroVolumes) += new RescueMacroVolume(this);
  }
}

RESCUEBOOL RescueBlockUnit::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueBlockUnit)
  {
    return TRUE;
  }
  else
  {
    return RescueWireframeOwner::IsOfType(thisType);
  }
}

RescueModel *RescueBlockUnit::ParentModel()
{
  return block->ParentModel();
}

void RescueBlockUnit::PrepareModelDelete()
{
  RESCUEINT64 howMany = gridGeometries->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    gridGeometries->NthObject(loop)->PrepareModelDelete();
  }
}

RESCUEINT32 RescueBlockUnit::PropertyCount()
{
  return (RESCUEINT32) PropertyCount64();
}

RESCUEINT64 RescueBlockUnit::PropertyCount64()
{
  RESCUEINT64 myReturn = 0;
  RESCUEINT64 howMany = gridGeometries->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    myReturn += gridGeometries->NthObject(loop)->PropertyCount64();
  }
  return myReturn;
}

RescueGrid *RescueBlockUnit::BlockUnitGrid(RESCUEINT64 zeroBasedOrdinal)
{
  RescueGrid *myReturn = 0;
  RescueGeometry *bug = gridGeometries->NthObject(zeroBasedOrdinal);
  if (bug != 0)
  {
    myReturn = bug->Grid();
  }
  return myReturn;
}

RescueGeometry *RescueBlockUnit::GeometryWithId(RESCUEINT64 id)
{
  RescueGeometry *myReturn = 0;
  RESCUEINT64 howMany = gridGeometries->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == 0; loop++)
  {
    RescueGeometry *bug = gridGeometries->NthObject(loop);
    if (bug->Identifier() == id)
    {
      myReturn = bug;
    }
  }
  return myReturn;
}

void RescueBlockUnit::SetColor(RescueColor *colorIn)
{
  if (color != 0)
  {
    delete color;
  }
  color = colorIn;
}

void RescueBlockUnit::SetColor(RescueColor &colorIn)
{
  RescueColor *newColor = new RescueColor(colorIn);
  SetColor(newColor);
}

void RescueBlockUnit::SetOrientation(RescueOrientationLedger *ledger,
                                     RescueCoordinateSystem::Orientation orientation)
{
  RESCUEINT64 howMany = gridGeometries->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    RescueGeometry *bug = gridGeometries->NthObject(loop);
    bug->SetOrientation(ledger, orientation);
  }
}

void RescueBlockUnit::FindUniquePropertyNames(cSetString *container)
{
  gridGeometries->FindUniquePropertyNames(container);
}

RESCUEINT32 RescueBlockUnit::PropertyCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = PropertyCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 RescueBlockUnit::CountOfPropertyGroup(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CountOfPropertyGroup();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 RescueBlockUnit::CountOfVolumes(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CountOfVolumes();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 RescueBlockUnit::CountOfGeobodies(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CountOfGeobodies();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 RescueBlockUnit::GeometryCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = GeometryCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}





