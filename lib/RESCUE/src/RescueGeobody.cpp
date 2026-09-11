/********************************************************************

  RescueGeobody.cpp

  The geobody object for RESCUE's data model.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "RescueGeobody.h"
#include "RescueModel.h"
#include "RescueGeobodyPart.h"
#include "RescueBuffer.h"
#include <string.h>

RESCUEBOOL RescueGeobody::AnyFileTruncated()
{
  return bodyParts->AnyFileTruncated();
}

void RescueGeobody::Archive(FILE *archiveFile)
{
  myfprintf(parentModel->Context(), archiveFile, "; RescueGeobody");
  myfprintf(parentModel->Context(), archiveFile, Identifier());
  bodyName->Archive(parentModel->Context(), archiveFile);
  groups->Archive(parentModel->Context(), archiveFile);
  bodyParts->Archive(parentModel->Context(), archiveFile);
  if (parentModel->Context()->FileVersion() >= 37)
  {
    myfprintf(parentModel->Context(), archiveFile, "EOD");
  }
}

RescueGeobody::RescueGeobody(RescueContext *contextIn, FILE *archiveFile):RescueWireframeOwner(contextIn)
{
  isA = R_RescueGeobody;
  ReadId(contextIn, archiveFile);
  bodyName = new RCHString(contextIn, archiveFile);
  groups = new cSetRescuePropertyGroup();
  groups->UnArchive(contextIn, archiveFile);
  bodyParts = new cSetRescueGeobodyPart();
  bodyParts->UnArchive(contextIn, archiveFile);
  if (contextIn->ReadFileVersion() >= 37)
  {
    RESCUECHAR myString[255];

    myfgets(contextIn, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0)
    {
      RescueBuffer buf(contextIn, archiveFile);
      myfgets(contextIn, myString, 255, archiveFile);
    }
  }
}

RescueProperty *RescueGeobody::PropertyIdentifiedBy(RESCUEINT64 id)
{
  RescueProperty *myReturn = 0;
  RESCUEINT64 ndx = 0;
  RescueGeobodyPart *part = bodyParts->NthObject(ndx++);
  while (myReturn == 0 && part != 0)
  {
    myReturn = part->PropertyIdentifiedBy(id);
    if (myReturn == 0)
    {
      part = bodyParts->NthObject(ndx++);
    }
  }
  return myReturn;
}

RescueGeobodyVolume *RescueGeobody::GeobodyVolumeIdentifiedBy(RESCUEINT64 id)
{
  RescueGeobodyVolume *myReturn = 0;
  RESCUEINT64 ndx = 0;
  RescueGeobodyPart *part = bodyParts->NthObject(ndx++);
  while (myReturn == 0 && part != 0)
  {
    myReturn = part->GeobodyVolumeIdentifiedBy(id);
    if (myReturn == 0)
    {
      part = bodyParts->NthObject(ndx++);
    }
  }
  return myReturn;
}

RescueGeobodySurface *RescueGeobody::GeobodySurfaceIdentifiedBy(RESCUEINT64 id)
{
  RescueGeobodySurface *myReturn = 0;
  RESCUEINT64 ndx = 0;
  RescueGeobodyPart *part = bodyParts->NthObject(ndx++);
  while (myReturn == 0 && part != 0)
  {
    myReturn = part->GeobodySurfaceIdentifiedBy(id);
    if (myReturn == 0)
    {
      part = bodyParts->NthObject(ndx++);
    }
  }
  return myReturn;
}

void RescueGeobody::Relink(RescueObject *parentModelIn)
{
  parentModel = (RescueModel *) parentModelIn;
  groups->Relink(this);
  bodyParts->Relink(this);
}

RescueGeobody::RescueGeobody(RESCUECHAR *newBodyName, RescueModel *newParentModel)
        :RescueWireframeOwner(newParentModel->Context())
{
  bodyName = new RCHString(newBodyName);
  parentModel = newParentModel;
  (*parentModel->geoBodies) += this;
  isA = R_RescueGeobody;
  groups = new cSetRescuePropertyGroup();
  bodyParts = new cSetRescueGeobodyPart();
}

RescueGeobody::~RescueGeobody()
{
  delete bodyName;
  delete groups;
  delete bodyParts;
}

void RescueGeobody::Dispose()
{
  if (bodyParts != 0)
  {
    RESCUEINT64 howMany = bodyParts->Count64();
    RESCUEINT64 loop;
    for (loop = 0; loop < howMany; loop++)
    {
      bodyParts->NthObject(loop)->Dispose();
    }
  }
}

RESCUEBOOL RescueGeobody::DeleteBodyPart(RescueGeobodyPart *existingBodyPart)
{
  if (existingBodyPart != 0)
  {
    existingBodyPart->Dispose();
  }
  return ((*bodyParts) -= existingBodyPart);
}

RESCUEBOOL RescueGeobody::IsNamed(const RESCUECHAR *possibleName)
{
  RESCUEBOOL myReturn = FALSE;
  if (bodyName != 0)
  {
    if ((*bodyName) == possibleName)
    {
      myReturn = TRUE;
    }
  }
  return myReturn;
}

RESCUEBOOL RescueGeobody::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueGeobody)
  {
    return TRUE;
  }
  else
  {
    return RescueWireframeOwner::IsOfType(thisType);
  }
}

void RescueGeobody::SetOrientation(RescueOrientationLedger *ledger, 
                                   RescueCoordinateSystem::Orientation orientation)
{
  RESCUEINT64 ndx = 0;
  RescueGeobodyPart *part = bodyParts->NthObject(ndx++);
  while (part != 0)
  {
    part->SetOrientation(ledger, orientation);
    part = bodyParts->NthObject(ndx++);
  }
}

void RescueGeobody::FindUniquePropertyNames(cSetString *container)
{
  bodyParts->FindUniquePropertyNames(container); 
}

RESCUEINT32 RescueGeobody::GeobodyPartCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = GeobodyPartCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}




