/********************************************************************

  RescuePolyLine.cpp

  An edge on a wireframe drawing.  It contains RescueTrimVertexStub
  objects for the ends, because they might be stored in a separate
  Wireframe object, but it contains the actual RescuePolyLineNode
  because they will always be in the same RescueWireframe object.

  Rod Hanks,  July 1996 / March 2001

*********************************************************************/
#include "RescuePolyLine.h"
#include "RescueBuffer.h"
#include "RescueModel.h"
#include "RescuePolyLineStub.h"
#include "RescueTriangulatedSurface.h"
#include "RescueTrimVertex.h"
#include "RescueTrimVertexStub.h"
#include "RescueWireframe.h"
#include <string.h>

RescuePolyLine::~RescuePolyLine() {
  if (polyLineNodes != 0) {
    delete polyLineNodes;
  }
  if (leftVertex != 0) {
    delete leftVertex;
  }
  if (rightVertex != 0) {
    delete rightVertex;
  }
  /*
    We own the left and right vertex objects, which are just stubs.  We
    don't own the RescueTrimVertex objects they represent.
  */
}

RescuePolyLine::RescuePolyLine(RescueWireframeOwner *ownerIn, RescueTrimVertex *leftVertexIn,
                               RescueTrimVertex *rightVertexIn)
    : RescueObject(ownerIn->ParentModel()->Context()), polyLineNodes(0), leftVertex(0), rightVertex(0), owner(0) {
  RescueContext *context = ownerIn->ParentModel()->Context();
  polyLineNodes = new cSetRescuePolyLineNode(this);
  isA = R_RescuePolyLine;
  owner = ownerIn->Wireframe();
  owner->AddPolyLine(this);
  leftVertex = new RescueTrimVertexStub(leftVertexIn);
  (*leftVertexIn->lines) += new RescuePolyLineStub(context, this);
  rightVertex = new RescueTrimVertexStub(rightVertexIn);
  (*rightVertexIn->lines) += new RescuePolyLineStub(context, this);
}

RescuePolyLineNode *RescuePolyLine::Traverse(RESCUEINT64 zeroBasedOrdinal) {
  RescuePolyLineNode *myReturn = 0;
  if (zeroBasedOrdinal == 0) {
    myReturn = LeftVertex();
  } else if (zeroBasedOrdinal <= polyLineNodes->Count64()) {
    myReturn = polyLineNodes->NthObject(zeroBasedOrdinal - 1);
  } else if (zeroBasedOrdinal == polyLineNodes->Count64() + 1) {
    myReturn = RightVertex();
  }
  return myReturn;
}

RescueTrimVertex *RescuePolyLine::LeftVertex() {
  RescueTrimVertex *myReturn = 0;
  if (leftVertex != 0) {
    myReturn = leftVertex->TrimVertex(ParentModel());
  }
  return myReturn;
}

RescueTrimVertex *RescuePolyLine::RightVertex() {
  RescueTrimVertex *myReturn = 0;
  if (rightVertex != 0) {
    myReturn = rightVertex->TrimVertex(ParentModel());
  }
  return myReturn;
}

RescuePolyLine::RescuePolyLine(RescueContext *context, FILE *archiveFile)
    : RescueObject(context), polyLineNodes(0), leftVertex(0), rightVertex(0), owner(0) {
  polyLineNodes = new cSetRescuePolyLineNode(this);
  isA = R_RescuePolyLine;
  ReadId(context, archiveFile);

  if (context->ReadFileVersion() >= 28) {
    leftVertex = new RescueTrimVertexStub(context, archiveFile);
    rightVertex = new RescueTrimVertexStub(context, archiveFile);
    if (context->ReadFileVersion() >= 37) {
      RESCUECHAR myString[255];

      myfgets(context, myString, 255, archiveFile);
      while (strcmp(myString, "EOD") != 0) {
        RescueBuffer buf(context, archiveFile);
        myfgets(context, myString, 255, archiveFile);
      }
    } else if (context->readFileMainSoftwareVersion >= 37 && context->readFileSubSoftwareVersion < 5) {
      RESCUECHAR myString[255];
      myfgets(context, myString, 255, archiveFile);
    }
    /*
      Beginning with v37.0 there was a problem in writing RescuePolyLine in
      which the EOD marker was written for all models version 28 and above,
      although it applies only to v37.0 and above.  We have to consume this
      for the rest of the read to go okay.
    */
  } else {
    RESCUEINT64 leftVertexID;
    RESCUEINT64 rightVertexID;
    RESCUEINT64 leftVertexOwner;
    RESCUEINT64 leftVertexNdx;
    RESCUEINT64 rightVertexOwner;
    RESCUEINT64 rightVertexNdx;
    myfscanf(context, archiveFile, &leftVertexID);
    myfscanf(context, archiveFile, &rightVertexID);
    if (context->ReadFileVersion() >= 27) {
      myfscanf(context, archiveFile, &leftVertexOwner);
      myfscanf(context, archiveFile, &leftVertexNdx);
      myfscanf(context, archiveFile, &rightVertexOwner);
      myfscanf(context, archiveFile, &rightVertexNdx);
    }
    leftVertex = new RescueTrimVertexStub(context, 0, leftVertexID);
    rightVertex = new RescueTrimVertexStub(context, 0, rightVertexID);
  }
  polyLineNodes->UnArchive(context, archiveFile);
}

void RescuePolyLine::ArchiveStub(FILE *archiveFile) {
  RescueContext *context = ParentModel()->Context();
  myfprintf(context, archiveFile, owner->Identifier());
  myfprintf(context, archiveFile, Identifier());
}

void RescuePolyLine::Archive(FILE *archiveFile) {
  RescueContext *context = ParentModel()->Context();
  myfprintf(context, archiveFile, "; PolyLine");
  myfprintf(context, archiveFile, Identifier());
  if (context->FileVersion() >= 28) {
    leftVertex->ArchiveStub(context, archiveFile);
    rightVertex->ArchiveStub(context, archiveFile);
    if (context->FileVersion() >= 37) {
      myfprintf(context, archiveFile, "EOD"); // This was unconditional from 37.0 thru 37.4!
                                              // This resulted in some v28 thru v36 models being
                                              // written with an EOD flag in them.
    }
  } else {
    myfprintf(context, archiveFile, leftVertex->BestIdentifier());
    myfprintf(context, archiveFile, rightVertex->BestIdentifier());
    if (context->FileVersion() >= 27) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
      myfprintf(context, archiveFile, (RESCUEINT64)0);
      myfprintf(context, archiveFile, (RESCUEINT64)0);
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    }
  }
  polyLineNodes->Archive(context, archiveFile);
}

void RescuePolyLine::Relink(RescueObject *parent) {
  owner = (RescueWireframe *)parent;
  RescueModel *model = ParentModel();
  polyLineNodes->Relink(model);
}

RESCUEBOOL RescuePolyLine::Equals(RescuePolyLineStub *other) {
  if (other == 0) {
    return FALSE;
  } else {
    return other->Equals(this);
  }
}

RESCUEBOOL RescuePolyLine::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescuePolyLine) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}

RESCUEINT32 RescuePolyLine::InnerNodeCount(RESCUEBOOL throwIfTooBig) {
  RESCUEINT64 output = InnerNodeCount64();
  if (throwIfTooBig) {
    if (output > 2147483647 || output < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32)output;
}
