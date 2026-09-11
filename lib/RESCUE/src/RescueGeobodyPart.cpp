/********************************************************************

  RescueGeobodyPart.h

  A part of a Geobody

  Rod Hanks,  June, 1999

*********************************************************************/
#include "RescueGeobodyPart.h"
#include "RescueBuffer.h"
#include "RescueColor.h"
#include "RescueGeobody.h"
#include "RescueGeobodyVolume.h"
#include "RescueHistory.h"
#include "RescueIdTree.h"
#include "RescueModel.h"
#include "RescueVertex.h"
#include "myHeaders.h"
#include <string.h>

void RescueGeobodyPart::DropGeometry(RescueGeometry *toDrop) {
  toDrop->RescueDeleteFile();
  (*gridGeometries) -= toDrop;
}

RESCUEINT64 RescueGeobodyPart::PropertyCount64() {
  RESCUEINT64 myReturn = 0;
  RESCUEINT64 howMany = gridGeometries->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    myReturn += gridGeometries->NthObject(loop)->PropertyCount64();
  }
  return myReturn;
}

RescueGeobodySurface *RescueGeobodyPart::GeobodySurfaceIdentifiedBy(RESCUEINT64 id) {
  RescueGeobodySurface *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < bodyVolumes->Count64() && myReturn == 0; loop++) {
    RescueGeobodyVolume *volume = bodyVolumes->NthObject(loop);
    myReturn = volume->GeobodySurfaceIdentifiedBy(id);
  }
  return myReturn;
}

RescueProperty *RescueGeobodyPart::PropertyIdentifiedBy(RESCUEINT64 id) {
  RescueProperty *myReturn = 0;
  RESCUEINT64 ordinal = 0;
  RescueGeometry *geometry = GridGeometry(ordinal++);
  while (geometry != 0 && myReturn == 0) {
    myReturn = geometry->PropertyIdentifiedBy(id);
    geometry = GridGeometry(ordinal++);
  }
  return myReturn;
}

RescueGeobodyPart::RescueGeobodyPart(RescueGeobody *parentBody)
    : RescueHistoryObject(parentBody->ParentModel()->Context()), body(parentBody), bodyVolumes(0), propertyGroups(0),
      propertyGroupsID(0), blockUnits(new cBagRescueBlockUnit), blockUnitsID(0), color(0) {
  bodyVolumes = new cSetRescueGeobodyVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  propertyGroupsID = 0;

  (*parentBody->bodyParts) += this;

  isA = R_RescueGeobodyPart;
  AddGeobodyVolume();
}

RescueGeobodyPart::RescueGeobodyPart(RescueCoordinateSystem::Orientation orientation, RescueGeobody *parentBody,
                                     RESCUEFLOAT i_origin, RESCUEFLOAT i_step, RESCUEINT64 i_lowbound,
                                     RESCUEINT64 i_count, RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                     RESCUEINT64 j_lowbound, RESCUEINT64 j_count, RESCUEFLOAT k_origin,
                                     RESCUEFLOAT k_step, RESCUEINT64 k_lowbound, RESCUEINT64 k_count,
                                     RESCUEFLOAT missingValue)
    : RescueHistoryObject(parentBody->ParentModel()->Context()), body(parentBody), bodyVolumes(0), propertyGroups(0),
      propertyGroupsID(0), blockUnits(new cBagRescueBlockUnit), blockUnitsID(0), color(0) {
  bodyVolumes = new cSetRescueGeobodyVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(
      parentBody->ParentModel()->Context(), orientation, parentBody->ParentModel()->CoordinateSystem()->Axis(0),
      i_origin, i_step, i_lowbound, i_count, parentBody->ParentModel()->CoordinateSystem()->Axis(1), j_origin, j_step,
      j_lowbound, j_count, k_origin, k_step, k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBody->bodyParts) += this;

  (*gridGeometries) += new RescueGeometry(parentBody->ParentModel(), blockUnitGrid, missingValue);
  GridGeometry()->parentGeobodyPart = this;
  isA = R_RescueGeobodyPart;
  AddGeobodyVolume();
}

RescueGeobodyPart::RescueGeobodyPart(RescueCoordinateSystem::Orientation orientation, RescueGeobody *parentBody,
                                     RESCUEFLOAT i_origin, RESCUEFLOAT i_step, RESCUEINT64 i_lowbound,
                                     RESCUEINT64 i_count, RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                     RESCUEINT64 j_lowbound, RESCUEINT64 j_count, RESCUEINT64 k_lowbound,
                                     RESCUEINT64 k_count, RESCUEFLOAT missingValue)
    : RescueHistoryObject(parentBody->ParentModel()->Context()), body(parentBody), bodyVolumes(0), propertyGroups(0),
      propertyGroupsID(0), blockUnits(new cBagRescueBlockUnit), blockUnitsID(0), color(0) {
  bodyVolumes = new cSetRescueGeobodyVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(
      parentBody->ParentModel()->Context(), orientation, parentBody->ParentModel()->CoordinateSystem()->Axis(0),
      i_origin, i_step, i_lowbound, i_count, parentBody->ParentModel()->CoordinateSystem()->Axis(1), j_origin, j_step,
      j_lowbound, j_count, k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBody->bodyParts) += this;

  (*gridGeometries) += new RescueGeometry(parentBody->ParentModel(), blockUnitGrid, missingValue);
  GridGeometry()->parentGeobodyPart = this;
  isA = R_RescueGeobodyPart;
  AddGeobodyVolume();
}

RescueGeobodyPart::RescueGeobodyPart(RescueCoordinateSystem::Orientation orientation, RescueGeobody *parentBody,
                                     RESCUEFLOAT i_origin, RESCUEFLOAT i_step, RESCUEINT64 i_lowbound,
                                     RESCUEINT64 i_count, RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                     RESCUEINT64 j_lowbound, RESCUEINT64 j_count, RESCUEINT64 k_lowbound,
                                     RESCUEINT64 k_count, RESCUEFLOAT missingValue,
                                     RescueReferenceSurface *topSurfaceIn, RESCUEFLOAT topOffsetIn,
                                     RescueReferenceSurface *bottomSurfaceIn, RESCUEFLOAT bottomOffsetIn)
    : RescueHistoryObject(parentBody->ParentModel()->Context()), body(parentBody), bodyVolumes(0), propertyGroups(0),
      propertyGroupsID(0), blockUnits(new cBagRescueBlockUnit), blockUnitsID(0), color(0) {
  bodyVolumes = new cSetRescueGeobodyVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(
      parentBody->ParentModel()->Context(), orientation, parentBody->ParentModel()->CoordinateSystem()->Axis(0),
      i_origin, i_step, i_lowbound, i_count, parentBody->ParentModel()->CoordinateSystem()->Axis(1), j_origin, j_step,
      j_lowbound, j_count, k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBody->bodyParts) += this;

  (*gridGeometries) += new RescueGeometry(parentBody->ParentModel(), blockUnitGrid, missingValue, topSurfaceIn,
                                          topOffsetIn, bottomSurfaceIn, bottomOffsetIn);
  GridGeometry()->parentGeobodyPart = this;
  isA = R_RescueGeobodyPart;
  AddGeobodyVolume();
}

RescueGeobodyPart::RescueGeobodyPart(RescueCoordinateSystem::Orientation orientation, RescueGeobody *parentBody,
                                     RESCUEFLOAT i_origin, RESCUEFLOAT i_step, RESCUEINT64 i_lowbound,
                                     RESCUEINT64 i_count, RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                     RESCUEINT64 j_lowbound, RESCUEINT64 j_count, RESCUEINT64 k_lowbound,
                                     RESCUEINT64 k_count, RESCUEFLOAT missingValue,
                                     RescueReferenceSurface *referenceSurfaceIn, RESCUEFLOAT referenceOffsetIn,
                                     RESCUEFLOAT thicknessIn, RescueGeometry::RescueLapType onOffLapIn)
    : RescueHistoryObject(parentBody->ParentModel()->Context()), body(parentBody), bodyVolumes(0), propertyGroups(0),
      propertyGroupsID(0), blockUnits(new cBagRescueBlockUnit), blockUnitsID(0), color(0) {
  bodyVolumes = new cSetRescueGeobodyVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  RescueGrid *blockUnitGrid = new RescueGrid(
      parentBody->ParentModel()->Context(), orientation, parentBody->ParentModel()->CoordinateSystem()->Axis(0),
      i_origin, i_step, i_lowbound, i_count, parentBody->ParentModel()->CoordinateSystem()->Axis(1), j_origin, j_step,
      j_lowbound, j_count, k_lowbound, k_count);
  propertyGroupsID = 0;

  (*parentBody->bodyParts) += this;

  (*gridGeometries) += new RescueGeometry(parentBody->ParentModel(), blockUnitGrid, missingValue, referenceSurfaceIn,
                                          referenceOffsetIn, thicknessIn, onOffLapIn);
  GridGeometry()->parentGeobodyPart = this;
  isA = R_RescueGeobodyPart;
  AddGeobodyVolume();
}

RescueGeobodyPart::~RescueGeobodyPart() {
  if (gridGeometries != 0) {
    delete gridGeometries;
  }
  if (bodyVolumes != 0) {
    delete bodyVolumes;
  }
  if (propertyGroups != 0) {
    delete propertyGroups;
  }
  if (propertyGroupsID != 0) {
    delete propertyGroupsID;
  }
  if (blockUnits != 0) {
    RESCUEINT64 howMany = blockUnits->Count64();
    RESCUEINT64 loop;
    for (loop = 0; loop < howMany; loop++) {
      RescueBlockUnit *bu = blockUnits->NthObject(loop);
      (*bu->geobodyParts) -= this;
    }
    delete blockUnits;
  }
}

RescueGeobodyVolume *RescueGeobodyPart::AddGeobodyVolume() {
  RescueGeobodyVolume *myVolume = new RescueGeobodyVolume(this);
  (*bodyVolumes) += myVolume;
  return myVolume;
}

void RescueGeobodyPart::DropGeobodyVolume(RescueGeobodyVolume *existingVolume) {
  (*bodyVolumes) -= existingVolume;
  if ((*bodyVolumes).Count64() == 0) {
    (*bodyVolumes) += new RescueGeobodyVolume(this);
  }
}

RESCUEBOOL RescueGeobodyPart::DeletePropertyGroup(RescuePropertyGroup *existingPropertyGroup) {
  body->ParentModel()->DeletePropertyGroup(existingPropertyGroup);
  return ((*propertyGroups) -= existingPropertyGroup);
}

RESCUEBOOL RescueGeobodyPart::AnyFileTruncated() {
  RESCUEBOOL myReturn = gridGeometries->AnyFileTruncated();
  return myReturn;
}

RESCUEBOOL RescueGeobodyPart::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueGeobodyPart) {
    return TRUE;
  } else {
    return RescueHistoryObject::IsOfType(thisType);
  }
}
RescueGeobodyPart::RescueGeobodyPart(RescueContext *contextIn, FILE *archiveFile)
    : RescueHistoryObject(contextIn), body(0), bodyVolumes(0), propertyGroups(0), gridGeometries(0),
      propertyGroupsID(0), blockUnits(new cBagRescueBlockUnit), blockUnitsID(0), color(0) {
  RESCUEINT64 count;
  isA = R_RescueGeobodyPart;
  bodyVolumes = new cSetRescueGeobodyVolume();
  gridGeometries = new cSetRescueGeometry();
  propertyGroups = new cBagRescuePropertyGroup();
  /*
    The grid is archived with the RescueGeometry.
  */
  propertyGroupsID = 0;
  isA = R_RescueGeobodyPart;
  contextIn->RescueProgress("Geobody Part");
  ReadId(contextIn, archiveFile);
  myfscanf(contextIn, archiveFile, &count);
  if (count > 0) {
    propertyGroupsID = new cBagInt();

    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++) {
      RESCUEINT64 id;

      myfscanf(contextIn, archiveFile, &id);
      (*propertyGroupsID) += id;
    }
  }
  gridGeometries->UnArchive(contextIn, archiveFile);
  (*bodyVolumes).UnArchive(contextIn, archiveFile);
  if (contextIn->ReadFileVersion() < 28) {
    cSetRescueProperty properties;
    properties.UnArchive(contextIn, archiveFile);
    RESCUEINT64 howMany = properties.Count64();
    RESCUEINT64 geoCount = gridGeometries->Count64();
    RESCUEINT64 pOrdinal = 0;
    while (pOrdinal < howMany) {
      RescueProperty *property = properties.NthObject(pOrdinal);
      RESCUEINT64 subloop;
      RESCUEBOOL found = FALSE;
      for (subloop = 0; subloop < geoCount && found == FALSE; subloop++) {
        RescueGeometry *geometry = gridGeometries->NthObject(subloop);
        if (geometry->Identifier() == property->geometryId) {
          properties.Relinquish(property);
          (*geometry->properties) += property;
          found = TRUE;
          howMany--;
        }
      }
      if (found == FALSE) {
        pOrdinal++;
      }
    }
    /*
      Those properties that we did not find a home for will be deleted when
      properties goes out of scope.  Hopefully there are none.  This was made
      easier because gridGeometries is unarchived before properties.
    */
  }
  myfscanf(contextIn, archiveFile, &count);
  if (count > 0) {
    blockUnitsID = new cBagInt();
    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++) {
      RESCUEINT64 id;

      myfscanf(contextIn, archiveFile, &id);
      (*blockUnitsID) += id;
    }
  }
  if (contextIn->ReadFileVersion() >= 22) {
    RESCUEINT64 colorFlag = 0;
    myfscanf(contextIn, archiveFile, &colorFlag);
    if (colorFlag == 1) {
      color = new RescueColor(contextIn, archiveFile);
    }
  }
  contextIn->geobodyParts->Add(this);
  if (contextIn->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(contextIn, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(contextIn, archiveFile);
      myfgets(contextIn, myString, 255, archiveFile);
    }
  }
}

void RescueGeobodyPart::Archive(FILE *archiveFile) {
  RescueContext *context = body->ParentModel()->Context();
  myfprintf(context, archiveFile, "; RescueGeobodyPart");

  if (GridGeometry() == 0 && context->FileVersion() < 23) {
    RescueGrid *fakeGrid = new RescueGrid(context, RescueCoordinateSystem::LDF, 0, 10, 0, 10, 0, 10);
    RescueGeometry *fakeGeometry = new RescueGeometry(body->ParentModel(), fakeGrid, (RESCUEFLOAT)-9999.0);
    AddGeometry(fakeGeometry);
  }
  /*
    If writing to an older model we must have a BUG of some sort.
  */
  myfprintf(context, archiveFile, Identifier());
  RESCUEINT64 howMany = propertyGroups->Count64();
  myfprintf(context, archiveFile, howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    RescuePropertyGroup *group = (RescuePropertyGroup *)(*propertyGroups).NthObject(loop);
    myfprintf(context, archiveFile, group->Identifier());
  }
  if (context->FileVersion() >= 15 && context->FileVersion() <= 21) {
    RESCUEINT64 ordinal = 0;
    RescueGeometry *geometry = gridGeometries->NthObject(ordinal++);
    while (geometry != 0) {
      if (color == 0) {
        geometry->SetColor(0);
      } else {
        geometry->SetColor(*color);
      }
      geometry = gridGeometries->NthObject(ordinal++);
    }
  }
  gridGeometries->Archive(context, archiveFile);
  bodyVolumes->Archive(context, archiveFile);
  if (context->FileVersion() < 28) {
    cSetRescueProperty properties;
    RESCUEINT64 gOrdinal = 0;
    RescueGeometry *geometry = gridGeometries->NthObject(gOrdinal++);
    while (geometry != 0) {
      RESCUEINT64 pOrdinal = 0;
      RescueProperty *property = geometry->NthRescueProperty(pOrdinal++);
      while (property != 0) {
        properties += property;
        property = geometry->NthRescueProperty(pOrdinal++);
      }
      geometry = gridGeometries->NthObject(gOrdinal++);
    }
    properties.Archive(context, archiveFile);
    properties.RelinquishAll();
  }
  howMany = blockUnits->Count64();
  myfprintf(context, archiveFile, howMany);
  for (loop = 0; loop < howMany; loop++) {
    RescueBlockUnit *bu = blockUnits->NthObject(loop);
    myfprintf(context, archiveFile, bu->Identifier());
  }
  if (context->FileVersion() >= 22) {
    if (color == 0) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    } else {
      myfprintf(context, archiveFile, (RESCUEINT64)1);
      color->Archive(context, archiveFile);
    }
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

void RescueGeobodyPart::AddGeometry(RescueGeometry *newGeometry) {
  (*gridGeometries) += newGeometry;
  newGeometry->parentGeobodyPart = this;
}

void RescueGeobodyPart::Relink(RescueObject *parent) {
  body = (RescueGeobody *)parent;
  RescueContext *context = body->ParentModel()->Context();
  if (propertyGroupsID != 0) {
    RESCUEINT64 loop;
    for (loop = 0; loop < propertyGroupsID->Count64(); loop++) {
      RESCUEINT64 groupID = propertyGroupsID->NthObject(loop);
      RescuePropertyGroup *group =
          (RescuePropertyGroup *)RescueHistory::FindObject(body->parentModel, R_RescuePropertyGroup, groupID);
      if (group != 0) {
        (*propertyGroups) += group;
      }
    }
    delete propertyGroupsID;
    propertyGroupsID = 0;
  }
  (*bodyVolumes).Relink(this);
  gridGeometries->Relink(this);
  if (blockUnitsID != 0) {
    RESCUEINT64 loop;
    for (loop = 0; loop < blockUnitsID->Count64(); loop++) {
      RescueBlockUnit *bu = body->ParentModel()->BlockUnitIdentifiedBy(blockUnitsID->NthObject(loop));
      if (bu != 0) {
        AddBlockUnit(bu);
      }
    }
    delete blockUnitsID;
    blockUnitsID = 0;
  }
  if (context->ReadFileVersion() >= 15 && context->ReadFileVersion() <= 21 && GridGeometry() != 0) {
    color = GridGeometry()->color;
    GridGeometry()->color = 0;
  }
}

void RescueGeobodyPart::AddBlockUnit(RescueBlockUnit *existingBlockUnit) {
  (*blockUnits) += existingBlockUnit;
  (*existingBlockUnit->geobodyParts) += this;
}

RESCUEBOOL RescueGeobodyPart::DropBlockUnit(RescueBlockUnit *existingBlockUnit) {
  (*existingBlockUnit->geobodyParts) -= this;
  return ((*blockUnits) -= existingBlockUnit);
}

RescueGeometry *RescueGeobodyPart::GeometryWithId(RESCUEINT64 id) {
  RescueGeometry *myReturn = 0;
  RESCUEINT64 howMany = gridGeometries->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == 0; loop++) {
    RescueGeometry *bug = gridGeometries->NthObject(loop);
    if (bug->Identifier() == id) {
      myReturn = bug;
    }
  }
  return myReturn;
}

void RescueGeobodyPart::Dispose() {
  if (gridGeometries != 0) {
    gridGeometries->Dispose();
  }
}

void RescueGeobodyPart::SetColor(RescueColor *colorIn) {
  if (color != 0) {
    delete color;
  }
  color = colorIn;
}

void RescueGeobodyPart::SetColor(RescueColor &colorIn) {
  RescueColor *newColor = new RescueColor(colorIn);
  SetColor(newColor);
}

void RescueGeobodyPart::SetOrientation(RescueOrientationLedger *ledger,
                                       RescueCoordinateSystem::Orientation orientation) {
  RESCUEINT64 ordinal = 0;
  RescueGeometry *geometry = gridGeometries->NthObject(ordinal++);
  while (geometry != 0) {
    geometry->SetOrientation(ledger, orientation);
    geometry = gridGeometries->NthObject(ordinal++);
  }
  ordinal = 0;
}

void RescueGeobodyPart::FindUniquePropertyNames(cSetString *container) {
  gridGeometries->FindUniquePropertyNames(container);
  bodyVolumes->FindUniquePropertyNames(container);
}

RESCUEINT32 RescueGeobodyPart::GeometryCount(RESCUEBOOL throwIfTooBig) {
  RESCUEINT64 output = GeometryCount64();
  if (throwIfTooBig) {
    if (output > 2147483647 || output < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32)output;
}
