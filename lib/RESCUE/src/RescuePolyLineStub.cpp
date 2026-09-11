/********************************************************************

  RescuePolyLineStub.cpp

  A RescuePolyLine object that has been lazily relinked.

  You can preview the object here and trade it for a real one
  later.

  Rod Hanks,  March 2001


*********************************************************************/
#include "RescuePolyLineStub.h"
#include "RescueModel.h"
#include "RescuePolyLine.h"
#include "RescueWireframe.h"
#include "myHeaders.h"

RESCUEBOOL RescuePolyLineStub::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescuePolyLineStub) {
    return TRUE;
  } else {
    return RescueWireframeStub::IsOfType(thisType);
  }
}

RESCUEBOOL RescuePolyLineStub::Equals(RescuePolyLineStub *other) {
  RESCUEBOOL myReturn = FALSE;
  if (other->WireframeId() == WireframeId() && other->ObjectId() == ObjectId()) {
    myReturn = TRUE;
  }
  /*
    Really should be sufficient just to compare the object ids.
  */
  return myReturn;
}

RESCUEBOOL RescuePolyLineStub::Equals(RescuePolyLine *other) {
  RESCUEBOOL myReturn = FALSE;
  if (other->Identifier() == ObjectId()) {
    myReturn = TRUE;
  }
  return myReturn;
}

RescuePolyLineStub::RescuePolyLineStub(RescueContext *context, FILE *archiveFile)
    : RescueWireframeStub(context, archiveFile), actualLine(0) {
  isA = R_RescuePolyLineStub;
}

RescuePolyLineStub::RescuePolyLineStub(RescueContext *context, RESCUEINT64 wireframeIdIn, RESCUEINT64 objectIdIn)
    : RescueWireframeStub(context, wireframeIdIn, objectIdIn), actualLine(0) {
  isA = R_RescuePolyLineStub;
}

RescuePolyLineStub::RescuePolyLineStub(RescueContext *context, RescuePolyLine *lineIn)
    : RescueWireframeStub(context, lineIn->Owner()->Identifier(), lineIn->Identifier()), actualLine(lineIn) {
  wireframeObj = lineIn->Owner();
  loadNo = wireframeObj->LoadNo();
}

RescuePolyLine *RescuePolyLineStub::PolyLine(RescueModel *model, RESCUEBOOL loadIfNeeded) {
  if (wireframeObj == 0 && model != 0) {
    wireframeObj = model->WireframeIdentifiedBy(wireframeId);
  }
  if (wireframeObj != 0) {
    if (wireframeObj->IsWireframeLoaded() == FALSE) {
      actualLine = 0;
      if (loadIfNeeded) {
        wireframeObj->LoadWireframe();
      }
    }
    /*
      If not currently loaded, load it up if the user has given us that discretion.
    */
    if (actualLine != 0) {
      if (loadNo != wireframeObj->LoadNo()) {
        actualLine = 0;
      }
    }
    /*
      If we have been asked before, we may have a pointer to the object that we want,
      but first we have to check to see if it is stale.
    */
    if (actualLine == 0) {
      actualLine = wireframeObj->PolyLineIdentifiedBy(objectId);
      loadNo = wireframeObj->LoadNo();
      /*
        If we do hook up to the object, remember which load.  If the reader unloads the
        RescueWireframe and then reloads it our pointer will be stale.
      */
    }
  } else {
    actualLine = 0;
  }
  return actualLine;
  ;
}
