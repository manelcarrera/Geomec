/********************************************************************

  RescueTriangleVertex.cpp

  A point on a triangulated surface.  We have to be a subclass of
  RescueObject, despite the overhead, because we need the unique ID
  feature to maintain relationships during storage and retrieval.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "RescueTriangleVertex.h"
#include "RescueBuffer.h"
#include "RescueModel.h"
#include "RescueTriangleFace.h"
#include "RescueTriangulatedSurface.h"
#include "RescueWireframe.h"
#include "cBagRescueTriangleVertex.h"
#include "cSetRescueTriangleFace.h"
#include <string.h>

void RescueTriangleVertex::DropSelf(cSetRescueTriangleFace *faceSet) {
  if (faces != 0) {
    RESCUEINT64 loop;
    for (loop = 0; loop < faceCount; loop++) {
      faces[loop]->DropSelf();
      if (faceSet != 0) {
        (*faceSet) -= faces[loop];
      }
    }
  }
}

RescueTriangleVertex::~RescueTriangleVertex() {
  if (faces != 0) {
    free(faces);
  }
}

RescueTriangleFace *RescueTriangleVertex::NthFace(RESCUEINT64 zeroBasedOrdinal) {
  RescueTriangleFace *myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < faceCount) {
    myReturn = faces[zeroBasedOrdinal];
  }
  return myReturn;
}

void RescueTriangleVertex::AddToFace(RescueTriangleFace *newFace) {
  if (faces == 0) {
    faceCount = 0;
    faceAllocated = 5;
    faces = (RescueTriangleFace **)malloc(sizeof(RescueTriangleFace *) * (size_t)faceAllocated);
  } else if (faceCount >= faceAllocated) {
    faceAllocated += 5;
    faces = (RescueTriangleFace **)realloc(faces, sizeof(RescueTriangleFace *) * (size_t)faceAllocated);
  }
  faces[faceCount++] = newFace;
}

void RescueTriangleVertex::RemoveFromFace(RescueTriangleFace *oldFace) {
  RESCUEBOOL found = FALSE;
  RESCUEINT64 ndx = 0;
  while (ndx < faceCount) {
    if (found == FALSE) {
      if (faces[ndx] == oldFace) {
        found = TRUE;
      }
    } else {
      faces[ndx - 1] = faces[ndx];
    }
    ndx++;
  }
}

RescueTriangleVertex::RescueTriangleVertex(RescueContext *context, FILE *archiveFile)
    : RescueObject(context), faceCount(0), faceAllocated(0), faces(0) {
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &x);
  myfscanf(context, archiveFile, &y);
  myfscanf(context, archiveFile, &z);
  if (context->ReadFileVersion() < 32) {
    RESCUEINT64 avatarId;
    myfscanf(context, archiveFile, &avatarId);
    if (avatarId != 0) {
      if (context->ReadFileVersion() >= 28) {
        RESCUEINT64 wireframeId;
        RESCUEINT64 objectId;
        myfscanf(context, archiveFile, &wireframeId);
        myfscanf(context, archiveFile, &objectId);
      }
    }
  }
  /*
    If in an older version consume the avatar id but don't do anything with it.
  */
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueTriangleVertex::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, x);
  myfprintf(context, archiveFile, y);
  myfprintf(context, archiveFile, z);
  if (context->FileVersion() < 32) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  }
  /*
    For older versions write out a zero avatar marker.
  */
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}
