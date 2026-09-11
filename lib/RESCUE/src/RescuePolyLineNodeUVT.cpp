/********************************************************************

  RescuePolyLineNodeUVT.cpp

  The description of a point in terms of it's position on some
  the face of some triangle in a RescueTriangulatedSurface

  Rod Hanks,  May 2002

*********************************************************************/
#include "RescuePolyLineNodeUVT.h"
#include "RescueBuffer.h"
#include "RescueModel.h"
#include "RescueSurface.h"
#include "RescueTriangulatedSurface.h"
#include <string.h>

RescueTriangulatedSurface *RescuePolyLineNodeUVT::Face(RescueModel *model) {
  if (face == 0) {
    face = model->TriangulatedSurfaceIdentifiedBy(faceID);
  }
  return face;
}

RESCUEBOOL RescuePolyLineNodeUVT::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescuePolyLineNodeUVT) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}

RESCUEBOOL RescuePolyLineNodeUVT::CompareFace(RescueTriangulatedSurface *toCompare) {
  RESCUEBOOL myReturn = FALSE;
  if (toCompare == face) {
    myReturn = TRUE;
  } else if (toCompare != 0) {
    if (toCompare->Identifier() == faceID) {
      myReturn = TRUE;
    }
  }
  return myReturn;
}

void RescuePolyLineNodeUVT::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, "; PolyLineNodeUVT");
  myfprintf(context, archiveFile, uValue);
  myfprintf(context, archiveFile, vValue);
  myfprintf(context, archiveFile, faceNdx);
  if (faceID == 0) {
    faceID = face->Identifier();
  }
  myfprintf(context, archiveFile, faceID);
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescuePolyLineNodeUVT::RescuePolyLineNodeUVT(RescueContext *context, FILE *archiveFile)
    : RescueObject(context), face(0) {
  isA = R_RescuePolyLineNodeUVT;
  myfscanf(context, archiveFile, &uValue);
  myfscanf(context, archiveFile, &vValue);
  myfscanf(context, archiveFile, &faceNdx);
  myfscanf(context, archiveFile, &faceID);
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}
