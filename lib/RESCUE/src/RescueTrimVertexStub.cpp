/********************************************************************

  RescueTrimVertexStub.cpp

  A RescueTrimVertex object that has been lazily relinked.

  You can preview the object here and trade it for a real one
  later.

  Rod Hanks,  March 2001


*********************************************************************/
#include "RescueTrimVertexStub.h"
#include "RescueModel.h"
#include "RescuePolyLine.h"
#include "RescueWireframe.h"
#include "myHeaders.h"

RESCUEBOOL RescueTrimVertexStub::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueTrimVertexStub) {
    return TRUE;
  } else {
    return RescueWireframeStub::IsOfType(thisType);
  }
}

RESCUEBOOL RescueTrimVertexStub::Equals(RescueTrimVertexStub *other) {
  RESCUEBOOL myReturn = FALSE;
  if (other->WireframeId() == WireframeId() && other->ObjectId() == ObjectId()) {
    myReturn = TRUE;
  }
  /*
    Really should be sufficient just to compare the object ids.
  */
  return myReturn;
}

RESCUEBOOL RescueTrimVertexStub::Equals(RescueTrimVertex *other) {
  RESCUEBOOL myReturn = FALSE;
  if (other->Identifier() == ObjectId()) {
    myReturn = TRUE;
  }
  return myReturn;
}

RescueTrimVertexStub::RescueTrimVertexStub(RescueContext *context, FILE *archiveFile)
    : RescueWireframeStub(context, archiveFile), actualVertex(0) {
  isA = R_RescueTrimVertexStub;
}

RescueTrimVertexStub::RescueTrimVertexStub(RescueContext *context, RESCUEINT64 wireframeIdIn, RESCUEINT64 objectIdIn)
    : RescueWireframeStub(context, wireframeIdIn, objectIdIn), actualVertex(0) {
  isA = R_RescueTrimVertexStub;
}

RescueTrimVertexStub::RescueTrimVertexStub(RescueTrimVertex *vertexIn)
    : RescueWireframeStub(vertexIn->Owner()->ParentModel()->Context(), vertexIn->Owner()->Identifier(),
                          vertexIn->Identifier()),
      actualVertex(vertexIn) {
  wireframeObj = vertexIn->Owner();
  loadNo = wireframeObj->LoadNo();
}

RescueTrimVertexStub::RescueTrimVertexStub(RescueContext *context, RescueTrimVertexStub *other)
    : RescueWireframeStub(context, other->wireframeId, other->objectId), actualVertex(0) {
  isA = R_RescueTrimVertexStub;
}

void RescueTrimVertexStub::CopyFrom(RescueTrimVertexStub *other) {
  wireframeId = other->wireframeId;
  objectId = other->objectId;
  wireframeObj = other->wireframeObj;
  loadNo = other->loadNo;
  actualVertex = other->actualVertex;
}

RescueTrimVertex *RescueTrimVertexStub::TrimVertex(RescueModel *model, RESCUEBOOL loadIfNeeded) {
  if (wireframeObj == 0 && model != 0) {
    wireframeObj = model->WireframeIdentifiedBy(wireframeId);
  }
  if (wireframeObj != 0) {
    if (wireframeObj->IsWireframeLoaded() == FALSE) {
      actualVertex = 0;
      if (loadIfNeeded) {
        wireframeObj->LoadWireframe();
      }
    }
    /*
      If not currently loaded, load it up if the user has given us that discretion.
    */
    if (actualVertex != 0) {
      if (loadNo != wireframeObj->LoadNo()) {
        actualVertex = 0;
      }
    }
    /*
      If we have been asked before, we may have a pointer to the object that we want,
      but first we have to check to see if it is stale.
    */
    if (actualVertex == 0) {
      actualVertex = wireframeObj->TrimVertexIdentifiedBy(objectId);
      loadNo = wireframeObj->LoadNo();
      /*
        If we do hook up to the object, remember which load.  If the reader unloads the
        RescueWireframe and then reloads it our pointer will be stale.
      */
    }
  } else {
    actualVertex = 0;
  }
  return actualVertex;
}
