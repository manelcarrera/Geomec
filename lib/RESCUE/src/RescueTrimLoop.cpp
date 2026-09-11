/********************************************************************

  RescueTrimLoop.cpp

  A set of edges which describe the relationship of a top, bottom, or
  side of a block unit to a set of surfaces.

  Rod Hanks,  July 1996

*********************************************************************/
#include "RescueTrimLoop.h"
#include "RescueModel.h"
#include "RescueTrimVertex.h"
#include "RCHString.h"
#include "RescueWireframe.h"
#include "RescueBuffer.h"
#include <string.h>

RescueTrimLoop::RescueTrimLoop(RescueContext *context, RESCUECHAR *loopClassIn)
                  :RescueObject(context)
    ,faceHint(-1)
    ,ownerSet(0)
    ,propertyID(0)
{
  isA = R_RescueTrimLoop;
  owner = 0;
  loopEdges = new cSetRescueTrimEdge();
  loopClass = new RCHString(loopClassIn);
}

RescueTrimLoop::RescueTrimLoop(RescueWireframeOwner *ownerIn, RESCUECHAR *loopClassIn)
                  :RescueObject(ownerIn->ParentModel()->Context())
    ,faceHint(-1)
    ,ownerSet(0)
    ,propertyID(0)
{
  isA = R_RescueTrimLoop;
  loopEdges = new cSetRescueTrimEdge();
  loopClass = new RCHString(loopClassIn);
  owner = ownerIn->Wireframe();
  owner->AddTrimLoop(this);
}

RescueTrimLoop::~RescueTrimLoop()
{
  if (loopClass != 0)
  {
    delete loopClass;
  }
  delete loopEdges;
}

void RescueTrimLoop::AddLoopEdge(RescueTrimEdge *existingEdge)
{
  (*loopEdges) += existingEdge;
  existingEdge->ownerLoop = this;
}

void RescueTrimLoop::DropLoopEdge(RescueTrimEdge *existingEdge)
{
  (*loopEdges) -= existingEdge;
  existingEdge->ownerLoop = 0;
}

void RescueTrimLoop::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; trim loop");
  if (context->FileVersion() >= 24)
  {
    myfprintf(context, archiveFile, Identifier());
  }
  if (context->FileVersion() >= 23)
  {
    loopClass->Archive(context, archiveFile);
  }
  if (context->FileVersion() >= 28)
  {
    myfprintf(context, archiveFile, faceHint);
  }
  if (context->FileVersion() >= 29)
  {
    myfprintf(context, archiveFile, propertyID);
  }
  loopEdges->Archive(context, archiveFile);
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueTrimLoop::RescueTrimLoop(RescueContext *context, FILE *archiveFile)
          :RescueObject(context)
   ,loopClass(0)
   ,faceHint(-1)
   ,owner(0)
   ,ownerSet(0)
   ,propertyID(0)
{
  isA = R_RescueTrimLoop;
  if (context->ReadFileVersion() >= 24)
  {
    ReadId(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 23)
  {
    loopClass = new RCHString(context, archiveFile);
  }
  else
  {
    loopClass = new RCHString();
  }
  if (context->ReadFileVersion() >= 28)
  {
    myfscanf(context, archiveFile, &faceHint);
  }
  if (context->ReadFileVersion() >= 29)
  {
    myfscanf(context, archiveFile, &propertyID);
  }
  loopEdges = new cSetRescueTrimEdge();
  loopEdges->UnArchive(context, archiveFile);
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

RescueTrimLoop::RescueTrimLoop(RescueContext *context, cSetRescueTrimEdge *loopEdgesIn)
                        :RescueObject(context)
   ,faceHint(-1)
   ,owner(0)
   ,ownerSet(0)
   ,propertyID(0)
{
  isA = R_RescueTrimLoop;
  loopEdges = loopEdgesIn;
  loopClass = new RCHString();
}

void RescueTrimLoop::Relink(RescueObject *object)
{
  owner = (RescueWireframe *) object;
  loopEdges->Relink(object);
  RESCUEINT64 ndx = 0;
  RescueTrimEdge *edge = NthLoopEdge(ndx++);
  while (edge != 0)
  {
    edge->ownerLoop = this;
    edge = NthLoopEdge(ndx++);
  }
}

RescueProperty *RescueTrimLoop::OwnerProperty(RescueModel *model)
{
  RescueProperty *myReturn = 0;
  if (propertyID != 0)
  {
    myReturn = model->GlobalPropertyIdentifiedBy(propertyID);
  }
  return myReturn;
}

RESCUEBOOL RescueTrimLoop::IsLoop(RescueModel *model)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 howMany = CountOfLoopEdge();
  if (howMany > 1)
  {
    RESCUEINT64 ndx = 0;
    RescueTrimEdge *edge = NthLoopEdge(ndx++);
    RescueTrimEdge *first = edge;
    myReturn = TRUE;

    while (myReturn == TRUE && ndx < howMany)
    {
      RescueTrimEdge *adjacent = NthLoopEdge(ndx++);
      if (edge->EndingPoint(model) != adjacent->StartingPoint(model))
      {
        myReturn = FALSE;
      }
/*
  Means loop is broken in the middle.
*/
      edge = adjacent;
    }
    if (edge->EndingPoint(model) != first->StartingPoint(model))
    {
      myReturn = FALSE;
    }
/*
  Means loop does not start and end at the same place.
*/
  }
  return myReturn;
}

RESCUEBOOL RescueTrimLoop::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueTrimLoop)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}

RESCUEINT32 RescueTrimLoop::CountOfLoopEdge(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CountOfLoopEdge();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}




