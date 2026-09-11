/********************************************************************

  RescueHistory.cpp

  A record of a change to the model, including a human-readable string,
  a parsable string, and relationships to model objects and other
  changes.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "myHeaders.h"
#include "RescueHistory.h"
#include "cBagRescueHistoryObject.h"
#include "cBagRescueHistory.h"
#include "cBagInt.h"
#include "RescueModel.h"
#include "RescueIdTree.h"
#include "RescueGeobody.h"
#include "RescueLookup.h"
#include "RescueFaultIntersection.h"
#include "RescueWireframe.h"
#include "RescueBuffer.h"
#include <string.h>

#include <time.h>
#include <stdio.h>

void RescueHistory::Archive(FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  myfprintf(context, archiveFile, "; RescueHistory");
  myfprintf(context, archiveFile, Identifier());
  changeDescription->Archive(context, archiveFile);
  parsableDescription->Archive(context, archiveFile);
  timeStamp->Archive(context, archiveFile);
  myfprintf(context, archiveFile, forwardRelatedChanges->Count64());
  RESCUEINT64 ordinal = 0;
  RescueHistory *related = forwardRelatedChanges->NthObject(ordinal++);
  while (related != 0)
  {
    myfprintf(context, archiveFile, related->Identifier());
    related = forwardRelatedChanges->NthObject(ordinal++);
  }
  myfprintf(context, archiveFile, objectsChanged->Count64());
  ordinal = 0;
  RescueHistoryObject *object = objectsChanged->NthObject(ordinal++);
  while (object != 0)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) object->IsA());
    myfprintf(context, archiveFile, object->Identifier());
    object = objectsChanged->NthObject(ordinal++);
  }
  myfprintf(context, archiveFile, relatedObjects->Count64());
  ordinal = 0;
  object = relatedObjects->NthObject(ordinal++);
  while (object != 0)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) object->IsA());
    myfprintf(context, archiveFile, object->Identifier());
    object = relatedObjects->NthObject(ordinal++);
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueHistory::RescueHistory(RescueContext *context, FILE *archiveFile)
                                               :RescueHistoryObject(context)
                                               ,objectsChanged(0)
                                               ,relatedObjects(0)
                                               ,forwardRelatedChanges(0)
                                               ,backwardRelatedChanges(0)
                                               ,parentModel(0)
{
  isA = R_RescueHistory;
  ReadId(context, archiveFile);
  changeDescription = new RCHString(context, archiveFile);
  parsableDescription = new RCHString(context, archiveFile);
  timeStamp = new RCHString(context, archiveFile);
  RESCUEINT64 howMany;
  myfscanf(context, archiveFile, &howMany);
  forwardHistory = new cBagInt();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    RESCUEINT64 id;

    myfscanf(context, archiveFile, &id);
    (*forwardHistory) += id;
  }
  objectTypes = new cBagInt();
  objectIds = new cBagInt();
  myfscanf(context, archiveFile, &howMany);
  for (loop = 0; loop < howMany; loop++)
  {
    RESCUEINT64 id;

    myfscanf(context, archiveFile, &id);
    (*objectTypes) += id;
    myfscanf(context, archiveFile, &id);
    (*objectIds) += id;
  }
  relatedTypes = new cBagInt();
  relatedIds = new cBagInt();
  myfscanf(context, archiveFile, &howMany);
  for (loop = 0; loop < howMany; loop++)
  {
    RESCUEINT64 id;

    myfscanf(context, archiveFile, &id);
    (*relatedTypes) += id;
    myfscanf(context, archiveFile, &id);
    (*relatedIds) += id;
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

void RescueHistory::Relink(RescueObject *object)
{
  parentModel = (RescueModel *) object;
  objectsChanged = new cBagRescueHistoryObject();
  relatedObjects = new cBagRescueHistoryObject();
  forwardRelatedChanges = new cBagRescueHistory();
  backwardRelatedChanges = new cBagRescueHistory();

  RESCUEINT64 howMany = forwardHistory->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    RescueHistory *relatedHistory = parentModel->HistoryIdentifiedBy(forwardHistory->NthObject(loop));
    AddRelatedHistory(relatedHistory);
  }
  howMany = objectTypes->Count64();
  for (loop = 0; loop < howMany; loop++)
  {
    _RescueObjectType thisType = (_RescueObjectType) objectTypes->NthObject(loop);
    RESCUEINT64 id = objectIds->NthObject(loop);
    RescueHistoryObject *object = FindObject(parentModel, thisType, id);
    AddObjectChanged(object);
  }
  howMany = relatedTypes->Count64();
  for (loop = 0; loop < howMany; loop++)
  {
    _RescueObjectType thisType = (_RescueObjectType) relatedTypes->NthObject(loop);
    RESCUEINT64 id = relatedIds->NthObject(loop);
    RescueHistoryObject *object = FindObject(parentModel, thisType, id);
    AddRelatedObject(object);
  }

  delete forwardHistory;
  delete objectTypes;
  delete objectIds;
  delete relatedTypes;
  delete relatedIds;

  forwardHistory = 0;
  objectTypes = 0;
  objectIds = 0;
  relatedTypes = 0;
  relatedIds = 0;
}

RescueHistory::RescueHistory(RescueModel *parentModelIn, RESCUECHAR *changeDescriptionIn, 
                             RESCUECHAR *parsableDescriptionIn)
                             :RescueHistoryObject(parentModelIn->Context())
{
  isA = R_RescueHistory;
  parentModel = parentModelIn;
  (*parentModel->history) += this;
  changeDescription = new RCHString(changeDescriptionIn);
  parsableDescription = new RCHString(parsableDescriptionIn);
  objectsChanged = new cBagRescueHistoryObject();
  relatedObjects = new cBagRescueHistoryObject();
  forwardRelatedChanges = new cBagRescueHistory();
  backwardRelatedChanges = new cBagRescueHistory();

  timeStamp = new RCHString(35);
  RescueContext::CurrentISOTime(*timeStamp);
}

void RescueHistory::SetChangeDescription(const RESCUECHAR *changeDescriptionIn)
{
  changeDescription->Replace(changeDescriptionIn);
}

void RescueHistory::SetParsableDescription(const RESCUECHAR *parsableDescriptionIn)
{
  parsableDescription->Replace(parsableDescriptionIn);
}

RescueHistory::~RescueHistory()
{
  delete changeDescription;
  delete parsableDescription;
  delete timeStamp;
  {
    RESCUEINT64 ordinal = 0;
    RescueHistoryObject *object = objectsChanged->NthObject(ordinal++);
    while (object != 0)
    {
      object->RemoveObjectChanged(this);
      object = objectsChanged->NthObject(ordinal++);
    }
    delete objectsChanged;
  }
  {
    RESCUEINT64 ordinal = 0;
    RescueHistoryObject *object = relatedObjects->NthObject(ordinal++);
    while (object != 0)
    {
      object->RemoveRelatedChange(this);
      object = relatedObjects->NthObject(ordinal++);
    }
    delete relatedObjects;
  }
  {
    RESCUEINT64 ordinal = 0;
    RescueHistory *object = forwardRelatedChanges->NthObject(ordinal++);
    while (object != 0)
    {
      (*object->backwardRelatedChanges) -= this;
      object = forwardRelatedChanges->NthObject(ordinal++);
    }
    delete forwardRelatedChanges;
  }
  {
    RESCUEINT64 ordinal = 0;
    RescueHistory *object = backwardRelatedChanges->NthObject(ordinal++);
    while (object != 0)
    {
      (*object->forwardRelatedChanges) -= this;
      object = backwardRelatedChanges->NthObject(ordinal++);
    }
    delete backwardRelatedChanges;
  }
}

void RescueHistory::AddObjectChanged(RescueHistoryObject *newObjectChanged)
{
  if (newObjectChanged != 0)
  {
    (*objectsChanged) += newObjectChanged;
    newObjectChanged->AddObjectChanged(this);
  }
}

void RescueHistory::AddRelatedObject(RescueHistoryObject *newRelatedObject)
{
  if (newRelatedObject != 0)
  {
    (*relatedObjects) += newRelatedObject;
    newRelatedObject->AddRelatedChange(this);
  }
}

void RescueHistory::AddRelatedHistory(RescueHistory *newRelatedHistory)
{
  if (newRelatedHistory != 0)
  {
    (*forwardRelatedChanges) += newRelatedHistory;
    (*(newRelatedHistory->backwardRelatedChanges)) += this;
  }
}

void RescueHistory::RemoveObjectChanged(RescueHistoryObject *newObjectChanged)
{
  if (newObjectChanged != 0)
  {
    (*objectsChanged) -= newObjectChanged;
    newObjectChanged->RemoveObjectChanged(this);
  }
}

void RescueHistory::RemoveRelatedObject(RescueHistoryObject *newRelatedObject)
{
  if (newRelatedObject != 0)
  {
    (*relatedObjects) -= newRelatedObject;
    newRelatedObject->RemoveRelatedChange(this);
  }
}

void RescueHistory::RemoveRelatedHistory(RescueHistory *newRelatedHistory)
{
  if (newRelatedHistory != 0)
  {
    (*forwardRelatedChanges) -= newRelatedHistory;
    (*(newRelatedHistory->backwardRelatedChanges)) -= this;
  }
}

RescueHistoryObject *RescueHistory::FindObject(RescueModel *model, 
                                               _RescueObjectType objectType, 
                                               RESCUEINT64 objectId)
{
  RescueHistoryObject *myReturn = 0;
  switch (objectType)
  {
  case R_RescueBlock:
    {
      myReturn = model->BlockIdentifiedBy(objectId);
    }
    break;
  case R_RescueBlockUnit:
    {
      myReturn = model->BlockUnitIdentifiedBy(objectId);
    }
    break;
  case R_RescuePropertyGroup:
    {
      myReturn = (RescueHistoryObject *) model->Context()->propertyGroups->Find(objectId);
    }
    break;
  case R_RescueBlockUnitSide:
    {
      myReturn = (RescueHistoryObject *) model->Context()->blockUnitSides->Find(objectId);
    }
    break;
  case R_RescueCoordinateSystem:
    {
      myReturn = model->CoordinateSystemIdentifiedBy(objectId);
    }
    break;
  case R_RescueGeobody:
    {
      myReturn = model->GeobodyIdentifiedBy(objectId);
    }
    break;
  case R_RescueGeobodyPart:
    {
      myReturn = (RescueHistoryObject *) model->Context()->geobodyParts->Find(objectId);
    }
    break;
  case R_RescueGeobodyVolume:
    {
      myReturn = (RescueHistoryObject *) model->Context()->geobodyVolumes->Find(objectId);
    }
    break;
  case R_RescueGeometry:
    {
      myReturn = (RescueHistoryObject *) model->Context()->geometries->Find(objectId);
    }
    break;
  case R_RescueGrid:
    {
      myReturn = (RescueHistoryObject *) model->Context()->grids->Find(objectId);
    }
    break;
  case R_RescueHistory:
    {
      myReturn = model->HistoryIdentifiedBy(objectId);
    }
    break;
  case R_RescueHorizon:
    {
      myReturn = model->HorizonIdentifiedBy(objectId);
    }
    break;
  case R_RescueLookup:
    {
      myReturn = model->LookupIdentifiedBy(objectId);
    }
    break;
  case R_RescueMacroVolume:
    {
      myReturn = (RescueHistoryObject *) model->Context()->macroVolumes->Find(objectId);
    }
    break;
  case R_RescueModel:
    {
      myReturn = model;
    }
    break;
  case R_RescueModelPropertyGroup:
    {
      myReturn = (RescueHistoryObject *) model->Context()->modelPropertyGroups->Find(objectId);
    }
    break;
  case R_RescueProperty:
    {
      myReturn = (RescueHistoryObject *) model->Context()->properties->Find(objectId);
    }
    break;
  case R_RescueReferenceSurface:
    {
      myReturn = model->ReferenceSurfaceIdentifiedBy(objectId);
    }
    break;
  case R_RescueBlockUnitHorizonSurface:
    {
      myReturn = model->HorizonSurfaceIdentifiedBy(objectId);
    }
    break;
  case R_RescueEventDescr:
    {
      myReturn = model->EventDescrIdentifiedBy(objectId);
    }
    break;
  case R_RescueFaultIntersection:
    {
      myReturn = model->FaultIntersectionIdentifiedBy(objectId);
    }
    break;
  case R_RescueWireframe:
    {
      myReturn = model->WireframeIdentifiedBy(objectId);
    }
    break;
  case R_RescueSection:
    {
      myReturn = model->SectionIdentifiedBy(objectId);
    }
    break;
  case R_RescueSurface:
    {
      myReturn = model->SurfaceIdentifiedBy(objectId);
    }
    break;
  case R_RescueIJSurface:
    {
      myReturn = model->IJSurfaceIdentifiedBy(objectId);
    }
    break;
  case R_RescueTimeStepGroup:
    {
      myReturn = (RescueHistoryObject *) model->Context()->timeStepGroups->Find(objectId);
    }
    break;
  case R_RescueWellbore:
    {
      myReturn = model->WellboreIdentifiedBy(objectId);
    }
    break;
  case R_RescueWellboreProperty:
    {
      myReturn = (RescueHistoryObject *) model->Context()->wellboreProperties->Find(objectId);
    }
    break;
  case R_RescueUnit:
    {
      myReturn = model->UnitIdentifiedBy(objectId);
    }
    break;
  default:
    break;   // Quiet warning
  }
  return myReturn;
}

RESCUEBOOL RescueHistory::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueHistory)
  {
    return TRUE;
  }
  else
  {
    return RescueHistoryObject::IsOfType(thisType);
  }
}



