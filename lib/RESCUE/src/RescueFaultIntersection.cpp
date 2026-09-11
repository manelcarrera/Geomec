                                                  /********************************************************************

  RescueFaultIntersection.cpp

  This object allows the classification of horizons in a model
  by age, by declaring a depositional ordering of horizons.

  Rod Hanks,  June 2000

*********************************************************************/
#include "RescueModel.h"
#include "RescueFaultIntersection.h"
#include "RescueTrimLoopStub.h"
#include "RescueBuffer.h"
#include <string.h>

RescueFaultIntersection::RescueFaultIntersection(RescueModel *model,
                                                 RescueSection *majorSectionIn, 
                                                 RescueSection *minorSectionIn)
                                                 :RescueHistoryObject(model->Context())
                                                 ,parentModel(model)
                                                 ,majorSection(majorSectionIn)
                                                 ,minorSection(minorSectionIn)
                                                 ,edges(0)
                                                 ,loopId(0)
                                                 ,context(model->Context())
                                                 ,sectionEdges(0)
{
  isA = R_RescueFaultIntersection;
  (*model->faultIntersections) += this;
}

RescueEdgeSetStub *RescueFaultIntersection::EdgesObj()
{
  if (ParentModel()->IsWireframeLoaded() == FALSE)
  {
  return 0;
  }
  else
  {
  if (edges == 0)
  {
      AutoEdgeSet();
  }
  return edges;
  }
}

RescueEdgeSet *RescueFaultIntersection::Edges()
{
  RescueModel *model = ParentModel();
  if (model->IsWireframeLoaded() == FALSE)
  {
  return 0;
  }
  else
  {
  if (edges == 0)
  {
      AutoEdgeSet();
  }
  RescueEdgeSet *myReturn = edges->EdgeSet(model);
  if (myReturn == 0)
  {
      delete edges;
      AutoEdgeSet();
      myReturn = edges->EdgeSet(model);
  }
  return myReturn;
  }
}

void RescueFaultIntersection::AutoEdgeSet()
{
  RescueEdgeSet *edgeSet = new RescueEdgeSet(majorSection);
  edgeSet->SetRescueBusinessOwner(majorSection);
  edges = new RescueEdgeSetStub(parentModel->Context(), edgeSet);
}

void RescueFaultIntersection::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  myfscanf(model->Context(), archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
  RESCUEINT64 faultId;
  RescueEdgeSetStub *edgeStub = 0;
  myfscanf(model->Context(), archiveFile, &faultId);
  RescueFaultIntersection *fint = model->FaultIntersectionIdentifiedBy(faultId);
  if (model->Context()->ReadFileVersion() >= 30)
  {
      RESCUEINT64 flag;
      myfscanf(model->Context(), archiveFile, &flag);
      if (flag == 1)
      {
    edgeStub = new RescueEdgeSetStub(model->Context(), archiveFile);
    if (fint != 0)
    {
          fint->edges = edgeStub;
    }
      }
      if (model->Context()->ReadFileVersion() >= 37)
      {
    RESCUECHAR myString[255];

    myfgets(model->Context(), myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0)
    {
          RescueBuffer buf(model->Context(), archiveFile);
          myfgets(model->Context(), myString, 255, archiveFile);
    }
      }
  }
  else
  {
      RESCUEINT64 loopId;
      myfscanf(model->Context(), archiveFile, &loopId);
      if (model->Context()->ReadFileVersion() >= 28)
      {
    if (loopId == 1)
    {
          edgeStub = new RescueEdgeSetStub(model->Context(), archiveFile);
          myfscanf(model->Context(), archiveFile, &loopId);
    }
      }
      if (fint != 0)
      {
    fint->loopId = loopId;
    fint->sectionEdges = edgeStub;
      }
  }
  delete edgeStub;
  }
}

void RescueFaultIntersection::ArchiveWireframeData(FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; FaultIntersection");
  myfprintf(context, archiveFile, Identifier());
  if (context->FileVersion() >= 30)
  {
  if (edges == 0)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else 
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      edges->Archive(context, archiveFile);
  }
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
/*
  We don't have the information we need to know the loopID for previous
  models so we just don't try.
*/
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueFaultIntersection::RelinkWireframeData(RescueObject *model)
{
  if (sectionEdges == 0 && loopId != 0)
  {
  RescueTrimLoop *tLoop = 0;
  RescueEdgeSet *candidateEdges = majorSection->Edges();
  if (candidateEdges != 0)
  {
      tLoop = candidateEdges->TrimLoopIdentifiedBy(loopId);
      if (tLoop != 0)
      {
    sectionEdges = new RescueEdgeSetStub(majorSection->ParentModel()->Context(), candidateEdges);
      }
  }
  if (sectionEdges == 0)
  {
      candidateEdges = minorSection->Edges();
      if (candidateEdges != 0)
      {
    tLoop = candidateEdges->TrimLoopIdentifiedBy(loopId);
    if (tLoop != 0)
    {
          sectionEdges = new RescueEdgeSetStub(minorSection->ParentModel()->Context(), candidateEdges);
    }
      }
  }
  }
/*
  If reading a model from before v28 we have to locate the edge set that the trim loop is in.
*/
  if (sectionEdges != 0 && loopId != 0)
  {
  RescueEdgeSet *actualEdges = sectionEdges->EdgeSet(parentModel, TRUE);
  if (actualEdges != 0)
  {
      RescueTrimLoop *tLoop = actualEdges->TrimLoopIdentifiedBy(loopId);
      if (tLoop != 0)
      {
    AutoEdgeSet();
    RescueEdgeSet *toBuild = edges->EdgeSet(parentModel);
    RescueTrimLoop *buildLoop = new RescueTrimLoop(majorSection, tLoop->LoopClass()->String());
    toBuild->AddBoundaryLoop(buildLoop);
    RESCUEINT64 eOrd = 0;
    RescueTrimEdge *eTrim = tLoop->NthLoopEdge(eOrd++);
    while (eTrim != 0)
    {
          RescueTrimEdge *buildEdge = new RescueTrimEdge(eTrim->PolyLine(parentModel), eTrim->Direction());
          buildLoop->AddLoopEdge(buildEdge);
          eTrim = tLoop->NthLoopEdge(eOrd++);
    }
      }
  }
  }
/*
  If reading a model from before v30 we have to create our own RescueEdgeSet which contains the
  polylines which are part of the identified trim loop.
*/
}

void RescueFaultIntersection::Archive(FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; FaultIntersection");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, majorSection->Identifier());
  myfprintf(context, archiveFile, minorSection->Identifier());
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueFaultIntersection::~RescueFaultIntersection()
{
  if (edges != 0)
  {
  delete edges;
  }
  if (sectionEdges != 0)
  {
  delete sectionEdges;
  }
}

RescueFaultIntersection::RescueFaultIntersection(RescueContext *contextIn, FILE *archiveFile)
                                                 :RescueHistoryObject(contextIn)
                                                 ,parentModel(0)
                                                 ,majorSection(0)
                                                 ,minorSection(0)
                                                 ,edges(0)
                                                 ,loopId(0)
                                                 ,context(contextIn)
                                                 ,sectionEdges(0)
{
  isA = R_RescueFaultIntersection;
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &majorId);
  myfscanf(context, archiveFile, &minorId);
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

void RescueFaultIntersection::Relink(RescueObject *object)
{
  parentModel = (RescueModel *) object;
  majorSection = parentModel->SectionIdentifiedBy(majorId);
  minorSection = parentModel->SectionIdentifiedBy(minorId);
}


RESCUEBOOL RescueFaultIntersection::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueFaultIntersection)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}


