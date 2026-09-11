/********************************************************************

  RescueGeobodySurface.cpp

  An association between a RescueSurface and a RescueGeobodyVolume.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "RescueGeobodySurface.h"
#include "RescueBuffer.h"
#include "RescueEdgeSet.h"
#include "RescueEdgeSetStub.h"
#include "RescueGeobody.h"
#include "RescueGeobodyPart.h"
#include "RescueGeobodyVolume.h"
#include "RescueHistory.h"
#include "RescueModel.h"
#include "RescueSurface.h"
#include "RescueWireframe.h"
#include "myHeaders.h"
#include <string.h>

void RescueGeobodySurface::AutoEdgeSet() {
  RescueEdgeSet *edgeSet = new RescueEdgeSet(Surface());
  edgeSet->SetRescueBusinessOwner(this);
  edges = new RescueEdgeSetStub(Surface()->ParentModel()->Context(), edgeSet);
}

RescueEdgeSetStub *RescueGeobodySurface::EdgesObj() {
  if (Surface()->ParentModel()->IsWireframeLoaded() == FALSE) {
    return 0;
  } else {
    if (edges == 0) {
      AutoEdgeSet();
    }
    return edges;
  }
}

RescueEdgeSet *RescueGeobodySurface::Edges() {
  RescueModel *model = Surface()->ParentModel();
  if (model->IsWireframeLoaded() == FALSE) {
    return 0;
  } else {
    if (edges == 0) {
      AutoEdgeSet();
    }
    RescueEdgeSet *myReturn = edges->EdgeSet(model);
    if (myReturn == 0) {
      delete edges;
      AutoEdgeSet();
      myReturn = edges->EdgeSet(model);
    }
    return myReturn;
  }
}

RescueGeobodySurface::RescueGeobodySurface(RescueGeobodyVolume *parentVolumeIn, RescueSurface *existingSurface,
                                           Role roleIn)
    : RescueObject(parentVolumeIn->ParentPart()->Body()->ParentModel()->Context()) {
  isA = R_RescueGeobodySurface;
  parentVolume = parentVolumeIn;
  edges = 0;
  surface = existingSurface;
  role = roleIn;
}

RescueGeobodySurface::~RescueGeobodySurface() {
  delete edges;
  /*
    The surface itself is owned elsewhere.
  */
}

RescueGeobodySurface::RescueGeobodySurface(RescueContext *context, FILE *archiveFile) : RescueObject(context) {
  isA = R_RescueGeobodySurface;
  ReadId(context, archiveFile);
  RESCUEINT64 type;
  myfscanf(context, archiveFile, &type);
  surfaceType = (_RescueObjectType)type;
  myfscanf(context, archiveFile, &surfaceID);
  surface = 0;
  RESCUEINT64 roleInt;
  myfscanf(context, archiveFile, &roleInt);
  role = (Role)roleInt;
  edges = 0;
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueGeobodySurface::Archive(FILE *archiveFile) {
  RescueContext *context = parentVolume->ParentPart()->Body()->ParentModel()->Context();
  myfprintf(context, archiveFile, "; RescueGeobodySurface");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, (RESCUEINT64)surface->IsA());
  myfprintf(context, archiveFile, surface->Identifier());
  RESCUEINT64 roleInt = (RESCUEINT64)role;
  myfprintf(context, archiveFile, roleInt);
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

void RescueGeobodySurface::Relink(RescueObject *parentGeobodyVolume) {
  parentVolume = (RescueGeobodyVolume *)parentGeobodyVolume;
  RescueModel *parentModel = parentVolume->ParentPart()->Body()->ParentModel();
  surface = (RescueSurface *)RescueHistory::FindObject(parentModel, surfaceType, surfaceID);
}

void RescueGeobodySurface::DropWireframeMemory() {
  if (edges != 0) {
    delete edges;
    edges = 0;
  }
}

void RescueGeobodySurface::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile) {
  RescueContext *context = model->Context();
  if (edges != 0) {
    delete edges;
    edges = 0;
  }
  if (context->ReadFileVersion() >= 28) {
    RESCUEINT64 flag;
    myfscanf(context, archiveFile, &flag);
    if (flag > 0) {
      edges = new RescueEdgeSetStub(context, archiveFile);
    }
  } else {
    RescueEdgeSet *edgesObj = new RescueEdgeSet(context, archiveFile);
    edges = new RescueEdgeSetStub(context, edgesObj);
    model->wireframes->SaveCompatibleEdgeSet(edgesObj, this, edges);
  }
}

void RescueGeobodySurface::RelinkWireframeData(RescueObject *parent) {}

void RescueGeobodySurface::ArchiveWireframeData(FILE *archiveFile) {
  RescueContext *context = parentVolume->ParentPart()->Body()->ParentModel()->Context();
  if (context->FileVersion() >= 28) {
    if (edges == 0) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    } else {
      myfprintf(context, archiveFile, (RESCUEINT64)1);
      edges->ArchiveStub(context, archiveFile);
    }
  } else {
    RescueEdgeSet *edgesObj = 0;
    if (edges != 0) {
      edgesObj = edges->EdgeSet(Surface()->ParentModel());
    }
    if (edgesObj == 0) {
      edgesObj = new RescueEdgeSet(parentVolume->ParentPart()->Body());
      if (edges != 0) {
        delete edges;
      }
      edges = new RescueEdgeSetStub(context, edgesObj);
    }
    edgesObj->Archive(archiveFile);
  }
}

void RescueGeobodySurface::FindUniquePropertyNames(cSetString *container) {
  Surface()->FindUniquePropertyNames(container);
}
