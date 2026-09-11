/********************************************************************

  RescueIJSurface.cpp

  A surface in RESCUE terms is a parametric 2d grid (i,j), with the
  geometry described by an (x,y,z) triplet for each node of the grid.
  The coordinate system of the triplet is the global coordinate system 
  of the model.

  Rod Hanks,  May 1996 / January 2000

*********************************************************************/
#include "RescueIJSurface.h"
#include "RescueSurface.h"
#include "RescueSurfaceCell.h"
#include "RescueSurfaceToGrid.h"
#include "RescueGridAxis.h"
#include "RescueModel.h"
#include "RescueEdgeSet.h"
#include "RescueColor.h"
#include "RescueVertex.h"
#include "RescueTriangulatedSurface.h"
#include "RescueBuffer.h"
#include "RescueBitArray.h"
#include "RescueWellboreSurface.h"
#include "RescueOrientationLedger.h"
#include <string.h>

RescueIJSurface::~RescueIJSurface()
{
  if (surfaceGeometry != 0)
  {
  delete surfaceGeometry;
  }
  if (surfaceGrid != 0)
  {
  delete surfaceGrid;
  }
  if (surfaceCell != 0)
  {
  delete surfaceCell;
  }
  if (surfaceToGrid != 0)
  {
  delete surfaceToGrid;
  }
  if (stairSteps != 0)
  {
  delete stairSteps;
  }
  if (surfaceWellbore != 0)
  {
  delete surfaceWellbore;
  }
  if (faceStatus != 0)
  {
  delete faceStatus;
  }
}

RescueIJSurface::RescueIJSurface(RescueSurface *surfaceIn,
                             RescueCoordinateSystem::Orientation orientation,
                             RescueCoordinateSystemAxis *i_axis,
                             RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RescueCoordinateSystemAxis *j_axis,
                             RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue)
                          :RescueHistoryObject(surfaceIn->ParentModel()->Context())
                          ,parentSurface(surfaceIn)
                          ,surfaceGrid(0)
                          ,surfaceCell(0)
                          ,surfaceToGrid(0)
                          ,stairSteps(0)
                          ,surfaceWellbore(0)
                          ,faceStatus(0)
{
  surfaceCell = new cBagRescueSurfaceCell();
  surfaceToGrid = new cBagRescueSurfaceToGrid();
  stairSteps = new cBagRescueStairSteppedFault();
  surfaceWellbore = new cBagRescueWellboreSurface();
  surfaceGrid = new RescueGrid(surfaceIn->ParentModel()->Context(),
                             orientation,
                             i_axis, i_origin, i_step, i_lowbound, i_count, 
                             j_axis, j_origin, j_step, j_lowbound, j_count);
  isA = R_RescueIJSurface;
  surfaceGeometry = new RescueTripletArray(surfaceIn->ParentModel()->Context(),
                                           surfaceGrid, missingValue, parentSurface->ParentModel());
  (*parentSurface->ijSurfaces) += this;
}

RescueIJSurface::RescueIJSurface(RescueSurface *surfaceIn,
                             RescueCoordinateSystem::Orientation orientation,
                             RescueCoordinateSystemAxis *i_axis,
                             RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RescueCoordinateSystemAxis *j_axis,
                             RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue, 
                             RESCUEFLOAT *values)
                          :RescueHistoryObject(surfaceIn->ParentModel()->Context())
                          ,parentSurface(surfaceIn)
                          ,surfaceGrid(0)
                          ,surfaceCell(0)
                          ,surfaceToGrid(0)
                          ,stairSteps(0)
                          ,surfaceWellbore(0)
                          ,faceStatus(0)
{
  surfaceGrid = new RescueGrid(surfaceIn->ParentModel()->Context(), orientation,
                             i_axis, i_origin, i_step, i_lowbound, i_count, 
                             j_axis, j_origin, j_step, j_lowbound, j_count);
  surfaceCell = new cBagRescueSurfaceCell();
  surfaceToGrid = new cBagRescueSurfaceToGrid();
  stairSteps = new cBagRescueStairSteppedFault();
  surfaceWellbore = new cBagRescueWellboreSurface();
  isA = R_RescueIJSurface;
  surfaceGeometry = new RescueTripletArray(surfaceIn->ParentModel()->Context(), 
                                           surfaceGrid, missingValue, parentSurface->ParentModel());
  surfaceGeometry->SetZValue(values);
  (*parentSurface->ijSurfaces) += this;
}

RescueIJSurface::RescueIJSurface(RescueSurface *surfaceIn,
                             RescueCoordinateSystem::Orientation orientation,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue)
                          :RescueHistoryObject(surfaceIn->ParentModel()->Context())
                          ,parentSurface(surfaceIn)
                          ,surfaceGrid(0)
                          ,surfaceCell(0)
                          ,surfaceToGrid(0)
                          ,stairSteps(0)
                          ,surfaceWellbore(0)
                          ,faceStatus(0)
{
  surfaceGrid = new RescueGrid(surfaceIn->ParentModel()->Context(),
                             orientation,
                             i_lowbound, i_count, 
                             j_lowbound, j_count);
  surfaceCell = new cBagRescueSurfaceCell();
  surfaceToGrid = new cBagRescueSurfaceToGrid();
  stairSteps = new cBagRescueStairSteppedFault();
  surfaceWellbore = new cBagRescueWellboreSurface();
  isA = R_RescueIJSurface;
  surfaceGeometry = new RescueTripletArray(surfaceIn->ParentModel()->Context(),
                                           surfaceGrid, missingValue, parentSurface->ParentModel());
  (*parentSurface->ijSurfaces) += this;
}

RescueIJSurface::RescueIJSurface(RescueSurface *surfaceIn,
                             RescueCoordinateSystem::Orientation orientation,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue, 
                             RESCUEFLOAT *tripletArray)
                          :RescueHistoryObject(surfaceIn->ParentModel()->Context())
                          ,parentSurface(surfaceIn)
                          ,surfaceGrid(0)
                          ,surfaceCell(0)
                          ,surfaceToGrid(0)
                          ,stairSteps(0)
                          ,surfaceWellbore(0)
                          ,faceStatus(0)
{
  surfaceCell = new cBagRescueSurfaceCell();
  surfaceToGrid = new cBagRescueSurfaceToGrid();
  stairSteps = new cBagRescueStairSteppedFault();
  surfaceWellbore = new cBagRescueWellboreSurface();
  surfaceGrid = new RescueGrid(surfaceIn->ParentModel()->Context(),
                             orientation,
                             i_lowbound, i_count, 
                             j_lowbound, j_count);
  isA = R_RescueIJSurface;
  surfaceGeometry = new RescueTripletArray(surfaceIn->ParentModel()->Context(),
                                           surfaceGrid, missingValue, parentSurface->ParentModel());
  if (tripletArray != 0)
  {
  surfaceGeometry->SetValue(missingValue, tripletArray);
  }
  (*parentSurface->ijSurfaces) += this;
}

RescueIJSurface::RescueIJSurface(RescueContext *context, FILE *archiveFile)
                          :RescueHistoryObject(context)
                          ,parentSurface(0)
                          ,surfaceGrid(0)
                          ,surfaceGeometry(0)
                          ,surfaceCell(0)
                          ,surfaceToGrid(0)
                          ,stairSteps(0)
                          ,surfaceWellbore(0)
                          ,faceStatus(0)
{
  isA = R_RescueIJSurface;
  if (context->ReadFileVersion() >= 23)
  {
  ReadId(context, archiveFile);
  }
  surfaceCell = new cBagRescueSurfaceCell();
  surfaceToGrid = new cBagRescueSurfaceToGrid();
  stairSteps = new cBagRescueStairSteppedFault();
  surfaceWellbore = new cBagRescueWellboreSurface();
  surfaceGrid = new RescueGrid(context, archiveFile);
  surfaceGeometry = new RescueTripletArray(context, archiveFile);
  surfaceGeometry->SetGrid(surfaceGrid);
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      if (strcmp(myString, "faceStatus") == 0)
      {
    RescueBuffer buf(context, archiveFile);
    if (faceStatus != 0)
    {
          delete faceStatus;
    }
    faceStatus = new RescueBitArray(context, buf);
      }
      else
      {
    RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
  }
  }
}

void RescueIJSurface::Relink(RescueObject *surfaceIn)
{
  parentSurface = (RescueSurface *) surfaceIn;
  RescueObject *parentModel = parentSurface->ParentModel();
  surfaceGrid->Relink(parentModel);
  surfaceGeometry->Relink(parentModel);
}

void RescueIJSurface::Archive(FILE *archiveFile)
{
  RescueContext *context = parentSurface->ParentModel()->Context();
  myfprintf(context, archiveFile, "; Surface");
  myfprintf(context, archiveFile, Identifier());
  surfaceGrid->Archive(context, archiveFile);
  surfaceGeometry->Archive(archiveFile);
  if (context->FileVersion() >= 37)
  {
  if (faceStatus != 0)
  {
      myfprintf(context, archiveFile, "faceStatus");
      RescueBuffer buf1(context, 100);
      faceStatus->Archive(buf1);
      buf1.Archive(archiveFile);
  }
  myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEFLOAT RescueIJSurface::ZValueAt(RESCUEINT64 i, RESCUEINT64 j)
{
  RescueGridAxis *iAxis = Grid()->Axis(0);
  RescueGridAxis *jAxis = Grid()->Axis(1);
  RESCUEINT64 iCount = iAxis->Count64();
  RESCUEFLOAT *zValues = surfaceGeometry->RescueGetZValue();
  if (zValues == 0)
  {
  return 0;
  }
  else
  {
  return zValues[(j - jAxis->LowBound64()) * iCount + (i - iAxis->LowBound64())];
  }
}

RESCUEBOOL RescueIJSurface::AnyFileTruncated()
{
  RESCUEBOOL myReturn = surfaceGeometry->FileTruncated();
  return myReturn;
}

RESCUEBOOL RescueIJSurface::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueIJSurface)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}

void RescueIJSurface::SetFaceStatus(RESCUEINT64 i, RESCUEINT64 j, RescueFaceStatus newStatus)
{
  if (faceStatus == 0)
  {
  faceStatus = new RescueBitArray(parentSurface->ParentModel()->Context(), surfaceGrid->NodeCount64(), 2);
  }
 RESCUEINT64 jNodes = surfaceGrid->Axis(1)->Count64();
  RESCUEINT64 ndx = (i * jNodes + j);
  faceStatus->SetBitField(ndx, newStatus);
}

RescueIJSurface::RescueFaceStatus RescueIJSurface::FaceStatus(RESCUEINT64 i, RESCUEINT64 j)
{
  if (faceStatus == 0)
  {
  return R_FACE_ACTIVE;
/*
  Default value is R_FACE_ACTIVE, and none have been explicitly set yet, so . . .
*/
  }
  else
  {
   RESCUEINT64 jNodes = surfaceGrid->Axis(1)->Count64();
  RESCUEINT64 ndx = (i * jNodes + j);
  return (RescueIJSurface::RescueFaceStatus) faceStatus->BitField(ndx);
  }
}

void RescueIJSurface::SetOrientation(RescueOrientationLedger *ledger,
                                     RescueCoordinateSystem::Orientation orientation)
{
  RescueCoordinateSystem::Orientation oNow = surfaceGrid->DisplayOrientation();
  bool swapI = false;
  bool swapJ = false;
  if (oNow == RescueCoordinateSystem::LUF
  ||  oNow == RescueCoordinateSystem::LUB
  ||  oNow == RescueCoordinateSystem::LDF
  ||  oNow == RescueCoordinateSystem::LDB)
  {
  if (orientation == RescueCoordinateSystem::RUF
  ||  orientation == RescueCoordinateSystem::RUB
  ||  orientation == RescueCoordinateSystem::RDF
  ||  orientation == RescueCoordinateSystem::RDB)
  {
      swapI = true;
  }
  }
  else if (orientation == RescueCoordinateSystem::LUF
       ||  orientation == RescueCoordinateSystem::LUB
       ||  orientation == RescueCoordinateSystem::LDF
       ||  orientation == RescueCoordinateSystem::LDB)
  {
  swapI = true;
  }
  if (oNow == RescueCoordinateSystem::LUF
  ||  oNow == RescueCoordinateSystem::LDF
  ||  oNow == RescueCoordinateSystem::RUF
  ||  oNow == RescueCoordinateSystem::RDF)
  {
  if (orientation == RescueCoordinateSystem::LUB
  ||  orientation == RescueCoordinateSystem::LDB
  ||  orientation == RescueCoordinateSystem::RUB
  ||  orientation == RescueCoordinateSystem::RDB)
  {
      swapJ = true;
  }
  }
  else if (orientation == RescueCoordinateSystem::LUF
       ||  orientation == RescueCoordinateSystem::LDF
       ||  orientation == RescueCoordinateSystem::RUF
       ||  orientation == RescueCoordinateSystem::RDF)
  {
  swapJ = true;
  }
  if (swapI == true || swapJ == true)
  {
  SwapAxes(ledger, swapI, swapJ);
  }
  surfaceGrid->SetOrientation(orientation);
}

void RescueIJSurface::SwapAxes(RescueOrientationLedger *ledger, bool swapI, bool swapJ)
{
  RescueGridAxis *iAxis = surfaceGrid->Axis(0);
  RescueGridAxis *jAxis = surfaceGrid->Axis(1);
  ledger->SetIJFor(this, iAxis->LowBound64(), iAxis->Count64(), jAxis->LowBound64(), jAxis->Count64(), swapI, swapJ);
  if (swapI)
  {
  iAxis->Swap();
  }
  if (swapJ)
  {
  jAxis->Swap();
  }
  if (surfaceGeometry->IsLoaded() == FALSE)
  {
  surfaceGeometry->Load();
  }
  surfaceGeometry->SwapAxes(swapI, swapJ);
  surfaceGeometry->MarkChanged();
  surfaceGeometry->Unload();
  if (surfaceCell != 0)
  {
  int ordinal = 0;
  RescueSurfaceCell *sc = surfaceCell->NthObject(ordinal++);
  while (sc != 0)
  {
      sc->Swap(swapI, iAxis, swapJ, jAxis);
      sc = surfaceCell->NthObject(ordinal++);
  }
  }
  if (surfaceWellbore != 0)
  {
  RESCUEINT64 ordinal = 0;
  RescueWellboreSurface *sw = surfaceWellbore->NthObject(ordinal++);
  while (sw != 0)
  {
      sw->Swap(swapI, iAxis, swapJ, jAxis);
      sw = surfaceWellbore->NthObject(ordinal++);
  }
  }
  if (faceStatus != 0)
  {
  faceStatus->SwapAxes(swapI, iAxis->Count64(), swapJ, jAxis->Count64());
  }
  RescueSurface *parentSurface = ParentSurface();
  if (parentSurface != 0)
  {
  RESCUEINT64 ordinal = 0;
  RescueProperty *property = parentSurface->properties->NthObject(ordinal++);
  while (property != 0)
  {
      if (property->ParentIJSurface() == this)
      {
    property->SwapAxes(swapI, iAxis->Count64(), swapJ, jAxis->Count64());
      }
      property = parentSurface->properties->NthObject(ordinal++);
  }
  }
}


