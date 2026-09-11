/********************************************************************

  RescueWireframeOwner.cpp

  A RescueHistory object which, like RescueBlockUnit and RescueGeobodyPart,
  is capable of owning a RescueWireframe.

  Rod Hanks,  March 2001


*********************************************************************/
#include "RescueWireframeOwner.h"
#include "RescueModel.h"
#include "RescueWireframe.h"

RescueWireframe *RescueWireframeOwner::Wireframe() {
  if (ParentModel()->IsWireframeLoaded() == FALSE) {
    if (wireframe != 0) {
      wireframe = 0;
    }
  } else {
    if (wireframe == 0) {
      wireframe = new RescueWireframe(ParentModel(), this);
    }
  }
  return wireframe;
}

RESCUEBOOL RescueWireframeOwner::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueWireframeOwner) {
    return TRUE;
  } else {
    return RescueHistoryObject::IsOfType(thisType);
  }
}
