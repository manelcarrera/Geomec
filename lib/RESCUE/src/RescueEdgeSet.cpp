/********************************************************************

  RescueEdgeSet.cpp

  A set of edges which describe the relationship of a top, bottom, or
  side of a block unit to a set of surfaces.

  Rod Hanks,  July 1996

*********************************************************************/
#include "RescueEdgeSet.h"
#include "RescueModel.h"
#include "RescueTrimVertex.h"
#include "RescueWireframe.h"
#include "RescueWireframeOwner.h"
#include "RescueMacroVolume.h"
#include "RescueFaultIntersection.h"
#include "RescueGeobodySurface.h"
#include "RescueGeobodyVolume.h"
#include "RescueBuffer.h"
#include <string.h>

RescueObject *RescueEdgeSet::RescueBusinessOwner(RescueModel *model)
{
  RescueObject *myReturn = 0;
  switch (ownerIsA)
  {
  case R_RescueBlockUnitHorizonSurface:
    myReturn = model->HorizonSurfaceIdentifiedBy(ownerId);
    break;
  case R_RescueReferenceSurface:
    myReturn = model->ReferenceSurfaceIdentifiedBy(ownerId);
    break;
  case R_RescueSection:
    myReturn = model->SectionIdentifiedBy(ownerId);
    break;
  case R_RescueMacroVolume:
    myReturn = model->MacroVolumeIdentifiedBy(ownerId);
    break;
  case R_RescueBlockUnitSide:
    myReturn = model->BlockUnitSideIdentifiedBy(ownerId);
    break;
  case R_RescueFaultIntersection:
    myReturn = model->FaultIntersectionIdentifiedBy(ownerId);
    break;
  case R_RescueGeobodySurface:
    myReturn = model->GeobodySurfaceIdentifiedBy(ownerId);
    break;
  case R_RescueGeobodyVolume:
    myReturn = model->GeobodyVolumeIdentifiedBy(ownerId);
    break;
  default:
    break;   // Quiet compiler warning about unused enums.
  }
  return myReturn;
}

void RescueEdgeSet::SetRescueBusinessOwner(RescueObject *ownerIn)
{
  ownerIsA = ownerIn->IsA();
  ownerId  = ownerIn->uniqueID;
  if (owner != 0)
  {
    owner->MarkWireframeChanged();
  }
}

void RescueEdgeSet::AddBoundaryLoop(RescueTrimLoop *existingLoop)
{
  (*boundaryLoops) += existingLoop;
  if (existingLoop->Owner() != 0)
  {
    existingLoop->Owner()->trimLoops->Relinquish(existingLoop);
    existingLoop->owner = 0;
  }
  existingLoop->ownerSet = this;
}

void RescueEdgeSet::AddInteriorLoop(RescueTrimLoop *existingLoop)
{
  (*interiorLoops) += existingLoop;
  if (existingLoop->Owner() != 0)
  {
    existingLoop->Owner()->trimLoops->Relinquish(existingLoop);
    existingLoop->owner = 0;
  }
  existingLoop->ownerSet = this;
}

void RescueEdgeSet::DropBoundaryLoop(RescueTrimLoop *existingLoop)
{
  (*boundaryLoops) -= existingLoop;
  existingLoop->ownerSet = 0;
}

void RescueEdgeSet::DropInteriorLoop(RescueTrimLoop *existingLoop)
{
  (*interiorLoops) -= existingLoop;
  existingLoop->ownerSet = 0;
}

RescueEdgeSet::~RescueEdgeSet()
{
  if (boundaryLoops != 0)
  {
    delete boundaryLoops;
  }
  if (interiorLoops != 0)
  {
    delete interiorLoops;
  }
}

RescueEdgeSet::RescueEdgeSet(RescueWireframeOwner *ownerIn):RescueObject(ownerIn->ParentModel()->Context())
                                                           ,boundaryLoops(0)
                                                           ,interiorLoops(0)
                                                           ,owner(ownerIn->Wireframe())
                                                           ,ownerIsA((_RescueObjectType) 0)
                                                           ,ownerId(0)
{
  isA = R_RescueEdgeSet;
  boundaryLoops = new cSetRescueTrimLoop();
  interiorLoops = new cSetRescueTrimLoop();
  owner->AddEdgeSet(this);
}

RescueTrimLoop *RescueEdgeSet::TrimLoopIdentifiedBy(RESCUEINT64 identifier)
{
  RescueTrimLoop *myReturn = 0;
  myReturn = boundaryLoops->ObjectIdentifiedBy(identifier);
  if (myReturn == 0)
  {
    myReturn = interiorLoops->ObjectIdentifiedBy(identifier);
  }
  return myReturn;
}

void RescueEdgeSet::Archive(FILE *archiveFile)
{
  RescueContext *context = owner->ParentModel()->Context();
  myfprintf(context, archiveFile, "; edge set");
  if (context->FileVersion() >= 28)
  {
    myfprintf(context, archiveFile, Identifier());
  }
  if (context->FileVersion() >= 29)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) ownerIsA);
    myfprintf(context, archiveFile, ownerId);
  }
  boundaryLoops->Archive(context, archiveFile);
  interiorLoops->Archive(context, archiveFile);
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueEdgeSet::RescueEdgeSet(RescueContext *context, FILE *archiveFile)
                                               :RescueObject(context)
                                               ,boundaryLoops(0)
                                               ,interiorLoops(0)
                                               ,owner(0)
                                               ,ownerIsA((_RescueObjectType) 0)
                                               ,ownerId(0)
{
  isA = R_TOMBSTONE;
  if (context->ReadFileVersion() >= 28)
  {
    ReadId(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 29)
  {
    RESCUEINT64 dummy;
    myfscanf(context, archiveFile, &dummy);
    ownerIsA = (_RescueObjectType) dummy;
    myfscanf(context, archiveFile, &ownerId);
  }
  boundaryLoops = new cSetRescueTrimLoop();
  interiorLoops = new cSetRescueTrimLoop();
  if (context->ReadFileVersion() < 6)
  {
    cSetRescueTrimEdge *boundaryEdges = new cSetRescueTrimEdge();
    boundaryEdges->UnArchive(context, archiveFile);
    (*boundaryLoops) += new RescueTrimLoop(context, boundaryEdges);
    cSetRescueTrimEdge *interiorEdges = new cSetRescueTrimEdge();
    interiorEdges->UnArchive(context, archiveFile);
    (*interiorLoops) += new RescueTrimLoop(context, interiorEdges);
  }
  else
  {
    boundaryLoops->UnArchive(context, archiveFile);
    interiorLoops->UnArchive(context, archiveFile);
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

void RescueEdgeSet::Relink(RescueObject *object)
{
  if (isA == R_TOMBSTONE)
  {
    isA = R_RescueEdgeSet;
    owner = (RescueWireframe *) object;
    boundaryLoops->Relink(object);
    interiorLoops->Relink(object);

    RESCUEINT64 ndx = 0;
    RescueTrimLoop *loop = boundaryLoops->NthObject(ndx++);
    while (loop != 0)
    {
      loop->ownerSet = this;
      loop = boundaryLoops->NthObject(ndx++);
    }
    ndx = 0;
    loop = interiorLoops->NthObject(ndx++);
    while (loop != 0)
    {
      loop->ownerSet = this;
      loop = interiorLoops->NthObject(ndx++);
    }
  }
}

RESCUEBOOL RescueEdgeSet::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueEdgeSet)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}

RESCUEINT32 RescueEdgeSet::CountOfBoundaryLoop(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CountOfBoundaryLoop();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 RescueEdgeSet::CountOfInteriorLoop(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CountOfInteriorLoop();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}



