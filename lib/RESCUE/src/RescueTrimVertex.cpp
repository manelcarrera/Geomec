/********************************************************************

  RescueTrimVertex.cpp

  The TrimVertex anchors an end of a polyline.  It is just like a
  regular PolyLineNode, except that it keeps a list of the RescuePolyLineStub
  of which it is a member.  This list can be used to navigate along
  wireframes.

  Rod Hanks,  July 1996 / March 2001

*********************************************************************/
#include "RescueTrimVertex.h"
#include "RescueBuffer.h"
#include "RescueGeometry.h"
#include "RescueModel.h"
#include "RescuePolyLine.h"
#include "RescuePolyLineStub.h"
#include "RescueTriangleVertex.h"
#include "RescueTrimVertexStub.h"
#include "RescueWireframe.h"
#include "RescueWireframeOwner.h"
#include "cBagRescueTriangleVertex.h"
#include <string.h>

RescueTrimVertex::~RescueTrimVertex() {
  if (lines != 0) {
    delete lines;
  }
}

RescueTrimVertex::RescueTrimVertex(RescueWireframeOwner *ownerIn, RESCUEDOUBLE x, RESCUEDOUBLE y, RESCUEDOUBLE z)
    : RescuePolyLineNode(ownerIn->ParentModel()->Context(), x, y, z), lines(0), owner(ownerIn->Wireframe()) {
  lines = new cSetRescuePolyLineStub();
  isA = R_RescueTrimVertex;
  owner->AddTrimVertex(this);
}

RescueTrimVertex::RescueTrimVertex(RescueWireframeOwner *ownerIn, RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
    : RescuePolyLineNode(ownerIn->ParentModel()->Context(), x, y, z), lines(0), owner(ownerIn->Wireframe()) {
  lines = new cSetRescuePolyLineStub();
  isA = R_RescueTrimVertex;
  owner->AddTrimVertex(this);
}

RescueTrimVertex::RescueTrimVertex(RescueContext *context, FILE *archiveFile)
    : RescuePolyLineNode(context, archiveFile), lines(new cSetRescuePolyLineStub()), owner(0) {
  RESCUEINT64 loop, count;
  isA = R_RescueTrimVertex;
  ReadId(context, archiveFile);
  if (context->ReadFileVersion() < 23) {
    myfscanf(context, archiveFile, &count);
    if (count > 0) {
      for (loop = 0; loop < count; loop++) {
        RESCUEINT64 lineID;

        myfscanf(context, archiveFile, &lineID);
      }
    }
  } else if (context->ReadFileVersion() >= 28) {
    myfscanf(context, archiveFile, &count);
    if (count > 0) {
      for (loop = 0; loop < count; loop++) {
        RescuePolyLineStub *stub = new RescuePolyLineStub(context, archiveFile);
        (*lines) += stub;
      }
    }
  }
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueTrimVertex::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, "; Trim Vertex");
  RescuePolyLineNode::Archive(context, archiveFile);
  if (context->FileVersion() >= 27) {
    myfprintf(context, archiveFile, Identifier());
  }
  if (context->FileVersion() >= 28) {
    RESCUEINT64 loop, count = (*lines).Count64();
    myfprintf(context, archiveFile, count);
    for (loop = 0; loop < count; loop++) {
      (*lines).NthObject(loop)->ArchiveStub(context, archiveFile);
    }
  }
  if (context->FileVersion() < 27) {
    myfprintf(context, archiveFile, Identifier());
  }
  if (context->FileVersion() < 23) {
    RESCUEINT64 loop, count = (*lines).Count64();
    myfprintf(context, archiveFile, count);
    for (loop = 0; loop < count; loop++) {
      myfprintf(context, archiveFile, (*lines).NthObject(loop)->BestIdentifier());
    }
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

void RescueTrimVertex::Relink(RescueObject *parent) {
  owner = (RescueWireframe *)parent;
  RescuePolyLineNode::Relink(owner->ParentModel());
}

RESCUEBOOL RescueTrimVertex::Equals(RescueTrimVertexStub *other) {
  if (other == 0) {
    return FALSE;
  } else {
    return other->Equals(this);
  }
}

RESCUEBOOL RescueTrimVertex::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueTrimVertex) {
    return TRUE;
  } else {
    return RescuePolyLineNode::IsOfType(thisType);
  }
}
