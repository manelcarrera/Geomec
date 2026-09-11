/********************************************************************

  RescueWellbore.cpp

  The wellbore for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueWellbore.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

void RescueWellbore::Dispose()
{
  wellboreGeometry->RescueDeleteFile();
  if (sampling != 0)
  {
  sampling->Dispose();
  }
}

RescueWellbore::~RescueWellbore()
{
  if (wellboreName != 0)
  {
  delete wellboreName;
  }
  if (sampling != 0)
  {
  delete sampling;
  }
  if (surfaceIntersections != 0)
  {
  delete surfaceIntersections;
  }
  if (cellIntersections != 0)
  {
  delete cellIntersections;
  }
  if (wellboreGrid != 0)
  {
  delete wellboreGrid;
  }
  if (wellboreGeometry != 0)
  {
  delete wellboreGeometry;
  }
}

void RescueWellbore::SetWellboreName(RESCUECHAR *newName)
{
  (*wellboreName) = newName;
}

RescueWellbore::RescueWellbore(RescueCoordinateSystem::Orientation orientation,
                               RescueModel *parentModelIn,
                               const RESCUECHAR *wellboreNameIn,
                               RESCUEFLOAT missingValue,
                               RESCUEINT64 i_lowbound, RESCUEINT64 i_count)                                               
                               :RescueHistoryObject(parentModelIn->Context())
                               ,wellboreName(0)
                               ,parentModel(parentModelIn)
                               ,sampling(0)
                               ,surfaceIntersections(0)
                               ,cellIntersections(0)
                               ,wellboreGrid(0)
{
  isA = R_RescueWellbore;
  wellboreName = new RCHString(wellboreNameIn);
  wellboreGrid = new RescueGrid(parentModelIn->Context(), orientation, i_lowbound, i_count);
  sampling = new cSetRescueWellboreSampling();
  surfaceIntersections = new cSetRescueWellboreSurface();
  cellIntersections = new cBagRescueWellboreCell();
  (*parentModel->wellbores) += this;

  wellboreGeometry = new RescueQuadrupletArray(parentModelIn->Context(),
                      WellboreGrid(), missingValue, parentModel);
}

RescueWellbore::RescueWellbore(RescueCoordinateSystem::Orientation orientation,
                               RescueModel *parentModelIn,
                               const RESCUECHAR *wellboreNameIn,
                               RESCUEFLOAT missingValue,
                               RESCUEFLOAT *quadrupletArray,
                               RESCUEINT64 i_lowbound, RESCUEINT64 i_count)                                               
                               :RescueHistoryObject(parentModelIn->Context())
                               ,wellboreName(0)
                               ,parentModel(parentModelIn)
                               ,sampling(0)
                               ,surfaceIntersections(0)
                               ,cellIntersections(0)
                               ,wellboreGrid(0)
{
  isA = R_RescueWellbore;
  wellboreName = new RCHString(wellboreNameIn);
  wellboreGrid = new RescueGrid(parentModelIn->Context(), orientation, i_lowbound, i_count);
  sampling = new cSetRescueWellboreSampling();
  surfaceIntersections = new cSetRescueWellboreSurface();
  cellIntersections = new cBagRescueWellboreCell();
  (*parentModel->wellbores) += this;

  wellboreGeometry = new RescueQuadrupletArray(WellboreGrid(), missingValue, quadrupletArray, 
                            parentModel);
}

RescueWellbore::RescueWellbore(RescueContext *context, FILE *archiveFile)
                              :RescueHistoryObject(context)
                              ,wellboreName(0)
                              ,parentModel(0)
         ,sampling(0)
         ,surfaceIntersections(0)
         ,cellIntersections(0)
                              ,wellboreGrid(0)
{
  RESCUECHAR myString[255];
  wellboreGrid = new RescueGrid(context, archiveFile);
  isA = R_RescueWellbore;
  ReadId(context, archiveFile);
  wellboreGeometry = new RescueQuadrupletArray(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  wellboreName = new RCHString(myString);
  sampling = new cSetRescueWellboreSampling();
  surfaceIntersections = new cSetRescueWellboreSurface();
  cellIntersections = new cBagRescueWellboreCell();
  context->RescueProgress(myString);
  sampling->UnArchive(context, archiveFile);
  surfaceIntersections->UnArchive(context, archiveFile);
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

void RescueWellbore::Relink(RescueObject *parentObject)
{
  parentModel = (RescueModel *) parentObject;
  wellboreGrid->Relink(parentModel);
  wellboreGeometry->Relink(parentModel);
  wellboreGeometry->SetGrid(wellboreGrid);
  sampling->Relink(this);
  surfaceIntersections->Relink(this);
}

void RescueWellbore::Archive(FILE *archiveFile)
{
  RescueContext *context = ParentModel()->Context();
  context->RescueProgress(wellboreName->String());
  myfprintf(context, archiveFile, "; Wellbore");
  wellboreGrid->Archive(context, archiveFile);
  myfprintf(context, archiveFile, Identifier());
  wellboreGeometry->Archive(archiveFile);
  myfprintf(context, archiveFile, wellboreName->String());
  sampling->Archive(context, archiveFile);
  surfaceIntersections->Archive(context, archiveFile);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueWellbore::SetOrientation(RescueOrientationLedger *ledger,
                  RescueCoordinateSystem::Orientation orientation)
{
  RescueCoordinateSystem::Orientation oNow = wellboreGrid->DisplayOrientation();
  if (oNow == RescueCoordinateSystem::LUF
  ||  oNow == RescueCoordinateSystem::LUB
  ||  oNow == RescueCoordinateSystem::RUF
  ||  oNow == RescueCoordinateSystem::RUB)
  {
  if (orientation == RescueCoordinateSystem::LDF
  ||  orientation == RescueCoordinateSystem::LDB
  ||  orientation == RescueCoordinateSystem::RDF
  ||  orientation == RescueCoordinateSystem::RDB)
  {
      SwapAxis();
  }
  }
  else if (orientation == RescueCoordinateSystem::LDF
       ||  orientation == RescueCoordinateSystem::LDB
       ||  orientation == RescueCoordinateSystem::RDF
       ||  orientation == RescueCoordinateSystem::RDB)
  {
  SwapAxis();
  }
  wellboreGrid->SetOrientation(orientation);
}

void RescueWellbore::SwapAxis()
{
  if (wellboreGeometry->IsLoaded() == FALSE)
  {
  wellboreGeometry->Load();
  }
  wellboreGrid->Axis(0)->Swap();
  wellboreGeometry->SwapArraysEndForEnd();
  RESCUEINT64 sOrd = 0;
  RescueWellboreSampling *sampling = NthRescueWellboreSampling(sOrd++);
  while (sampling != 0)
  {
  sampling->Swap();
  sampling = NthRescueWellboreSampling(sOrd++);
  }

  wellboreGeometry->MarkChanged();
  wellboreGeometry->Unload();
}

RESCUEBOOL RescueWellbore::AnyFileTruncated()
{
  RESCUEBOOL myReturn = wellboreGeometry->FileTruncated();
  if (myReturn == FALSE)
  {
  myReturn = sampling->AnyFileTruncated();
  }
  return myReturn;
}

RESCUEBOOL RescueWellbore::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueWellbore)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}


