/********************************************************************

  RescueBlockUnitHorizonSurface.cpp

  Holds the horizon which bounds a particular block unit
  for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueBlockUnitHorizonSurface.h"
#include "RescueHorizon.h"
#include "RescueModel.h"
#include "RescueCoordinateSystem.h"
#include "RescueMacroVolume.h"
#include "RescueBuffer.h"
#include <string.h>

RescueBlockUnitHorizonSurface::RescueBlockUnitHorizonSurface
                            (RescueCoordinateSystem::Orientation orientation,
                                RescueHorizon *parentHorizonIn,
                                RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                                RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                                RESCUEFLOAT missingValue,
                                RescueSurface::SurfaceType typeIn)
                                :RescueReferenceSurface(parentHorizonIn->ParentModel(),
                                                orientation,
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(0),
                                                i_origin, i_step,
                                                i_lowbound, i_count,
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(1),
                                                j_origin, j_step,
                                                j_lowbound, j_count,
                                                missingValue, typeIn)
           ,parentHorizon(parentHorizonIn)
                                ,unitAboveMe(0)
                                ,unitBelowMe(0)
{
  (*parentHorizon->surfaces) += this;
  isA = R_RescueBlockUnitHorizonSurface;
}

RescueBlockUnitHorizonSurface::RescueBlockUnitHorizonSurface
                               (RescueCoordinateSystem::Orientation orientation,
                                RescueHorizon *parentHorizonIn,
                                RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                                RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                                RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                                RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                                RESCUEFLOAT missingValue,
                                RESCUEFLOAT *valueTriplets,
                                RescueSurface::SurfaceType typeIn)
                                :RescueReferenceSurface(parentHorizonIn->ParentModel(),
                                                orientation,
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(0),
                                                i_origin, i_step,
                                                i_lowbound, i_count,
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(1),
                                                j_origin, j_step,
                                                j_lowbound, j_count,
                                                missingValue, 
                                                valueTriplets, typeIn)
                                ,parentHorizon(parentHorizonIn)
                                ,unitAboveMe(0)
                                ,unitBelowMe(0)
{
  (*parentHorizon->surfaces) += this;
  isA = R_RescueBlockUnitHorizonSurface;
}

RescueBlockUnitHorizonSurface::RescueBlockUnitHorizonSurface
                            (RescueCoordinateSystem::Orientation orientation,
                                RescueHorizon *parentHorizonIn,
                                RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                                RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                                RESCUEFLOAT missingValue,
                                RescueSurface::SurfaceType typeIn)
                                :RescueReferenceSurface(parentHorizonIn->ParentModel(),
                                                orientation, 
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(0),
                                                0.0, 0.0, 
                                                i_lowbound, i_count,
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(1),
                                                0.0, 0.0,
                                                j_lowbound, j_count,
                                                missingValue, typeIn)

                                ,parentHorizon(parentHorizonIn)
                                ,unitAboveMe(0)
                                ,unitBelowMe(0)
{
  (*parentHorizon->surfaces) += this;
  isA = R_RescueBlockUnitHorizonSurface;
}

RescueBlockUnitHorizonSurface::RescueBlockUnitHorizonSurface
                            (RescueCoordinateSystem::Orientation orientation,
                                RescueHorizon *parentHorizonIn,
                                RESCUEFLOAT missingValue,
                                RescueSurface::SurfaceType typeIn)
                                :RescueReferenceSurface(parentHorizonIn->ParentModel(),
                                                orientation, 
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(0),
                                                0.0, 0.0, 
                                                0, 0,
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(1),
                                                0.0, 0.0,
                                                0, 0,
                                                missingValue, typeIn)
           ,parentHorizon(parentHorizonIn)
                                ,unitAboveMe(0)
                                ,unitBelowMe(0)
{
  (*parentHorizon->surfaces) += this;
  isA = R_RescueBlockUnitHorizonSurface;
}

RescueBlockUnitHorizonSurface::RescueBlockUnitHorizonSurface
                               (RescueCoordinateSystem::Orientation orientation,
                                RescueHorizon *parentHorizonIn,
                                RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                                RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                                RESCUEFLOAT missingValue,
                                RESCUEFLOAT *valueTriplets,
                                RescueSurface::SurfaceType typeIn)
                                :RescueReferenceSurface(parentHorizonIn->ParentModel(),
                                                orientation,
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(0),
                                                0.0, 0.0, 
                                                i_lowbound, i_count,
                                                parentHorizonIn->ParentModel()->CoordinateSystem()->Axis(1),
                                                0.0, 0.0,
                                                j_lowbound, j_count,
                                                missingValue, 
                                                valueTriplets, typeIn)
    ,parentHorizon(parentHorizonIn)
                                ,unitAboveMe(0)
                                ,unitBelowMe(0)
{
  (*parentHorizon->surfaces) += this;
  isA = R_RescueBlockUnitHorizonSurface;
}

RescueBlockUnitHorizonSurface::RescueBlockUnitHorizonSurface
                                      (RescueContext *context, FILE *archiveFile)
                                      :RescueReferenceSurface(context, archiveFile, 0)
                                      ,parentHorizon(0)
                                      ,unitAboveMe(0)
                                      ,unitBelowMe(0)
                                      ,edges(0)
{
  if (context->ReadFileVersion() < 6)
  {
    edges = new RescueEdgeSet(context, archiveFile);
  }
/*
  If file version is September 1997 ILAB or prior, then read edges here.
  Will be passed to RescueMacroVolume at relink time.
*/
  isA = R_RescueBlockUnitHorizonSurface;
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &unitAboveID);
  myfscanf(context, archiveFile, &unitBelowID);
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


RescueBlockUnitHorizonSurface::~RescueBlockUnitHorizonSurface()
{
   delete edges;
}

void RescueBlockUnitHorizonSurface::Relink(RescueObject *parentObject)
{
  parentHorizon = (RescueHorizon *) parentObject;
  unitAboveMe = parentHorizon->ParentModel()->BlockUnitIdentifiedBy(unitAboveID);
  unitBelowMe = parentHorizon->ParentModel()->BlockUnitIdentifiedBy(unitBelowID);
  RescueSurface::Relink(parentHorizon->ParentModel());
  if (edges != 0)
  {
    RescueMacroVolume *macroVolume = 0;
    if (unitAboveMe != 0)
    {
      macroVolume = unitAboveMe->NthMacroVolume(0);
    }
    else if (unitBelowMe != 0)
    {
      macroVolume = unitBelowMe->NthMacroVolume(0);
    }
    if (macroVolume != 0)
    {
      RescueWireframe *wireframe = macroVolume->ParentBlockUnit()->Wireframe();
/*
  Fortunately blocks are relinked before horizons, so the backward pointer in RescueMacroVolume
  should be there.
*/
      wireframe->AddEdgeSet(edges);
      edges->Relink(wireframe);

      if (unitAboveMe != 0)
      {
        unitAboveMe->NthMacroVolume(0)->SetTopEdge(edges);
      }
      if (unitBelowMe != 0)
      {
        unitBelowMe->NthMacroVolume(0)->SetBottomEdge(edges);
      }
    }
    delete edges;
  }
}

void RescueBlockUnitHorizonSurface::Archive(FILE *archiveFile)
{
  RescueContext *context = ParentModel()->Context();
  myfprintf(context, archiveFile, "; Block Unit Horizon Surface");
  RescueSurface::Archive(archiveFile);
  myfprintf(context, archiveFile, Identifier());
  if (unitAboveMe == 0)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
    myfprintf(context, archiveFile, unitAboveMe->Identifier());
  }
  if (unitBelowMe == 0)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
    myfprintf(context, archiveFile, unitBelowMe->Identifier());
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEBOOL RescueBlockUnitHorizonSurface::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueBlockUnitHorizonSurface)
  {
    return TRUE;
  }
  else
  {
    return RescueReferenceSurface::IsOfType(thisType);
  }
}



