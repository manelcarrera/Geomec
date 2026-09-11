/********************************************************************
                                   
  RescueGrid.cpp

  Grid for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueGrid.h"
#include "RescueModel.h"
#include "RescueVertex.h"
#include "RescueIdTree.h"
#include "RescueBuffer.h"
#include <string.h>

RescueGrid::~RescueGrid()
{
  if (axes != 0)
  {
  delete axes;
  }
}

RescueGrid::RescueGrid(RescueContext *context,
                       RescueCoordinateSystem::Orientation displayOrientationIn,
                  RESCUEINT64 i_lowbound, RESCUEINT64 i_count)
                  :RescueHistoryObject(context)
                       ,displayOrientation(displayOrientationIn)
                       ,axes(0)
{
   axes = new cSetRescueGridAxis();
   isA = R_RescueGrid;
   (*axes) += new RescueGridAxis(context, this, i_lowbound, i_count);
}

RescueGrid::RescueGrid(RescueContext *context,
                       RescueCoordinateSystem::Orientation displayOrientationIn, 
             RESCUEFLOAT i_origin, RESCUEFLOAT i_step, RESCUEINT64 i_lowbound, RESCUEINT64 i_count)
    :RescueHistoryObject(context)
             ,displayOrientation(displayOrientationIn)
             ,axes(0)
{
   axes = new cSetRescueGridAxis();
   isA = R_RescueGrid;
   (*axes) += new RescueGridAxis(context, this, 0, i_origin, i_step, i_lowbound, i_count);
}

RescueGrid::RescueGrid(RescueContext *context,
                       RescueCoordinateSystem::Orientation displayOrientationIn,
    RESCUEINT64 i_lowbound, RESCUEINT64 i_count, RESCUEINT64 j_lowbound, RESCUEINT64 j_count)
    :RescueHistoryObject(context)
             ,displayOrientation(displayOrientationIn)
             ,axes(0)
{
   axes = new cSetRescueGridAxis();
   isA = R_RescueGrid;
   (*axes) += new RescueGridAxis(context, this, i_lowbound, i_count);
   (*axes) += new RescueGridAxis(context, this, j_lowbound, j_count);
}

RescueGrid::RescueGrid(RescueContext *context,
                       RescueCoordinateSystem::Orientation displayOrientationIn,
                  RescueCoordinateSystemAxis *i_axis,
                       RESCUEFLOAT i_origin, RESCUEFLOAT i_step, RESCUEINT64 i_lowbound, RESCUEINT64 i_count, 
                       RescueCoordinateSystemAxis *j_axis,
                       RESCUEFLOAT j_origin, RESCUEFLOAT j_step, RESCUEINT64 j_lowbound, RESCUEINT64 j_count)
    :RescueHistoryObject(context)
             ,displayOrientation(displayOrientationIn)
             ,axes(0)
{
   axes = new cSetRescueGridAxis();
   isA = R_RescueGrid;
   (*axes) += new RescueGridAxis(context, this, i_axis, i_origin, i_step, i_lowbound, i_count);
   (*axes) += new RescueGridAxis(context, this, j_axis, j_origin, j_step, j_lowbound, j_count);
}

RescueGrid::RescueGrid(RescueContext *context,
                       RescueCoordinateSystem::Orientation displayOrientationIn,
    RESCUEINT64 i_lowbound, RESCUEINT64 i_count, RESCUEINT64 j_lowbound, RESCUEINT64 j_count, RESCUEINT64 k_lowbound, RESCUEINT64 k_count)
    :RescueHistoryObject(context)
             ,displayOrientation(displayOrientationIn)
             ,axes(0)
{
   axes = new cSetRescueGridAxis();
   isA = R_RescueGrid;
   (*axes) += new RescueGridAxis(context, this, i_lowbound, i_count);
   (*axes) += new RescueGridAxis(context, this, j_lowbound, j_count);
   (*axes) += new RescueGridAxis(context, this, k_lowbound, k_count);
}

RescueGrid::RescueGrid(RescueContext *context,
                       RescueCoordinateSystem::Orientation displayOrientationIn,
                  RescueCoordinateSystemAxis *i_axis,
                       RESCUEFLOAT i_origin, RESCUEFLOAT i_step, RESCUEINT64 i_lowbound, RESCUEINT64 i_count, 
                       RescueCoordinateSystemAxis *j_axis,
                       RESCUEFLOAT j_origin, RESCUEFLOAT j_step, RESCUEINT64 j_lowbound, RESCUEINT64 j_count, 
                       RESCUEINT64 k_lowbound, RESCUEINT64 k_count)
    :RescueHistoryObject(context)
             ,displayOrientation(displayOrientationIn)
             ,axes(0)
{
   axes = new cSetRescueGridAxis();
   isA = R_RescueGrid;
   (*axes) += new RescueGridAxis(context, this, i_axis, i_origin, i_step, i_lowbound, i_count);
   (*axes) += new RescueGridAxis(context, this, j_axis, j_origin, j_step, j_lowbound, j_count);
   (*axes) += new RescueGridAxis(context, this, k_lowbound, k_count);
}

RescueGrid::RescueGrid(RescueContext *context,
                       RescueCoordinateSystem::Orientation displayOrientationIn,
                  RescueCoordinateSystemAxis *i_axis,
                       RESCUEFLOAT i_origin, RESCUEFLOAT i_step, RESCUEINT64 i_lowbound, RESCUEINT64 i_count, 
                       RescueCoordinateSystemAxis *j_axis,
                       RESCUEFLOAT j_origin, RESCUEFLOAT j_step, RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                       RESCUEFLOAT k_origin, RESCUEFLOAT k_step, RESCUEINT64 k_lowbound, RESCUEINT64 k_count)
    :RescueHistoryObject(context)
             ,displayOrientation(displayOrientationIn)
             ,axes(0)
{
   axes = new cSetRescueGridAxis();
   isA = R_RescueGrid;
   (*axes) += new RescueGridAxis(context, this, i_axis, i_origin, i_step, i_lowbound, i_count);
   (*axes) += new RescueGridAxis(context, this, j_axis, j_origin, j_step, j_lowbound, j_count);
   (*axes) += new RescueGridAxis(context, this, k_origin, k_step, k_lowbound, k_count);
}

RescueGrid::RescueGrid(RescueContext *context)
      :RescueHistoryObject(context)
      ,displayOrientation(RescueCoordinateSystem::LUF)
      ,axes(new cSetRescueGridAxis())
{
  isA = R_RescueGrid;
}

void RescueGrid::AddAxis(RescueGridAxis *axis)
{
  if (axis->parentGrid != 0)
  {
  throw "Attempt to add an owned axis.";
  }
  (*axes) += axis;
  axis->parentGrid = this;
}

RescueGrid::RescueGrid(RescueContext *context,
                       FILE *archiveFile):RescueHistoryObject(context)
                                         ,axes(0)
{
  axes = new cSetRescueGridAxis();
  isA = R_RescueGrid;
  ReadId(context, archiveFile);
  if (context->ReadFileVersion() < 32)
  {
  RESCUEFLOAT zRotation;
  myfscanf(context, archiveFile, &zRotation);
  }
  RESCUEINT64 orientationFlag;
  myfscanf(context, archiveFile, &orientationFlag);
  displayOrientation = (RescueCoordinateSystem::Orientation) orientationFlag;
  (*axes).UnArchive(context, archiveFile);
  if (context->ReadFileVersion() >= 5 && context->ReadFileVersion() < 32)
  {
  RESCUEINT64 vertexFlag;

  myfscanf(context, archiveFile, &vertexFlag);
  if (vertexFlag == 1)
  {
      RescueVertex *vertex = new RescueVertex(context, archiveFile);
      delete vertex;
  }
  }
  if (context->grids != 0)
  {
  context->grids->Add(this);
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

void RescueGrid::Relink(RescueObject *parentModel)
{
  RESCUEINT64 ndx = 0;
  RescueGridAxis *axis = (*axes).NthObject(ndx++);
  while (axis != NULL)
  {
  axis->Relink(this, ((RescueModel *) parentModel)->CoordinateSystem());
  axis = (*axes).NthObject(ndx++);
  }
}


void RescueGrid::Archive(RescueContext *context,
                         FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; Grid");
  myfprintf(context, archiveFile, Identifier());
  if (context->FileVersion() < 32)
  {
  RESCUEFLOAT zRotation = (RESCUEFLOAT) 0;
  myfprintf(context, archiveFile, zRotation);
  }
  myfprintf(context, archiveFile, (RESCUEINT64) displayOrientation);
  (*axes).Archive(context, archiveFile);
  if (context->FileVersion() < 32)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEBOOL RescueGrid::IsRegular(RESCUEINT64 which)
{
  RESCUEBOOL myReturn = FALSE;
  RescueGridAxis *axis = this->Axis(which);
  if (axis != 0)
  {
  myReturn = axis->IsRegular();
  }
  return myReturn;
}

RESCUEINT64 RescueGrid::NodeCount64(RESCUEBOOL cellCentered)
{
  RESCUEINT64 count = (*axes).Count64();
  RESCUEINT64 loop;
  RESCUEINT64 size = 1;

  for (loop = 0; loop < count; loop++)
  {
  RESCUEINT64 nodes = (*axes).NthObject(loop)->Count64();
  if (nodes > 1 && cellCentered)
  {
      nodes--;
  }
  size *= nodes;
  }
  return size;
}

RESCUEINT64 RescueGrid::Vertices64()
{
 if ((*axes).Count64() == 1)
 {
  return 0;
 }
 else
 {
  RESCUEINT64 loop;
  RESCUEINT64 size = 1;

  for (loop = 0; loop < 2; loop++)
  {
      size *= (*axes).NthObject(loop)->Count64();
  }
  return size;
 }
}

RESCUEBOOL RescueGrid::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueGrid)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}

void RescueGrid::SetOrientation(RescueCoordinateSystem::Orientation orientIn, bool swapI, bool swapJ, bool swapK)
{
  displayOrientation = orientIn;
  RESCUEINT64 howMany = (*axes).Count64();
  if (swapI)
  {
  (*axes).NthObject(0)->Swap();
  }
  if (swapJ && howMany >= 2)
  {
  (*axes).NthObject(1)->Swap();
  }
  if (swapK && howMany >= 3)
  {
  (*axes).NthObject(2)->Swap();
  }
}

RESCUEINT32 RescueGrid::Vertices(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = Vertices64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}



