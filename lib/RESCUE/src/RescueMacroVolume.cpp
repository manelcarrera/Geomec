/********************************************************************

  RescueMacroVolume.cpp

  The macro model description for a volume in a block unit.

  Rod Hanks,  September 1997

*********************************************************************/
#include "RescueMacroVolume.h"
#include "RescueModel.h"
#include "RescueIdTree.h"
#include "cSetRescueEdgeSet.h"
#include "RescueEdgeSetStub.h"
#include "RescueWireframe.h"
#include "cBagRescueEdgeSet.h"
#include "RescueBuffer.h"
#include <string.h>

RescueMacroVolume::RescueMacroVolume(RescueBlockUnit *parentBUin)
                          :RescueHistoryObject(parentBUin->ParentModel()->Context())
                          ,oldEdgeSets(0),oldTopEdge(0),oldBottomEdge(0)
{
  isA = R_RescueMacroVolume;
  parentBU = parentBUin;
  topEdges = 0;
  bottomEdges = 0;
  blockUnitSides = new cSetRescueBlockUnitSide();
  interiorSections = new cBagRescueSection();
  kLayerEdges = new cSetRescueEdgeSetStub();
  iLayerEdges = new cSetRescueEdgeSetStub();
  jLayerEdges = new cSetRescueEdgeSetStub();
  interiorSectionIDs = 0;
}

RescueEdgeSet *RescueMacroVolume::TopEdges()
{
  RescueEdgeSet *myReturn = 0;
  if (topEdges != 0)
  {
  myReturn = topEdges->EdgeSet(ParentModel());
  }
  return myReturn;
}

RescueEdgeSet *RescueMacroVolume::BottomEdges()
{
  RescueEdgeSet *myReturn = 0;
  if (bottomEdges != 0)
  {
  myReturn = bottomEdges->EdgeSet(ParentModel());
  }
  return myReturn;
}

void RescueMacroVolume::SetTopEdge(RescueEdgeSet *topEdge)
{
  RescueContext *context =parentBU->ParentModel()->Context();
  if (topEdges != 0)
  {
  delete topEdges;
  }
  topEdges = new RescueEdgeSetStub(context, topEdge);
  topEdge->SetRescueBusinessOwner(this);
}

void RescueMacroVolume::SetBottomEdge(RescueEdgeSet *bottomEdge)
{
  RescueContext *context =parentBU->ParentModel()->Context();
  if (bottomEdges != 0)
  {
  delete bottomEdges;
  }
  bottomEdges = new RescueEdgeSetStub(context, bottomEdge);
  bottomEdge->SetRescueBusinessOwner(this);
}

RescueMacroVolume::~RescueMacroVolume()
{
  if (topEdges != 0)
  {
  delete topEdges;
  }
  if (bottomEdges != 0)
  {
  delete bottomEdges;
  }
  delete blockUnitSides;
  delete interiorSections;
  delete kLayerEdges;
  delete iLayerEdges;
  delete jLayerEdges;
}

void RescueMacroVolume::PartialRead(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 loop, count;

  myfscanf(context, archiveFile, &count);
  for (loop = 0; loop < count; loop++)
  {
  RESCUEINT64 id;

  if (interiorSectionIDs == 0)
  {
      interiorSectionIDs = new cBagInt();
  }
  myfscanf(context, archiveFile, &id);
  (*interiorSectionIDs) += id;
  }
  blockUnitSides = new cSetRescueBlockUnitSide();
  blockUnitSides->UnArchive(context, archiveFile);
  kLayerEdges = new cSetRescueEdgeSetStub();
  iLayerEdges = new cSetRescueEdgeSetStub();
  jLayerEdges = new cSetRescueEdgeSetStub();
  if (context->ReadFileVersion() <= 12)
  {
  oldEdgeSets = new cSetRescueEdgeSet();
  oldEdgeSets->UnArchive(context, archiveFile);
  }
}

RescueMacroVolume::RescueMacroVolume(RescueContext *context, FILE *archiveFile)
                                      :RescueHistoryObject(context)
                                      ,parentBU(0)
                                      ,blockUnitSides(0)
                                      ,interiorSections(0)
                                      ,topEdges(0)
                                      ,bottomEdges(0)
                                      ,kLayerEdges(0)
                                      ,iLayerEdges(0)
                                      ,jLayerEdges(0)
                                      ,interiorSectionIDs(0)
                                      ,oldEdgeSets(0)
                                      ,oldTopEdge(0)
                                      ,oldBottomEdge(0)
{
  isA = R_RescueMacroVolume;
  if (context->ReadFileVersion() >= 20)
  {
  ReadId(context, archiveFile);
  }
  PartialRead(context, archiveFile);
  if (context->ReadFileVersion() <= 12)
  {
  RESCUEINT64 flag;
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
      oldTopEdge = new RescueEdgeSet(context, archiveFile);
  }
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
      oldBottomEdge = new RescueEdgeSet(context, archiveFile);
  }
  }
  context->macroVolumes->Add(this);
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

void RescueMacroVolume::DropWireframeMemory()
{
  if (topEdges != 0)
  {
  delete topEdges;
  topEdges = 0;
  }
  if (bottomEdges != 0)
  {
  delete bottomEdges;
  bottomEdges = 0;
  }
  delete kLayerEdges;
  kLayerEdges = new cSetRescueEdgeSetStub();
  delete iLayerEdges;
  iLayerEdges = new cSetRescueEdgeSetStub();
  delete jLayerEdges;
  jLayerEdges = new cSetRescueEdgeSetStub();
  blockUnitSides->DropWireframeMemory();
}

void RescueMacroVolume::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RescueContext *context = parentBU->ParentModel()->Context();
  oldEdgeSets = 0;
  oldTopEdge = 0;
  oldBottomEdge = 0;

  RESCUEINT64 flag;
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  if (context->ReadFileVersion() >= 28)
  {
      topEdges = new RescueEdgeSetStub(context, archiveFile);
  }
  else
  {
      RescueEdgeSet *edgesObj = new RescueEdgeSet(context, archiveFile);
      topEdges = new RescueEdgeSetStub(context, edgesObj);
      model->wireframes->SaveCompatibleEdgeSet(edgesObj, this, topEdges);
  }
  }
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  if (context->ReadFileVersion() >= 28)
  {
      bottomEdges = new RescueEdgeSetStub(context, archiveFile);
  }
  else
  {
      RescueEdgeSet *edgesObj = new RescueEdgeSet(context, archiveFile);
      bottomEdges = new RescueEdgeSetStub(context, edgesObj);
      model->wireframes->SaveCompatibleEdgeSet(edgesObj, this, bottomEdges);
  }
  }
  blockUnitSides->UnArchiveWireframeData(model, archiveFile);
  delete kLayerEdges;
  kLayerEdges = new cSetRescueEdgeSetStub();
  delete iLayerEdges;
  iLayerEdges = new cSetRescueEdgeSetStub();
  delete jLayerEdges;
  jLayerEdges = new cSetRescueEdgeSetStub();
  if (context->ReadFileVersion() >= 17)
  {
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
  }
  if (context->ReadFileVersion() >= 32)
  {
  iLayerEdges->UnArchive(context, archiveFile);
  jLayerEdges->UnArchive(context, archiveFile);
  }
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

void RescueMacroVolume::RelinkWireframeData(RescueObject *parentModel)
{
  blockUnitSides->RelinkWireframeData(parentModel);
}

void RescueMacroVolume::ArchiveWireframeData(FILE *archiveFile)
{
  RescueContext *context = parentBU->ParentModel()->Context();
  WriteActualEdgeSet(archiveFile, topEdges);
  WriteActualEdgeSet(archiveFile, bottomEdges);
  blockUnitSides->ArchiveWireframeData(archiveFile);
  if (context->FileVersion() >= 28)
  {
  kLayerEdges->Archive(context, archiveFile);
  }
  else
  {
  WriteActualEdgeSet(archiveFile);
  }
  if (context->FileVersion() >= 32)
  {
  iLayerEdges->Archive(context, archiveFile);
  jLayerEdges->Archive(context, archiveFile);
  }
/*
  Archive the actual object.
*/
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueMacroVolume::WriteActualEdgeSet(FILE *archiveFile, RescueEdgeSetStub *toWrite)
{
  RescueContext *context = parentBU->ParentModel()->Context();
  if (toWrite == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  if (context->FileVersion() >= 28)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      toWrite->Archive(context, archiveFile);
  }
  else
  {
      RescueEdgeSet *actualSet = toWrite->EdgeSet(ParentModel());
      if (actualSet == 0)
      {
    myfprintf(context, archiveFile, (RESCUEINT64) 0);
      }
      else
      {
    myfprintf(context, archiveFile, (RESCUEINT64) 1);
    actualSet->Archive(archiveFile);
      }
  }
  }
}

void RescueMacroVolume::WriteActualEdgeSet(FILE *archiveFile)
{
  RescueContext *context = parentBU->ParentModel()->Context();
  RescueModel *model = ParentModel();
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

void RescueMacroVolume::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;

  parentBU = (RescueBlockUnit *) parent;
  RescueModel *parentModel = ParentModel();
  RescueContext *context = parentModel->Context();
  blockUnitSides->Relink(this);
  interiorSections = new cBagRescueSection();
  if (interiorSectionIDs != 0)
  {
  for (loop = 0; loop < interiorSectionIDs->Count64(); loop++)
  {
      (*interiorSections) += 
    parentModel->SectionIdentifiedBy(
            interiorSectionIDs->NthObject(loop));
  }
  delete interiorSectionIDs;
  interiorSectionIDs = 0;
  }
  if (oldEdgeSets != 0)
  {
  RESCUEINT64 howMany = oldEdgeSets->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
      RescueEdgeSet *oldSet = oldEdgeSets->NthObject(loop);
      RescueEdgeSetStub *stub = new RescueEdgeSetStub(context, oldSet);
      parentModel->wireframes->SaveCompatibleEdgeSet(oldSet, this, stub);
      (*kLayerEdges) += stub;
  }
  oldEdgeSets->GiveUpObjects();
  delete oldEdgeSets;
  oldEdgeSets = 0;
  }
  if (oldTopEdge != 0)
  {
  topEdges = new RescueEdgeSetStub(context, oldTopEdge);
  parentModel->wireframes->SaveCompatibleEdgeSet(oldTopEdge, this, topEdges);
  oldTopEdge = 0;
  }
  if (oldBottomEdge != 0)
  {
  bottomEdges = new RescueEdgeSetStub(context, oldBottomEdge);
  parentModel->wireframes->SaveCompatibleEdgeSet(oldBottomEdge, this, bottomEdges);
  oldBottomEdge = 0;
  }
}

void RescueMacroVolume::Archive(FILE *archiveFile)
{
  RescueContext *context = parentBU->ParentModel()->Context();
  RESCUEINT64 loop;

  myfprintf(context, archiveFile, "; Macro Volume");
  if (context->FileVersion() >= 20)
  {
  myfprintf(context, archiveFile, Identifier());
  }
  myfprintf(context, archiveFile, interiorSections->Count64());
  for (loop = 0; loop < interiorSections->Count64(); loop++)
  {
   RescueSection *group = interiorSections->NthObject(loop);
   myfprintf(context, archiveFile, group->Identifier());
  }
  blockUnitSides->Archive(context, archiveFile);
  if (context->FileVersion() == 9)
  {
  WriteActualEdgeSet(archiveFile);
  WriteActualEdgeSet(archiveFile, topEdges);
  WriteActualEdgeSet(archiveFile, bottomEdges);
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueMacroVolume::AddKLayerEdge(RescueEdgeSet *existingEdgeSet)
{
  RescueContext *context = parentBU->ParentModel()->Context();
  (*kLayerEdges) += new RescueEdgeSetStub(context, existingEdgeSet);
  existingEdgeSet->SetRescueBusinessOwner(this);
}
 
RESCUEBOOL RescueMacroVolume::DeleteKLayerEdge(RescueEdgeSet *existingEdgeSet)
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

void RescueMacroVolume::AddILayerEdge(RescueEdgeSet *existingEdgeSet)
{
  RescueContext *context = parentBU->ParentModel()->Context();
  (*iLayerEdges) += new RescueEdgeSetStub(context, existingEdgeSet);
  existingEdgeSet->SetRescueBusinessOwner(this);
}
 
RESCUEBOOL RescueMacroVolume::DeleteILayerEdge(RescueEdgeSet *existingEdgeSet)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 howMany = iLayerEdges->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == FALSE; loop++)
  {
  RescueEdgeSetStub *stub = iLayerEdges->NthObject(loop);
  if (stub->Equals(existingEdgeSet))
  {
      myReturn = ((*iLayerEdges) -= loop);
  }
  }
  return myReturn;
}

void RescueMacroVolume::AddJLayerEdge(RescueEdgeSet *existingEdgeSet)
{
  RescueContext *context = parentBU->ParentModel()->Context();
  (*jLayerEdges) += new RescueEdgeSetStub(context, existingEdgeSet);
  existingEdgeSet->SetRescueBusinessOwner(this);
}
 
RESCUEBOOL RescueMacroVolume::DeleteJLayerEdge(RescueEdgeSet *existingEdgeSet)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 howMany = jLayerEdges->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == FALSE; loop++)
  {
  RescueEdgeSetStub *stub = jLayerEdges->NthObject(loop);
  if (stub->Equals(existingEdgeSet))
  {
      myReturn = ((*jLayerEdges) -= loop);
  }
  }
  return myReturn;
}

void RescueMacroVolume::AddBlockUnitSide(RescueBlockUnitSide *existingSide)
{
  (*blockUnitSides) += existingSide;
  existingSide->parentVolume = this;
}

RESCUEBOOL RescueMacroVolume::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueMacroVolume)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}

RESCUEINT32 RescueMacroVolume::CountOfSides(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CountOfSides();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}



