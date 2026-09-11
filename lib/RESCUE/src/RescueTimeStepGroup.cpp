/********************************************************************

  RescueTimeStepGroup.h

  A bag of RescueProperty instances which belong to a particular
  time step.

  Rod Hanks,  June 1998

*********************************************************************/
#include "RescueTimeStepGroup.h"
#include "RescueModel.h"
#include "RescueIdTree.h"
#include "RescuePropertyGroup.h"
#include "RescueGeobodyPart.h"
#include "RescueGeobody.h"
#include "RescueBuffer.h"
#include "RescueDataContainer.h"
#include <string.h>

RESCUEINT64 RescueTimeStepGroup::RescueGeometryCount64()
{
  LatentRelink();

  return properties->Count64();
}

void RescueTimeStepGroup::AddGeometry(RescueGeometry *toAdd)
{
  LatentRelink();
  RescueGeometry *parentBug = toAdd->ParentBUG();
  if (parentBug != toAdd)
  {
    if (toAdd->ReferenceID() == 0)
    {
      throw("LGRs added to RescueTimeStepGroups need a unique non-zero reference id");
    }
  }

  (*geometries) += toAdd;
}

RescueGeometry *RescueTimeStepGroup::NthRescueGeometry(RESCUEINT64 zeroBasedOrdinal)
{
  LatentRelink();

  return geometries->NthObject(zeroBasedOrdinal);
}

RESCUEBOOL RescueTimeStepGroup::RemoveRescueGeometry(RescueGeometry *unitToDrop)
{
  LatentRelink();

  return ((*geometries) -= unitToDrop);
}

RESCUEBOOL RescueTimeStepGroup::RemoveRescueProperty(RescueProperty *unitToDrop) 
{
  LatentRelink();

  return ((*properties) -= unitToDrop);
}

RescueProperty *RescueTimeStepGroup::NthRescueProperty(RESCUEINT64 zeroBasedOrdinal) 
{
  LatentRelink();

  return properties->NthObject(zeroBasedOrdinal);
}

RESCUEINT64 RescueTimeStepGroup::RescuePropertyCount64()
{
  LatentRelink();

  return properties->Count64();
}

void RescueTimeStepGroup::AddProperty(RescueProperty *toAdd)
{
  LatentRelink();

  if (group->Container() != toAdd->Container())
  {
    throw("Property owner is mismatched to container");
  }
/*
  Works because if neither has anything to do with a container
  both will be zero.
*/
  ((*properties) += toAdd);
}

RescueProperty *RescueTimeStepGroup::GetProperty(RescueGeometry *geometry, RESCUECHAR *propertyName)
{
  LatentRelink();
  RescueProperty *myReturn = 0;
  int ndx = 0;
  RescueProperty *candidate = properties->NthObject(ndx++);
  while (candidate != 0 && myReturn == 0)
  {
    if (candidate->Geometry() == geometry)
    {
      if (candidate->IsNamed(propertyName))
      {
        myReturn = candidate;
      }
    }
    candidate = properties->NthObject(ndx++);
  }
  return myReturn;
}

RescueProperty *RescueTimeStepGroup::PropertyNamed(RESCUECHAR *nameToFind)
{
  LatentRelink();
  return properties->PropertyNamed(nameToFind);
}

void RescueTimeStepGroup::Relink(RescueObject *parent)
{
/*
  Pass the RescueBlockUnitGroup I belong to.
*/
  group = (RescuePropertyGroup *) parent;
  RescueContext *context = group->Context();
  relinkingModel = context->unarchivingModel;
}

void RescueTimeStepGroup::LatentRelink()
{
  if (relinkingModel != 0)
  {
    if (propertyID != 0)
    {
      RescueBlockUnit *blockUnit = group->ParentBlockUnit();
      RescueGeobodyPart *bodyPart = group->ParentGeobodyPart();
      RescueSurface *surface = group->ParentSurface();
      RescueDataContainer *container = group->Container();
/*
  One of these things has just got to be a chicken!
*/
      RESCUEINT64 loop;
      for (loop = 0; loop < propertyID->Count64(); loop++)
      {
        RescueProperty *property = 0;
        if (blockUnit != 0)
        {
          property = blockUnit->PropertyIdentifiedBy(propertyID->NthObject(loop));
          if (property == 0)
          {
            property = relinkingModel->PropertyIdentifiedBy(propertyID->NthObject(loop));
          }
        }
        else if (bodyPart != 0)
        {
          property = bodyPart->PropertyIdentifiedBy(propertyID->NthObject(loop));
          if (property == 0)
          {
            property = relinkingModel->PropertyIdentifiedBy(propertyID->NthObject(loop));
          }
        }
        else if (surface != 0)
        {
          property = surface->PropertyIdentifiedBy(propertyID->NthObject(loop));
          if (property == 0)
          {
            property = relinkingModel->PropertyIdentifiedBy(propertyID->NthObject(loop));
          }
        }
        else if (container != 0)
        {
          property = container->PropertyIdentifiedBy(propertyID->NthObject(loop));
        }
        else
        {
          property = relinkingModel->PropertyIdentifiedBy(propertyID->NthObject(loop));
        }
        if (property != 0)
        {
          (*properties) += property;
        }
      }
      delete propertyID;
      propertyID = 0;
    }
    if (geometryID != 0)
    {
      RESCUEINT64 loop;
      for (loop = 0; loop < geometryID->Count64(); loop += 2)
      {
        RescueGeometry *geometry = 0;
        RESCUEINT64 rootID = geometryID->NthObject(loop);
        RESCUEINT64 lgrRefId = geometryID->NthObject(loop + 1);
        RescueGeometry *rootGeometry = relinkingModel->GeometryIdentifiedBy(rootID);
        if (rootGeometry != 0)
        {
          if (lgrRefId == 0)
          {
            geometry = rootGeometry;
          }
          else
          {
            geometry = rootGeometry->ProgenyHavingID(lgrRefId);
          }
        }
        if (geometry != 0)
        {
          (*geometries) += geometry;
        }
      }
      delete geometryID;
      geometryID = 0;
    }
    relinkingModel = 0;
  }
}

void RescueTimeStepGroup::Archive(FILE *archiveFile)
{
  LatentRelink();

  RescueContext *context = group->Context();
  myfprintf(context, archiveFile, "; RescueTimeStepGroup");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, timeStepName->String());
  if (context->FileVersion() >= 21)
  {
    myfprintf(context, archiveFile, isLocked);
  }

  myfprintf(context, archiveFile, (*properties).Count64());
  RESCUEINT64 loop;
  for (loop = 0; loop < (*properties).Count64(); loop++)
  {
    RescueProperty *property = (RescueProperty *) 
                            (*properties).NthObject(loop);
    myfprintf(context, archiveFile, property->Identifier());
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "lgrBag");
    RESCUEINT64 howMany = geometries->Count64();
    RescueBuffer nameBuf(context, ((howMany * 2) * sizeof(RESCUEUINT64)) + sizeof(RESCUEUINT64));
    nameBuf << howMany;
    for (loop = 0; loop < howMany; loop++)
    {
      RescueGeometry *geometry = (RescueGeometry *) geometries->NthObject(loop);
      RescueGeometry *rootGeometry = geometry->ParentBUG();
      nameBuf << rootGeometry->Identifier();
      nameBuf << geometry->ReferenceID();
    }
    nameBuf.Archive(archiveFile);

    myfprintf(context, archiveFile, "EOD");
  }
}


RESCUEBOOL RescueTimeStepGroup::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueTimeStepGroup)
  {
    return TRUE;
  }
  else
  {
    return RescueHistoryObject::IsOfType(thisType);
  }
}

RescueTimeStepGroup::~RescueTimeStepGroup()
{
  if (timeStepName != 0)
  {
    delete timeStepName;
  }
  if (properties != 0)
  {
    delete properties;
  }
  if (geometries != 0)
  {
    delete geometries;
  }
  if (propertyID != 0)
  {
    delete propertyID;
  }
  if (geometryID != 0)
  {
    delete geometryID;
  }
}

RescueTimeStepGroup::RescueTimeStepGroup
                              (const RESCUECHAR *timeStepNameIn,
                               RescuePropertyGroup *groupIn)
                               :RescueHistoryObject(groupIn->Context())
                               ,timeStepName(0)
                               ,group(groupIn)
                               ,properties(0)
                               ,isLocked(FALSE)
                               ,propertyID(0)
                               ,geometryID(0)
                               ,relinkingModel(0)
{
  timeStepName = new RCHString(timeStepNameIn);
  properties = new cBagRescueProperty();
  geometries = new cBagRescueGeometry();

  (*group->timeSteps) += this;
  isA = R_RescueTimeStepGroup;
}

void RescueTimeStepGroup::SetLock()
{
  isLocked = TRUE;
  RESCUEINT64 loop;
  for (loop = 0; loop < (*properties).Count64(); loop++)
  {
    RescueProperty *property = (RescueProperty *) (*properties).NthObject(loop);
    property->SetLock();
  }
}

RescueTimeStepGroup::RescueTimeStepGroup
                                (RescueContext *context, FILE *archiveFile)
                                :RescueHistoryObject(context)
                                ,timeStepName(0)
                               ,properties(0)
                               ,isLocked(FALSE)
                               ,propertyID(0)
                               ,geometryID(0)
                               ,relinkingModel(0)
{
  RESCUECHAR myString[255];
  RESCUEINT64 count;

  isA = R_RescueTimeStepGroup;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  timeStepName = new RCHString(myString);
  
  if (context->ReadFileVersion() >= 21)
  {
    myfscanf(context, archiveFile, &isLocked);
  }

  properties = new cBagRescueProperty();
  geometries = new cBagRescueGeometry();
  myfscanf(context, archiveFile, &count);
  if (count > 0)
  {
    propertyID = new cBagInt();

    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++)
    {
      RESCUEINT64 id;

      myfscanf(context, archiveFile, &id);
      (*propertyID) += id;
    }
  }
  if (context->timeStepGroups != 0)
  {
    context->timeStepGroups->Add(this);
  }
  if (context->ReadFileVersion() >= 37)
  {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0)
    {
      if (strcmp(myString, "lgrBag") == 0)
      {
        geometryID = new cBagInt();
        RESCUEINT64 howMany;
        RescueBuffer buf(context, archiveFile);
        buf >> howMany;
        RESCUEINT64 loop;
        for (loop = 0; loop < howMany; loop++)
        {
          RESCUEINT64 id;
          buf >> id;
          (*geometryID) += id;
          buf >> id;
          (*geometryID) += id;
/*
  For each, we have the root geometry, then the lgr.
*/
        }
      }
      else
      {
        RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
    }
  }
}




