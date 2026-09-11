/********************************************************************

  RescueEdgeSetStub.cpp

  A RescueEdgeSet object that has been lazily relinked.
  
  You can preview the object here and trade it for a real one
  later.

  Rod Hanks,  March 2001


*********************************************************************/
#include "myHeaders.h"
#include "RescueEdgeSetStub.h"
#include "RescueEdgeSet.h"
#include "RescueWireframe.h"
#include "RescueModel.h"

RESCUEBOOL RescueEdgeSetStub::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueEdgeSetStub)
  {
    return TRUE;
  }
  else
  {
    return RescueWireframeStub::IsOfType(thisType);
  }
}

RESCUEBOOL RescueEdgeSetStub::Equals(RescueEdgeSetStub *other)
{
  RESCUEBOOL myReturn = FALSE;
  if (other->WireframeId() == WireframeId()
  &&  other->ObjectId()    == ObjectId())
  {
    myReturn = TRUE;
  }
/*
  Really should be sufficient just to compare the object ids.
*/
  return myReturn;
}

RESCUEBOOL RescueEdgeSetStub::Equals(RescueEdgeSet *other)
{
  RESCUEBOOL myReturn = FALSE;
  if (other->Identifier() == ObjectId())
  {
    myReturn = TRUE;
  }
  return myReturn;
}

RescueEdgeSetStub::RescueEdgeSetStub(RescueContext *context, FILE *archiveFile)
                                                             :RescueWireframeStub(context, archiveFile)
                                                             ,actualEdgeSet(0)
{
  isA = R_RescueEdgeSetStub;
}

RescueEdgeSetStub::RescueEdgeSetStub(RescueContext *context, RESCUEINT64 wireframeIdIn, RESCUEINT64 objectIdIn)
                          :RescueWireframeStub(context, wireframeIdIn, objectIdIn)
                          ,actualEdgeSet(0)
{
  isA = R_RescueEdgeSetStub;
}

RescueEdgeSetStub::RescueEdgeSetStub(RescueContext *context, RescueEdgeSet *edgeSetIn)
                  :RescueWireframeStub(context, edgeSetIn->Owner(), edgeSetIn->Identifier())
                  ,actualEdgeSet(edgeSetIn)
{
  if (wireframeObj != 0)
  {
    loadNo = wireframeObj->LoadNo();
  }
}

RescueEdgeSet *RescueEdgeSetStub::EdgeSet(RescueModel *model, RESCUEBOOL loadIfNeeded)
{
  if (wireframeObj == 0 && model != 0)
  {
    wireframeObj = model->WireframeIdentifiedBy(wireframeId);
  }
  if (wireframeObj != 0)
  {
    if (wireframeObj->IsWireframeLoaded() == FALSE)
    {
      actualEdgeSet = 0;
      if (loadIfNeeded)
      {
        wireframeObj->LoadWireframe();
      }
    }
/*
  If not currently loaded, load it up if the user has given us that discretion.
*/
    if (actualEdgeSet != 0)
    {
      if (loadNo != wireframeObj->LoadNo())
      {
        actualEdgeSet = 0;
      }
    }
/*
  If we have been asked before, we may have a pointer to the object that we want,
  but first we have to check to see if it is stale.
*/
    if (actualEdgeSet == 0)
    {
      actualEdgeSet = wireframeObj->EdgeSetIdentifiedBy(objectId);
      loadNo = wireframeObj->LoadNo();
/*
  If we do hook up to the object, remember which load.  If the reader unloads the
  RescueWireframe and then reloads it our pointer will be stale.
*/
    }
  }
  else
  {
    actualEdgeSet = 0;
  }
  return actualEdgeSet;
}



