/********************************************************************

  RescueRadialGrid.cpp

  A geometry handler for RESCUE style corner point grids.
  See description of each array in the private section of
  the header.

  Rod Hanks,  June 2003

*********************************************************************/
#include <stdio.h>
#include <memory.h>
#include "myHeaders.h"
#include "RescueContext.h"
#include "RescueModel.h"
#include "RescueGeometry.h"
#include "RescueQuadrupletArray.h"
#include "RescueRadialGrid.h"
#include "RescueBuffer.h"
#include <string.h>

RescueRadialGrid::~RescueRadialGrid()
{
  if (vertexX != 0)
  {
  delete [] vertexX;
  }
  if (vertexY != 0)
  {
  delete [] vertexY;
  }
  if (vertexZ != 0)
  {
  delete [] vertexZ;
  }
  if (centerX != 0)
  {
  delete [] centerX;
  }
  if (centerY != 0)
  {
  delete [] centerY;
  }
  if (centerZ != 0)
  {
  delete [] centerZ;
  }
  if (centerMD != 0)
  {
  delete [] centerMD;
  }
}

RESCUEINT64 RescueRadialGrid::CopyArrays(RESCUEINT64 lowNdx, RESCUEINT64 count, 
                                  RESCUEFLOAT *preAllocatedX, RESCUEFLOAT *preAllocatedY, 
                                  RESCUEFLOAT *preAllocatedZ, RESCUEINT64 offset)
{
  RESCUEINT64 myReturn = 0;
  RESCUEINT64 vertexCount = Nodes();
  if (vertexX != 0 && vertexY != 0 && vertexZ != 0)
  {
  RESCUEINT64 loop;
  for (loop = 0; loop < count && lowNdx + loop < vertexCount; loop++)
  {
      if (preAllocatedX != 0)
      {
    preAllocatedX[offset + loop] = vertexX[lowNdx + loop];
      }
      if (preAllocatedY != 0)
      {
    preAllocatedY[offset + loop] = vertexY[lowNdx + loop];
      }
      if (preAllocatedZ != 0)
      {
    preAllocatedZ[offset + loop] = vertexZ[lowNdx + loop];
      }
      myReturn++;
  }
  }
  return myReturn;
}

RESCUEINT64 RescueRadialGrid::CopyCenter(RESCUEINT64 lowNdx, RESCUEINT64 count, 
                                 RESCUEFLOAT *preAllocatedX, RESCUEFLOAT *preAllocatedY, 
                                 RESCUEFLOAT *preAllocatedZ, RESCUEFLOAT *preAllocatedMD,
                                 RESCUEINT64 offset)
{
  RESCUEINT64 myReturn = 0;
  RESCUEINT64 vertexCount = MaxK();
  if (centerX != 0 && centerY != 0 && centerZ != 0 && centerMD != 0)
  {
  RESCUEINT64 loop;
  for (loop = 0; loop < count && lowNdx + loop < vertexCount; loop++)
  {
      if (preAllocatedX != 0)
      {
    preAllocatedX[offset + loop] = centerX[lowNdx + loop];
      }
      if (preAllocatedY != 0)
      {
    preAllocatedY[offset + loop] = centerY[lowNdx + loop];
      }
      if (preAllocatedZ != 0)
      {
    preAllocatedZ[offset + loop] = centerZ[lowNdx + loop];
      }
      if (preAllocatedMD != 0)
      {
    preAllocatedMD[offset + loop] = centerMD[lowNdx + loop];
      }
      myReturn++;
  }
  }
  return myReturn;
}

void RescueRadialGrid::Accept(RESCUEINT64 radialCellsIn, RESCUEINT64 angularCellsIn, RESCUEINT64 kLayersIn,
                              RESCUEBOOL closedIn, RESCUEFLOAT *vertexXin, RESCUEFLOAT *vertexYin,
                              RESCUEFLOAT *vertexZin, RESCUEFLOAT *centerXin, RESCUEFLOAT *centerYin,
                              RESCUEFLOAT *centerZin, RESCUEFLOAT *centerMDin)
{
  if (vertexX != 0)
  {
  delete [] vertexX;
  }
  if (vertexY != 0)
  {
  delete [] vertexY;
  }
  if (vertexZ != 0)
  {
  delete [] vertexZ;
  }
  if (centerX != 0)
  {
  delete [] centerX;
  }
  if (centerY != 0)
  {
  delete [] centerY;
  }
  if (centerZ != 0)
  {
  delete [] centerZ;
  }
  if (centerMD != 0)
  {
  delete [] centerMD;
  }
  radialCells = radialCellsIn;
  angularCells = angularCellsIn;
  kLayers = kLayersIn;
  closed = closedIn;
  vertexX = vertexXin;
  vertexY = vertexYin;
  vertexZ = vertexZin;
  centerX = centerXin;
  centerY = centerYin;
  centerZ = centerZin;
  centerMD = centerMDin;
  SetupGrid();
}

void RescueRadialGrid::SetupGrid()
{
  RescueGrid *grid = parentGeometry->Grid();
  grid->Axis(0)->SetCount(MaxI());
  grid->Axis(1)->SetCount(MaxJ());
  grid->Axis(2)->SetCount(MaxK());
}

void RescueRadialGrid::Set(RESCUEINT64 radialCellsIn, RESCUEINT64 angularCellsIn, RESCUEINT64 kLayersIn,
                           RESCUEBOOL closedIn, RESCUEFLOAT *vertexXin, RESCUEFLOAT *vertexYin,
                           RESCUEFLOAT *vertexZin, RESCUEFLOAT *centerXin, RESCUEFLOAT *centerYin,
                           RESCUEFLOAT *centerZin, RESCUEFLOAT *centerMDin)
{
  RESCUEINT64 newNodes = Nodes(radialCellsIn, angularCellsIn, closedIn, kLayersIn);
  RESCUEINT64 vertexCount = Nodes();
  if (vertexCount != newNodes)
  {
  if (vertexX != 0)
  {
      delete [] vertexX;
  }
  if (vertexY != 0)
  {
      delete [] vertexY;
  }
  if (vertexZ != 0)
  {
      delete [] vertexZ;
  }
  vertexX = new RESCUEFLOAT[(size_t) newNodes];
  vertexY = new RESCUEFLOAT[(size_t) newNodes];
  vertexZ = new RESCUEFLOAT[(size_t) newNodes];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < newNodes; loop++)
  {
  vertexX[loop] = vertexXin[loop];
  vertexY[loop] = vertexYin[loop];
  vertexZ[loop] = vertexZin[loop];
  }
  if (centerXin == 0)
  {
      if (centerX != 0)
      {
    delete [] centerX;
      }
      if (centerY != 0)
      {
    delete [] centerY;
      }
      if (centerZ != 0)
      {
    delete [] centerZ;
      }
      if (centerMD != 0)
      {
    delete [] centerMD;
      }
      centerX = 0;
      centerY = 0;
      centerZ = 0;
      centerMD = 0;
  }
  else
  {
  RESCUEINT64 newCenters = MaxK(kLayersIn);
  RESCUEINT64 nowCenters = MaxK();
  if (newCenters != nowCenters)
  {
      if (centerX != 0)
      {
    delete [] centerX;
      }
      if (centerY != 0)
      {
    delete [] centerY;
      }
      if (centerZ != 0)
      {
    delete [] centerZ;
      }
      if (centerMD != 0)
      {
    delete [] centerMD;
      }
      centerX = new RESCUEFLOAT[(size_t) newCenters];
      centerY = new RESCUEFLOAT[(size_t) newCenters];
      centerZ = new RESCUEFLOAT[(size_t) newCenters];
      centerMD = new RESCUEFLOAT[(size_t) newCenters];
  }
  for (loop = 0; loop < newCenters; loop++)
  {
      centerX[loop] = vertexXin[loop];
      centerY[loop] = vertexYin[loop];
      centerZ[loop] = vertexZin[loop];
      centerMD[loop] = vertexZin[loop];
  }
  }
  radialCells = radialCellsIn;
  angularCells = angularCellsIn;
  kLayers = kLayersIn;
  closed = closedIn;
  SetupGrid();
}

void RescueRadialGrid::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, kLayers);
  myfprintf(context, archiveFile, angularCells);
  myfprintf(context, archiveFile, radialCells);
  myfprintf(context, archiveFile, closed);
  RESCUEINT64 howMany = Nodes();
  myfprintf(context, archiveFile, vertexX, howMany, TRUE);
  myfprintf(context, archiveFile, vertexY, howMany, TRUE);
  myfprintf(context, archiveFile, vertexZ, howMany, TRUE);
  howMany = MaxK();
  if (centerX == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, centerX, howMany, TRUE);
  myfprintf(context, archiveFile, centerY, howMany, TRUE);
  myfprintf(context, archiveFile, centerZ, howMany, TRUE);
  myfprintf(context, archiveFile, centerMD, howMany, TRUE);
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueRadialGrid::RescueRadialGrid(RescueGeometry *lgrGeometry)
{
  parentGeometry = lgrGeometry;
  lgrGeometry->SetRadialGrid(this);
  kLayers = 0; 
  angularCells = 0;
  radialCells =0;
  closed = FALSE; 
  vertexX = 0;
  vertexY = 0;
  vertexZ = 0;
  centerX = 0;
  centerY = 0;
  centerZ = 0;
  centerMD = 0;
}

RescueRadialGrid::RescueRadialGrid(RescueContext *context, FILE *archiveFile)
{
  centerX = 0;
  centerY = 0;
  centerZ = 0;
  centerMD = 0;
  myfscanf(context, archiveFile, &kLayers);
  myfscanf(context, archiveFile, &angularCells);
  myfscanf(context, archiveFile, &radialCells);
  myfscanf(context, archiveFile, &closed);
  RESCUEINT64 howMany = Nodes();
  vertexX = new RESCUEFLOAT[(size_t) howMany];
  myfscanf(context, archiveFile, vertexX, howMany, TRUE);
  vertexY = new RESCUEFLOAT[(size_t) howMany];
  myfscanf(context, archiveFile, vertexY, howMany, TRUE);
  vertexZ = new RESCUEFLOAT[(size_t) howMany];
  myfscanf(context, archiveFile, vertexZ, howMany, TRUE);
  howMany = MaxK();
  RESCUEINT64 temp;
  myfscanf(context, archiveFile, &temp);
  if (temp == 1)
  {
  centerX = new RESCUEFLOAT[(size_t) howMany];
  myfscanf(context, archiveFile, centerX, howMany, TRUE);
  centerY = new RESCUEFLOAT[(size_t) howMany];
  myfscanf(context, archiveFile, centerY, howMany, TRUE);
  centerZ = new RESCUEFLOAT[(size_t) howMany];
  myfscanf(context, archiveFile, centerZ, howMany, TRUE);
  centerMD = new RESCUEFLOAT[(size_t) howMany];
  myfscanf(context, archiveFile, centerMD, howMany, TRUE);
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

void RescueRadialGrid::Relink(RescueObject *object)
{
  parentGeometry = (RescueGeometry *) object;
}

void RescueRadialGrid::CalculateMinMax(RESCUEFLOAT &minX, RESCUEFLOAT &maxX, 
                                       RESCUEFLOAT &minY, RESCUEFLOAT &maxY, 
                                       RESCUEFLOAT &minZ, RESCUEFLOAT &maxZ)
{
  RESCUEFLOAT missingValue = parentGeometry->missingValue;
  RESCUEINT64 howMany = Nodes();
  bool firstXIteration = true;
  bool firstYIteration = true;
  bool firstZIteration = true;
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
  if (vertexX[loop] != missingValue)
  {
      if (firstXIteration)
      {
    minX = vertexX[loop];
    maxX = vertexX[loop];
    firstXIteration = false;
      }
      else
      {
    if (vertexX[loop] < minX)
    {
          minX = vertexX[loop];
    }
    if (vertexX[loop] > maxX)
    {
          maxX = vertexX[loop];
    }
      }
  }
  if (vertexY[loop] != missingValue)
  {
      if (firstYIteration)
      {
    minY = vertexY[loop];
    maxY = vertexY[loop];
    firstYIteration = false;
      }
      else
      {
    if (vertexY[loop] < minY)
    {
          minY = vertexY[loop];
    }
    if (vertexY[loop] > maxY)
    {
          maxY = vertexY[loop];
    }
      }
  }
  if (vertexZ[loop] != missingValue)
  {
      if (firstZIteration)
      {
    minZ = vertexZ[loop];
    maxZ = vertexZ[loop];
    firstZIteration = false;
      }
      else
      {
    if (vertexZ[loop] < minZ)
    {
          minZ = vertexZ[loop];
    }
    if (vertexZ[loop] > maxZ)
    {
          maxZ = vertexZ[loop];
    }
      }
  }
  }
}


