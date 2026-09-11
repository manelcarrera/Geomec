/********************************************************************

  RescueVertex.cpp

  Vertex (used for Coordinate system) for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueVertex.h"
#include "RescueBuffer.h"
#include "RescueModel.h"
#include "RescueUnit.h"
#include <string.h>

RescueVertex::~RescueVertex() {
  if (vertexName != 0) {
    delete vertexName;
  }
  if (parentModel != 0) {
    parentModel->UnRegisterObject(this);
  }
}

RescueVertex::RescueVertex(const RESCUECHAR *name, RescueCoordinateSystem *existingCoordinateSystem, RESCUEDOUBLE xIn,
                           RESCUEDOUBLE yIn, RESCUEDOUBLE zIn)
    : RescueObject(existingCoordinateSystem->Context()), vertexName(0), coordinateSystem(existingCoordinateSystem),
      x(xIn), y(yIn), z(zIn), parentModel(0) {
  vertexName = new RCHString(name);
  isA = R_RescueVertex;
}

void RescueVertex::SetXYZ(RESCUEFLOAT xIn, RESCUEFLOAT yIn, RESCUEFLOAT zIn) {
  x = xIn;
  y = yIn;
  z = zIn;
}

void RescueVertex::RegisterWith(RescueModel *parentModelIn) {
  if (parentModel == 0) {
    parentModel = parentModelIn;
    parentModel->RegisterObject(this);
    if (coordinateSystem != 0) {
      coordinateSystem->RegisterWith(parentModel);
    }
  }
}

RESCUEBOOL RescueVertex::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueVertex) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}

RescueVertex::RescueVertex(RescueContext *context, FILE *archiveFile)
    : RescueObject(context), vertexName(0), coordinateSystem(0), parentModel(0) {
  RESCUECHAR myString[255];
  RESCUEINT64 csFlag;

  isA = R_RescueVertex;
  myfgets(context, myString, 255, archiveFile);
  vertexName = new RCHString(myString);
  myfscanf(context, archiveFile, &x);
  myfscanf(context, archiveFile, &y);
  myfscanf(context, archiveFile, &z);
  myfscanf(context, archiveFile, &csFlag);
  if (csFlag == 1) {
    coordinateSystem = new RescueCoordinateSystem(context, archiveFile);
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

void RescueVertex::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, "; Vertex");
  myfprintf(context, archiveFile, vertexName->String());
  myfprintf(context, archiveFile, x);
  myfprintf(context, archiveFile, y);
  myfprintf(context, archiveFile, z);
  if (coordinateSystem == 0) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  } else {
    myfprintf(context, archiveFile, (RESCUEINT64)1);
    coordinateSystem->Archive(archiveFile);
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}
