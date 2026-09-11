/********************************************************************

  RescueTrimLoopStub.cpp

  A RescueTrimLoop object that has been lazily relinked.
  
  You can preview the object here and trade it for a real one
  later.

  Rod Hanks,  March 2001


*********************************************************************/
#include "myHeaders.h"
#include "RescueTrimLoopStub.h"
#include "RescuePolyLine.h"
#include "RescueWireframe.h"
#include "RescueModel.h"

RESCUEBOOL RescueTrimLoopStub::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueTrimLoopStub)
  {
  return TRUE;
  }
  else
  {
  return RescueWireframeStub::IsOfType(thisType);
  }
}

RESCUEBOOL RescueTrimLoopStub::Equals(RescueTrimLoopStub *other)
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

RESCUEBOOL RescueTrimLoopStub::Equals(RescueTrimLoop *other)
{
  RESCUEBOOL myReturn = FALSE;
  if (other->Identifier() == ObjectId())
  {
  myReturn = TRUE;
  }
  return myReturn;
}

RescueTrimLoopStub::RescueTrimLoopStub(RescueContext *context, FILE *archiveFile)
                          :RescueWireframeStub(context, archiveFile)
                                                             ,actualLoop(0)
{
  isA = R_RescueTrimLoopStub;
}

RescueTrimLoopStub::RescueTrimLoopStub(RescueContext *context, RESCUEINT64 wireframeIdIn, RESCUEINT64 objectIdIn)
                          :RescueWireframeStub(context, wireframeIdIn, objectIdIn)
                          ,actualLoop(0)
{
  isA = R_RescueTrimLoopStub;
}

RescueTrimLoopStub::RescueTrimLoopStub(RescueContext *context, RescueTrimLoop *LoopIn)
                  :RescueWireframeStub(context, LoopIn->Owner(), LoopIn->Identifier())
                  ,actualLoop(LoopIn)
{
  if (wireframeObj != 0)
  {
  loadNo = wireframeObj->LoadNo();
  }
}

RescueTrimLoop *RescueTrimLoopStub::TrimLoop(RescueModel *model, RESCUEBOOL loadIfNeeded)
{
  if (wireframeObj == 0 && model != 0)
  {
  wireframeObj = model->WireframeIdentifiedBy(wireframeId);
  }
  if (wireframeObj != 0)
  {
  if (wireframeObj->IsWireframeLoaded() == FALSE)
  {
      actualLoop = 0;
      if (loadIfNeeded)
      {
    wireframeObj->LoadWireframe();
      }
  }
/*
  If not currently loaded, load it up if the user has given us that discretion.
*/
  if (actualLoop != 0)
  {
      if (loadNo != wireframeObj->LoadNo())
      {
    actualLoop = 0;
      }
  }
/*
  If we have been asked before, we may have a pointer to the object that we want,
  but first we have to check to see if it is stale.
*/
  if (actualLoop == 0)
  {
      actualLoop = wireframeObj->TrimLoopIdentifiedBy(objectId);
      loadNo = wireframeObj->LoadNo();
/*
  If we do hook up to the object, remember which load.  If the reader unloads the
  RescueWireframe and then reloads it our pointer will be stale.
*/
  }
  }
  else
  {
  actualLoop = 0;
  }
  return actualLoop;;
}



