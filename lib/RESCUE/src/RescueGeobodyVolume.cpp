/********************************************************************

  RescueGeobodyVolume.cpp

  The volume decription for a RescueGeobodyPart.

  Rod Hanks,  June 1999

*********************************************************************/
#include "myHeaders.h"
#include "RescueGeobodyPart.h"
#include "RescueGeobodyVolume.h"
#include "RescueGeobodySurface.h"
#include "RescueModel.h"
#include "RescueIdTree.h"
#include "RescueEdgeSetStub.h"
#include "RescueWireframe.h"
#include "RescueGeobody.h"
#include "cBagRescueEdgeSet.h"
#include "RescueBuffer.h"
#include <string.h>

void RescueGeobodyVolume::AddKLayerEdge(RescueEdgeSet *existingEdgeSet)
{
  (*kLayerEdges) += new RescueEdgeSetStub(this->ParentPart()->Body()->ParentModel()->Context(), 
                                          existingEdgeSet);
  existingEdgeSet->SetRescueBusinessOwner(this);
}
 
RESCUEBOOL RescueGeobodyVolume::DeleteKLayerEdge(RescueEdgeSet *existingEdgeSet)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 howMany = kLayerEdges->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == FALSE; loop++)
  {
  RescueEdgeSetStub *stub = kLayerEdges->NthObject(loop);
  if (stub->Equals(existingEdgeSet))
  {
      myReturn = ((*kLayerEdges) -= loop);
  }
  }
  return myReturn;
}

void RescueGeobodyVolume::AddGeobodySurface(RescueSurface *existingSurface, 
                      RescueGeobodySurface::Role role)
{
  RescueGeobodySurface *newSurface = new RescueGeobodySurface(this, existingSurface, role);
  (*surfaces) += newSurface;
}

void RescueGeobodyVolume::DropGeobodySurface(RescueSurface *existingSurface)
{
  RESCUEINT64 ndx = 0;
  RescueGeobodySurface *surface = surfaces->NthObject(ndx);
  while (surface != 0)
  {
  if (surface->Surface() == existingSurface)
  {
      (*surfaces) -= surface;
  }
  else
  {
      ndx++;
  }
  surface = surfaces->NthObject(ndx);
  }
}

RescueGeobodyVolume::~RescueGeobodyVolume()
{
  delete surfaces;
  delete kLayerEdges;
}

RESCUEBOOL RescueGeobodyVolume::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueGeobodyVolume)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}

RescueGeobodyVolume::RescueGeobodyVolume(RescueGeobodyPart *part)
          :RescueHistoryObject(part->Body()->ParentModel()->Context())
          ,parentPart(part)
{
  isA = R_RescueGeobodyVolume;
  surfaces = new cSetRescueGeobodySurface();
  kLayerEdges = new cSetRescueEdgeSetStub();
}

RescueGeobodyVolume::RescueGeobodyVolume(RescueContext *context, FILE *archiveFile)
          :RescueHistoryObject(context)
          ,parentPart(0)
{
  isA = R_RescueGeobodyVolume;
  surfaces = new cSetRescueGeobodySurface();
  kLayerEdges = new cSetRescueEdgeSetStub();
  ReadId(context, archiveFile);
  surfaces->UnArchive(context, archiveFile);
  context->geobodyVolumes->Add(this);
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
  }
  }
}

void RescueGeobodyVolume::Archive(FILE *archiveFile)
{
  RescueContext *context = parentPart->Body()->ParentModel()->Context();
  myfprintf(context, archiveFile, "; RescueGeobodyVolume");
  myfprintf(context, archiveFile, Identifier());
  surfaces->Archive(context, archiveFile);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueGeobodyVolume::Relink(RescueObject *parentIn)
{
  parentPart = (RescueGeobodyPart *) parentIn;
  surfaces->Relink(this);
}

void RescueGeobodyVolume::DropWireframeMemory()
{
  delete kLayerEdges;
  kLayerEdges = new cSetRescueEdgeSetStub();
  surfaces->DropWireframeMemory();
}

void RescueGeobodyVolume::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RescueContext *context = model->Context();
  delete kLayerEdges;
  kLayerEdges = new cSetRescueEdgeSetStub();
  if (context->ReadFileVersion() >= 28)
  {
  kLayerEdges->UnArchive(context, archiveFile);
  }
  else
  {
  RESCUEINT64 howMany;
  myfscanf(context, archiveFile, &howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
      RescueEdgeSet *edgesObj = new RescueEdgeSet(context, archiveFile);
      RescueEdgeSetStub *stub = new RescueEdgeSetStub(context, edgesObj);
      model->wireframes->SaveCompatibleEdgeSet(edgesObj, this, stub);
      (*kLayerEdges) += stub;
  }
  }
  surfaces->UnArchiveWireframeData(model, archiveFile);
}

void RescueGeobodyVolume::RelinkWireframeData(RescueObject *parentModel)
{
  surfaces->RelinkWireframeData(parentModel);
}

void RescueGeobodyVolume::ArchiveWireframeData(FILE *archiveFile)
{
  RescueContext *context = parentPart->Body()->ParentModel()->Context();
  if (context->FileVersion() >= 28)
  {
  kLayerEdges->Archive(context, archiveFile);
  }
  else
  {
  WriteActualEdgeSet(context, archiveFile);
  }
  surfaces->ArchiveWireframeData(archiveFile);
}

void RescueGeobodyVolume::WriteActualEdgeSet(RescueContext *context, FILE *archiveFile)
{
  RescueModel *model = ParentPart()->Body()->ParentModel();
  cBagRescueEdgeSet *setObjs = new cBagRescueEdgeSet();
  RESCUEINT64 ordinal = 0;
  RescueEdgeSetStub *stub = kLayerEdges->NthObject(ordinal++);
  while (stub != 0)
  {
  RescueEdgeSet *edgeObj = stub->EdgeSet(model);
  if (edgeObj != 0)
  {
      (*setObjs) += edgeObj;
  }
  stub = kLayerEdges->NthObject(ordinal++);
  }
/*
  Because we don't know if we have any "nonsignifigant trailing branches"
  or not, we have to get the list of objects before we can write out the
  count.
*/
  RESCUEINT64 howMany = setObjs->Count64();
  myfprintf(context, archiveFile, howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
  RescueEdgeSet *edgeObj = setObjs->NthObject(loop);
  edgeObj->Archive(archiveFile);
  }
  delete setObjs;
}

void RescueGeobodyVolume::FindUniquePropertyNames(cSetString *container)
{
  surfaces->FindUniquePropertyNames(container);
}



