/********************************************************************

  RescueBlockUnitSide.cpp

  A set of edges which describe the relationship of a top, bottom, or
  side of a block unit to a set of surfaces.

  Rod Hanks,  July 1996

*********************************************************************/
#include "RescueBlockUnitSide.h"
#include "RescueModel.h"
#include "RescueTrimVertex.h"
#include "RescueIdTree.h"
#include "RescueWireframe.h"
#include "RescueMacroVolume.h"
#include "RescueBuffer.h"
#include <string.h>

void RescueBlockUnitSide::AutoEdgeSet()
{
  RescueEdgeSet *edgeSet = new RescueEdgeSet(Section());
  edgeSet->SetRescueBusinessOwner(this);
  edges = new RescueEdgeSetStub(Section()->ParentModel()->Context(), edgeSet);
}

RescueEdgeSetStub *RescueBlockUnitSide::EdgesObj()
{
  if (Section()->ParentModel()->IsWireframeLoaded() == FALSE)
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

RescueEdgeSet *RescueBlockUnitSide::Edges()
{
  RescueModel *model = Section()->ParentModel();
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

RescueBlockUnitSide::~RescueBlockUnitSide()
{
  if (edges != 0)
  {
    delete edges;
  }
}

RescueBlockUnitSide::RescueBlockUnitSide(RescueSection *existingSection)
                                        :RescueHistoryObject(existingSection->ParentModel()->Context())
                                        ,edges(0)
{
  isA = R_RescueBlockUnitSide;
  section = existingSection;
}

void RescueBlockUnitSide::DropWireframeMemory()
{
  if (edges != 0)
  {
    delete edges;
    edges = 0;
  }
}

void RescueBlockUnitSide::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RescueContext *context = model->Context();
  if (edges != 0)
  {
    delete edges;
    edges = 0;
  }
  if (context->ReadFileVersion() >= 28)
  {
    RESCUEINT64 flag;
    myfscanf(context, archiveFile, &flag);
    if (flag > 0)
    {
      edges = new RescueEdgeSetStub(context, archiveFile);
    }
  }
  else
  {
    RescueEdgeSet *edgesObj = new RescueEdgeSet(context, archiveFile);
    edges = new RescueEdgeSetStub(context, edgesObj);
    model->wireframes->SaveCompatibleEdgeSet(edgesObj, this, edges);
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

void RescueBlockUnitSide::RelinkWireframeData(RescueObject *parent)
{
}

void RescueBlockUnitSide::ArchiveWireframeData(FILE *archiveFile)
{
  RescueContext *context = Section()->ParentModel()->Context();
  if (context->FileVersion() >= 28)
  {
    if (edges == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      edges->ArchiveStub(context, archiveFile);
    }
  }
  else
  {
    RescueEdgeSet *edgesObj = 0;
    if (edges != 0)
    {
      edgesObj = edges->EdgeSet(Section()->ParentModel());
    }
    if (edgesObj == 0)
    {
      edgesObj = new RescueEdgeSet(parentVolume->ParentBlockUnit());
      if (edges != 0)
      {
        delete edges;
      }
      edges = new RescueEdgeSetStub(context, edgesObj);
    }
    edgesObj->Archive(archiveFile);
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}


void RescueBlockUnitSide::Archive(FILE *archiveFile)
{
  RescueContext *context = Section()->ParentModel()->Context();
  myfprintf(context, archiveFile, "; block unit side");
  if (context->FileVersion() >= 20)
  {
    myfprintf(context, archiveFile, Identifier());
  }
  if (context->FileVersion() == 9)
  {
    RescueEdgeSet *edgesObj = 0;
    if (edges != 0)
    {
       edgesObj = edges->EdgeSet(Section()->ParentModel());
    }
    if (edgesObj == 0)
    {
      edgesObj = new RescueEdgeSet(parentVolume->ParentBlockUnit());
      if (edges != 0)
      {
        delete edges;
      }
      edges = new RescueEdgeSetStub(context, edgesObj);
    }
    edgesObj->Archive(archiveFile);
  }
  myfprintf(context, archiveFile, section->Identifier());
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueBlockUnitSide::RescueBlockUnitSide(RescueContext *context, FILE *archiveFile)
            :RescueHistoryObject(context),edges(0)
{
  isA = R_RescueBlockUnitSide;
  if (context->ReadFileVersion() >= 20)
  {
    ReadId(context, archiveFile);
  }
  if (context->ReadFileVersion() <= 12)
  {
    RescueEdgeSet *edgesObj = new RescueEdgeSet(context, archiveFile);
    edges = new RescueEdgeSetStub(context, edgesObj);
    context->unarchivingModel->wireframes->SaveCompatibleEdgeSet(edgesObj, this, edges);
  }
  else
  {
    edges = 0;
  }
  RESCUEINT64 count;
  RESCUEINT64 loop;

  if (context->ReadFileVersion() >= 5)
  {
    count = 1;
  }
  else
  {
    myfscanf(context, archiveFile, &count);
  }
  for (loop = 0; loop < count; loop++)
  {
    myfscanf(context, archiveFile, &sectionID);
  }
  context->blockUnitSides->Add(this);
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

void RescueBlockUnitSide::Relink(RescueObject *object)
{
  parentVolume = (RescueMacroVolume *) object;
  section = parentVolume->ParentModel()->SectionIdentifiedBy(sectionID);
}

RESCUEBOOL RescueBlockUnitSide::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueBlockUnitSide)
  {
    return TRUE;
  }
  else
  {
    return RescueHistoryObject::IsOfType(thisType);
  }
}



