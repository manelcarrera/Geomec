/****************************************************************************

Copyright 1995 Petrotechnical Open Software Corporation

POSC grants permission to copy or reproduce this material in its original
form for internal use only.

This software is subject to the provision of the "POSC Software License
Agreement" which states in part:

1) Licensee accepts a non-exclusive, non-transferable, license  to use,
display, modify and distribute works derived from the licensed documentation
and Software Product.

2) Licensee shall have no right to distribute in its original form any
Software Product or documentation licensed under this agreement.

****************************************************************************/
/*************************************************************************

        cSetRescueWireframe.cpp

 Keeps a list of pointers to some RescueWireframe.

        Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueWireframe.h"
#include "RescueWireframe.h"
#include "cSetRescuePolyLine.h"
#include "cSetRescueEdgeSet.h"
#include "RescueTrimVertexStub.h"
#include "RescueMacroVolume.h"
#include "RescueGeobodyVolume.h"
#include "RescueGeobodySurface.h"
#include "RescueGeobodyPart.h"
#include "RescueGeobody.h"
#include "RescueFaultIntersection.h"
#include <assert.h>

RESCUEBOOL cSetRescueWireframe::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
    myReturn = objects[loop]->WireframeFileTruncated();
  }
  return myReturn;
}

void cSetRescueWireframe::ArchiveCompatiblePolyLines(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 totalCount = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    if (objects[loop]->isDeleted == FALSE)
    {
      totalCount += objects[loop]->polyLines->Count();
    }
  }
  myfprintf(context, archiveFile, totalCount);
  for (loop = 0; loop < count; loop++)
  {
    if (objects[loop]->isDeleted == FALSE)
    {
      RESCUEINT64 howMany = objects[loop]->polyLines->Count64();
      RESCUEINT64 subloop;
      for (subloop = 0; subloop < howMany; subloop++)
      {
        objects[loop]->polyLines->NthObject(subloop)->Archive(archiveFile);
      }
    }
  }
}

void cSetRescueWireframe::ArchiveCompatibleTrimVertexes(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 totalCount = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    if (objects[loop]->isDeleted == FALSE)
    {
      totalCount += objects[loop]->trimVertexes->Count64();
    }
  }
  myfprintf(context, archiveFile, totalCount);
  for (loop = 0; loop < count; loop++)
  {
    if (objects[loop]->isDeleted == FALSE)
    {
      RESCUEINT64 howMany = objects[loop]->trimVertexes->Count64();
      RESCUEINT64 subloop;
      for (subloop = 0; subloop < howMany; subloop++)
      {
        objects[loop]->trimVertexes->NthObject(subloop)->Archive(context, archiveFile);
      }
    }
  }
}

void cSetRescueWireframe::RadiateFrom(RescueTrimVertex *vertex)
{
  if (vertex != 0)
  {
    RESCUEINT64 ordinal = 0;
    RescuePolyLineStub *stub = vertex->NthPolyLine(ordinal++);
    while (stub != 0)
    {
      RescuePolyLine *line = oldPolyLines->ObjectIdentifiedBy(stub->BestIdentifier());
      if (line != 0)
      {
        if (line->Owner() == 0)
        {
          CopyFromVertex(line);
        }
      }
      stub = vertex->NthPolyLine(ordinal++);
    }
  }
}

void cSetRescueWireframe::CopyFromVertex(RescuePolyLine *line)
{
  if (line->Owner() == 0)
  {
    RescueTrimVertexStub *stub = line->LeftVertexObj();
    if (stub != 0)
    {
      RescueTrimVertex *leftVertex = 0;
      if (stub->WireframeId() > 0)
      {
        leftVertex = line->LeftVertex();
      }
      else
      {
        leftVertex = oldTrimVertexes->ObjectIdentifiedBy(stub->BestIdentifier());
      }
      if (leftVertex != 0)
      {
        if (leftVertex->Owner() != 0)
        {
          line->Relink(leftVertex->Owner());
          leftVertex->Owner()->AddPolyLine(line);
          ExtendFromWireframe(line->Owner(), line->RightVertexObj());
        }
      }
    }
/*
  Look left.
*/
    if (line->Owner() == 0)
    {
      stub = line->RightVertexObj();
      if (stub != 0)
      {
        RescueTrimVertex *rightVertex = 0;
        if (stub->WireframeId() > 0)
        {
          rightVertex = line->RightVertex();
        }
        else
        {
          rightVertex = oldTrimVertexes->ObjectIdentifiedBy(stub->BestIdentifier());
        }
        if (rightVertex != 0)
        {
          if (rightVertex->Owner() != 0)
          {
            line->Relink(rightVertex->Owner());
            rightVertex->Owner()->AddPolyLine(line);
            ExtendFromWireframe(line->Owner(), line->LeftVertexObj());
          }
        }
      }
    }
/*
  Look right.
*/
    if (line->Owner() != 0)
    {
      RadiateFrom(line->LeftVertex());
      RadiateFrom(line->RightVertex());
    }
/*
  Since we're only going thru the polyline loop once, we need to extend this as far as it
  will go.
*/
  }
}

void cSetRescueWireframe::ExtendFromWireframe(RescueWireframe *wireframe, RescueTrimVertexStub *stub)
{
  RescueTrimVertex *vertex = oldTrimVertexes->ObjectIdentifiedBy(stub->BestIdentifier());
  if (vertex != 0)
  {
    if (vertex->Owner() == 0)
    {
      vertex->Relink(wireframe);
      wireframe->AddTrimVertex(vertex);
    }
    stub->SetWireframe(vertex->Owner());
  }
}

void cSetRescueWireframe::ExtendFromWireframe(RescueWireframe *wireframe, RescuePolyLine *line)
{
  ExtendFromWireframe(wireframe, line->LeftVertexObj());
  ExtendFromWireframe(wireframe, line->RightVertexObj());

  (*line->LeftVertex()->lines) += new RescuePolyLineStub(wireframe->ParentModel()->Context(), line);
  (*line->RightVertex()->lines) += new RescuePolyLineStub(wireframe->ParentModel()->Context(), line);
}

void cSetRescueWireframe::ExtendFromWireframe(RescueWireframe *wireframe, RescueTrimEdge *edge)
{
  RescuePolyLineStub *stub = edge->PolyLineObj();
  RESCUEINT64 lineId = stub->BestIdentifier();
  RescuePolyLine *line = oldPolyLines->ObjectIdentifiedBy(lineId);
  if (line != 0)
  {
    if (line->Owner() == 0)
    {
      line->Relink(wireframe);
      wireframe->AddPolyLine(line);
      ExtendFromWireframe(wireframe, line);
    }
    stub->SetWireframe(line->Owner());
    RescueContext *context = wireframe->ParentModel()->Context();
    edge->leftVertex = new RescueTrimVertexStub(context, line->LeftVertexObj());
    edge->rightVertex = new RescueTrimVertexStub(context, line->RightVertexObj());
  }
}

void cSetRescueWireframe::ExtendFromWireframe(RescueWireframe *wireframe, RescueTrimLoop *trimLoop)
{
  RESCUEINT64 howMany = trimLoop->CountOfLoopEdge();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    ExtendFromWireframe(wireframe, trimLoop->NthLoopEdge(loop));
  }
}

void cSetRescueWireframe::ExtendFromWireframe(RescueWireframe *wireframe, cSetRescueTrimLoop *trimLoopSet)
{
  RESCUEINT64 howMany = trimLoopSet->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    ExtendFromWireframe(wireframe, trimLoopSet->NthObject(loop));
  }
}

void cSetRescueWireframe::ExtendFromWireframe(RescueWireframe *wireframe, RescueEdgeSet *edgeSet)
{
  ExtendFromWireframe(wireframe, edgeSet->interiorLoops);
  ExtendFromWireframe(wireframe, edgeSet->boundaryLoops);
}

void cSetRescueWireframe::UnArchiveCompatiblePolyLines(RescueContext *context, FILE *archiveFile)
{
  oldPolyLines = new cSetRescuePolyLine();
  oldPolyLines->UnArchive(context, archiveFile);
}

void cSetRescueWireframe::UnArchiveCompatibleTrimVertexes(RescueContext *context, FILE *archiveFile)
{
  oldTrimVertexes = new cSetRescueTrimVertex();
  oldTrimVertexes->UnArchive(context, archiveFile);
}

void cSetRescueWireframe::SaveTriplet(Triplet *toSave)
{
  if (compatibleCount == compatibleAllocated)
  {
    compatibleAllocated += 2000;
    if (compatibleCount == 0)
    {
      savedCompatible = (Triplet **) malloc(sizeof(Triplet *) * (size_t) compatibleAllocated);
    }
    else
    {
      savedCompatible = (Triplet **) realloc(savedCompatible, sizeof(Triplet *) * (size_t) compatibleAllocated);
    }
  }
  savedCompatible[compatibleCount++] = toSave;
}

void cSetRescueWireframe::SaveCompatibleEdgeSet(RescueEdgeSet *edgeSet, RescueObject *user,
                                                RescueWireframeStub *stub)
{
  Triplet *triplet = new Triplet();
  triplet->wireframeObj = edgeSet;
  triplet->userObj = user;
  triplet->stub = stub;
  SaveTriplet(triplet);
/*
  We can't setup the Wireframe owner now because if we are loading a version 9 file we
  are doing this during regular reading, and the relationships between the various
  model elements are not setup yet.

  The user will have a RescueEdgeSetStub object that points to this edge that has an
  incorrect wireframeId and a zero wireframeObj pointer.
*/
}

void cSetRescueWireframe::SaveCompatibleTrimLoop(RescueTrimLoop *trimLoop, RescueObject *user,
                                                 RescueWireframeStub *stub)
{
  Triplet *triplet = new Triplet();
  triplet->wireframeObj = trimLoop;
  triplet->userObj = user;
  triplet->stub = stub;
  SaveTriplet(triplet);
/*
  We can't setup the Wireframe owner now because if we are loading a version 9 file we
  are doing this during regular reading, and the relationships between the various
  model elements are not setup yet.

  The user will have a RescueEdgeSetStub object that points to this edge that has an
  incorrect wireframeId and a zero wireframeObj pointer.
*/
}

void cSetRescueWireframe::FindWireframeForCompatibles()
{
  if (oldPolyLines != 0)
  {
/**************************************************************************************

      RescueTrimVertex has a set of RescuePolyLineNodeStub objects, but this set
      will be empty.  You will need to relink vertexes to poly lines from the poly
      line side and add stubs to the TrimVertex.

      RescuePolyLine will have RescueTrimVertexStub objects, but the wireframeId
      member of these will equal zero.

      You will need to add RescuePolyLineNodes and RescueTrimVertexes which do not
      have an owner pointer to an existing RescueWireframe file.  Look for owner = 0.

      Remember that RescuePolyLineNodes (including 
      the TrimVertexes) should be relinked with RescueModel (though you should not
      pass ParentModel to RescueTrimVertex::Relink or RescuePolyLineNode::Relink).
      This sets up their surfaceUVs etc. which you
      can use to locate an appropriate block.  This will not have been done for
      these objects.

      RescueEdgeSet, RescueTrimLoop, and RescueTrimEdge objects should be relinked
      with the RescueWireframe object that owns them, once it is determined. All this
      does is set the owner.

      RescueTrimEdge contains a RescuePolyLineStub object which will have a zero wireframeId.
      This will need to be set to the real wireframeID.

***********************************************************************************/
    RESCUEINT64 loop;
    for (loop = 0; loop < compatibleCount; loop++)
    {
      switch (savedCompatible[loop]->userObj->IsA())
      {
      case R_RescueMacroVolume:
        {
          RescueMacroVolume *mVolume = (RescueMacroVolume *) savedCompatible[loop]->userObj;
          RescueEdgeSet *edgeSet = (RescueEdgeSet *) savedCompatible[loop]->wireframeObj;
          RescueWireframe *wireframe = mVolume->ParentBlockUnit()->Wireframe();
/*
  Here are the objects involved.
*/
          edgeSet->Relink(wireframe);
          wireframe->AddEdgeSet(edgeSet);
          savedCompatible[loop]->stub->SetWireframe(wireframe);
/*
  Hand the edgeSet to the proper wireframe object.
*/
          edgeSet->SetRescueBusinessOwner(mVolume);
          ExtendFromWireframe(wireframe, edgeSet);
/*
  Try to find lines and vertexes used by this edge set and extend them as well.
  This also fixes up the stubs.
*/
        }
        break;
      case R_RescueGeobodyVolume:
        {
          RescueGeobodyVolume *gVolume = (RescueGeobodyVolume *) savedCompatible[loop]->userObj;
          RescueEdgeSet *edgeSet = (RescueEdgeSet *) savedCompatible[loop]->wireframeObj;
          RescueWireframe *wireframe = gVolume->ParentPart()->Body()->Wireframe();

          edgeSet->Relink(wireframe);
          wireframe->AddEdgeSet(edgeSet);
          savedCompatible[loop]->stub->SetWireframe(wireframe);

          edgeSet->SetRescueBusinessOwner(gVolume);
          ExtendFromWireframe(wireframe, edgeSet);
        }
        break;
      case R_RescueGeobodySurface:
        {
          RescueGeobodySurface *surface = (RescueGeobodySurface *) savedCompatible[loop]->userObj;
          RescueEdgeSet *edgeSet = (RescueEdgeSet *) savedCompatible[loop]->wireframeObj;
          RescueWireframe *wireframe = surface->parentVolume->ParentPart()->Body()->Wireframe();
          edgeSet->Relink(wireframe);
          wireframe->AddEdgeSet(edgeSet);
          savedCompatible[loop]->stub->SetWireframe(wireframe);

          edgeSet->SetRescueBusinessOwner(surface);
          ExtendFromWireframe(wireframe, edgeSet);
        }
        break;
      case R_RescueBlockUnitSide:
        {
          RescueBlockUnitSide *buSide = (RescueBlockUnitSide *) savedCompatible[loop]->userObj;
          RescueEdgeSet *edgeSet = (RescueEdgeSet *) savedCompatible[loop]->wireframeObj;
          RescueWireframe *wireframe = buSide->parentVolume->ParentBlockUnit()->Wireframe();

          edgeSet->Relink(wireframe);
          wireframe->AddEdgeSet(edgeSet);
          savedCompatible[loop]->stub->SetWireframe(wireframe);

          edgeSet->SetRescueBusinessOwner(buSide);
          ExtendFromWireframe(wireframe, edgeSet);
        }
        break;
      case R_RescueBlockUnitHorizonSurface:
        {
          RescueBlockUnitHorizonSurface *buSurf = (RescueBlockUnitHorizonSurface *) savedCompatible[loop]->userObj;
          RescueEdgeSet *edgeSet = (RescueEdgeSet *) savedCompatible[loop]->wireframeObj;
          RescueBlockUnit *bu = buSurf->BlockUnitAboveMe();
          if (bu == 0)
          {
            bu = buSurf->BlockUnitBelowMe();
          }
          RescueWireframe *wireframe;
          if (bu == 0)
          {
            wireframe = buSurf->Wireframe();
          }
          else
          {
            wireframe = bu->Wireframe();
          }
          edgeSet->Relink(wireframe);
          wireframe->AddEdgeSet(edgeSet);
          savedCompatible[loop]->stub->SetWireframe(wireframe);

          edgeSet->SetRescueBusinessOwner(buSurf);
          ExtendFromWireframe(wireframe, edgeSet);
        }
        break;
      case R_RescueSection:
        {
          RescueSection *section = (RescueSection *) savedCompatible[loop]->userObj;
          RescueEdgeSet *edgeSet = (RescueEdgeSet *) savedCompatible[loop]->wireframeObj;
          RescueWireframe *wireframe = section->Wireframe();

          edgeSet->Relink(wireframe);
          wireframe->AddEdgeSet(edgeSet);
          savedCompatible[loop]->stub->SetWireframe(wireframe);

          edgeSet->SetRescueBusinessOwner(section);
          ExtendFromWireframe(wireframe, edgeSet);
        }
        break;
      case R_RescueReferenceSurface:
        {
          RescueReferenceSurface *surface = (RescueReferenceSurface *) savedCompatible[loop]->userObj;
          RescueEdgeSet *edgeSet = (RescueEdgeSet *) savedCompatible[loop]->wireframeObj;
          RescueWireframe *wireframe = surface->Wireframe();

          edgeSet->Relink(wireframe);
          wireframe->AddEdgeSet(edgeSet);
          savedCompatible[loop]->stub->SetWireframe(wireframe);

          edgeSet->SetRescueBusinessOwner(surface);
          ExtendFromWireframe(wireframe, edgeSet);
        }
        break;
      case R_RescueProperty:
        {
          RescueProperty *property = (RescueProperty *) savedCompatible[loop]->userObj;
          RescueTrimLoop *trimLoop = (RescueTrimLoop *) savedCompatible[loop]->wireframeObj;
          RescueWireframe *wireframe = property->ParentSurface()->Wireframe();
/*
  Only properties on surfaces have trim loops.
*/
          trimLoop->Relink(wireframe);
          wireframe->AddTrimLoop(trimLoop);
          savedCompatible[loop]->stub->SetWireframe(wireframe);

          trimLoop->propertyID = property->Identifier();
          ExtendFromWireframe(wireframe, trimLoop);
        }
        break;
      case R_RescueFaultIntersection:
        {
          RescueFaultIntersection *fint = (RescueFaultIntersection *) savedCompatible[loop]->userObj;
          RescueTrimLoop *trimLoop = (RescueTrimLoop *) savedCompatible[loop]->wireframeObj;
          RescueWireframe *wireframe = fint->MajorSection()->Wireframe();

          trimLoop->Relink(wireframe);
          wireframe->AddTrimLoop(trimLoop);
          savedCompatible[loop]->stub->SetWireframe(wireframe);

          ExtendFromWireframe(wireframe, trimLoop);
        }
        break;
      default:
        assert(1==0);
        break;
/*
  Means we don't know what kind of object our user is.  This obviously should never happen.
*/
      }
    }

    for (loop = 0; loop < compatibleCount; loop++)
    {
      delete(savedCompatible[loop]);
    }
    free(savedCompatible); 
/*
  Above, do all RescueEdgeSets and RescueTrimLoops that have relationships elsewhere in the
  model.
*/
    RESCUEINT64 howMany = oldPolyLines->Count64();
    for (loop = 0; loop < howMany; loop++)
    {
      RescuePolyLine *line = oldPolyLines->NthObject(loop);
      if (line != 0)
      {
        CopyFromVertex(line);
      }
    }
/*
  Above, look for poly lines that don't have direct relationships in the model.  If they
  do share an end with a poly line that does, then add them to the same file as that
  poly line.
*/
    RescueWireframe *lastResort = NthObject(0);

    howMany = oldPolyLines->Count64();
    for (loop = 0; loop < howMany; loop++)
    {
      RescuePolyLine *line = oldPolyLines->NthObject(loop);
      if (line->Owner() == 0)
      {
        if (lastResort == 0)
        {
          delete line;
        }
        else
        {
          line->Relink(lastResort);
          lastResort->AddPolyLine(line);
          ExtendFromWireframe(lastResort, line);
        }
      }
    }
    howMany = oldTrimVertexes->Count64();
    for (loop = 0; loop < howMany; loop++)
    {
      RescueTrimVertex *vertex = oldTrimVertexes->NthObject(loop);
      if (vertex->Owner() == 0)
      {
        if (lastResort == 0)
        {
          delete vertex;
        }
        else
        {
          vertex->Relink(lastResort);
          lastResort->AddTrimVertex(vertex);
        }
      }
    }
/*
  At this point we may have RescuePolyLine and RescueTrimVertex objects that have no relationship 
  to the model at all.  These will be trimmed from the model.  
*/
    oldPolyLines->GiveUpObjects();
    oldTrimVertexes->GiveUpObjects();
    delete oldPolyLines;
    delete oldTrimVertexes;
    oldPolyLines = 0;
    oldTrimVertexes = 0;
/*
  Cleanup temporary memory.
*/
  }
}

cSetRescueWireframe::cSetRescueWireframe():oldPolyLines(0),oldTrimVertexes(0)
{
  allocated = 10;
  count = 0;
  objects = (RescueWireframe **) malloc(sizeof(RescueWireframe *) * (size_t) allocated);
  savedCompatible = 0;
  compatibleAllocated = 0;
  compatibleCount = 0;

}

cSetRescueWireframe::~cSetRescueWireframe()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueWireframe::CopyWireframeData(RESCUECHAR *oldPathName)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->CopyWireframeData(oldPathName);
  }
}

void cSetRescueWireframe::DropWireframeMemory()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->DropWireframeMemory();
  }
}

void cSetRescueWireframe::UpdateWireframeData()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->UpdateWireframeData();
  }
}

void cSetRescueWireframe::UnloadWireframe()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->UnloadWireframe();
  }
}

void cSetRescueWireframe::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueWireframe::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Relink(parent);
  }
  FindWireframeForCompatibles();
}

void cSetRescueWireframe::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
    RescueWireframe *newObject = new RescueWireframe(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueWireframe::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueWireframe::operator+=(RescueWireframe *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RescueWireframe **) realloc(objects, sizeof(RescueWireframe *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

void cSetRescueWireframe::Relinquish(RescueWireframe *existingObject)
{
  RESCUEBOOL found = FALSE;
  RESCUEINT64 ndx = 0;

  while (ndx < count && found == FALSE)
  {
    if (existingObject == objects[ndx])
    {
      found = TRUE;
    }
    else
    {
      ndx++;
    }
  }
  if (found)
  {
    RESCUEINT64 loop;

    count--;
    for (loop = ndx; loop < count; loop++)
    {
      objects[loop] = objects[loop + 1];
    }
  }
}

RESCUEBOOL cSetRescueWireframe::operator-=(RescueWireframe *existingObject)
{
  RESCUEBOOL found = FALSE;
  RESCUEINT64 ndx = 0;

  while (ndx < count && found == FALSE)
  {
    if (existingObject == objects[ndx])
    {
      found = TRUE;
    }
    else
    {
      ndx++;
    }
  }
  if (found)
  {
    RESCUEINT64 loop;

    delete objects[ndx];
    count--;
    for (loop = ndx; loop < count; loop++)
    {
      objects[loop] = objects[loop + 1];
    }
  }
  return found;
}

RescueWireframe *cSetRescueWireframe::ObjectNamed(const RESCUECHAR *mayBeName)
{
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE)
  {
    if (objects[ndx]->IsNamed(mayBeName))
    {
      found = TRUE;
    }
    else
    {
      ndx++;
    }
  }
  if (found)
  {
    return objects[ndx];
  }
  else
  {
    return 0;
  }
}

RescueWireframe *cSetRescueWireframe::ObjectIdentifiedBy(RESCUEINT64 identifier)
{
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE)
  {
    if (objects[ndx]->IsIdentifiedBy(identifier))
    {
      found = TRUE;
    }
    else
    {
      ndx++;
    }
  }
  if (found)
  {
    return objects[ndx];
  }
  else
  {
    return 0;
  }
}

RESCUEBOOL cSetRescueWireframe::operator-=(RESCUEINT64 ndx)
{
  if (ndx >= 0 && ndx < count)
  {
    RESCUEINT64 loop;

    delete objects[ndx];
    count--;
    for (loop = ndx; loop < count; loop++)
    {
      objects[loop] = objects[loop + 1];
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

RescueWireframe *cSetRescueWireframe::NthObject(RESCUEINT64 ordinal)
{
  if (ordinal < 0 || ordinal >= count)
  {
    return 0;
  }
  else
  {
    return objects[ordinal];
  }
}

RESCUEINT64 cSetRescueWireframe::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueWireframe::Count(void)
{
  return (RESCUEINT32) count;
}

RESCUEINT32 cSetRescueWireframe::Count(RESCUEBOOL throwIfTrue)
{
  if (count > 2147483647)
  {
    if (throwIfTrue)
    {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  }
  else
  {
    return (RESCUEINT32) count;
  }
}




