/********************************************************************

  RescueGeometry.cpp

  A geometry handler for RESCUE style corner point grids.

  Rod Hanks,  January, 1997

*********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "RescueGeometry.h"
#include "RescueGeometryObject.h"
#include "RescueZStack.h"
#include "RescueCoordinateLine.h"
#include "RescueCoordinatePolyLine.h"
#include "RescueSplitLine.h"
#include "RescueGrid.h"
#include "RescueGridAxis.h"
#include "RescueWellboreCell.h"
#include "RCH3DArray.h"
#include "RescueModel.h"
#include "RescueReferenceSurface.h"
#include "RescueBitArray.h"
#include "RescueColor.h"
#include "RescueIdTree.h"
#include "cSetRescueSurfaceToGrid.h"
#include "RescueSurfaceToGrid.h"
#include "cSetRescueTrimVertex.h"
#include "cSetRescuePolyLine.h"
#include "RescueGeobody.h"
#include "RescueGeobodyPart.h"
#include "RescueStairSteppedFault.h"
#include "RescueParametricLGRMap.h"
#include "cSetRescueInterfaceSurfaceDesc.h"
#include "RescueUnstructuredGrid.h"
#include "RescueRadialGrid.h"
#include "RescuePillar.h"
#include "RescueBuffer.h"
#include "cSetRescueDataContainer.h"
#include "RescueOrientationLedger.h"

#if !defined(WIN32) && !defined(WIN64) && !defined(_WINDOWS)
#define _strdup strdup
#endif  // !defined(WIN32) && !defined(WIN64)

#define GEOMETRY_FILE_VERSION 3
// Version 2 - Compression of RESCUEFLOAT and RESCUEDOUBLE arrays.
// Version 3 - file version moved to front of file.
RescueGeometry::RescueGeometry(RescueModel *parentModelIn, RescueGrid *existingGrid, RESCUEFLOAT missingValueIn)
                                              :RescueHistoryObject(parentModelIn->Context())
                                              ,grid(existingGrid)
                                              ,missingValue(missingValueIn)
                                              ,parentModel(parentModelIn)
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)
                                              ,referenceID(0)
                                              ,referenceName(0)
                                              ,lgrType(R_NOT_LGR)
                                              ,lgrGridType(R_STRUCTURED_CPG)
                                              ,parentILowBound(0)
                                              ,parentICount(0)
                                              ,parentJLowBound(0)
                                              ,parentJCount(0)
                                              ,parentKLowBound(0)
                                              ,parentKCount(0)
                                              ,parentGeometry(0)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)
                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  if (grid->axes->Count64() != 3)
  {
    throw "Grids for constructing RescueGeometry objects must have three axes.";
  }
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueModel *parentModelIn, RescueGrid *existingGrid, RESCUEFLOAT missingValueIn,
                               RescueReferenceSurface *topSurfaceIn, RESCUEFLOAT topOffsetIn,
                               RescueReferenceSurface *bottomSurfaceIn, RESCUEFLOAT  bottomOffsetIn)
                                              :RescueHistoryObject(parentModelIn->Context())
                                              ,grid(existingGrid)
                                              ,missingValue(missingValueIn)
                                              ,parentModel(parentModelIn)
                                              ,referenceSurface1(topSurfaceIn)
                                              ,referenceSurface2(bottomSurfaceIn)
                                              ,offset1(topOffsetIn)
                                              ,offset2(bottomOffsetIn)
                                              ,thickness((RESCUEFLOAT) 0.0)
                                              ,onOffLap(R_ONLAP)
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)
                                              ,referenceID(0)
                                              ,referenceName(0)
                                              ,lgrType(R_NOT_LGR)
                                              ,lgrGridType(R_STRUCTURED_CPG)
                                              ,parentILowBound(0)
                                              ,parentICount(0)
                                              ,parentJLowBound(0)
                                              ,parentJCount(0)
                                              ,parentKLowBound(0)
                                              ,parentKCount(0)
                                              ,parentGeometry(0)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)

                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
}

RescueGeometry::RescueGeometry(RescueModel *parentModelIn, RescueGrid *existingGrid, RESCUEFLOAT missingValueIn,
                               RescueReferenceSurface *referenceSurfaceIn, RESCUEFLOAT referenceOffsetIn, 
                               RESCUEFLOAT thicknessIn, RescueLapType onOffLapIn)
                                              :RescueHistoryObject(parentModelIn->Context())
                                              ,grid(existingGrid)
                                              ,missingValue(missingValueIn)
                                              ,parentModel(parentModelIn)
                                              ,referenceSurface1(referenceSurfaceIn)
                                              ,referenceSurface2(0)
                                              ,offset1(referenceOffsetIn)
                                              ,offset2((RESCUEFLOAT) 0.0)
                                              ,thickness(thicknessIn)
                                              ,onOffLap(onOffLapIn)
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)
                                              ,referenceID(0)
                                              ,referenceName(0)
                                              ,lgrType(R_NOT_LGR)
                                              ,lgrGridType(R_STRUCTURED_CPG)
                                              ,parentILowBound(0)
                                              ,parentICount(0)
                                              ,parentJLowBound(0)
                                              ,parentJCount(0)
                                              ,parentKLowBound(0)
                                              ,parentKCount(0)
                                              ,parentGeometry(0)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)
                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid, 
                               RESCUEINT64 referenceIDin,
                               RESCUEINT64 parentILowBoundin,       
                               RESCUEINT64 parentICountin,  
                               RESCUEINT64 refinedILowBoundin,
                               RESCUEINT64 refinedICountin,         
                               RESCUEINT64 parentJLowBoundin,   
                               RESCUEINT64 parentJCountin,          
                               RESCUEINT64 refinedJLowBoundin,
                               RESCUEINT64 refinedJCountin,         
                               RESCUEINT64 parentKLowBoundin,      
                               RESCUEINT64 parentKCountin, 
                               RESCUEINT64 refinedKLowBoundin,
                               RESCUEINT64 refinedKCountin)
                               :RescueHistoryObject(parentGrid->ParentModel()->Context())
                               ,missingValue(parentGrid->MissingValue())
                               ,parentModel(parentGrid->ParentModel())
                               ,referenceSurface1(0)
                               ,referenceSurface2(0)
                               ,thickness(0)
                               ,onOffLap(R_ONLAP)
                               ,surfaceCell(0)
                               ,wellboreCell(0)
                               ,surfaceToGrid(0)
                               ,stairSteps(0)
                               ,geometryUnit(0)
                               ,properties(0)
                               ,hasChanged(TRUE)
                              ,dirtyMinMax(TRUE)
                               ,loaded(TRUE)
                               ,color(0)
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)

                               ,referenceID(referenceIDin)
                              ,referenceName(0)
                               ,lgrType(R_RULE_BASED)
                               ,lgrGridType(R_STRUCTURED_CPG)
                               ,parentILowBound(parentILowBoundin)
                               ,parentICount(parentICountin)
                               ,parentJLowBound(parentJLowBoundin)
                               ,parentJCount(parentJCountin)
                               ,parentKLowBound(parentKLowBoundin)
                               ,parentKCount(parentKCountin)
                               ,parentGeometry(parentGrid)
                               ,lgrs(new cSetRescueGeometry())
                               ,xWeightLength(0)
                               ,xWeight(0)
                               ,yWeightLength(0)
                               ,yWeight(0)
                               ,zWeightLength(0)
                               ,zWeight(0)
                               ,parametricMap(0)
                               ,isds(new cSetRescueInterfaceSurfaceDesc())
                               ,unstructuredGrid(0)
                               ,radialGrid(0)
                              ,minX(0)
                              ,maxX(0)
                              ,minY(0)
                              ,maxY(0)
                              ,minZ(0)
                              ,maxZ(0)
                              ,propertyContainers(0)
                              ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  (*parentGeometry->lgrs) += this;
  RescueContext *context = parentGrid->ParentModel()->Context();
  grid = new RescueGrid(context, parentGrid->Grid()->DisplayOrientation(), 
                        refinedILowBoundin, refinedICountin, 
                        refinedJLowBoundin, refinedJCountin, 
                        refinedKLowBoundin, refinedKCountin);
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid, 
                               RESCUEINT32 referenceIDin,
                               RESCUEINT32 parentILowBoundin,       
                               RESCUEINT32 parentICountin,  
                               RESCUEINT32 refinedILowBoundin,
                               RESCUEINT32 refinedICountin,         
                               RESCUEINT32 parentJLowBoundin,   
                               RESCUEINT32 parentJCountin,          
                               RESCUEINT32 refinedJLowBoundin,
                               RESCUEINT32 refinedJCountin,         
                               RESCUEINT32 parentKLowBoundin,      
                               RESCUEINT32 parentKCountin, 
                               RESCUEINT32 refinedKLowBoundin,
                               RESCUEINT32 refinedKCountin)
                               :RescueHistoryObject(parentGrid->ParentModel()->Context())
                               ,missingValue(parentGrid->MissingValue())
                               ,parentModel(parentGrid->ParentModel())
                               ,referenceSurface1(0)
                               ,referenceSurface2(0)
                               ,thickness(0)
                               ,onOffLap(R_ONLAP)
                               ,surfaceCell(0)
                               ,wellboreCell(0)
                               ,surfaceToGrid(0)
                               ,stairSteps(0)
                               ,geometryUnit(0)
                               ,properties(0)
                               ,hasChanged(TRUE)
          ,dirtyMinMax(TRUE)
                               ,loaded(TRUE)
                               ,color(0)
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,referenceID(referenceIDin)
          ,referenceName(0)
                               ,lgrType(R_RULE_BASED)
                               ,lgrGridType(R_STRUCTURED_CPG)
                               ,parentILowBound(parentILowBoundin)
                               ,parentICount(parentICountin)
                               ,parentJLowBound(parentJLowBoundin)
                               ,parentJCount(parentJCountin)
                               ,parentKLowBound(parentKLowBoundin)
                               ,parentKCount(parentKCountin)
                               ,parentGeometry(parentGrid)
                               ,lgrs(new cSetRescueGeometry())
                               ,xWeightLength(0)
                               ,xWeight(0)
                               ,yWeightLength(0)
                               ,yWeight(0)
                               ,zWeightLength(0)
                               ,zWeight(0)

                               ,parametricMap(0)
                               ,isds(new cSetRescueInterfaceSurfaceDesc())
                               ,unstructuredGrid(0)
                               ,radialGrid(0)

                              ,minX(0)
                              ,maxX(0)
                              ,minY(0)
                              ,maxY(0)
                              ,minZ(0)
                              ,maxZ(0)
                              ,propertyContainers(0)
                              ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  (*parentGeometry->lgrs) += this;
  RescueContext *context = parentGrid->ParentModel()->Context();
  grid = new RescueGrid(context, parentGrid->Grid()->DisplayOrientation(), 
                        refinedILowBoundin, refinedICountin, 
                        refinedJLowBoundin, refinedJCountin, 
                        refinedKLowBoundin, refinedKCountin);
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid, 
                               RESCUEINT64 referenceIDin, 
                               RESCUEINT64 parentILowBoundin,
                               RESCUEINT64 parentICountin,
                               RESCUEINT64 *refinedICountArrayin,
                               RESCUEINT64 parentJLowBoundin,
                               RESCUEINT64 parentJCountin,
                               RESCUEINT64 *refinedJCountArrayin,     
                               RESCUEINT64 parentKLowBoundin,        
                               RESCUEINT64 parentKCountin,           
                               RESCUEINT64 *refinedKCountArrayin)       
                               :RescueHistoryObject(parentGrid->ParentModel()->Context())
                               ,missingValue(parentGrid->MissingValue())
                               ,parentModel(parentGrid->ParentModel())
                               ,referenceSurface1(0)
                               ,referenceSurface2(0)
                               ,thickness(0)
                               ,onOffLap(R_ONLAP)
                               ,surfaceCell(0)
                               ,wellboreCell(0)
                               ,surfaceToGrid(0)
                               ,stairSteps(0)
                               ,geometryUnit(0)
                               ,properties(0)
                               ,hasChanged(TRUE)
                                ,dirtyMinMax(TRUE)
                               ,loaded(TRUE)
                               ,color(0)
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)

                               ,referenceID(referenceIDin)
          ,referenceName(0)
                               ,lgrType(R_RULE_BASED_NOSPAN)
                               ,lgrGridType(R_STRUCTURED_CPG)
                               ,parentILowBound(parentILowBoundin)
                               ,parentICount(parentICountin)
                               ,parentJLowBound(parentJLowBoundin)
                               ,parentJCount(parentJCountin)
                               ,parentKLowBound(parentKLowBoundin)
                               ,parentKCount(parentKCountin)
                               ,parentGeometry(parentGrid)
                               ,lgrs(new cSetRescueGeometry())
                               ,xWeightLength(1)
                               ,xWeight(0)
                               ,yWeightLength(1)
                               ,yWeight(0)
                               ,zWeightLength(1)
                               ,zWeight(0)
                               ,parametricMap(0)
                               ,isds(new cSetRescueInterfaceSurfaceDesc())
                               ,unstructuredGrid(0)
                               ,radialGrid(0)
                                ,minX(0)
                                ,maxX(0)
                                ,minY(0)
                                ,maxY(0)
                                ,minZ(0)
                                ,maxZ(0)
                                ,propertyContainers(0)
                                ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  (*parentGeometry->lgrs) += this;
  RescueContext *context = parentGrid->ParentModel()->Context();
  RESCUEINT64 loop;
  for (loop = 0; loop < parentICount; loop++)
  {
    xWeightLength += (refinedICountArrayin[loop] <= 0) ? 1 : refinedICountArrayin[loop];
  }
  for (loop = 0; loop < parentJCount; loop++)
  {
    yWeightLength += (refinedJCountArrayin[loop] <= 0) ? 1 : refinedJCountArrayin[loop];
  }
  for (loop = 0; loop < parentKCount; loop++)
  {
    zWeightLength += (refinedKCountArrayin[loop] <= 0) ? 1 : refinedKCountArrayin[loop];
  }
  grid = new RescueGrid(context, parentGrid->Grid()->DisplayOrientation(), 
                        parentILowBound, xWeightLength, 
                        parentJLowBound, yWeightLength, 
                        parentKLowBound, zWeightLength);
  xWeight = new RESCUEDOUBLE[(size_t) xWeightLength];
  yWeight = new RESCUEDOUBLE[(size_t) yWeightLength];
  zWeight = new RESCUEDOUBLE[(size_t) zWeightLength];

  RESCUEINT64 xNdx = 0;
  xWeight[xNdx++] = (RESCUEDOUBLE) parentILowBound;
  for (loop = 0; loop < parentICount; loop++)
  {
    RESCUEINT64 count = refinedICountArrayin[loop];
    RESCUEINT64 lowNdx = parentILowBound + loop;
    RESCUEINT64 ndx;
    for (ndx = 1; ndx <= count; ndx++)
    {
      xWeight[xNdx++] = (RESCUEDOUBLE) lowNdx + ((RESCUEDOUBLE) ndx / (RESCUEDOUBLE) count);
    }
    if (count <= 0)
    {
      xWeight[xNdx++] = (RESCUEDOUBLE) (lowNdx + 1);
    }
  }
  
  RESCUEINT64 yNdx = 0;
  yWeight[yNdx++] = (RESCUEDOUBLE) parentJLowBound;
  for (loop = 0; loop < parentJCount; loop++)
  {
    RESCUEINT64 count = refinedJCountArrayin[loop];
    RESCUEINT64 lowNdx = parentJLowBound + loop;
    RESCUEINT64 ndx;
    for (ndx = 1; ndx <= count; ndx++)
    {
      yWeight[yNdx++] = (RESCUEDOUBLE) lowNdx + ((RESCUEDOUBLE) ndx / (RESCUEDOUBLE) count);
    }
    if (count <= 0)
    {
      yWeight[yNdx++] = (RESCUEDOUBLE) (lowNdx + 1);
    }
  }
  
  RESCUEINT64 zNdx = 0;
  zWeight[zNdx++] = (RESCUEDOUBLE) parentKLowBound;
  for (loop = 0; loop < parentKCount; loop++)
  {
    RESCUEINT64 count = refinedKCountArrayin[loop];
    RESCUEINT64 lowNdx = parentKLowBound + loop;
    RESCUEINT64 ndx;
    for (ndx = 1; ndx <= count; ndx++)
    {
      zWeight[zNdx++] = (RESCUEDOUBLE) lowNdx + ((RESCUEDOUBLE) ndx / (RESCUEDOUBLE) count);
    }
    if (count <= 0)
    {
      zWeight[zNdx++] = (RESCUEDOUBLE) (lowNdx + 1);
    }
  }
  

  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid, 
                               RESCUEINT32 referenceIDin, 
                               RESCUEINT32 parentILowBoundin,
                               RESCUEINT32 parentICountin,
                               RESCUEINT32 *refinedICountArrayin,
                               RESCUEINT32 parentJLowBoundin,
                               RESCUEINT32 parentJCountin,
                               RESCUEINT32 *refinedJCountArrayin,     
                               RESCUEINT32 parentKLowBoundin,        
                               RESCUEINT32 parentKCountin,           
                               RESCUEINT32 *refinedKCountArrayin)       
                               :RescueHistoryObject(parentGrid->ParentModel()->Context())
                               ,missingValue(parentGrid->MissingValue())
                               ,parentModel(parentGrid->ParentModel())
                               ,referenceSurface1(0)
                               ,referenceSurface2(0)
                               ,thickness(0)
                               ,onOffLap(R_ONLAP)
                               ,surfaceCell(0)
                               ,wellboreCell(0)
                               ,surfaceToGrid(0)
                               ,stairSteps(0)
                               ,geometryUnit(0)
                               ,properties(0)
                               ,hasChanged(TRUE)
                                ,dirtyMinMax(TRUE)
                               ,loaded(TRUE)
                               ,color(0)
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)

                               ,referenceID(referenceIDin)
                              ,referenceName(0)
                               ,lgrType(R_RULE_BASED_NOSPAN)
                               ,lgrGridType(R_STRUCTURED_CPG)
                               ,parentILowBound(parentILowBoundin)
                               ,parentICount(parentICountin)
                               ,parentJLowBound(parentJLowBoundin)
                               ,parentJCount(parentJCountin)
                               ,parentKLowBound(parentKLowBoundin)
                               ,parentKCount(parentKCountin)
                               ,parentGeometry(parentGrid)
                               ,lgrs(new cSetRescueGeometry())
                               ,xWeightLength(1)
                               ,xWeight(0)
                               ,yWeightLength(1)
                               ,yWeight(0)
                               ,zWeightLength(1)
                               ,zWeight(0)
                               ,parametricMap(0)
                               ,isds(new cSetRescueInterfaceSurfaceDesc())
                               ,unstructuredGrid(0)
                               ,radialGrid(0)
                                ,minX(0)
                                ,maxX(0)
                                ,minY(0)
                                ,maxY(0)
                                ,minZ(0)
                                ,maxZ(0)
                                ,propertyContainers(0)
                                ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  (*parentGeometry->lgrs) += this;
  RescueContext *context = parentGrid->ParentModel()->Context();
  RESCUEINT32 loop;
  for (loop = 0; loop < parentICount; loop++)
  {
    xWeightLength += (refinedICountArrayin[loop] <= 0) ? 1 : refinedICountArrayin[loop];
  }
  for (loop = 0; loop < parentJCount; loop++)
  {
    yWeightLength += (refinedJCountArrayin[loop] <= 0) ? 1 : refinedJCountArrayin[loop];
  }
  for (loop = 0; loop < parentKCount; loop++)
  {
    zWeightLength += (refinedKCountArrayin[loop] <= 0) ? 1 : refinedKCountArrayin[loop];
  }
  grid = new RescueGrid(context, parentGrid->Grid()->DisplayOrientation(), 
                        parentILowBound, xWeightLength, 
                        parentJLowBound, yWeightLength, 
                        parentKLowBound, zWeightLength);
  xWeight = new RESCUEDOUBLE[(size_t) xWeightLength];
  yWeight = new RESCUEDOUBLE[(size_t) yWeightLength];
  zWeight = new RESCUEDOUBLE[(size_t) zWeightLength];

  RESCUEINT32 xNdx = 0;
  xWeight[xNdx++] = (RESCUEDOUBLE) parentILowBound;
  for (loop = 0; loop < parentICount; loop++)
  {
    RESCUEINT32 count = refinedICountArrayin[loop];
    RESCUEINT32 lowNdx = (RESCUEINT32) parentILowBound + loop;
    RESCUEINT32 ndx;
    for (ndx = 1; ndx <= count; ndx++)
    {
      xWeight[xNdx++] = (RESCUEDOUBLE) lowNdx + ((RESCUEDOUBLE) ndx / (RESCUEDOUBLE) count);
    }
    if (count <= 0)
    {
      xWeight[xNdx++] = (RESCUEDOUBLE) (lowNdx + 1);
    }
  }
  
  RESCUEINT32 yNdx = 0;
  yWeight[yNdx++] = (RESCUEDOUBLE) parentJLowBound;
  for (loop = 0; loop < parentJCount; loop++)
  {
    RESCUEINT32 count = refinedJCountArrayin[loop];
    RESCUEINT32 lowNdx = (RESCUEINT32) parentJLowBound + loop;
    RESCUEINT32 ndx;
    for (ndx = 1; ndx <= count; ndx++)
    {
      yWeight[yNdx++] = (RESCUEDOUBLE) lowNdx + ((RESCUEDOUBLE) ndx / (RESCUEDOUBLE) count);
    }
    if (count <= 0)
    {
      yWeight[yNdx++] = (RESCUEDOUBLE) (lowNdx + 1);
    }
  }
  
  RESCUEINT32 zNdx = 0;
  zWeight[zNdx++] = (RESCUEDOUBLE) parentKLowBound;
  for (loop = 0; loop < parentKCount; loop++)
  {
    RESCUEINT32 count = refinedKCountArrayin[loop];
    RESCUEINT32 lowNdx = (RESCUEINT32) parentKLowBound + loop;
    RESCUEINT32 ndx;
    for (ndx = 1; ndx <= count; ndx++)
    {
      zWeight[zNdx++] = (RESCUEDOUBLE) lowNdx + ((RESCUEDOUBLE) ndx / (RESCUEDOUBLE) count);
    }
    if (count <= 0)
    {
      zWeight[zNdx++] = (RESCUEDOUBLE) (lowNdx + 1);
    }
  }
  

  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid, 
                               RESCUEINT64 referenceIDin,  
                               RESCUEINT64 xLowBoundin,
                               RESCUEDOUBLE *xWeightsIn,           
                               RESCUEINT64 xWeightLengthIn,
                               RESCUEINT64 yLowBoundin,
                               RESCUEDOUBLE *yWeightsIn,  
                               RESCUEINT64 yWeightLengthIn,
                               RESCUEINT64 zLowBoundin,
                               RESCUEDOUBLE *zWeightsIn,  
                               RESCUEINT64 zWeightLengthIn)
                               :RescueHistoryObject(parentGrid->ParentModel()->Context())
                               ,missingValue(parentGrid->MissingValue())
                               ,parentModel(parentGrid->ParentModel())
                               ,referenceSurface1(0)
                               ,referenceSurface2(0)
                               ,thickness(0)
                               ,onOffLap(R_ONLAP)
                               ,surfaceCell(0)
                               ,wellboreCell(0)
                               ,surfaceToGrid(0)
                               ,stairSteps(0)
                               ,geometryUnit(0)
                               ,properties(0)
                               ,hasChanged(TRUE)
                                ,dirtyMinMax(TRUE)
                               ,loaded(TRUE)
                               ,color(0)
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)

                               ,referenceID(referenceIDin)
                                ,referenceName(0)
                               ,lgrType(R_WEIGHT_BASED)
                               ,lgrGridType(R_STRUCTURED_CPG)
                               ,parentILowBound(0)
                               ,parentICount(0)
                               ,parentJLowBound(0)
                               ,parentJCount(0)
                               ,parentKLowBound(0)
                               ,parentKCount(0)
                               ,parentGeometry(parentGrid)
                               ,lgrs(new cSetRescueGeometry())
                               ,xWeightLength(xWeightLengthIn)
                               ,xWeight(0)
                               ,yWeightLength(yWeightLengthIn)
                               ,yWeight(0)
                               ,zWeightLength(zWeightLengthIn)
                               ,zWeight(0)
                               ,parametricMap(0)
                               ,isds(new cSetRescueInterfaceSurfaceDesc())
                               ,unstructuredGrid(0)
                               ,radialGrid(0)

                                ,minX(0)
                                ,maxX(0)
                                ,minY(0)
                                ,maxY(0)
                                ,minZ(0)
                                ,maxZ(0)
                                ,propertyContainers(0)
                                ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  xWeight = new RESCUEDOUBLE[(size_t) xWeightLength];
  memcpy(xWeight, xWeightsIn, sizeof(RESCUEDOUBLE) * (int) xWeightLength);
  yWeight = new RESCUEDOUBLE[(size_t) yWeightLength];
  memcpy(yWeight, yWeightsIn, sizeof(RESCUEDOUBLE) * (int) yWeightLength);
  zWeight = new RESCUEDOUBLE[(size_t) zWeightLength];
  memcpy(zWeight, zWeightsIn, sizeof(RESCUEDOUBLE) * (int) zWeightLength);

  (*parentGeometry->lgrs) += this;
  RescueContext *context = parentGrid->ParentModel()->Context();
  grid = new RescueGrid(context, parentGrid->Grid()->DisplayOrientation(), 
                        xLowBoundin, xWeightLength, 
                        yLowBoundin, yWeightLength, 
                        zLowBoundin, zWeightLength);
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid, 
                               RESCUEINT32 referenceIDin,  
                               RESCUEINT32 xLowBoundin,
                               RESCUEDOUBLE *xWeightsIn,           
                               RESCUEINT32 xWeightLengthIn,
                               RESCUEINT32 yLowBoundin,
                               RESCUEDOUBLE *yWeightsIn,  
                               RESCUEINT32 yWeightLengthIn,
                               RESCUEINT32 zLowBoundin,
                               RESCUEDOUBLE *zWeightsIn,  
                               RESCUEINT32 zWeightLengthIn)
                               :RescueHistoryObject(parentGrid->ParentModel()->Context())
                               ,missingValue(parentGrid->MissingValue())
                               ,parentModel(parentGrid->ParentModel())
                               ,referenceSurface1(0)
                               ,referenceSurface2(0)
                               ,thickness(0)
                               ,onOffLap(R_ONLAP)
                               ,surfaceCell(0)
                               ,wellboreCell(0)
                               ,surfaceToGrid(0)
                               ,stairSteps(0)
                               ,geometryUnit(0)
                               ,properties(0)
                               ,hasChanged(TRUE)
                                ,dirtyMinMax(TRUE)
                               ,loaded(TRUE)
                               ,color(0)
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)

                               ,referenceID(referenceIDin)
          ,referenceName(0)
                               ,lgrType(R_WEIGHT_BASED)
                               ,lgrGridType(R_STRUCTURED_CPG)
                               ,parentILowBound(0)
                               ,parentICount(0)
                               ,parentJLowBound(0)
                               ,parentJCount(0)
                               ,parentKLowBound(0)
                               ,parentKCount(0)
                               ,parentGeometry(parentGrid)
                               ,lgrs(new cSetRescueGeometry())
                               ,xWeightLength(xWeightLengthIn)
                               ,xWeight(0)
                               ,yWeightLength(yWeightLengthIn)
                               ,yWeight(0)
                               ,zWeightLength(zWeightLengthIn)
                               ,zWeight(0)
                               ,parametricMap(0)
                               ,isds(new cSetRescueInterfaceSurfaceDesc())
                               ,unstructuredGrid(0)
                               ,radialGrid(0)
                                ,minX(0)
                                ,maxX(0)
                                ,minY(0)
                                ,maxY(0)
                                ,minZ(0)
                                ,maxZ(0)
                                ,propertyContainers(0)
                                ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  xWeight = new RESCUEDOUBLE[(size_t) xWeightLength];
  memcpy(xWeight, xWeightsIn, sizeof(RESCUEDOUBLE) * (int) xWeightLength);
  yWeight = new RESCUEDOUBLE[(size_t) yWeightLength];
  memcpy(yWeight, yWeightsIn, sizeof(RESCUEDOUBLE) * (int) yWeightLength);
  zWeight = new RESCUEDOUBLE[(size_t) zWeightLength];
  memcpy(zWeight, zWeightsIn, sizeof(RESCUEDOUBLE) * (int) zWeightLength);

  (*parentGeometry->lgrs) += this;
  RescueContext *context = parentGrid->ParentModel()->Context();
  grid = new RescueGrid(context, parentGrid->Grid()->DisplayOrientation(), 
                        xLowBoundin, xWeightLength, 
                        yLowBoundin, yWeightLength, 
                        zLowBoundin, zWeightLength);
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid, 
                               RESCUEINT64 referenceIDin, 
                               RESCUEINT64 parentILowBoundin,
                               RESCUEINT64 parentICountin, 
                               RESCUEINT64 *xWeightLengthin,
                               RESCUEDOUBLE **xWeightsin,
                               RESCUEINT64 parentJLowBoundin,
                               RESCUEINT64 parentJCountin,
                               RESCUEINT64 *yWeightLengthin,
                               RESCUEDOUBLE **yWeightsin,
                               RESCUEINT64 parentKLowBoundin,
                               RESCUEINT64 parentKCountin,           
                               RESCUEINT64 *zWeightLengthin,
                               RESCUEDOUBLE **zWeightsin)  
                               :RescueHistoryObject(parentGrid->ParentModel()->Context())
                               ,missingValue(parentGrid->MissingValue())
                               ,parentModel(parentGrid->ParentModel())
                               ,referenceSurface1(0)
                               ,referenceSurface2(0)
                               ,thickness(0)
                               ,onOffLap(R_ONLAP)
                               ,surfaceCell(0)
                               ,wellboreCell(0)
                               ,surfaceToGrid(0)
                               ,stairSteps(0)
                               ,geometryUnit(0)
                               ,properties(0)
                               ,hasChanged(TRUE)
                                ,dirtyMinMax(TRUE)
                               ,loaded(TRUE)
                               ,color(0)
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)

                               ,referenceID(referenceIDin)
          ,referenceName(0)
                               ,lgrType(R_WEIGHT_BASED_NOSPAN)
                               ,lgrGridType(R_STRUCTURED_CPG)
                               ,parentILowBound(parentILowBoundin)
                               ,parentICount(parentICountin)
                               ,parentJLowBound(parentJLowBoundin)
                               ,parentJCount(parentJCountin)
                               ,parentKLowBound(parentKLowBoundin)
                               ,parentKCount(parentKCountin)
                               ,parentGeometry(parentGrid)
                               ,lgrs(new cSetRescueGeometry())
                               ,xWeightLength(1)
                               ,xWeight(0)
                               ,yWeightLength(1)
                               ,yWeight(0)
                               ,zWeightLength(1)
                               ,zWeight(0)
                               ,parametricMap(0)
                               ,isds(new cSetRescueInterfaceSurfaceDesc())
                               ,unstructuredGrid(0)
                               ,radialGrid(0)
                                ,minX(0)
                                ,maxX(0)
                                ,minY(0)
                                ,maxY(0)
                                ,minZ(0)
                                ,maxZ(0)
                                ,propertyContainers(0)
                                ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  (*parentGeometry->lgrs) += this;
  RescueContext *context = parentGrid->ParentModel()->Context();
  RESCUEINT64 loop;
  for (loop = 0; loop < parentICountin; loop++)
  {
    xWeightLength += (xWeightLengthin[loop] <= 0) ? 1 : xWeightLengthin[loop];
  }
  for (loop = 0; loop < parentJCountin; loop++)
  {
    yWeightLength += (yWeightLengthin[loop] <= 0) ? 1 : yWeightLengthin[loop];
  }
  for (loop = 0; loop < parentKCountin; loop++)
  {
    zWeightLength += (zWeightLengthin[loop] <= 0) ? 1 : zWeightLengthin[loop];
  }
  xWeight = new RESCUEDOUBLE[(size_t) xWeightLength];
  RESCUEINT64 ndx = 0;
  xWeight[ndx++] = (RESCUEDOUBLE) parentILowBound;
  for (loop = 0; loop < parentICountin; loop++)
  {
    RESCUEDOUBLE baseNdx = (RESCUEDOUBLE) (parentILowBound + loop);
    RESCUEDOUBLE cum = 0.0;
    RESCUEINT64 subloop;
    for (subloop = 0; subloop < xWeightLengthin[loop]; subloop++)
    {
      if (subloop == xWeightLengthin[loop] - 1)
      {
        cum = 1.0;
      }
      else
      {
        cum += xWeightsin[loop][subloop];
      }
      xWeight[ndx++] = baseNdx + cum;
    }
    if (xWeightLengthin[loop] <= 0)
    {
      xWeight[ndx++] = (baseNdx + 1.0);
    }
  }
  yWeight = new RESCUEDOUBLE[(size_t) yWeightLength];
  ndx = 0;
  yWeight[ndx++] = (RESCUEDOUBLE) parentJLowBound;
  for (loop = 0; loop < parentJCountin; loop++)
  {
    RESCUEDOUBLE baseNdx = (RESCUEDOUBLE) (parentJLowBound + loop);
    RESCUEDOUBLE cum = 0.0;
    RESCUEINT64 subloop;
    for (subloop = 0; subloop < yWeightLengthin[loop]; subloop++)
    {
      if (subloop == yWeightLengthin[loop] - 1)
      {
        cum = 1.0;
      }
      else
      {
        cum += yWeightsin[loop][subloop];
      }
      yWeight[ndx++] = baseNdx + cum;
    }
    if (yWeightLengthin[loop] <= 0)
    {
      yWeight[ndx++] = (baseNdx + 1.0);
    }
  }
  zWeight = new RESCUEDOUBLE[(size_t) zWeightLength];
  ndx = 0;
  zWeight[ndx++] = (RESCUEDOUBLE) parentKLowBound;
  for (loop = 0; loop < parentKCountin; loop++)
  {
    RESCUEDOUBLE baseNdx = (RESCUEDOUBLE) (parentKLowBound + loop);
    RESCUEDOUBLE cum = 0.0;
    RESCUEINT64 subloop;
    for (subloop = 0; subloop < zWeightLengthin[loop]; subloop++)
    {
      if (subloop == zWeightLengthin[loop] - 1)
      {
        cum = 1.0;
      }
      else
      {
        cum += zWeightsin[loop][subloop];
      }
      zWeight[ndx++] = baseNdx + cum;
    }
    if (zWeightLengthin[loop] <= 0)
    {
      zWeight[ndx++] = (baseNdx + 1.0);
    }
  }

  grid = new RescueGrid(context, parentGrid->Grid()->DisplayOrientation(), 
                        0, xWeightLength, 
                        0, yWeightLength, 
                        0, zWeightLength);
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid, 
                               RESCUEINT32 referenceIDin, 
                               RESCUEINT32 parentILowBoundin,
                               RESCUEINT32 parentICountin, 
                               RESCUEINT32 *xWeightLengthin,
                               RESCUEDOUBLE **xWeightsin,
                               RESCUEINT32 parentJLowBoundin,
                               RESCUEINT32 parentJCountin,
                               RESCUEINT32 *yWeightLengthin,
                               RESCUEDOUBLE **yWeightsin,
                               RESCUEINT32 parentKLowBoundin,
                               RESCUEINT32 parentKCountin,           
                               RESCUEINT32 *zWeightLengthin,
                               RESCUEDOUBLE **zWeightsin)  
                               :RescueHistoryObject(parentGrid->ParentModel()->Context())
                               ,missingValue(parentGrid->MissingValue())
                               ,parentModel(parentGrid->ParentModel())
                               ,referenceSurface1(0)
                               ,referenceSurface2(0)
                               ,thickness(0)
                               ,onOffLap(R_ONLAP)
                               ,surfaceCell(0)
                               ,wellboreCell(0)
                               ,surfaceToGrid(0)
                               ,stairSteps(0)
                               ,geometryUnit(0)
                               ,properties(0)
                               ,hasChanged(TRUE)
                                ,dirtyMinMax(TRUE)
                               ,loaded(TRUE)
                               ,color(0)
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)

                               ,referenceID(referenceIDin)
          ,referenceName(0)
                               ,lgrType(R_WEIGHT_BASED_NOSPAN)
                               ,lgrGridType(R_STRUCTURED_CPG)
                               ,parentILowBound(parentILowBoundin)
                               ,parentICount(parentICountin)
                               ,parentJLowBound(parentJLowBoundin)
                               ,parentJCount(parentJCountin)
                               ,parentKLowBound(parentKLowBoundin)
                               ,parentKCount(parentKCountin)
                               ,parentGeometry(parentGrid)
                               ,lgrs(new cSetRescueGeometry())
                               ,xWeightLength(1)
                               ,xWeight(0)
                               ,yWeightLength(1)
                               ,yWeight(0)
                               ,zWeightLength(1)
                               ,zWeight(0)
                               ,parametricMap(0)
                               ,isds(new cSetRescueInterfaceSurfaceDesc())
                               ,unstructuredGrid(0)
                               ,radialGrid(0)
                                ,minX(0)
                                ,maxX(0)
                                ,minY(0)
                                ,maxY(0)
                                ,minZ(0)
                                ,maxZ(0)
                                ,propertyContainers(0)
                                ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  (*parentGeometry->lgrs) += this;
  RescueContext *context = parentGrid->ParentModel()->Context();
  RESCUEINT32 loop;
  for (loop = 0; loop < parentICountin; loop++)
  {
    xWeightLength += (xWeightLengthin[loop] <= 0) ? 1 : xWeightLengthin[loop];
  }
  for (loop = 0; loop < parentJCountin; loop++)
  {
    yWeightLength += (yWeightLengthin[loop] <= 0) ? 1 : yWeightLengthin[loop];
  }
  for (loop = 0; loop < parentKCountin; loop++)
  {
    zWeightLength += (zWeightLengthin[loop] <= 0) ? 1 : zWeightLengthin[loop];
  }
  xWeight = new RESCUEDOUBLE[(size_t) xWeightLength];
  RESCUEINT32 ndx = 0;
  xWeight[ndx++] = (RESCUEDOUBLE) parentILowBound;
  for (loop = 0; loop < parentICountin; loop++)
  {
    RESCUEDOUBLE baseNdx = (RESCUEDOUBLE) (parentILowBound + loop);
    RESCUEDOUBLE cum = 0.0;
    RESCUEINT32 subloop;
    for (subloop = 0; subloop < xWeightLengthin[loop]; subloop++)
    {
      if (subloop == xWeightLengthin[loop] - 1)
      {
        cum = 1.0;
      }
      else
      {
        cum += xWeightsin[loop][subloop];
      }
      xWeight[ndx++] = baseNdx + cum;
    }
    if (xWeightLengthin[loop] <= 0)
    {
      xWeight[ndx++] = (baseNdx + 1.0);
    }
  }
  yWeight = new RESCUEDOUBLE[(size_t) yWeightLength];
  ndx = 0;
  yWeight[ndx++] = (RESCUEDOUBLE) parentJLowBound;
  for (loop = 0; loop < parentJCountin; loop++)
  {
    RESCUEDOUBLE baseNdx = (RESCUEDOUBLE) (parentJLowBound + loop);
    RESCUEDOUBLE cum = 0.0;
    RESCUEINT32 subloop;
    for (subloop = 0; subloop < yWeightLengthin[loop]; subloop++)
    {
      if (subloop == yWeightLengthin[loop] - 1)
      {
        cum = 1.0;
      }
      else
      {
        cum += yWeightsin[loop][subloop];
      }
      yWeight[ndx++] = baseNdx + cum;
    }
    if (yWeightLengthin[loop] <= 0)
    {
      yWeight[ndx++] = (baseNdx + 1.0);
    }
  }
  zWeight = new RESCUEDOUBLE[(size_t) zWeightLength];
  ndx = 0;
  zWeight[ndx++] = (RESCUEDOUBLE) parentKLowBound;
  for (loop = 0; loop < parentKCountin; loop++)
  {
    RESCUEDOUBLE baseNdx = (RESCUEDOUBLE) (parentKLowBound + loop);
    RESCUEDOUBLE cum = 0.0;
    RESCUEINT32 subloop;
    for (subloop = 0; subloop < zWeightLengthin[loop]; subloop++)
    {
      if (subloop == zWeightLengthin[loop] - 1)
      {
        cum = 1.0;
      }
      else
      {
        cum += zWeightsin[loop][subloop];
      }
      zWeight[ndx++] = baseNdx + cum;
    }
    if (zWeightLengthin[loop] <= 0)
    {
      zWeight[ndx++] = (baseNdx + 1.0);
    }
  }

  grid = new RescueGrid(context, parentGrid->Grid()->DisplayOrientation(), 
                        0, xWeightLength, 
                        0, yWeightLength, 
                        0, zWeightLength);
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid,
                               RESCUEINT64 referenceIDIn,            
                               RescueGrid *existingGrid, 
                               RESCUEFLOAT missingValueIn)
                                              :RescueHistoryObject(parentGrid->ParentModel()->Context())
                                              ,grid(existingGrid)
                                              ,missingValue(missingValueIn)
                                              ,parentModel(parentGrid->ParentModel())
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)
                                              ,referenceID(referenceIDIn)
                                              ,referenceName(0)
                                              ,lgrType(R_UNCONSTRAINED)
                                              ,lgrGridType(R_STRUCTURED_CPG)
                                              ,parentILowBound(0)
                                              ,parentICount(0)
                                              ,parentJLowBound(0)
                                              ,parentJCount(0)
                                              ,parentKLowBound(0)
                                              ,parentKCount(0)
                                              ,parentGeometry(parentGrid)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)
                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  (*parentGeometry->lgrs) += this;
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid,
                               RESCUEINT32 referenceIDIn,            
                               RescueGrid *existingGrid, 
                               RESCUEFLOAT missingValueIn)
                                              :RescueHistoryObject(parentGrid->ParentModel()->Context())
                                              ,grid(existingGrid)
                                              ,missingValue(missingValueIn)
                                              ,parentModel(parentGrid->ParentModel())
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)
                                              ,referenceID(referenceIDIn)
                                              ,referenceName(0)
                                              ,lgrType(R_UNCONSTRAINED)
                                              ,lgrGridType(R_STRUCTURED_CPG)
                                              ,parentILowBound(0)
                                              ,parentICount(0)
                                              ,parentJLowBound(0)
                                              ,parentJCount(0)
                                              ,parentKLowBound(0)
                                              ,parentKCount(0)
                                              ,parentGeometry(parentGrid)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)
                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  (*parentGeometry->lgrs) += this;
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid,
                               RESCUEINT64 referenceIDIn,
                               RescueLGRGridType gridType,
                               RESCUEINT64 expectedVertexCount,
                               RESCUEINT64 expectedFaceCount,
                               RESCUEINT64 expectedFaceVertexTuples,
                               RESCUEINT64 expectedCellCount,
                               RESCUEINT64 expectedCellFaceTuples,
                               RESCUEINT64 expectedVertexRealloc,
                               RESCUEINT64 expectedFaceRealloc,
                               RESCUEINT64 expectedFaceVertexRealloc,
                               RESCUEINT64 expectedCellRealloc,
                               RESCUEINT64 expectedCellFaceRealloc)
                                              :RescueHistoryObject(parentGrid->ParentModel()->Context())
                                              ,grid(0)
                                              ,missingValue(0)
                                              ,parentModel(parentGrid->ParentModel())
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)

                                              ,referenceID(referenceIDIn)
                                              ,referenceName(0)
                                              ,lgrType(R_UNCONSTRAINED)
                                              ,lgrGridType(gridType)
                                              ,parentILowBound(0)
                                              ,parentICount(0)
                                              ,parentJLowBound(0)
                                              ,parentJCount(0)
                                              ,parentKLowBound(0)
                                              ,parentKCount(0)
                                              ,parentGeometry(parentGrid)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)
                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  (*parentGeometry->lgrs) += this;
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
  grid = new RescueGrid(parentGrid->ParentModel()->Context(), RescueCoordinateSystem::LUF, 0, expectedCellCount+1);
  unstructuredGrid = new RescueUnstructuredGrid(this, expectedVertexCount,
                                                      expectedFaceCount,
                                                      expectedFaceVertexTuples,
                                                      expectedCellCount,
                                                      expectedCellFaceTuples,
                                                      expectedVertexRealloc,
                                                      expectedFaceRealloc,
                                                      expectedFaceVertexRealloc,
                                                      expectedCellRealloc,
                                                      expectedCellFaceRealloc);

}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid,
                               RESCUEINT64 parentILowBoundIn,
                               RESCUEINT64 parentICountIn,
                               RESCUEINT64 parentJLowBoundIn,
                               RESCUEINT64 parentJCountIn,      
                               RESCUEINT64 parentKLowBoundIn, 
                               RESCUEINT64 parentKCountIn,           
                               RESCUEINT64 referenceIDIn,
                               RescueLGRGridType gridType,
                               RESCUEINT64 expectedVertexCount,
                               RESCUEINT64 expectedFaceCount,
                               RESCUEINT64 expectedFaceVertexTuples,
                               RESCUEINT64 expectedCellCount,
                               RESCUEINT64 expectedCellFaceTuples,
                               RESCUEINT64 expectedVertexRealloc,
                               RESCUEINT64 expectedFaceRealloc,
                               RESCUEINT64 expectedFaceVertexRealloc,
                               RESCUEINT64 expectedCellRealloc,
                               RESCUEINT64 expectedCellFaceRealloc)
                                              :RescueHistoryObject(parentGrid->ParentModel()->Context())
                                              ,grid(0)
                                              ,missingValue(0)
                                              ,parentModel(parentGrid->ParentModel())
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)
                                              ,referenceID(referenceIDIn)
                                              ,referenceName(0)
                                              ,lgrType(R_UNCONSTRAINED)
                                              ,lgrGridType(gridType)
                                              ,parentILowBound(parentILowBoundIn)
                                              ,parentICount(parentICountIn)
                                              ,parentJLowBound(parentJLowBoundIn)
                                              ,parentJCount(parentJCountIn)
                                              ,parentKLowBound(parentKLowBoundIn)
                                              ,parentKCount(parentKCountIn)
                                              ,parentGeometry(parentGrid)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)
                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  (*parentGeometry->lgrs) += this;
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
  grid = new RescueGrid(parentGrid->ParentModel()->Context(), RescueCoordinateSystem::LUF, 0, expectedCellCount+1);
  unstructuredGrid = new RescueUnstructuredGrid(this, expectedVertexCount,
                                                      expectedFaceCount,
                                                      expectedFaceVertexTuples,
                                                      expectedCellCount,
                                                      expectedCellFaceTuples,
                                                      expectedVertexRealloc,
                                                      expectedFaceRealloc,
                                                      expectedFaceVertexRealloc,
                                                      expectedCellRealloc,
                                                      expectedCellFaceRealloc);

}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid,
                               RESCUEINT64 referenceIDIn)
                                              :RescueHistoryObject(parentGrid->ParentModel()->Context())
                                              ,grid(0)
                                              ,missingValue(0)
                                              ,parentModel(parentGrid->ParentModel())
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)
                                              ,referenceID(referenceIDIn)
                                              ,referenceName(0)
                                              ,lgrType(R_UNCONSTRAINED)
                                              ,lgrGridType(R_STRUCTURED_RADIAL)
                                              ,parentILowBound(0)
                                              ,parentICount(0)
                                              ,parentJLowBound(0)
                                              ,parentJCount(0)
                                              ,parentKLowBound(0)
                                              ,parentKCount(0)
                                              ,parentGeometry(parentGrid)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)
                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  (*parentGeometry->lgrs) += this;
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
  grid = new RescueGrid(parentGrid->ParentModel()->Context(), RescueCoordinateSystem::LUF, 0, 1, 0, 1, 0, 1);
  radialGrid = new RescueRadialGrid(this);
}

RescueGeometry::RescueGeometry(RescueGeometry *parentGrid,
                               RESCUEINT64 parentILowBoundIn,
                               RESCUEINT64 parentICountIn,
                               RESCUEINT64 parentJLowBoundIn,
                               RESCUEINT64 parentJCountIn,      
                               RESCUEINT64 parentKLowBoundIn, 
                               RESCUEINT64 parentKCountIn,           
                               RESCUEINT64 referenceIDIn)
                                              :RescueHistoryObject(parentGrid->ParentModel()->Context())
                                              ,grid(0)
                                              ,missingValue(0)
                                              ,parentModel(parentGrid->ParentModel())
                                              ,surfaceCell(0)
                                              ,wellboreCell(0)
                                              ,surfaceToGrid(0)
                                              ,stairSteps(0)
                                              ,geometryUnit(0)
                                              ,properties(0)
                                              ,hasChanged(TRUE)
                                              ,dirtyMinMax(TRUE)
                                              ,loaded(TRUE)
                                              ,color(0)
                                              ,parentBlockUnit(0)
                                              ,parentGeobodyPart(0)
                                              ,referenceID(referenceIDIn)
                                              ,referenceName(0)
                                              ,lgrType(R_UNCONSTRAINED)
                                              ,lgrGridType(R_STRUCTURED_RADIAL)
                                              ,parentILowBound(parentILowBoundIn)
                                              ,parentICount(parentICountIn)
                                              ,parentJLowBound(parentJLowBoundIn)
                                              ,parentJCount(parentJCountIn)

                                              ,parentKLowBound(parentKLowBoundIn)
                                              ,parentKCount(parentKCountIn)
                                              ,parentGeometry(parentGrid)
                                              ,lgrs(new cSetRescueGeometry())
                                              ,xWeightLength(0)
                                              ,xWeight(0)
                                              ,yWeightLength(0)
                                              ,yWeight(0)
                                              ,zWeightLength(0)
                                              ,zWeight(0)
                                              ,parametricMap(0)
                                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                                              ,unstructuredGrid(0)
                                              ,radialGrid(0)
                                              ,minX(0)
                                              ,maxX(0)
                                              ,minY(0)
                                              ,maxY(0)
                                              ,minZ(0)
                                              ,maxZ(0)
                                              ,propertyContainers(0)
                                              ,propertyContainerId(0)
                                              ,lgrSetName(0)
                                              ,lgrStartTime(0)
                                              ,lgrEndTime(0)
{
  isA = R_RescueGeometry;
  surfaceCell = new cSetRescueSurfaceCell();
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  surfaceToGrid = new cSetRescueSurfaceToGrid();
  stairSteps = new cSetRescueStairSteppedFault();
  wellboreCell = new cSetRescueWellboreCell();
  (*parentGeometry->lgrs) += this;
  CommonInitialization();
  referenceSurface1 = 0;
  referenceSurface2 = 0;
  offset1 = (RESCUEFLOAT) 0.0;
  offset2 = (RESCUEFLOAT) 0.0;
  thickness = (RESCUEFLOAT) 0.0;
  onOffLap = R_ONLAP;
  grid = new RescueGrid(parentGrid->ParentModel()->Context(), RescueCoordinateSystem::LUF, 0, 1, 0, 1, 0, 1);
  radialGrid = new RescueRadialGrid(this);
}

void RescueGeometry::CommonInitialization()
{
 isA = R_RescueGeometry;
 vertexIs = R_EQUAL_AXIS;
  if (grid == 0)
  {
    jNodes = 0;
    kNodes = 0;
    vertices = 0;
    kLayerIDs = 0;
    cellStatus = 0;
  }
  else
  {
   jNodes = grid->Axis(1)->Count64();
    kNodes = grid->Axis(2)->Count64();
/*
 We're going to need this a lot so lets make a copy.
*/
   RESCUEINT64 howMany = grid->Vertices64();
   vertices = (RescueGeometryObject **) malloc(sizeof(RescueGeometryObject *) * (size_t) howMany);
   RESCUEINT64 loop;
   for (loop = 0; loop < howMany; loop++)
   {
    vertices[loop] = 0;
   }
    RESCUEINT64 kCount = grid->Axis(2)->Count64();
    kLayerIDs = new RESCUEINT64[(size_t) kCount];
    for (loop = 0; loop < kCount; loop++)
    {
      kLayerIDs[loop] = 0;
    }
    cellStatus = new RescueBitArray(parentModel->Context(), grid->NodeCount64(), (int) 2);
  }
  loaded = TRUE;
  mainFileVersion = 0;
  wireframeVersion = 0;
}

RESCUEBOOL RescueGeometry::DropGeometry(RescueGeometry *toDrop)
{
  RESCUEBOOL didDrop = (*lgrs) -= toDrop;
  if (didDrop == FALSE)
  {
    RESCUEINT64 gOrd = 0;
    RescueGeometry *child = lgrs->NthObject(gOrd++);
    while (child != 0 && didDrop == FALSE)
    {
      didDrop = child->DropGeometry(toDrop);
      child = lgrs->NthObject(gOrd++);
    }
  }
  return didDrop;
}

RescueGeometry::~RescueGeometry()
{
  if (color != 0)
  {
    delete color;
  }
  if (surfaceCell != 0)
  {
    delete surfaceCell;
  }
  if (geometryUnit != 0)
  {
    delete geometryUnit;
  }
  if (properties != 0)
  {
    delete properties;
  }
  if (surfaceToGrid != 0)
  {
    delete surfaceToGrid;
  }
  if (stairSteps != 0)
  {
    delete stairSteps;
  }
  if (wellboreCell != 0)
  {
    delete wellboreCell;
  }
  if (lgrs != 0)
  {
    delete lgrs;
  }
  if (isds != 0)
  {
    delete isds;
  }
  if (xWeight != 0)
  {
    delete [] xWeight;
  }
  if (yWeight != 0)
  {
    delete [] yWeight;
  }
  if (zWeight != 0)
  {
    delete [] zWeight;
  }
  if (vertices != 0)
  {
   RESCUEINT64 howMany = grid->Vertices64();
   RESCUEINT64 loop;
   for (loop = 0; loop < howMany; loop++)
   {
    if (vertices[loop] != 0)
    {
        RescueGeometryObject *object = (RescueGeometryObject *) vertices[loop];
        switch (object->VertexIs())
        {
        case R_UNEQUAL_AXIS:
          delete (RescueZStack *) object;
          break;
        case R_COORDINATE_LINE:
          delete (RescueCoordinateLine *) object;
          break;
        case R_COORDINATE_POLYLINE:
          delete (RescueCoordinatePolyLine *) object;
          break;
        case R_SPLINE_LINE:
          delete (RescuePillar *) object;
          break;
        case R_SPLIT_LINE:
          delete (RescueSplitLine *) object;
          break;
 default:
   break;  // Quiet warning
        }
    }
   }
   free(vertices);
  }
  if (kLayerIDs != 0)
  {
    delete[] kLayerIDs;
  }
  if (cellStatus != 0)
  {
    delete cellStatus;
  }
  if (grid != 0)
  {
    delete grid;
  }
  if (parametricMap != 0)
  {
    delete parametricMap;
  }
  if (unstructuredGrid != 0)
  {
    delete unstructuredGrid;
  }
  if (radialGrid != 0)
  {
    delete radialGrid;
  }
  if (propertyContainers != 0)
  {
    delete propertyContainers;
  }
  if (referenceName != 0)
  {
    free( referenceName );
  }
}

RescueGeometry *RescueGeometry::ProgenyHavingID(RESCUEINT64 refIdIn)
{
  RescueGeometry *myReturn = 0;
  if (referenceID == refIdIn)
  {
    myReturn = this;
  }
  else
  {
    RESCUEINT64 ordinal = 0;
    RescueGeometry *child = lgrs->NthObject(ordinal++);
    while (child != 0 && myReturn == 0)
    {
      myReturn = child->ProgenyHavingID(refIdIn);
      child = lgrs->NthObject(ordinal++);
    }
  }
  return myReturn;
}


RESCUEINT32 RescueGeometry::LGRCount()
{
  return lgrs->Count();
}

RESCUEINT64 RescueGeometry::LGRCount64()
{
  return lgrs->Count64();
}

RescueGeometry *RescueGeometry::NthLGR(RESCUEINT64 zeroBasedOrdinal)
{
  return lgrs->NthObject(zeroBasedOrdinal);
}

RescueGeometry *RescueGeometry::ParentBUG()
{
  RescueGeometry *myReturn = this;
  while (myReturn->ParentGeometry() != 0)
  {
    myReturn = myReturn->ParentGeometry();
  }
  return myReturn;
}

RESCUEINT32 *RescueGeometry::ObjectCounts(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(ObjectCounts(), 4, true, throwIfTooBig);
}

RESCUEINT64 *RescueGeometry::ObjectCounts()
{
  EnsureLoaded();
  RESCUEINT64 *myReturn = 0;
  if (vertices != 0)
  {
    myReturn = new RESCUEINT64[5];
   RESCUEINT64 howMany = grid->Vertices64();
   RESCUEINT64 loop;
    for (loop = 0; loop < 4; loop++)
    {
      myReturn[loop] = 0;
    }
   for (loop = 0; loop < howMany; loop++)
   {
    if (vertices[loop] != 0)
    {
        RescueGeometryObject *object = (RescueGeometryObject *) vertices[loop];
        switch (object->VertexIs())
        {
        case R_UNEQUAL_AXIS:
          myReturn[0] += 1;
          break;
        case R_COORDINATE_LINE:
          myReturn[1] += 1;
          break;
        case R_COORDINATE_POLYLINE:
          myReturn[2] += 1;
          break;
        case R_SPLIT_LINE:
          myReturn[3] += 1;
          break;
        case R_SPLINE_LINE:
          myReturn[4] += 1;
          break;
 default:
   break;
        }
    }
   }
  }
  return myReturn;
}

void RescueGeometry::RescueDeleteFile()
{
  if (parentModel != 0)
  {
    if (parentModel->currentPathName != 0)
    {
      if (parentModel->currentPathName[0] != 0)
      {
        RCHString fileName;
        fileName << parentModel->currentPathName << "." << Identifier();
        parentModel->ScheduleDeleteFile(fileName.String());
      }
    }
  }
  RescueProperty *property = NthRescueProperty(1);
  while (property != 0)
  {
    DropRescueProperty(property);
    property = NthRescueProperty(1);
  }
}

RescueGeometry::RescueGridType RescueGeometry::GeometryType()
{
  if (VertexIs() == R_EQUAL_AXIS
  ||  VertexIs() == R_UNEQUAL_AXIS)
  {
    return R_XY_ORTHOGONAL;
  }
  else
  {
    RescueGridType myReturn = R_CONFORMABLE;
    if (VertexIs() == R_COORDINATE_LINE
    ||  VertexIs() == R_COORDINATE_POLYLINE)
    {
      myReturn = R_SQUASHED_ORTHOGONAL;
      RESCUEINT64 orthogonalCount = 0;
/*
  My definition of SQUASHED_ORTHOGONAL is pretty loose.  It is that 50% of the
  nodes are xy orthogonal and all of the ones that are not are around the edge
  such that I can reach the edge by walking in a straight line without passing
  over an xy orthogonal node.
*/
      RESCUEINT64 rowLow = grid->Axis(0)->LowBound64();
      RESCUEINT64 rowHigh = rowLow + grid->Axis(0)->Count64() - 1;
      RESCUEINT64 columnLow = grid->Axis(1)->LowBound64();
      RESCUEINT64 columnHigh = columnLow + grid->Axis(1)->Count64() - 1;

      RESCUEINT64 row, column;
      for (row = rowLow + 1; row < rowHigh && myReturn == R_SQUASHED_ORTHOGONAL; row++)
      {
        for (column = columnLow + 1; column < columnHigh && myReturn == R_SQUASHED_ORTHOGONAL; column++)
        {
          RescueVertexType vType = VertexIs(row, column);
          if (vType == R_COORDINATE_LINE || vType == R_COORDINATE_POLYLINE)
          {
            RESCUEBOOL found = FALSE;
            RESCUEINT64 tryLoop;
            for (tryLoop = 0; tryLoop < 4 && found == FALSE; tryLoop++)
            {
              RESCUEINT64 tryRow = row;
              RESCUEINT64 tryColumn = column;
              RESCUEBOOL failed = FALSE;
              switch(tryLoop)
              {
              case 0:
                tryRow--;
                while (failed == FALSE && tryRow >= rowLow)
                {
                  if (VertexIs(tryRow, tryColumn) < R_COORDINATE_LINE)
                  {
                    failed = TRUE;
                  }
                  tryRow--;
                }
                break;
              case 1:
                tryRow++;
                while (failed == FALSE && tryRow < rowHigh)
                {
                  if (VertexIs(tryRow, tryColumn) < R_COORDINATE_LINE)
                  {
                    failed = TRUE;
                  }
                  tryRow++;
                }
                break;
              case 2:
                tryColumn--;
                while (failed == FALSE && tryColumn >= columnLow)
                {
                  if (VertexIs(tryRow, tryColumn) < R_COORDINATE_LINE)
                  {
                    failed = TRUE;
                  }
                  tryColumn--;
                }
                break;
              case 3:
                tryColumn++;
                while (failed == FALSE && tryColumn < columnHigh)
                {
                  if (VertexIs(tryRow, tryColumn) < R_COORDINATE_LINE)
                  {
                    failed = TRUE;
                  }
                  tryColumn++;
                }
                break;
              }
              if (failed == FALSE)
              {
                found = TRUE;
              }
            }
            if (found == FALSE)
            {
              myReturn = R_CONFORMABLE;
            }
          }
          else
          {
            orthogonalCount++;
          }
/*
  Fairly simple algorithm assumes I should be able to step in a straight line to the
  edge.  I could make an algorithm which would step in any path to the edge, but it
  would be much more complicated.  I would have to keep a list of all paths I had
  tried.
*/
        }
      }
      if (myReturn == R_SQUASHED_ORTHOGONAL)
      {
        if (orthogonalCount > 0)
        {
          if ((grid->Axis(0)->Count64() * grid->Axis(1)->Count64()) / orthogonalCount > 2)
          {
            myReturn = R_CONFORMABLE;
          }
        }
      }
    }
    return myReturn;
  }
}

RescueGeometry::RescueVertexType RescueGeometry::VertexIs(RESCUEINT64 i, RESCUEINT64 j)
{
 RescueGeometryObject **object = VertexAddress(i, j);
  if (object == 0)
  {
    throw "Off the grid";
  }
 if (*object == 0)
 {
  return R_EQUAL_AXIS;
 }
 else
 {
  return (*object)->VertexIs();
 }
}

void RescueGeometry::SetCoordinateLine(RESCUEINT64 i, RESCUEINT64 j, 
                                       RESCUEFLOAT xTop, RESCUEFLOAT yTop, RESCUEFLOAT zTop,
                                      RESCUEFLOAT xBottom, RESCUEFLOAT yBottom, RESCUEFLOAT zBottom,
                            RESCUEFLOAT *middleZvalues)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = VertexAddress(i, j);
  if (object == 0)
  {
    throw "Off the grid";
  }
 if (*object != 0)
 {
    delete (*object);
  }
  *object = new RescueCoordinateLine(this,
                                     xTop, yTop, zTop, xBottom, 
                                     yBottom, zBottom,
                                     middleZvalues);
  if (vertexIs == R_EQUAL_AXIS
  ||  vertexIs == R_UNEQUAL_AXIS)
  {
    vertexIs = R_COORDINATE_LINE;
  }
}

RescueGeometryObject **RescueGeometry::PromoteToSplitLine(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner)
{
 RescueGeometryObject **object = VertexAddress(i, j, corner);
  if (object == 0)
  {
    throw "Off the grid";
  }
  if (*object == 0)
  {
    RESCUEINT64 iPrime, jPrime, kPrime;

    CellToNode(i, j, k, corner, iPrime, jPrime, kPrime);
    *object = new RescueSplitLine(this, grid->Axis(0)->ValueAt(iPrime),
                                        grid->Axis(1)->ValueAt(jPrime),
                                        grid->Axis(2)->Step(),
                                        grid->Axis(2)->Origin());
  }
  else if ((*object)->VertexIs() != R_SPLIT_LINE)
  {
    RESCUEINT64 iPrime, jPrime, kPrime;

    CellToNode(i, j, k, corner, iPrime, jPrime, kPrime);
    RescueSplitLine *newObject = new RescueSplitLine(this, iPrime, jPrime, *object);
    delete *object;
    *object = newObject;
  }
  vertexIs = R_SPLIT_LINE;
  return object; 
}

void RescueGeometry::SetCornerNode(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = PromoteToSplitLine(i, j, k, corner);
  ((RescueSplitLine *) (*object))->SetCornerNode(this, i, j, k, corner, x, y, z);
}

void RescueGeometry::SetXValue(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, RESCUEFLOAT x)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = PromoteToSplitLine(i, j, k, corner);
  ((RescueSplitLine *) (*object))->SetXValue(this, i, j, k, corner, x);
}

void RescueGeometry::SetYValue(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, RESCUEFLOAT y)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = PromoteToSplitLine(i, j, k, corner);
  ((RescueSplitLine *) (*object))->SetYValue(this, i, j, k, corner, y);
}

void RescueGeometry::SetZValue(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, RESCUEFLOAT z)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = PromoteToSplitLine(i, j, k, corner);
  ((RescueSplitLine *) (*object))->SetZValue(this, i, j, k, corner, z);
}

void RescueGeometry::SetCoordinatePolyLine(RESCUEINT64 i, RESCUEINT64 j, RESCUEFLOAT *values)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = VertexAddress(i, j);
  if (object == 0)
  {
    throw "Off the grid";
  }
 if (*object != 0)
 {
    delete (*object);
  }
  *object = new RescueCoordinatePolyLine(this, values);
  if (vertexIs < R_COORDINATE_POLYLINE)
  {
    vertexIs = R_COORDINATE_POLYLINE;
  }
}

RescuePillar *RescueGeometry::DemandPillar(RESCUEINT64 i, RESCUEINT64 j)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = VertexAddress(i, j);
  if (object == 0)
  {
    throw "Off the grid";
  }
  if (*object != 0)
  {
    if ((*object)->VertexIs() != R_SPLINE_LINE)
    {
      delete (*object);
      *object = 0;
    }
  }
  if (*object == 0)
  {
    *object = new RescuePillar();
  }
  if (vertexIs < R_SPLINE_LINE && vertexIs != R_SPLIT_LINE)
  {
    vertexIs = R_SPLINE_LINE;
  }
  return (RescuePillar *) *object;
}

void RescueGeometry::SetZValue(RESCUEFLOAT *value)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
  RESCUEINT64 iCount = grid->Axis(0)->Count64();
  RESCUEINT64 jCount = grid->Axis(1)->Count64();
  RESCUEINT64 klayers = grid->Axis(2)->Count64();
  RESCUEINT64 iLoop;
  RESCUEINT64 jLoop;
  RESCUEINT64 kLoop;
  RCH3DArray array = RCH3DArray(iCount, jCount, klayers, value);
/*
  This makes a control structure for the array so it is easy to handle.
*/
  for (iLoop = 0; iLoop < iCount; iLoop++)
  {
    for (jLoop = 0; jLoop < jCount; jLoop++)
    {
      RESCUEFLOAT *stackAddress = new RESCUEFLOAT[(size_t) klayers];
      for (kLoop = 0; kLoop < klayers; kLoop++)
      {
        stackAddress[kLoop] = array.Value(iLoop, jLoop, kLoop);
      }
      SetZValue(iLoop, jLoop, stackAddress);
    }
  }
}

void RescueGeometry::SetZValue(RESCUEINT64 i, RESCUEINT64 j, RESCUEFLOAT *value)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = VertexAddress(i, j);
  if (object == 0)
  {
    throw "Off the grid";
  }
 if (*object == 0)
 {
    *object = new RescueZStack(this, value);
    if (vertexIs == R_EQUAL_AXIS)
    {
      vertexIs = R_UNEQUAL_AXIS;
    }
 }
 else
  {
    (*object)->ZStack(this, value);
    delete[] value;
  }
}

void RescueGeometry::SetZValue(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT value)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    throw "Illegal attempt to set geometry of implicit LGR";
  }
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to set geometry of non-CPG LGR";
  }
 RescueGeometryObject **object = VertexAddress(i, j);
  if (object == 0)
  {
    throw "Off the grid";
  }
 if (*object == 0)
 {
    *object = new RescueZStack(this);
    (*object)->ZValue(k, value);
    if (vertexIs == R_EQUAL_AXIS)
    {
      vertexIs = R_UNEQUAL_AXIS;
    }
 }
 else
  {
    (*object)->ZValue(k, value);
  }
}

RESCUEBOOL RescueGeometry::ProportionalValueAt(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT &z)
{
  RESCUEBOOL myReturn = TRUE;
  RESCUEINT64 nKLayers = grid->Axis(2)->Count64() - 1;
  RESCUEFLOAT top = referenceSurface1->NthIJSurface(0)->ZValueAt(i, j);
  RESCUEFLOAT bottom = referenceSurface2->NthIJSurface(0)->ZValueAt(i, j);
  if (top > bottom)
  {
    top -= offset1;
    bottom += offset2;
    RESCUEFLOAT localThickness = (top - bottom) / (RESCUEFLOAT) nKLayers;
    z = bottom + (localThickness * (RESCUEFLOAT) k);
  }
  else
  {
    top += offset1;
    bottom -= offset2;
    RESCUEFLOAT localThickness = (bottom - top) / (RESCUEFLOAT) nKLayers;
    z = top + (localThickness * (RESCUEFLOAT) k);
  }
  if (top == referenceSurface1->NthIJSurface(0)->Geometry()->MissingValue())
  {
    myReturn = FALSE;
  }
  else if (bottom == referenceSurface2->NthIJSurface(0)->Geometry()->MissingValue())
  {
    myReturn = FALSE;
  }
  return myReturn;
}

RESCUEBOOL RescueGeometry::ReferencedValueAt(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT &z)
{
  RESCUEBOOL myReturn = TRUE;
  RescueIJSurface *refSurface = referenceSurface1->NthIJSurface(0);
  RESCUEFLOAT zAtNode = refSurface->ZValueAt(i, j);
  if (onOffLap == R_ONLAP)
  {
    RESCUEINT64 lastKLayer = grid->Axis(2)->Count64() - 1;
    z = zAtNode + offset1 - (thickness * (RESCUEFLOAT) (lastKLayer - k));
  }
  else
  {
    z = zAtNode + offset1 + (thickness * (RESCUEFLOAT) k);
  }
  if (zAtNode == refSurface->Geometry()->MissingValue())
  {
    myReturn = FALSE;
  }
  return myReturn;
}

RESCUEFLOAT RescueGeometry::ZValue(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k)
{
  RESCUEFLOAT myReturn;
  RESCUEBOOL state = Values(i, j, k, myReturn);
  if (state == FALSE)
  {
    myReturn = FLT_MIN;
  }
  return myReturn;
}

RESCUEBOOL RescueGeometry::Values(RESCUEINT32 i, RESCUEINT32 j, RESCUEINT32 k, RESCUEFLOAT &z)
{
  return Values((RESCUEINT64) i, (RESCUEINT64) j, (RESCUEINT64) k, z);
}

RESCUEBOOL RescueGeometry::Values(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT &z)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    RESCUEFLOAT x, y;
    return Values(i, j, k, x, y, z);
  }
  else if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  else
  {
   RescueGeometryObject **object = VertexAddress(i, j);
    if (object == 0)
    {
      throw "Off the grid";
    }
   if (*object == 0)
   {
      RESCUEBOOL myReturn = TRUE;
      if (IsProportional())
      {
        myReturn = ProportionalValueAt(i, j, k, z);
      }
      else if (IsReferenced())
      {
        myReturn = ReferencedValueAt(i, j, k, z);
      }
      else
      {
      z = Grid()->Axis(2)->ValueAt(k);
      }
    return myReturn;
   }
   else if ((*object)->VertexIs() == R_SPLIT_LINE)
   {
    return FALSE;
   }
    else if ((*object)->VertexIs() == R_SPLINE_LINE)
    {
      if (((RescuePillar *) *object)->dirCount != 1)
      {
        return FALSE;
      }
      else
      {
      (*object)->Values(k, z);
        return TRUE;
      }
    }
   else
   {
    (*object)->Values(k, z);
    return TRUE;
   }
  }
}

RESCUEFLOAT *RescueGeometry::UVWNodeValues(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k)
{
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  RESCUEFLOAT *myReturn = 0;
  RESCUEFLOAT x, y, z;
  RESCUEBOOL state = Values(i, j, k, x, y, z);
  if (state == TRUE)
  {
    myReturn = new RESCUEFLOAT[3];
    myReturn[0] = x;
    myReturn[1] = y;
    myReturn[2] = z;
  }
  return myReturn;
}

RESCUEFLOAT *RescueGeometry::UVWNodeValues2(RESCUEDOUBLE i, RESCUEDOUBLE j, RESCUEDOUBLE k)
{
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  RESCUEFLOAT *myReturn = 0;
  RESCUEFLOAT x, y, z;
  RESCUEBOOL state = Values(i, j, k, x, y, z);
  if (state == TRUE)
  {
    myReturn = new RESCUEFLOAT[3];
    myReturn[0] = x;
    myReturn[1] = y;
    myReturn[2] = z;
  }
  return myReturn;
}

RESCUEFLOAT *RescueGeometry::NodeValues(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k)
{
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  RESCUEFLOAT *myReturn = 0;
  RESCUEFLOAT x, y, z;
  RESCUEBOOL state = Values(i, j, k, x, y, z);
  if (state == TRUE)
  {
    myReturn = new RESCUEFLOAT[3];
    myReturn[0] = x;
    myReturn[1] = y;
    myReturn[2] = z;
  }
  return myReturn;
}

void RescueGeometry::UVWValues(RESCUEDOUBLE i, RESCUEDOUBLE j, RESCUEDOUBLE k, RESCUEDOUBLE &uTerm, RESCUEDOUBLE &vTerm, RESCUEDOUBLE &wTerm)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    if (parentGeometry == 0)
    {
      throw "Failed to find parent geometry";
    }
    else
    {
      if (parentGeometry->IsLoaded() == false)
      {
        parentGeometry->Load();
      }
      RescueGridAxis *xAxis = grid->Axis(0);
      RescueGridAxis *yAxis = grid->Axis(1);
      RescueGridAxis *zAxis = grid->Axis(2);
      switch (lgrType)
      {
      case R_RULE_BASED:
        {
          if (xAxis->Count64() > 1)
          {
            uTerm = (RESCUEDOUBLE) parentILowBound + (((RESCUEDOUBLE) (parentICount - 1) / (RESCUEDOUBLE) (xAxis->Count64() - 1)) * i);
          }
          else
          {
            uTerm = (RESCUEDOUBLE) parentILowBound;
          }
          if (yAxis->Count64() > 1)
          {
            vTerm = (RESCUEDOUBLE) parentJLowBound + (((RESCUEDOUBLE) (parentJCount - 1) / (RESCUEDOUBLE) (yAxis->Count64() - 1)) * j);
          }
          else
          {
            vTerm = (RESCUEDOUBLE) parentJLowBound;
          }
          if (zAxis->Count64() > 1)
          {
            wTerm = (RESCUEDOUBLE) parentKLowBound + (((RESCUEDOUBLE) (parentKCount - 1) / (RESCUEDOUBLE) (zAxis->Count64() - 1)) * k);
          }
          else
          {
            wTerm = (RESCUEDOUBLE) parentKLowBound;
          }
        }
        break;
      case R_RULE_BASED_NOSPAN:
      case R_WEIGHT_BASED:
      case R_WEIGHT_BASED_NOSPAN:
        {
          RESCUEINT64 iLow = (RESCUEINT64) floor(i);
          RESCUEINT64 iHigh = (RESCUEINT64) ceil(i);
          RESCUEDOUBLE iFrac = i - (RESCUEDOUBLE) iLow;
          if (iLow < 0)
          {
            iLow = 0;
          }
          else if (iLow >= xWeightLength)
          {
            iLow = xWeightLength - 1;
          }
          if (iHigh < 0)
          {
            iHigh = 0;
          }
          else if (iHigh >= xWeightLength)
          {
            iHigh = xWeightLength - 1;
          }
          //Gilles Bourgault
          //uTerm = (xWeight[iLow] * (1.0 - iFrac)) 
          //      + (xWeight[iHigh] * iFrac);
          uTerm = xWeight[iLow] + (xWeight[iHigh]-xWeight[iLow]) * iFrac;
        }
        {
          RESCUEINT64 jLow = (RESCUEINT64) floor(j);
          RESCUEINT64 jHigh = (RESCUEINT64) ceil(j);
          RESCUEDOUBLE jFrac = j - (RESCUEDOUBLE) jLow;
          if (jLow < 0)
          {
            jLow = 0;
          }
          else if (jLow >= yWeightLength)
          {
            jLow = yWeightLength - 1;
          }
          if (jHigh < 0)
          {
            jHigh = 0;
          }
          else if (jHigh >= yWeightLength)
          {
            jHigh = yWeightLength - 1;
          }
          //Gilles Bourgault
          //vTerm = (yWeight[jLow] * (1.0 - jFrac)) 
          //      + (yWeight[jHigh] * jFrac);
          vTerm = yWeight[jLow] + (yWeight[jHigh]-yWeight[jLow]) * jFrac;
        }
        {
          RESCUEINT64 kLow = (RESCUEINT64) floor(k);
          RESCUEINT64 kHigh = (RESCUEINT64) ceil(k);
          RESCUEDOUBLE kFrac = k - (RESCUEDOUBLE) kLow;
          if (kLow < 0)
          {
            kLow = 0;
          }
          else if (kLow >= zWeightLength)
          {
            kLow = zWeightLength - 1;
          }
          if (kHigh < 0)
          {
            kHigh = 0;
          }
          else if (kHigh >= zWeightLength)
          {
            kHigh = zWeightLength - 1;
          }
          //Gilles Bourgault
          //wTerm = (zWeight[kLow] * (1.0 - kFrac)) 
          //      + (zWeight[kHigh] * kFrac);
          wTerm = zWeight[kLow] + (zWeight[kHigh]-zWeight[kLow]) * kFrac;
        }
        break;
      default:
 break;   // Quiet warning
      }
    }
  }
  else
  {
    throw "UVWValues is only for rule and weight based LGRs";
  }
}

void RescueGeometry::UVWValues(RESCUEINT32 i, RESCUEINT32 j, RESCUEINT32 k, 
                               RESCUEDOUBLE &uTerm, RESCUEDOUBLE &vTerm, RESCUEDOUBLE &wTerm)
{
  UVWValues((RESCUEINT64) i, (RESCUEINT64) j, (RESCUEINT64) k, uTerm, vTerm, wTerm);
}

void RescueGeometry::UVWValues(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, 
                               RESCUEDOUBLE &uTerm, RESCUEDOUBLE &vTerm, RESCUEDOUBLE &wTerm)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    if (parentGeometry == 0)
    {
      throw "Failed to find parent geometry";
    }
    else
    {
      if (parentGeometry->IsLoaded() == false)
      {
        parentGeometry->Load();
      }
      RescueGridAxis *xAxis = grid->Axis(0);
      RescueGridAxis *yAxis = grid->Axis(1);
      RescueGridAxis *zAxis = grid->Axis(2);
      switch (lgrType)
      {
      case R_RULE_BASED:
        {
          if (xAxis->Count64() > 1)
          {
            uTerm = (RESCUEDOUBLE) parentILowBound + (((RESCUEDOUBLE) (parentICount - 1) / (RESCUEDOUBLE) (xAxis->Count64() - 1)) * (RESCUEDOUBLE) i);
          }
          else
          {
            uTerm = (RESCUEDOUBLE) parentILowBound;
          }
          if (yAxis->Count64() > 1)
          {
            vTerm = (RESCUEDOUBLE) parentJLowBound + (((RESCUEDOUBLE) (parentJCount - 1) / (RESCUEDOUBLE) (yAxis->Count64() - 1)) * (RESCUEDOUBLE) j);
          }
          else
          {
            vTerm = (RESCUEDOUBLE) parentJLowBound;
          }
          if (zAxis->Count64() > 1)
          {
            wTerm = (RESCUEDOUBLE) parentKLowBound + (((RESCUEDOUBLE) (parentKCount - 1) / (RESCUEDOUBLE) (zAxis->Count64() - 1)) * (RESCUEDOUBLE) k);
          }
          else
          {
            wTerm = (RESCUEDOUBLE) parentKLowBound;
          }
        }
        break;
      case R_RULE_BASED_NOSPAN:
      case R_WEIGHT_BASED:
      case R_WEIGHT_BASED_NOSPAN:
        {
          if (i < 0)
          {
            i = 0;
          }
          else if (i >= xWeightLength)
          {
            i = xWeightLength - 1;
          }
          uTerm = xWeight[i];
          if (j < 0)
          {
            j = 0;
          }
          else if (j >= yWeightLength)
          {
            j = yWeightLength - 1;
          }
          vTerm = yWeight[j];
          if (k < 0)
          {
            k = 0;
          }
          else if (k >= zWeightLength)
          {
            k = zWeightLength - 1;
          }
          wTerm = zWeight[k];
        }
        break;
      default:
 break;   // Quiet warning
      }
    }
  }
  else
  {
    throw "UVWValues is only for rule and weight based LGRs";
  }
}

RESCUEBOOL RescueGeometry::Values(RESCUEDOUBLE u, RESCUEDOUBLE v, RESCUEDOUBLE w, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    RESCUEDOUBLE uTerm, vTerm, wTerm;
    UVWValues(u, v, w, uTerm, vTerm, wTerm);
    return parentGeometry->Values(uTerm, vTerm, wTerm, x, y, z);
  }
  else if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  else
  {
#if 0
    RESCUEINT64 iLow = (RESCUEINT64) floor(u);
    RESCUEDOUBLE iFrac = u - (RESCUEDOUBLE) iLow;
    RESCUEDOUBLE iFracA = 1.0 - iFrac;
    RESCUEINT64 jLow = (RESCUEINT64) floor(v);
    RESCUEDOUBLE jFrac = v - (RESCUEDOUBLE) jLow;
    RESCUEDOUBLE jFracA = 1.0 - jFrac;
    RESCUEINT64 kLow = (RESCUEINT64) floor(w);
    RESCUEDOUBLE kFrac = w - (RESCUEDOUBLE) kLow;
    RESCUEDOUBLE kFracA = 1.0 - kFrac;
    RESCUEDOUBLE uTerm = 0.0;
    RESCUEDOUBLE uBase = 0.0;
    RESCUEDOUBLE vTerm = 0.0;
    RESCUEDOUBLE vBase = 0.0;
    RESCUEDOUBLE wTerm = 0.0;
    RESCUEDOUBLE wBase = 0.0;
    RESCUEFLOAT *corners[8];
    RESCUEINT64 cLoop;
    for (cLoop = 0; cLoop < 8; cLoop++)
    {
      bool needCorner = true;
      switch (cLoop)
      {
      case 1:
        if (iFrac == 0.0)
        {
          needCorner = false;
        }
        break;
      case 2:
      case 3:
        if (jFrac == 0.0)
        {
          needCorner = false;
        }
        break;
      case 4:
        if (kFrac == 0.0)
        {
          needCorner = false;
        }
        break;
      case 5:
        if (iFrac == 0.0 || kFrac == 0.0)
        {
          needCorner = false;
        }
        break;
      case 6:
        if (iFrac == 0.0 || jFrac == 0.0 || kFrac == 0.0)
        {
          needCorner = false;
        }
        break;
      case 7:
        if (jFrac == 0.0 || kFrac == 0.0)
        {
          needCorner = false;
        }
      }
      if (needCorner)
      {
        corners[cLoop] = CornerValues(iLow, jLow, kLow, cLoop);
      }
      else
      {
        corners[cLoop] = 0;
      }
    }
    for (cLoop = 0; cLoop < 4; cLoop++)
    {
      switch (cLoop)
      {
      case 0:
        if (corners[0] != 0)
        {
          if (corners[1] != 0)
          {
            uTerm += (corners[1][0] - corners[0][0]) * jFracA * kFracA * iFrac; // Planes J1-K1
          }
          uBase += corners[0][0] * jFracA * kFracA;
        }

        if (corners[0] != 0)
        {
          if (corners[3] != 0)
          {
            vTerm += (corners[3][1] - corners[0][1]) * iFracA * kFracA * jFrac; // Planes I1-K1
          }
          vBase += corners[0][1] * iFracA * kFracA;
        }

        if (corners[0] != 0)
        {
          if (corners[4] != 0)
          {
            wTerm += (corners[4][2] - corners[0][2]) * iFracA * jFracA * kFrac; // Planes I1-J1
          }
          wBase += corners[0][2] * iFracA * jFracA;
        }
        break;
      case 1:
        if (corners[4] != 0)
        {
          if (corners[5] != 0)
          {
            uTerm += (corners[5][0] - corners[4][0]) * jFracA * kFrac * iFrac; // Planes J1-K2
          }
          uBase += corners[4][0] * jFracA * kFrac;
        }

        if (corners[4] != 0)
        {
          if (corners[7] != 0)
          {
            vTerm += (corners[7][1] - corners[4][1]) * iFracA * kFrac * jFrac; // Planes I1-K2
          }
          vBase += corners[4][1] * iFracA * kFrac;
        }

        if (corners[3] != 0)
        {
          if (corners[7] != 0)
          {
            wTerm += (corners[7][2] - corners[3][2]) * iFracA * jFrac * kFrac; // Planes I1-J2
          }
          wBase += corners[3][2] * iFracA * jFrac;
        }
        break;
      case 2:
        if (corners[2] != 0)
        {
          if (corners[3] != 0)
          {
            uTerm += (corners[3][0] - corners[2][0]) * jFrac * kFracA * iFrac; // Planes J2-K1
          }
          uBase += corners[2][0] * jFrac * kFracA;
        }

        if (corners[1] != 0)
        {
          if (corners[2] != 0)
          {
            vTerm += (corners[2][1] - corners[1][1]) * iFrac * kFracA * jFrac; // Planes I2-K1
          }
          vBase += corners[1][1] * iFrac * kFracA;
        }

        if (corners[1] != 0)
        {
          if (corners[5] != 0)
          {
            wTerm += (corners[5][2] - corners[1][2]) * iFrac * jFracA * kFrac; // Planes I2-J1
          }
          wBase += corners[1][2] * iFrac * jFracA;
        }
        break;
      case 3:
        if (corners[7] != 0)
        {
          if (corners[6] != 0)
          {
            uTerm += (corners[6][0] - corners[7][0]) * jFrac * kFrac * iFrac;  // Planes J2-K2
          }
          uBase += corners[7][0] * jFrac * kFrac;
        }

        if (corners[5] != 0)
        {
          if (corners[6] != 0)
          {
            vTerm += (corners[6][1] - corners[5][1]) * iFrac * kFrac * jFrac; // Planes I2-K2
          }
          vBase += corners[5][1] * iFrac * kFrac;
        }
        
        if (corners[2] != 0)
        {
          if (corners[6] != 0)
          {
            wTerm += (corners[6][2] - corners[2][2]) * iFrac * jFrac * kFrac; // Planes I2-J2
          }
          wBase += corners[2][2] * iFrac * jFrac;
        }
        break;
      }
    }
    x = (RESCUEFLOAT) (uBase + uTerm);
    y = (RESCUEFLOAT) (vBase + vTerm);
    z = (RESCUEFLOAT) (wBase + wTerm);
    for (cLoop = 0; cLoop < 8; cLoop++)
    {
      if (corners[cLoop] != 0)
      {
        delete []corners[cLoop];
      }
    }
#else
    bool dataIsMissing = false;

    // Make sure that iLow doesn't exceed count in the i-direction.
    // If it does, then iFrac will be >= 1.0.
    RESCUEINT64 iLow = (RESCUEINT64) floor(u);
    while (iLow >= grid->Axis(0)->Count64() - 1)
      iLow--;
    RESCUEDOUBLE iFrac = u - (RESCUEDOUBLE) iLow;

    // Make sure that jLow doesn't exceed count in the j-direction.
    // If it does, then jFrac will be >= 1.0.
    RESCUEINT64 jLow = (RESCUEINT64) floor(v);
    while (jLow >= grid->Axis(1)->Count64() - 1)
      jLow--;
    RESCUEDOUBLE jFrac = v - (RESCUEDOUBLE) jLow;

    // Make sure that kLow doesn't exceed count in the k-direction.
    // If it does, then kFrac will be >= 1.0.
    RESCUEINT64 kLow = (RESCUEINT64) floor(w);
    while (kLow >= grid->Axis(2)->Count64() - 1)
      kLow--;
    RESCUEDOUBLE kFrac = w - (RESCUEDOUBLE) kLow;

    RESCUEFLOAT *corners[8];
    RESCUEINT64 cLoop;
    for (cLoop = 0; cLoop < 8; cLoop++)
    {
      try
      {
        corners[cLoop] = CornerValues(iLow, jLow, kLow, cLoop);
      }
      catch (...)
      {
        corners[cLoop] = 0;
      }
    }
    for (cLoop = 0; cLoop < 8; cLoop++)
    {
      if (corners[cLoop] == 0)
      {
        RESCUEINT64 pref = 0;
        switch (cLoop)
        {
        case 0:
          pref = 1;
          break;
        case 1:
          pref = 0;
          break;
        case 2:
          pref = 3;
          break;
        case 3:
          pref = 2;
          break;
        case 4:
          pref = 5;
          break;
        case 5:
          pref = 4;
          break;
        case 6:
          pref = 7;
          break;
        case 7:
          pref = 8;
          break;
        }
        if (corners[pref] == 0)
        {
          RESCUEINT64 pLoop;
          for (pLoop = 0; pLoop < 8 && corners[pref] == 0; pLoop++)
          {
            if (corners[pLoop] != 0)
            {
              pref = pLoop;
            }
          }
        }
        if (corners[pref] != 0)
        {
          corners[cLoop] = new RESCUEFLOAT[3];
          corners[cLoop][0] = corners[pref][0];
          corners[cLoop][1] = corners[pref][1];
          corners[cLoop][2] = corners[pref][2];
        }
      }
    }
/*
  Around the edges we can end up off the grid when we ask for all corners.
  In a valid situation, this happens only when we don't actually need those
  corners anyway.
*/
    for (cLoop = 0; cLoop < 8; cLoop++)
    {
      if (corners[cLoop][0] == missingValue
      ||  corners[cLoop][1] == missingValue
      ||  corners[cLoop][2] == missingValue)
      {
        if (iFrac == 0.0
        && (cLoop == 1 || cLoop == 2 || cLoop == 5 || cLoop == 6))
        {
          RESCUEINT64 alt = 0;
          switch (cLoop)
          {
          case 2:
            alt = 3;
            break;
          case 5:
            alt = 4;
            break;
          case 6:
            alt = 7;
            break;
          }
          corners[cLoop][0] = corners[alt][0];
          corners[cLoop][1] = corners[alt][1];
          corners[cLoop][2] = corners[alt][2];
        }
        else
        {
          dataIsMissing = true;
        }
      }
    }
/*
  Look for critical missing data and try to make up for missing data if it
  isn't critical.
*/
    if (dataIsMissing)
    {
      x = missingValue;
      y = missingValue;
      z = missingValue;
    }
/*
  Severest possible interpretation.  With more work we should be able to deal
  with some missing data in corners 1, 2, 5, or 6 if iFrac, jFrac, and kFrac
  are zero.
*/
    else
    {
     //Notations used
      //u01 : interpolated u position on edge connecting corners 0-1
      //u0123: interpolated u position between interpolated u positions on edge 0-1 and edge 3-2
      //u4567: interpolated u position between interpolated u positions on edge 4-5 and edge 7-6
      RESCUEDOUBLE u01, u32, u45, u76;
      RESCUEDOUBLE v01, v32, v45, v76;
      RESCUEDOUBLE w01, w32, w45, w76;
      RESCUEDOUBLE u0123, v0123, w0123;
      RESCUEDOUBLE u4567, v4567, w4567;
      //interpolating u,v,w on edge 0-1 using I fraction
      u01 = corners[0][0] + (corners[1][0] - corners[0][0]) * iFrac;
      v01 = corners[0][1] + (corners[1][1] - corners[0][1]) * iFrac;
      w01 = corners[0][2] + (corners[1][2] - corners[0][2]) * iFrac;
   
      //interpolating u,v,w on edge 3-2 using I fraction
      u32 = corners[3][0] + (corners[2][0] - corners[3][0]) * iFrac;
      v32 = corners[3][1] + (corners[2][1] - corners[3][1]) * iFrac;
      w32 = corners[3][2] + (corners[2][2] - corners[3][2]) * iFrac;
      //interpolating u,v,w on edge 5-4 using I fraction
      u45 = corners[4][0] + (corners[5][0] - corners[4][0]) * iFrac;
      v45 = corners[4][1] + (corners[5][1] - corners[4][1]) * iFrac;
      w45 = corners[4][2] + (corners[5][2] - corners[4][2]) * iFrac;
      //interpolating u,v,w on edge 7-6 using I fraction
      u76 = corners[7][0] + (corners[6][0] - corners[7][0]) * iFrac;
      v76 = corners[7][1] + (corners[6][1] - corners[7][1]) * iFrac;
      w76 = corners[7][2] + (corners[6][2] - corners[7][2]) * iFrac;
      //interpolating u,v,w between edge 0-1 and edge 3-2 using J fraction
      u0123 = u01 + (u32 - u01) * jFrac;
      v0123 = v01 + (v32 - v01) * jFrac;
      w0123 = w01 + (w32 - w01) * jFrac;
      //interpolating u,v,w between edge 4-5 and edge 7-6 using J fraction
      u4567 = u45 + (u76 - u45) * jFrac;
      v4567 = v45 + (v76 - v45) * jFrac;
      w4567 = w45 + (w76 - w45) * jFrac;
      //interpolating between bottom 0123 and top 4567 using K fraction
      x = RESCUEFLOAT(u0123 + (u4567 - u0123) * kFrac);
      y = RESCUEFLOAT(v0123 + (v4567 - v0123) * kFrac);
      z = RESCUEFLOAT(w0123 + (w4567 - w0123) * kFrac);
    }
/*
  The above algorithm contributed by Gilles Bourgault of CMG.
*/
    for (cLoop = 0; cLoop < 8; cLoop++)
    {
      if (corners[cLoop] != 0)
      {
        delete []corners[cLoop];
      }
    }
#endif
  }
  return true;
}

RESCUEBOOL RescueGeometry::Values(RESCUEINT32 i, RESCUEINT32 j, RESCUEINT32 k, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  return Values((RESCUEINT64) i, (RESCUEINT64) j, (RESCUEINT64) k, x, y, z);
}

RESCUEBOOL RescueGeometry::Values(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
    RESCUEDOUBLE uTerm, vTerm, wTerm;
    UVWValues(i, j, k, uTerm, vTerm, wTerm);
    return parentGeometry->Values(uTerm, vTerm, wTerm, x, y, z);
  }
  else if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  else
  {
   RescueGeometryObject **object = VertexAddress(i, j);
    if (object == 0)
    {
      throw "Off the grid";
    }
   if (*object == 0)
   {
      RESCUEBOOL myReturn = TRUE;
    x = Grid()->Axis(0)->ValueAt(i);
    y = Grid()->Axis(1)->ValueAt(j);
      if (IsProportional())
      {
        myReturn = ProportionalValueAt(i, j, k, z);
      }
      else if (IsReferenced())
      {
        myReturn = ReferencedValueAt(i, j, k, z);
      }
      else
      {
      z = Grid()->Axis(2)->ValueAt(k);
      }
    return myReturn;
   }
   else if ((*object)->VertexIs() == R_SPLIT_LINE)
   {
    return FALSE;
   }
    else if ((*object)->VertexIs() == R_SPLINE_LINE)
    {
      if (((RescuePillar *) *object)->dirCount != 1)
      {
        return FALSE;
      }
      else
      {
      (*object)->Values(this, i, j, k, x, y, z);
        return TRUE;
      }
    }
   else
   {
    (*object)->Values(this, i, j, k, x, y, z);
      return TRUE;
   }
  }
}

RESCUEFLOAT *RescueGeometry::CornerValues(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner)
{
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  RESCUEFLOAT *myReturn = new RESCUEFLOAT[3];
  Values(i, j, k, corner, myReturn[0], myReturn[1], myReturn[2]);
  return myReturn;
}

void RescueGeometry::Values(RESCUEINT32 i, RESCUEINT32 j, RESCUEINT32 k, RESCUEINT32 corner, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  Values((RESCUEINT64) i, (RESCUEINT64) j, (RESCUEINT64) k, (RESCUEINT64) corner, x, y, z);
}

void RescueGeometry::Values(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  if (lgrType == R_RULE_BASED        || lgrType == R_WEIGHT_BASED
  ||  lgrType == R_RULE_BASED_NOSPAN || lgrType == R_WEIGHT_BASED_NOSPAN)
  {
  CellToNode(i, j, k, corner, i, j, k);
    Values(i, j, k, x, y, z);
/*
  LGR's don't work with split nodes because the position of the node is
  ambiguous. Therefore, we basically ignore the issue of split nodes when
  figuring out LGR positions.
*/
  }
  else
  {
    if (lgrGridType != R_STRUCTURED_CPG)
    {
      throw "Illegal attempt to read geometry of non-CPG LGR";
    }
   RescueGeometryObject **object = VertexAddress(i, j, corner);
    if (object == 0)
    {
      throw "Off the grid";
    }
   else if (*object == 0)
   {
    CellToNode(i, j, k, corner, i, j, k);
    x = Grid()->Axis(0)->ValueAt(i);
    y = Grid()->Axis(1)->ValueAt(j);
      if (IsProportional())
      {
        ProportionalValueAt(i, j, k, z);
      }
      else if (IsReferenced())
      {
        ReferencedValueAt(i, j, k, z);
      }
      else
      {
      z = Grid()->Axis(2)->ValueAt(k);
      }
   }
   else
   {
    (*object)->Values(this, i, j, k, corner, x, y, z);
   }
  }
}

RESCUEINT32 RescueGeometry::AllCornerValuesLength()
{
  RESCUEINT64 howMany = AllCornerValuesLength64();
  return (RESCUEINT32) howMany;
}

RESCUEINT32 RescueGeometry::AllCornerValuesLength(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 howMany = AllCornerValuesLength64();
  if (throwIfTooBig)
  {
    if (howMany > 2147483647 || howMany < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) howMany;
}

void RescueGeometry::LayerCornerValues(RESCUEINT64 kLayer, RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  CornerValuesPrimitive(kLayer, TRUE, buffer, offset, bufferLength);
}

void RescueGeometry::AllCornerValues(RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  CornerValuesPrimitive(0, FALSE, buffer, offset, bufferLength);
}

void RescueGeometry::CornerValuesPrimitive(RESCUEINT64 kLayer, RESCUEBOOL layerValid, 
                                             RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  RESCUEINT64 count = Grid()->Dimensions();
  RESCUEINT64 loop;
  RESCUEINT64 ni = 1;
  RESCUEINT64 nj = 1;
  RESCUEINT64 nk = 1;

  for (loop = 0; loop < count; loop++)
  {
    RescueGridAxis *axis = Grid()->Axis(loop);
    RESCUEINT64 nodes = axis->Count64();
    if (nodes > 1)
    {
      nodes--;
    }
    if (axis->AxisIndex() == RescueGridAxis::I_AXIS)
    {
      ni = nodes;
    }
    else if (axis->AxisIndex() == RescueGridAxis::J_AXIS)
    {
      nj = nodes;
    }
    else if (axis->AxisIndex() == RescueGridAxis::K_AXIS
         &&  layerValid == FALSE)
    {
      nk = nodes;
    }
  }
  RESCUEFLOAT *pos = &buffer[offset];
  RESCUEINT64 kStart = 0;
  if (layerValid)
  {
    kStart = kLayer;
    nk = kLayer + 1;
  }
  RESCUEINT64 iLoop;
  RESCUEINT64 jLoop;
  RESCUEINT64 kLoop;
  RESCUEINT64 cLoop;
  for (kLoop = kStart; kLoop < nk; kLoop++)
  {
    for (jLoop = 0; jLoop < nj; jLoop++)
    {
      for (iLoop = 0; iLoop < ni; iLoop++)
      {
/*
  Note the order of these three loops was reversed between v35 and v36.
*/
        for (cLoop = 0; cLoop < 8; cLoop++)
        {
          RESCUEFLOAT xValue, yValue, zValue;
          Values(iLoop, jLoop, kLoop, cLoop, xValue, yValue, zValue);
          if (bufferLength-- >= 0) *pos++ = xValue;
          if (bufferLength-- >= 0) *pos++ = yValue;
          if (bufferLength-- >= 0) *pos++ = zValue;
        }
      }
    }
  }
}

void RescueGeometry::RefinedCountArray(RescueGridAxis::GridNdx axis, RESCUEINT32 *buffer, RESCUEINT32 offset, RESCUEINT32 bufferLen)
{
  RESCUEINT32 *pos = &buffer[offset];
  RESCUEINT64 howMany = 0;
  RESCUEINT64 loop;
  switch (axis)
  {
  case RescueGridAxis::I_AXIS:
    howMany = parentICount;
    break;
  case RescueGridAxis::J_AXIS:
    howMany = parentJCount;
    break;
  case RescueGridAxis::K_AXIS:
    howMany = parentKCount;
    break;
  default:
    break;   // Quiet warning
  }
  for (loop = 0; loop < howMany && bufferLen > 0; loop++)
  {
    RESCUEINT64 node = WeightBasedNodesFor(axis, loop);
    if (node > 2147483647)
    {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    else
    {
      *pos++ = (RESCUEINT32) node;
    }
    bufferLen--;
  }
}

void RescueGeometry::RefinedCountArray(RescueGridAxis::GridNdx axis, RESCUEINT64 *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLen)
{
  RESCUEINT64 *pos = &buffer[offset];
  RESCUEINT64 howMany = 0;
  RESCUEINT64 loop;
  switch (axis)
  {
  case RescueGridAxis::I_AXIS:
    howMany = parentICount;
    break;
  case RescueGridAxis::J_AXIS:
    howMany = parentJCount;
    break;
  case RescueGridAxis::K_AXIS:
    howMany = parentKCount;
    break;
  default:
    break;   // Quiet warning
  }
  for (loop = 0; loop < howMany && bufferLen > 0; loop++)
  {
    *pos++ = WeightBasedNodesFor(axis, loop);
    bufferLen--;
  }
}

RESCUEINT64 RescueGeometry::WeightBasedNodesFor(RescueGridAxis::GridNdx axis, RESCUEINT64 ndx)
{
  RESCUEDOUBLE nFloor = (RESCUEDOUBLE) ndx;
  RESCUEDOUBLE nCeil = (RESCUEDOUBLE) (ndx + 1);
  RESCUEINT64 myReturn = 0;
  RESCUEINT64 loop;
  switch (axis)
  {
  case RescueGridAxis::I_AXIS:
    {
      nFloor += (RESCUEDOUBLE) parentILowBound;
      nCeil += (RESCUEDOUBLE) parentILowBound;
      for (loop = 0; loop < xWeightLength; loop++)
      {
        if (xWeight[loop] > nFloor && xWeight[loop] <= nCeil)
        {
          myReturn++;
        }
      }
    }
    break;
  case RescueGridAxis::J_AXIS:
    {
      nFloor += (RESCUEDOUBLE) parentJLowBound;
      nCeil += (RESCUEDOUBLE) parentJLowBound;
      for (loop = 0; loop < yWeightLength; loop++)
      {
        if (yWeight[loop] > nFloor && yWeight[loop] <= nCeil)
        {
          myReturn++;
        }
      }
    }
    break;
  case RescueGridAxis::K_AXIS:
    {
      nFloor += (RESCUEDOUBLE) parentKLowBound;
      nCeil += (RESCUEDOUBLE) parentKLowBound;
      for (loop = 0; loop < zWeightLength; loop++)
      {
        if (zWeight[loop] > nFloor && zWeight[loop] <= nCeil)
        {
          myReturn++;
        }
      }
    }
    break;
  default:
    break;   // Quiet warning
  }
  return myReturn;
}

void RescueGeometry::WeightFor(RescueGridAxis::GridNdx axis, RESCUEINT64 iNdx, RESCUEDOUBLE *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLen)
{
  RESCUEDOUBLE nFloor = (RESCUEDOUBLE) iNdx;
  RESCUEDOUBLE nCeil = (RESCUEDOUBLE) (iNdx + 1);
  RESCUEDOUBLE *pos = &buffer[offset];
  RESCUEINT64 loop;
  switch (axis)
  {
  case RescueGridAxis::I_AXIS:
    {
      nFloor += (RESCUEDOUBLE) parentILowBound;
      nCeil += (RESCUEDOUBLE) parentILowBound;
      RESCUEDOUBLE last = nFloor;
      for (loop = 0; loop < xWeightLength && bufferLen > 0; loop++)
      {
        if (xWeight[loop] > nFloor && xWeight[loop] <= nCeil)
        {
          *pos++ = xWeight[loop] - last;
          last = xWeight[loop];
          bufferLen--;
        }
      }
    }
    break;
  case RescueGridAxis::J_AXIS:
    {
      nFloor += (RESCUEDOUBLE) parentJLowBound;
      nCeil += (RESCUEDOUBLE) parentJLowBound;
      RESCUEDOUBLE last = nFloor;
      for (loop = 0; loop < yWeightLength && bufferLen > 0; loop++)
      {
        if (yWeight[loop] > nFloor && yWeight[loop] <= nCeil)
        {
          *pos++ = yWeight[loop] - last;
          last = yWeight[loop];
          bufferLen--;
        }
      }
    }
    break;
  case RescueGridAxis::K_AXIS:
    {
      nFloor += (RESCUEDOUBLE) parentKLowBound;
      nCeil += (RESCUEDOUBLE) parentKLowBound;
      RESCUEDOUBLE last = nFloor;
      for (loop = 0; loop < zWeightLength && bufferLen > 0; loop++)
      {
        if (zWeight[loop] > nFloor && zWeight[loop] <= nCeil)
        {
          *pos++ = zWeight[loop] - last;
          last = zWeight[loop];
          bufferLen--;
        }
      }
    }
    break;
  default:
    break;   // Quiet warning
  }
}

RESCUEINT64 RescueGeometry::ParentLowBound64(RescueGridAxis::GridNdx ndx)
{
  switch (ndx)
  {
  case RescueGridAxis::I_AXIS:
    return parentILowBound;
    break;
  case RescueGridAxis::J_AXIS:
    return parentJLowBound;
    break;
  case RescueGridAxis::K_AXIS:
    return parentKLowBound;
    break;
  default:
    break;   // Quiet warning
  }
  return 0;
}

RESCUEINT64 RescueGeometry::ParentCount64(RescueGridAxis::GridNdx ndx)
{
  switch (ndx)
  {
  case RescueGridAxis::I_AXIS:
    return parentICount;
    break;
  case RescueGridAxis::J_AXIS:
    return parentJCount;
    break;
  case RescueGridAxis::K_AXIS:
    return parentKCount;
    break;
  default:
    break;   // Quiet warning
  }
  return 0;
}

RESCUEINT64 RescueGeometry::WeightLength64(RescueGridAxis::GridNdx ndx)
{
  switch (ndx)
  {
  case RescueGridAxis::I_AXIS:
    return XWeightLength64();
    break;
  case RescueGridAxis::J_AXIS:
    return YWeightLength64();
    break;
  case RescueGridAxis::K_AXIS:
    return ZWeightLength64();
    break;
  default:
    break;   // Quiet warning
  }
  return 0;
}

RESCUEDOUBLE *RescueGeometry::Weight(RescueGridAxis::GridNdx ndx)
{
  switch (ndx)
  {
  case RescueGridAxis::I_AXIS:
    return XWeight();
    break;
  case RescueGridAxis::J_AXIS:
    return YWeight();
    break;
  case RescueGridAxis::K_AXIS:
    return ZWeight();
    break;
  default:
    break;   // Quiet warning
  }
  return 0;
}


void RescueGeometry::Weight(RescueGridAxis::GridNdx ndx, RESCUEDOUBLE *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  switch (ndx)
  {
  case RescueGridAxis::I_AXIS:
    XWeight(buffer, offset, bufferLength);
    break;
  case RescueGridAxis::J_AXIS:
    YWeight(buffer, offset, bufferLength);
    break;
  case RescueGridAxis::K_AXIS:
    ZWeight(buffer, offset, bufferLength);
    break;
  default:
    break;   // Quiet warning
  }
}

void RescueGeometry::XWeight(RESCUEDOUBLE *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  if (xWeight != 0)
  {
    RESCUEDOUBLE *pos = &buffer[offset];
    RESCUEDOUBLE *pos2 = xWeight;
    RESCUEINT64 count = 0;
    while (bufferLength > 0 && count < xWeightLength)
    {
      *pos++ = *pos2++;
      count++;
      bufferLength--;
    }
  }
}

void RescueGeometry::YWeight(RESCUEDOUBLE *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  if (yWeight != 0)
  {
    RESCUEDOUBLE *pos = &buffer[offset];
    RESCUEDOUBLE *pos2 = yWeight;
    RESCUEINT64 count = 0;
    while (bufferLength > 0 && count < yWeightLength)
    {
      *pos++ = *pos2++;
      count++;
      bufferLength--;
    }
  }
}

void RescueGeometry::ZWeight(RESCUEDOUBLE *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  if (zWeight != 0)
  {
    RESCUEDOUBLE *pos = &buffer[offset];
    RESCUEDOUBLE *pos2 = zWeight;
    RESCUEINT64 count = 0;
    while (bufferLength > 0 && count < zWeightLength)
    {
      *pos++ = *pos2++;
      count++;
      bufferLength--;
    }
  }
}

RESCUEINT64 RescueGeometry::AllCornerValuesLength64()
{
  return Grid()->NodeCount(TRUE) * 8 * 3;
}

RESCUEINT32 RescueGeometry::LayerCornerValuesLength()
{
  RESCUEINT64 howMany = LayerCornerValuesLength64();
  return (RESCUEINT32) howMany;
}

RESCUEINT64 RescueGeometry::LayerCornerValuesLength64()
{
  if (lgrGridType != R_STRUCTURED_CPG)
  {
    throw "Illegal attempt to read geometry of non-CPG LGR";
  }
  RESCUEINT64 count = Grid()->Dimensions();
  RESCUEINT64 loop;
  RESCUEINT64 size = 8 * 3;

  for (loop = 0; loop < count; loop++)
  {
    RescueGridAxis *axis = Grid()->Axis(loop);
    if (axis->AxisIndex() == RescueGridAxis::I_AXIS
    ||  axis->AxisIndex() == RescueGridAxis::J_AXIS)
    {
      RESCUEINT64 nodes = axis->Count64();
      if (nodes > 1)
      {
        nodes--;
      }
      size *= nodes;
    }
  }
  return size;
}

void RescueGeometry::CellToNode(RESCUEINT32 i, RESCUEINT32 j, RESCUEINT32 k, RESCUEINT32 corner, 
                                RESCUEINT32 &i2, RESCUEINT32 &j2, RESCUEINT32 &k2)
{
  RESCUEINT64 i64;
  RESCUEINT64 j64;
  RESCUEINT64 k64;
  CellToNode((RESCUEINT64) i, (RESCUEINT64) j, (RESCUEINT64) k, (RESCUEINT64) corner, i64, j64, k64);
  if (i64 > 2147483647 || j64 > 2147483647 || k64 > 2147483647)
  {
    throw "Model is too large to be accessed in 32 bit mode.";
  }
  i2 = (RESCUEINT32) i64;
  j2 = (RESCUEINT32) j64;
  k2 = (RESCUEINT32) k64;
}

void RescueGeometry::CellToNode(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, 
                                RESCUEINT64 &i2, RESCUEINT64 &j2, RESCUEINT64 &k2)
{
 i2 = i;
 j2 = j;
 k2 = k;
 if (corner > 3)
 {
  k2++;
  corner -= 4;
 }
 if (corner == 1 || corner == 2)
 {
  i2++;
  }
 if (corner == 2 || corner == 3)
 {
  j2++;
 }
}

RescueGeometryObject **RescueGeometry::VertexAddress(RESCUEINT64 i, RESCUEINT64 j)
{
  EnsureLoaded();
  if (grid == 0)
  {
    return &vertices[i * jNodes + j];
  }
/*
  This can happen when unarchiving a version 9 file.
*/
  else
  {
    RESCUEINT64 ndx = i * jNodes + j;
    RESCUEINT64 howMany = grid->Vertices64();
    RescueGeometryObject **myReturn = 0;
    if (ndx >= 0 && ndx < howMany)
    {
      myReturn = &vertices[ndx];
    }
    return myReturn;
  }
}

RescueGeometryObject **RescueGeometry::VertexAddress(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 corner)
{
 if (corner > 3)
 {
  corner -= 4;
 }
/*
 Top and bottom corners are on the same vertex.
*/
 switch (corner)
 {
 case 0:
  return VertexAddress(i, j);
 case 1:
  return VertexAddress(i + 1, j);
 case 2:
  return VertexAddress(i + 1, j + 1);
 default:
  return VertexAddress(i, j + 1);
 }
/*
 This assumes that the order of corners begins in the same corner as the
 origin of the grid.
*/
}

RescueStairSteppedFault *RescueGeometry::StairStepsFor(RescueIJSurface *surface)
{
  EnsureLoaded();
  RescueStairSteppedFault *myReturn = 0;
  if (stairSteps != 0)
  {
    RESCUEINT64 howMany = stairSteps->Count64();
    RESCUEINT64 ndx = 0;
    while (myReturn == 0 && ndx < howMany)
    {
      RescueStairSteppedFault *ssf = stairSteps->NthObject(ndx++);
      if (ssf->Surface() == surface)
      {
        myReturn = ssf;
      }
    }
  }
  return myReturn;
}

RescueSurfaceToGrid *RescueGeometry::IntersectionFor(RescueIJSurface *surface)
{
  EnsureLoaded();
  RescueSurfaceToGrid *myReturn = 0;
  if (surfaceToGrid != 0)
  {
    RESCUEINT64 howMany = surfaceToGrid->Count64();
    RESCUEINT64 ndx = 0;
    while (myReturn == 0 && ndx < howMany)
    {
      RescueSurfaceToGrid *stg = surfaceToGrid->NthObject(ndx++);
      if (stg->Surface() == surface)
      {
        myReturn = stg;
      }
    }
  }
  return myReturn;
}

void RescueGeometry::SetStairSteppedSurface(RescueIJSurface *surface,
                            RescueIJKCellPair *cellPairs, RESCUEINT64 arrayLength)
{
  EnsureLoaded();
  RescueStairSteppedFault *current = StairStepsFor(surface);
  if (current == 0)
  {
    if (arrayLength > 0)
    {
      (*stairSteps) += new RescueStairSteppedFault(this, surface, cellPairs, arrayLength);
    }
  }
  else
  {
    if (arrayLength > 0)
    {
      current->SetList(cellPairs, arrayLength);
    }
    else
    {
      (*stairSteps) -= current;
    }
  }
}


void RescueGeometry::SetSurfaceIntersection(RescueIJSurface *surface,
                                            RESCUEINT32 *iList, RESCUEINT32 *jList, RESCUEINT32 listLength)
{
  EnsureLoaded();
  RescueSurfaceToGrid *current = IntersectionFor(surface);
  if (current == 0)
  {
    if (listLength > 0)
    {
      (*surfaceToGrid) += new RescueSurfaceToGrid(this, surface, RescueContext::Allocate64For32(iList, listLength, true), 
                                                                 RescueContext::Allocate64For32(jList, listLength, true), 
                                                                 listLength);
/*
  Constructor will hook it up on the surface side.
*/
    }
  }
  else
  {
    if (listLength > 0)
    {
      current->SetList(RescueContext::Allocate64For32(iList, listLength, true), 
                       RescueContext::Allocate64For32(jList, listLength, true), 
                       listLength);
    }
    else
    {
      (*surfaceToGrid) -= current;
    }
  }
}

void RescueGeometry::SetSurfaceIntersection(RescueIJSurface *surface,
                                            RESCUEINT64 *iList, RESCUEINT64 *jList, RESCUEINT64 listLength)
{
  EnsureLoaded();
  RescueSurfaceToGrid *current = IntersectionFor(surface);
  if (current == 0)
  {
    if (listLength > 0)
    {
      (*surfaceToGrid) += new RescueSurfaceToGrid(this, surface, iList, jList, listLength);
/*
  Constructor will hook it up on the surface side.
*/
    }
  }
  else
  {
    if (listLength > 0)
    {
      current->SetList(iList, jList, listLength);
    }
    else
    {
      (*surfaceToGrid) -= current;
    }
  }
}

void RescueGeometry::SetSurfaceIntersection(RescueIJSurface *surface,
                                            RescueSurfaceCell::SurfaceSide side,
                                            RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k,
                                            RESCUEINT64 cellFaceNumber,
                                            RESCUEFLOAT *uvs)
{
  EnsureLoaded();
  RESCUEINT64 maxI = grid->Axis(0)->Count64();
  RESCUEINT64 maxJ = grid->Axis(1)->Count64();
  RESCUEINT64 cellNumber = (k * (maxI * maxJ)) 
                 + (j * maxI)
                 + i;
  (*surfaceCell) += new RescueSurfaceCell(this, surface, cellNumber, cellFaceNumber,
                                       side, uvs);
/*
  Constructor will hook it up on the surface side.
*/
}

void RescueGeometry::SetWellboreIntersection(RescueWellbore *wellbore,
                                            RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k)
{
  RESCUEINT64 maxI = grid->Axis(0)->Count64();
  RESCUEINT64 maxJ = grid->Axis(1)->Count64();
  RESCUEINT64 cellNumber = (k * (maxI * maxJ)) 
                 + (j * maxI)
                 + i;
  (*wellboreCell) += new RescueWellboreCell(this, wellbore, cellNumber);
/*
  Constructor will hook it up on the wellbore side.
*/
}

RESCUEBOOL RescueGeometry::SetKLayerID(RESCUEINT64 zeroBasedNdx, RESCUEINT64 globalID)
{
  RESCUEINT64 layers = grid->Axis(2)->Count64() - 1;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < layers)
  {
    kLayerIDs[zeroBasedNdx] = globalID;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

RESCUEINT64 RescueGeometry::KLayerID(RESCUEINT64 zeroBasedNdx)
{
  RESCUEINT64 layers = grid->Axis(2)->Count64() - 1;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < layers)
  {
    return kLayerIDs[zeroBasedNdx];
  }
  else
  {
    return -1;
  }
}

RESCUEBOOL RescueGeometry::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueGeometry)
  {
    return TRUE;
  }
  else
  {
    return RescueHistoryObject::IsOfType(thisType);
  }
}

void RescueGeometry::SetLGRGroupName(RESCUECHAR *newName)
{
  if (lgrSetName != 0)
  {
    free(lgrSetName);
  }
  if (newName == 0)
  {
    lgrSetName = 0;
  }
  else
  {
    lgrSetName = _strdup(newName);
  }
}

void RescueGeometry::SetLGRStartTime(RESCUECHAR *newName)
{
  if (lgrStartTime != 0)
  {
    free(lgrStartTime);
  }
  if (newName == 0)
  {
    lgrStartTime = 0;
  }
  else
  {
    lgrStartTime = _strdup(newName);
  }
}

void RescueGeometry::SetLGREndTime(RESCUECHAR *newName)
{
  if (lgrEndTime != 0)
  {
    free(lgrEndTime);
  }
  if (newName == 0)
  {
    lgrEndTime = 0;
  }
  else
  {
    lgrEndTime = _strdup(newName);
  }
}

void RescueGeometry::SetReferenceName(RESCUECHAR *newName)
{
  if (referenceName != 0)
  {
    free(referenceName);
  }
  if (newName == 0)
  {
    referenceName = 0;
  }
  else
  {
    referenceName = _strdup(newName);
  }
}

void RescueGeometry::Archive(FILE *archiveFile)
{
  if (parentModel->Context()->FileVersion() == 9)
  {
    RESCUEBOOL loadedNow = IsLoaded();
    if (loadedNow == FALSE)
    {
      ReadData(parentModel->oldPathName);
      RelinkGeometryData();
    }
    ArchiveData(archiveFile, 1);
    if (loadedNow == FALSE)
    {
      DropMemory();
    }
  }
  else
  {
    myfprintf(parentModel->Context(), archiveFile, Identifier());
    grid->Archive(parentModel->Context(), archiveFile);
    if (parentModel->Context()->FileVersion() >= 28)
    {
      geometryUnit->Archive(parentModel->Context(), archiveFile);
      properties->Archive(parentModel->Context(), archiveFile);
    }
    if (parentModel->Context()->FileVersion() >= 33)
    {
      myfprintf(parentModel->Context(), archiveFile, referenceID);
      myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) lgrType);
      myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) lgrGridType);
      myfprintf(parentModel->Context(), archiveFile, parentILowBound);
      myfprintf(parentModel->Context(), archiveFile, parentICount);
      myfprintf(parentModel->Context(), archiveFile, parentJLowBound);
      myfprintf(parentModel->Context(), archiveFile, parentJCount);
      myfprintf(parentModel->Context(), archiveFile, parentKLowBound);
      myfprintf(parentModel->Context(), archiveFile, parentKCount);
      myfprintf(parentModel->Context(), archiveFile, xWeightLength);
      if (xWeightLength > 0)
      {
        myfprintf(parentModel->Context(), archiveFile, xWeight, xWeightLength, TRUE);
      }
      myfprintf(parentModel->Context(), archiveFile, yWeightLength);
      if (yWeightLength > 0)
      {
        myfprintf(parentModel->Context(), archiveFile, yWeight, yWeightLength, TRUE);
      }
      myfprintf(parentModel->Context(), archiveFile, zWeightLength);
      if (zWeightLength > 0)
      {
        myfprintf(parentModel->Context(), archiveFile, zWeight, zWeightLength, TRUE);
      }
      lgrs->Archive(parentModel->Context(), archiveFile);
    }
    if (parentModel->Context()->FileVersion() >= 37)
    {
      myfprintf(parentModel->Context(), archiveFile, "dirtyMinMax");
      RescueBuffer buf(parentModel->Context(), sizeof(dirtyMinMax) + 5);
      buf << dirtyMinMax;
      buf.Archive(archiveFile);
      myfprintf(parentModel->Context(), archiveFile, "minMax");
      RescueBuffer mmBuf(parentModel->Context(), (sizeof(minX) * 6) + 5);
      mmBuf << minX;
      mmBuf << maxX;
      mmBuf << minY;
      mmBuf << maxY;
      mmBuf << minZ;
      mmBuf << maxZ;
      mmBuf.Archive(archiveFile);
      if (referenceName != 0)
      {
        myfprintf(parentModel->Context(), archiveFile, "referenceName");
        RescueBuffer nameBuf(parentModel->Context(), strlen(referenceName) + 4);
        nameBuf << referenceName;
        nameBuf.Archive(archiveFile);
      }
      if (lgrSetName != 0)
      {
        myfprintf(parentModel->Context(), archiveFile, "lgrSetName");
        RescueBuffer nameBuf(parentModel->Context(), strlen(lgrSetName) + 4);
        nameBuf << lgrSetName;
        nameBuf.Archive(archiveFile);
      }
      if (lgrStartTime != 0)
      {
        myfprintf(parentModel->Context(), archiveFile, "lgrStartTime");
        RescueBuffer nameBuf(parentModel->Context(), strlen(lgrStartTime) + 4);
        nameBuf << lgrStartTime;
        nameBuf.Archive(archiveFile);
      }
      if (lgrEndTime != 0)
      {
        myfprintf(parentModel->Context(), archiveFile, "lgrEndTime");
        RescueBuffer nameBuf(parentModel->Context(), strlen(lgrEndTime) + 4);
        nameBuf << lgrEndTime;
        nameBuf.Archive(archiveFile);
      }
      if (propertyContainers == 0 
      &&  propertyContainerId != 0 
      &&  ParentModel()->propertyActionImmediate == TRUE)
      {
        propertyContainers = new cSetRescueDataContainer(ParentModel(), propertyContainerId);  // Will Read.
      }
      if (propertyContainers != 0)
      {
        myfprintf(parentModel->Context(), archiveFile, "properties");
        RescueBuffer buf1(parentModel->Context(), 10);
        buf1 << propertyContainers->Identifier();
        buf1.Archive(archiveFile);

        propertyContainers->Archive(); // Goes into it's own file.
      }
      else if (propertyContainers == 0 && propertyContainerId != 0)
      {
        myfprintf(parentModel->Context(), archiveFile, "properties");
        RescueBuffer buf1(parentModel->Context(), 10);
        buf1 << propertyContainerId;
        buf1.Archive(archiveFile);
      }
      myfprintf(parentModel->Context(), archiveFile, "EOD");
    }
    if (IsLoaded())
    {
      if (parentModel->Context()->delayedGeometryLoad == FALSE
      ||  hasChanged == TRUE
      ||  parentModel->geometryActionImmediate == TRUE)
      {
        WriteData();
      }
    }
    else if (parentModel->geometryActionImmediate)
    {
      if (ReadData(parentModel->oldPathName))
      {
        RelinkGeometryData();
        WriteData();
        DropMemory();
      }
    }
/*
  Above, main geometry file.
*/
  }
  if (parentModel->Context()->FileVersion() >= 23
  &&  parentModel->Context()->FileVersion() < 28)
  {
    WriteWireframeData();
  }
/*
  Make an empty RescueGeometry file so users won't have trouble with
  AnyFileTruncated().
*/
}

RESCUEBOOL RescueGeometry::WriteWireframeData()
{
  RESCUEBOOL myReturn = FALSE;
  RCHString fileName;
  fileName << parentModel->currentPathName << "." << Identifier()
                                                  << ".wireframe";
  parentModel->MakeBackupFile(fileName.String());
  FILE *archiveFile;
  if (parentModel->currentBinary)
  {
    archiveFile = (FILE *) fopen(fileName.String(), "wb");
  }
  else
  {
    archiveFile = (FILE *) fopen(fileName.String(), "wt");
  }
  if (archiveFile != 0)
  {
    parentModel->Context()->fileVersion = parentModel->desiredFileVersion;
    parentModel->Context()->binaryFlag = parentModel->currentBinary;
    if (parentModel->Context()->binaryFlag)
    {
      putc((RESCUEUCHAR) parentModel->Context()->fileVersion, archiveFile);
      fwrite("Rescue Geometry Wireframe File", sizeof(RESCUECHAR), 30, archiveFile);
    }
    else
    {
      fprintf(archiveFile, "Rescue Geometry Wireframe File Version %d\n", parentModel->Context()->fileVersion);
    }
    myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) 0);
    myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) 0);
    myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) 0);
    myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) 0);
    myfprintf(parentModel->Context(), archiveFile, "abracadabra jump jump");
    if (ferror(archiveFile) != 0)
    {
      RCHString message;
      message << "Error writing Rescue Geometry Wireframe File:";
      message << fileName.String();
      message << " (";
      message << (RESCUEINT64) ferror(archiveFile);
      message << ")";
      parentModel->Context()->SetError(message.NonNullString());
    }
    fclose(archiveFile);
    myReturn = TRUE;
  }
  return myReturn;
}

void RescueGeometry::ArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  RescueContext *context = parentModel->Context();
  myfprintf(context, archiveFile, "; Rescue Geometry");
  myfprintf(context, archiveFile, missingValue);
  if (lgrGridType == R_STRUCTURED_CPG)
  {
    RESCUEINT64 iCount = grid->Axis(0)->Count64();
    RESCUEINT64 jCount = grid->Axis(1)->Count64();
    RESCUEINT64 kCount = grid->Axis(2)->Count64();
    myfprintf(context, archiveFile, iCount);
    myfprintf(context, archiveFile, jCount);
    myfprintf(context, archiveFile, kCount);
    RESCUEINT64 iLoop, jLoop;
    for (iLoop = 0; iLoop < iCount; iLoop++)
    {
      for (jLoop = 0; jLoop < jCount; jLoop++)
      {
        RescueGeometryObject **object = VertexAddress(iLoop, jLoop);
        if (object == 0)
        {
          throw "Off the grid";
        }
        if (*object == 0)
        {
          myfprintf(context, archiveFile, (RESCUEINT64) R_EQUAL_AXIS);
        }
        else
        {
          RESCUEINT64 style = (*object)->VertexIs();
          if (style != R_SPLINE_LINE || context->fileVersion >= 34)
          {
            myfprintf(context, archiveFile, style);
            (*object)->Archive(context, kCount, archiveFile, (fileVersion > 1));
          }
          else
          {
            myfprintf(context, archiveFile, (RESCUEINT64) R_EQUAL_AXIS);
          }
        }
      }
    }
    surfaceCell->Archive(context, archiveFile);
    wellboreCell->Archive(context, archiveFile);

    RESCUEINT64 kLoop;
    for (kLoop = 0; kLoop < kCount; kLoop++)
    {
      myfprintf(context, archiveFile, kLayerIDs[kLoop]);
    }

    if (referenceSurface1 == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, referenceSurface1->Identifier());
    }
    if (referenceSurface2 == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, referenceSurface2->Identifier());
    }
    myfprintf(context, archiveFile, offset1);
    myfprintf(context, archiveFile, offset2);
    myfprintf(context, archiveFile, thickness);
    myfprintf(context, archiveFile, (RESCUEINT64) onOffLap);

  }
  else
  {
    surfaceCell->Archive(context, archiveFile);
    wellboreCell->Archive(context, archiveFile);
  }
  if (context->fileVersion >= 14)
  {
    if (cellStatus == 0)
    {
      cellStatus = new RescueBitArray(parentModel->Context(), grid->NodeCount64(), 2);
    }
    cellStatus->Archive(archiveFile);
  }
  if (context->fileVersion >= 15 && context->fileVersion <= 21)
  {
    if (color == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      color->Archive(context, archiveFile);
    }
  }
  if (context->fileVersion >= 20)
  {
    surfaceToGrid->Archive(context, archiveFile);
  }
  if (context->fileVersion >= 30)
  {
    stairSteps->Archive(context, archiveFile);
  }
  if (context->fileVersion >= 33)
  {
    if (parametricMap == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      parametricMap->Archive(context, archiveFile);
    }
    isds->Archive(context, archiveFile);
    if (unstructuredGrid == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      unstructuredGrid->Archive(context, archiveFile);
    }
    if (radialGrid == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      radialGrid->Archive(context, archiveFile);
    }
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueGeometry::RescueGeometry(RescueContext *context, FILE *archiveFile)
                              :RescueHistoryObject(context)
                              ,grid(0)
                              ,vertices(0)
                              ,cellStatus(0)
                              ,parentModel(0)
                              ,kLayerIDs(0)
                              ,referenceSurface1(0)
                              ,referenceSurface2(0)
                              ,referenceSurface1Id(0)
                              ,referenceSurface2Id(0)
                              ,surfaceCell(0)
                              ,wellboreCell(0)
                              ,surfaceToGrid(0)
                              ,stairSteps(0)
                              ,geometryUnit(0)
                              ,properties(0)
                              ,hasChanged(FALSE)
                              ,dirtyMinMax(TRUE)
                              ,loaded(FALSE)
                              ,color(0)
                              ,parentBlockUnit(0)
                              ,parentGeobodyPart(0)
                              ,referenceID(0)
                              ,referenceName(0)
                              ,lgrType(R_NOT_LGR)
                              ,lgrGridType(R_STRUCTURED_CPG)
                              ,parentILowBound(0)
                              ,parentICount(0)
                              ,parentJLowBound(0)
                              ,parentJCount(0)
                              ,parentKLowBound(0)
                              ,parentKCount(0)
                              ,parentGeometry(0)
                              ,lgrs(new cSetRescueGeometry())
                              ,xWeightLength(0)
                              ,xWeight(0)
                              ,yWeightLength(0)
                              ,yWeight(0)
                              ,zWeightLength(0)
                              ,zWeight(0)
                              ,parametricMap(0)
                              ,isds(new cSetRescueInterfaceSurfaceDesc())
                              ,unstructuredGrid(0)
                              ,radialGrid(0)
                              ,minX(0)
                              ,maxX(0)
                              ,minY(0)
                              ,maxY(0)
                              ,minZ(0)
                              ,maxZ(0)
                              ,propertyContainers(0)
                              ,propertyContainerId(0)
                              ,lgrSetName(0)
                              ,lgrStartTime(0)
                              ,lgrEndTime(0)
{
  geometryUnit = new cSetRescueGeometryUnit();
  properties = new cSetRescueProperty();
  isA = R_RescueGeometry;
  if (context->ReadFileVersion() < 14)
  {
    loaded = TRUE;
    UnArchiveData(context, archiveFile, 1);
  }
  else
  {
    loaded = FALSE;
    ReadId(context, archiveFile);
    grid = new RescueGrid(context, archiveFile);
    if (context->ReadFileVersion() >= 28)
    {
      geometryUnit->UnArchive(context, archiveFile);
      properties->UnArchive(context, archiveFile);
    }
    if (context->ReadFileVersion() >= 33)
    {
      myfscanf(context, archiveFile, &referenceID);
      RESCUEINT64 temp;
      myfscanf(context, archiveFile, &temp);
      lgrType = (RescueLGRType) temp;
      myfscanf(context, archiveFile, &temp);
      lgrGridType = (RescueLGRGridType) temp;
      myfscanf(context, archiveFile, &parentILowBound);
      myfscanf(context, archiveFile, &parentICount);
      myfscanf(context, archiveFile, &parentJLowBound);
      myfscanf(context, archiveFile, &parentJCount);
      myfscanf(context, archiveFile, &parentKLowBound);
      myfscanf(context, archiveFile, &parentKCount);
      myfscanf(context, archiveFile, &xWeightLength);
      if (xWeightLength > 0)
      {
        xWeight = new RESCUEDOUBLE[(size_t) xWeightLength];
        if (context->ReadFileVersion() >= 36)
        {
          myfscanf(context, archiveFile, xWeight, xWeightLength, TRUE);
        }
        else
        {
          RESCUEFLOAT *xWeightStandIn = new RESCUEFLOAT[(size_t) xWeightLength];
          myfscanf(context, archiveFile, xWeightStandIn, xWeightLength, TRUE);
          RESCUEINT64 loop;
          for (loop = 0; loop < xWeightLength; loop++)
          {
            xWeight[loop] = (RESCUEDOUBLE) xWeightStandIn[loop];
          }
          delete[] xWeightStandIn;
        }
      }
      myfscanf(context, archiveFile, &yWeightLength);
      if (yWeightLength > 0)
      {
        yWeight = new RESCUEDOUBLE[(size_t) yWeightLength];
        if (context->ReadFileVersion() >= 36)
        {
          myfscanf(context, archiveFile, yWeight, yWeightLength, TRUE);
        }
        else
        {
          RESCUEFLOAT *yWeightStandIn = new RESCUEFLOAT[(size_t) yWeightLength];
          myfscanf(context, archiveFile, yWeightStandIn, yWeightLength, TRUE);
          RESCUEINT64 loop;
          for (loop = 0; loop < yWeightLength; loop++)
          {
            yWeight[loop] = (RESCUEDOUBLE) yWeightStandIn[loop];
          }
          delete[] yWeightStandIn;
        }
      }
      myfscanf(context, archiveFile, &zWeightLength);
      if (zWeightLength > 0)
      {
        zWeight = new RESCUEDOUBLE[(size_t) zWeightLength];
        if (context->ReadFileVersion() >= 36)
        {
          myfscanf(context, archiveFile, zWeight, zWeightLength, TRUE);
        }
        else
        {
          RESCUEFLOAT *zWeightStandIn = new RESCUEFLOAT[(size_t) zWeightLength];
          myfscanf(context, archiveFile, zWeightStandIn, zWeightLength, TRUE);
          RESCUEINT64 loop;
          for (loop = 0; loop < zWeightLength; loop++)
          {
            zWeight[loop] = (RESCUEDOUBLE) zWeightStandIn[loop];
          }
          delete[] zWeightStandIn;
        }
      }
      lgrs->UnArchive(context, archiveFile);
    }
    if (context->ReadFileVersion() >= 37)
    {
      RESCUECHAR myString[255];

      myfgets(context, myString, 255, archiveFile);
      while (strcmp(myString, "EOD") != 0)
      {
        if (strcmp(myString, "dirtyMinMax") == 0)
        {
          RescueBuffer buf(context, archiveFile);
          buf >> dirtyMinMax;
        }
        else if (strcmp(myString, "minMax") == 0)
        {
          RescueBuffer buf(context, archiveFile);
          buf >> minX;
          buf >> maxX;
          buf >> minY;
          buf >> maxY;
          buf >> minZ;
          buf >> maxZ;
        }
        else if (strcmp(myString, "referenceName") == 0)
        {
          RescueBuffer buf(context, archiveFile);
          RCHString refBuf;
          buf >> refBuf;
          referenceName = _strdup(refBuf.String());
        }
        else if (strcmp(myString, "lgrSetName") == 0)
        {
          RescueBuffer buf(context, archiveFile);
          RCHString groupBuf;
          buf >> groupBuf;
          lgrSetName = _strdup(groupBuf.String());
        }
        else if (strcmp(myString, "lgrStartTime") == 0)
        {
          RescueBuffer buf(context, archiveFile);
          RCHString groupBuf;
          buf >> groupBuf;
          lgrStartTime = _strdup(groupBuf.String());
        }
        else if (strcmp(myString, "lgrEndTime") == 0)
        {
          RescueBuffer buf(context, archiveFile);
          RCHString groupBuf;
          buf >> groupBuf;
          lgrEndTime = _strdup(groupBuf.String());
        }
        else if (strcmp(myString, "properties") == 0)
        {
          RescueBuffer buf(context, archiveFile);
          buf >> propertyContainerId;
        }
        else
        {
          RescueBuffer buf(context, archiveFile);
        }
        myfgets(context, myString, 255, archiveFile);
      }
    }
  }
  context->geometries->Add(this);
}

void RescueGeometry::UnArchiveData(RescueContext *context, FILE *archiveFile, RESCUEINT64 fileVersion)
{
/*
  The file version is the RESCUE_GEOMETRY_VERSION.  If we are using this
  routine to read from a single file (RescueModel file version 9) then
  1 is passed, since it has the same format.
*/
  if (surfaceCell != 0)
  {
    surfaceCell->EmptySelf();
  }
  else
  {
    surfaceCell = new cSetRescueSurfaceCell();
  }
  if (surfaceToGrid != 0)
  {
    surfaceToGrid->EmptySelf();
  }
  else
  {
    surfaceToGrid = new cSetRescueSurfaceToGrid();
  }
  if (stairSteps != 0)
  {
    stairSteps->EmptySelf();
  }
  else
  {
    stairSteps = new cSetRescueStairSteppedFault();
  }
  if (wellboreCell != 0)
  {
    wellboreCell->EmptySelf();
  }
  else
  {
    wellboreCell = new cSetRescueWellboreCell();
  }

  loaded = TRUE;
/*
  You want to make sure this is set because otherwise VertexAddress()
  will hard loop us.
*/
  isA = R_RescueGeometry;
  myfscanf(context, archiveFile, &missingValue);
  if (lgrGridType == R_STRUCTURED_CPG)
  {
    RESCUEINT64 iCount;
    RESCUEINT64 jCount;
    RESCUEINT64 kCount;
    myfscanf(context, archiveFile, &iCount);
    myfscanf(context, archiveFile, &jCount);
    myfscanf(context, archiveFile, &kCount);
    vertexIs = R_EQUAL_AXIS;
    jNodes = jCount;
    kNodes = kCount;
    RESCUEINT64 howMany = iCount * jCount;
    vertices = (RescueGeometryObject **) malloc(sizeof(RescueGeometryObject *) * (size_t) howMany);
    kLayerIDs = new RESCUEINT64[(size_t) kCount];
    RESCUEINT64 iLoop, jLoop;
    for (iLoop = 0; iLoop < iCount; iLoop++)
    {
      for (jLoop = 0; jLoop < jCount; jLoop++)
      {
        RescueGeometryObject **object = VertexAddress(iLoop, jLoop);
        RESCUEINT64 type;

        myfscanf(context, archiveFile, &type);
        switch (type)
        {
        case R_EQUAL_AXIS:
          *object = 0;
          break;
        case R_UNEQUAL_AXIS:
          *object = new RescueZStack(context, kCount, archiveFile, fileVersion > 1);
          break;
        case R_COORDINATE_LINE:
          *object = new RescueCoordinateLine(context, archiveFile, fileVersion > 1);
          break;
        case R_COORDINATE_POLYLINE:
          *object = new RescueCoordinatePolyLine(context, kCount, archiveFile, fileVersion > 1);
          break;
        case R_SPLIT_LINE:
          *object = new RescueSplitLine(context, kCount, archiveFile, fileVersion > 1);
          break;
        case R_SPLINE_LINE:
          *object = new RescuePillar(context, archiveFile, TRUE);
          break;
        }
        if (vertexIs < type)
        {
          vertexIs = (RescueVertexType) type;
        }
      }
    }
    surfaceCell->UnArchive(context, archiveFile);
    wellboreCell->UnArchive(context, archiveFile);

    RESCUEINT64 kLoop;
    if (context->ReadFileVersion() >= 5)
    {
      for (kLoop = 0; kLoop < kCount; kLoop++)
      {
        myfscanf(context, archiveFile, &kLayerIDs[kLoop]);
      }
      myfscanf(context, archiveFile, &referenceSurface1Id);
      myfscanf(context, archiveFile, &referenceSurface2Id);
      myfscanf(context, archiveFile, &offset1);
      myfscanf(context, archiveFile, &offset2);
      myfscanf(context, archiveFile, &thickness);
      RESCUEINT64 temp;
      myfscanf(context, archiveFile, &temp);
      onOffLap = (RescueLapType) temp;
    }
    else
    {
      for (kLoop = 0; kLoop < kCount; kLoop++)
      {
        kLayerIDs[kLoop] = 0;
      }
      referenceSurface1Id = 0;
      referenceSurface2Id = 0;
      offset1 = (RESCUEFLOAT) 0.0;
      offset2 = (RESCUEFLOAT) 0.0;
      thickness = (RESCUEFLOAT) 0.0;
      onOffLap = R_ONLAP;
    }
  }
  else
  {
    surfaceCell->UnArchive(context, archiveFile);
    wellboreCell->UnArchive(context, archiveFile);
    referenceSurface1Id = 0;
    referenceSurface2Id = 0;
    offset1 = (RESCUEFLOAT) 0.0;
    offset2 = (RESCUEFLOAT) 0.0;
    thickness = (RESCUEFLOAT) 0.0;
    onOffLap = R_ONLAP;
    jNodes = 0;
    kNodes = 0;
    vertexIs = R_EQUAL_AXIS;
  }
  if (context->ReadFileVersion() >= 14)
  {
    if (cellStatus != 0)
    {
      delete cellStatus;
    }
    cellStatus = new RescueBitArray(parentModel->Context(), archiveFile);
    if (fileVersion < 3)
    {
      myfscanf(context, archiveFile, &mainFileVersion);
    }
  }
  if (context->ReadFileVersion() >= 15 && context->ReadFileVersion() <= 21)
  {
    RESCUEINT64 colorFlag = 0;
    myfscanf(context, archiveFile, &colorFlag);
    if (colorFlag == 1)
    {
      color = new RescueColor(context, archiveFile);
    }
  }
  if (context->ReadFileVersion() >= 20)
  {
    surfaceToGrid->UnArchive(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 30)
  {
    stairSteps->UnArchive(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 33)
  {
    if (parametricMap != 0)
    {
      delete parametricMap;
    }
    parametricMap = 0;
    RESCUEINT64 mapFlag = 0;
    myfscanf(context, archiveFile, &mapFlag);
    if (mapFlag == 1)
    {
      parametricMap = new RescueParametricLGRMap(context, archiveFile);
      parametricMap->Relink(this);
    }
    isds->EmptySelf();
    isds->UnArchive(context, archiveFile);
    isds->Relink(this);
    if (unstructuredGrid != 0)
    {
      delete unstructuredGrid;
    }
    unstructuredGrid = 0;
    myfscanf(context, archiveFile, &mapFlag);
    if (mapFlag == 1)
    {
      unstructuredGrid = new RescueUnstructuredGrid(context, archiveFile);
      unstructuredGrid->Relink(this);
    }
    if (radialGrid != 0)
    {
      delete radialGrid;
    }
    radialGrid = 0;
    myfscanf(context, archiveFile, &mapFlag);
    if (mapFlag == 1)
    {
      radialGrid = new RescueRadialGrid(context, archiveFile);
      radialGrid->Relink(this);
    }
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


void RescueGeometry::Relink(RescueObject *parent)
{
  if (parent->IsA() == R_RescueBlockUnit)
  {
    parentBlockUnit = (RescueBlockUnit *) parent;
    parentModel = parentBlockUnit->Block()->ParentModel();
  }
  else if (parent->IsA() == R_RescueGeobodyPart)
  {
    parentGeobodyPart = (RescueGeobodyPart *) parent;
    parentModel = parentGeobodyPart->Body()->ParentModel();
  }
  else if (parent->IsA() == R_RescueGeometry)
  {
    parentGeometry = (RescueGeometry *) parent;
    parentModel = parentGeometry->ParentModel();
  }
  else
  {
    parentModel = (RescueModel *) parent;
  }
// parent should be RescueModel, RescueBlockUnit, or RescueGeobodyPart.
  lgrs->Relink(this);
// for LGR it is RescueGeometry.
  grid->Relink(parentModel);
  RelinkGeometryData();
  if (parentModel->geometryActionImmediate && IsLoaded() == FALSE)
  {
    Load();
  }
}

void RescueGeometry::RelinkGeometryData()
{
  if (surfaceCell != 0)
  {
    surfaceCell->Relink(this);
  }
  if (geometryUnit != 0)
  {
    geometryUnit->Relink(this);
  }
  if (properties != 0)
  {
    properties->Relink(this);
  }
  if (surfaceToGrid != 0)
  {
    surfaceToGrid->Relink(this);
  }
  if (stairSteps != 0)
  {
    stairSteps->Relink(this);
  }
  if (wellboreCell != 0)
  {
    wellboreCell->Relink(this);
  }
  if (referenceSurface1Id != 0)
  {
    referenceSurface1 = parentModel->ReferenceSurfaceIdentifiedBy(referenceSurface1Id);
  }
  if (referenceSurface2Id != 0)
  {
    referenceSurface2 = parentModel->ReferenceSurfaceIdentifiedBy(referenceSurface2Id);
  }
}

RESCUEINT32 RescueGeometry::Version(RESCUEBOOL reload)
{
  if (reload)
  {
    RESCUEINT32 myReturn = mainFileVersion;
    RESCUEINT32 savedArrayVersion = mainFileVersion;
    if (ReadData(parentModel->currentPathName, TRUE))
    {
      myReturn = mainFileVersion;
      mainFileVersion = savedArrayVersion;
    }
    return myReturn;
  }
  else
  {
    return mainFileVersion;
  }
}

void RescueGeometry::LoadRefSurface(RescueReferenceSurface *surface)
{
  if (surface != 0)
  {
    RescueTripletArray *geom = surface->NthIJSurface(0)->Geometry();
    if (geom->IsLoaded() == FALSE && geom->ParentModel() != 0)
    {
      geom->Load();
    }
  }
}

RESCUEBOOL RescueGeometry::Load()
{
  RESCUEBOOL myReturn = FALSE;
  if (IsLoaded())
  {
    DropMemory();
  }
  if (parentModel->ExistingModel())
  {
    parentModel->Context()->readFileVersion = parentModel->currentFileVersion;
    parentModel->Context()->readFileMainSoftwareVersion = parentModel->readFileMainSoftwareVersion;
    parentModel->Context()->readFileSubSoftwareVersion = parentModel->readFileSubSoftwareVersion;
    loaded = TRUE;    // We don't want EnsureLoaded to put us into a hard loop.
    myReturn = ReadData(parentModel->currentPathName);
  }
  if (myReturn == TRUE)
  {
    RelinkGeometryData();
    hasChanged = FALSE;
    loaded = TRUE;

    LoadRefSurface(TopReferenceSurface());
    LoadRefSurface(BottomReferenceSurface());
    LoadRefSurface(ReferenceSurface());
  }
  return myReturn;
}

RESCUEBOOL RescueGeometry::Unload()
{
  RESCUEBOOL myReturn = FALSE;
  if (hasChanged == FALSE)
  {
    DropMemory();
    myReturn = TRUE;
  }
  else if (IsLoaded() && parentModel->ExistingModel())
  {
    if (WriteData())
    {
      DropMemory();
      myReturn = TRUE;
    }
  }
  loaded = FALSE;
  return myReturn;
}

RESCUEBOOL RescueGeometry::ReadData(RESCUECHAR *basePathName, RESCUEBOOL versionOnly)
{
  RescueContext *context = parentModel->Context();
  parentModel->LoadContext();
  RESCUEBOOL myReturn = FALSE;
  RESCUECHAR buffer[30];
  RESCUEBOOL modelFile = FALSE;

  RCHString fileName;
  fileName << basePathName << "." << Identifier();
  FILE *archiveFile = (FILE *) fopen(fileName.String(), "rb");
  if (archiveFile != 0)
  {
    RESCUEINT64 fileVersion = getc(archiveFile);
    if (fileVersion == 'R')
    {
      context->binaryFlag = FALSE;
      fread(buffer, sizeof(RESCUECHAR), 20, archiveFile);
      if (strncmp(buffer, "escue Geometry File", 19) == 0)
      {
        modelFile = TRUE;
      }
    }
    else
    {
      context->binaryFlag = TRUE;
      fread(buffer, sizeof(RESCUECHAR), 20, archiveFile);
      if (strncmp(buffer, "Rescue Geometry File", 20) == 0)
      {
        modelFile = TRUE;
      }
    }
    fclose(archiveFile);
    if (modelFile)
    {
      int fileVersion = 0;
      if (context->binaryFlag)
      {
        archiveFile = (FILE *) fopen(fileName.String(), "rb");
        fileVersion = getc(archiveFile);
        fseek(archiveFile, 20, SEEK_CUR);
        if (fileVersion >= 3)
        {
          myfscanf(context, archiveFile, &mainFileVersion);
        }
        if (versionOnly == FALSE)
        {
          UnArchiveData(context, archiveFile, fileVersion);
        }
        fclose(archiveFile);
        myReturn = TRUE;
      }
      else
      {
        RESCUECHAR myString[255];

        archiveFile = (FILE *) fopen(fileName.String(), "rt");
        myfgets(context, myString, 255, archiveFile);
        sscanf(myString, "Rescue Geometry File Version %d\n", &fileVersion);
        if (fileVersion >= 3)
        {
          myfscanf(context, archiveFile, &mainFileVersion);
        }
        if (versionOnly == FALSE)
        {
          UnArchiveData(context, archiveFile, fileVersion);
        }
        fclose(archiveFile);
        myReturn = TRUE;
      }
    }
    else
    {
      RCHString message;
      message << "Not a Rescue Geometry File:" << fileName.NonNullString();
      context->SetError(message.NonNullString());
    }
  }
  return myReturn;
}

RESCUEBOOL RescueGeometry::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FileTruncated();
  if (myReturn == FALSE && properties != 0)
  {
    myReturn = properties->AnyFileTruncated();
  }
  if (myReturn == FALSE && stairSteps != 0)
  {
    RESCUEBOOL loaded = this->IsLoaded();
    if (loaded == FALSE)
    {
      this->Load();
    }
    myReturn = stairSteps->AnyFileTruncated();
    if (myReturn == FALSE)
    {
      myReturn = surfaceToGrid->AnyFileTruncated();
    }
    if (myReturn == FALSE)
    {
      myReturn = isds->AnyFileTruncated();
    }
    if (myReturn == FALSE && parametricMap != 0)
    {
      myReturn = parametricMap->AnyFileTruncated();
    }
    if (loaded == FALSE)
    {
      this->Unload();
    }
  }
  if (myReturn == FALSE)
  {
    if (propertyContainers == 0 && propertyContainerId != 0)
    {
      propertyContainers = new cSetRescueDataContainer(ParentModel(), propertyContainerId);  // Will Read.
    }
    if (propertyContainers != 0)
    {
      myReturn = propertyContainers->AnyFileTruncated();
    }
  }
  return myReturn;
}

RESCUEBOOL RescueGeometry::FileTruncated()
{
  RCHString fileName;
  fileName << parentModel->currentPathName << "." << Identifier();
  RESCUEBOOL myReturn = parentModel->Context()->FileTruncatedPrimitive(fileName.String());
  return myReturn;
}

RESCUEBOOL RescueGeometry::WriteData()
{
  RESCUEBOOL myReturn = FALSE;
  RCHString fileName;
  fileName << parentModel->currentPathName << "." << Identifier();
  FILE *archiveFile;
  parentModel->MakeBackupFile(fileName.String());
  if (parentModel->currentBinary)
  {
    archiveFile = (FILE *) fopen(fileName.String(), "wb");
  }
  else
  {
    archiveFile = (FILE *) fopen(fileName.String(), "wt");
  }
  if (archiveFile != 0)
  {
    RescueContext *context = parentModel->Context(true);
    context->binaryFlag = parentModel->currentBinary;
    if (context->binaryFlag)
    {
      putc((RESCUEUCHAR) GEOMETRY_FILE_VERSION, archiveFile);
      fwrite("Rescue Geometry File", sizeof(RESCUECHAR), 20, archiveFile);
    }
    else
    {
      fprintf(archiveFile, "Rescue Geometry File Version %d\n", GEOMETRY_FILE_VERSION);
    }
#ifdef TESTING
    myfprintf(context, archiveFile, mainFileVersion);
#else
    myfprintf(context, archiveFile, ++mainFileVersion);
#endif
    ArchiveData(archiveFile, GEOMETRY_FILE_VERSION);
    myfprintf(context, archiveFile, "abracadabra jump jump");
    if (ferror(archiveFile) != 0)
    {
      RCHString message;
      message << "Error writing Rescue Geometry File:";
      message << fileName.NonNullString();
      message << " (";
      message << (RESCUEINT64) ferror(archiveFile);
      message << ")";
      context->SetError(message.NonNullString());
    }
    fclose(archiveFile);
    myReturn = TRUE;
  }
  return myReturn;
}

void RescueGeometry::DropMemory()
{
  if (surfaceCell != 0)
  {
    delete surfaceCell;
    surfaceCell = new cSetRescueSurfaceCell();
  }
  if (surfaceToGrid != 0)
  {
    delete surfaceToGrid;
    surfaceToGrid = new cSetRescueSurfaceToGrid();
  }
  if (stairSteps != 0)
  {
    delete stairSteps;
    stairSteps = new cSetRescueStairSteppedFault();
  }
  if (wellboreCell != 0)
  {
    delete wellboreCell;
    wellboreCell = new cSetRescueWellboreCell();
  }
 RESCUEINT64 howMany = grid->Vertices64();
 RESCUEINT64 loop;
  if (vertices != 0)
  {
   for (loop = 0; loop < howMany; loop++)
   {
    if (vertices[loop] != 0)
    {
        RescueGeometryObject *object = (RescueGeometryObject *) vertices[loop];
        switch (object->VertexIs())
        {
        case R_UNEQUAL_AXIS:
          delete (RescueZStack *) object;
          break;
        case R_COORDINATE_LINE:
          delete (RescueCoordinateLine *) object;
          break;
        case R_COORDINATE_POLYLINE:
          delete (RescueCoordinatePolyLine *) object;
          break;
        case R_SPLIT_LINE:
          delete (RescueSplitLine *) object;
          break;
        case R_SPLINE_LINE:
          delete (RescuePillar *) object;
          break;
 default:
   break;   // Quiet warning
        }
    }
   }
   free(vertices);
  }
  vertices = 0;
  if (kLayerIDs != 0)
  {
    delete[] kLayerIDs;
  }
  kLayerIDs = 0;
}

void RescueGeometry::SetCellStatus(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RescueCellStatus newStatus)
{
  EnsureLoaded();
  if (cellStatus == 0)
  {
    cellStatus = new RescueBitArray(parentModel->Context(), grid->NodeCount64(), 2);
  }
  RESCUEINT64 ndx = (i * jNodes + j) * kNodes + k;
  cellStatus->SetBitField(ndx, newStatus);
}

RescueGeometry::RescueCellStatus RescueGeometry::CellStatus(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k)
{
  EnsureLoaded();
  if (cellStatus == 0)
  {
    return R_ACTIVE;
/*
  Default value is R_ACTIVE, and none have been explicitly set yet, so . . .
*/
  }
  else
  {
    RESCUEINT64 ndx = (i * jNodes + j) * kNodes + k;
    return (RescueGeometry::RescueCellStatus) cellStatus->BitField(ndx);
  }
}

void RescueGeometry::SetColor(RescueColor *colorIn)
{
  if (color != 0)
  {
    delete color;
  }
  color = colorIn;
}

void RescueGeometry::SetColor(RescueColor &colorIn)
{
  RescueColor *newColor = new RescueColor(colorIn);
  SetColor(newColor);
}

RescueGeometryUnit *RescueGeometry::UnitIntersect(RescueUnit *unit)
{
  EnsureLoaded();
  RESCUEINT64 unitID = unit->Identifier();
  RESCUEINT64 howMany = geometryUnit->Count64();
  RESCUEINT64 loop;
  RescueGeometryUnit *myReturn = 0;
  for (loop = 0; loop < howMany && myReturn == 0; loop++)
  {
    RescueGeometryUnit *intersect = geometryUnit->NthObject(loop);
    if (intersect->unitID == unitID)
    {
      myReturn = intersect;
    }
  }
  return myReturn;
}

RESCUEBOOL RescueGeometry::DropUnitIntersect(RescueUnit *aUnit)
{
  EnsureLoaded();
  RescueGeometryUnit *unit = UnitIntersect(aUnit);
  if (unit == 0)
  {
    return FALSE;
  }
  else
  {
    return DropUnitIntersect(unit);
  }
}

RESCUEBOOL RescueGeometry::DropUnitIntersect(RescueGeometryUnit *unit)
{
  EnsureLoaded();
  return (*geometryUnit) -= unit;
}

RESCUEBOOL RescueGeometry::DropRescueProperty(RescueProperty *unitToDrop)
{
  unitToDrop->Dispose();

  RescueModel *model = ParentModel();
  RESCUEINT64 ordinal = 0;
  RescueUnit *unit = model->NthRescueUnit(ordinal++);
  while (unit != 0)
  {
    RESCUEINT64 ndx = 0;
    RescuePropertyGroup *group = unit->NthRescuePropertyGroup(ndx++);
    while (group != 0)
    {
      group->RemoveRescueProperty(unitToDrop);
      group = unit->NthRescuePropertyGroup(ndx++);
    }
    unit = model->NthRescueUnit(ordinal++);
  }

  ordinal = 0;
  RescueGeobody *geobody = model->NthRescueGeobody(ordinal++);
  while (geobody != 0)
  {
    RESCUEINT64 ndx = 0;
    RescuePropertyGroup *group = geobody->NthRescuePropertyGroup(ndx++);
    while (group != 0)
    {
      group->RemoveRescueProperty(unitToDrop);
      group = geobody->NthRescuePropertyGroup(ndx++);
    }
    geobody = model->NthRescueGeobody(ordinal++);
  }

  return ((*properties) -= unitToDrop);
}

RescueBlockUnit *RescueGeometry::ParentBlockUnit()
{
  if (parentGeometry == 0)
  {
    return parentBlockUnit;
  }
  else
  {
    return parentGeometry->ParentBlockUnit();
  }
}

RescueGeobodyPart *RescueGeometry::ParentGeobodyPart() 
{
  if (parentGeometry == 0)
  {
    return parentGeobodyPart;
  }
  else
  {
    return parentGeometry->ParentGeobodyPart();
  }
}

RescueProperty *RescueGeometry::PropertyIdentifiedBy(RESCUEINT64 identifier)
{
  RescueProperty *myReturn = properties->ObjectIdentifiedBy(identifier);
  if (myReturn == 0 && lgrs != 0)
  {
    RESCUEINT64 ord = 0;
    RescueGeometry *lgr = lgrs->NthObject(ord++);
    while (myReturn == 0 && lgr != 0)
    {
      myReturn = lgr->PropertyIdentifiedBy(identifier);
      lgr = lgrs->NthObject(ord++);
    }
  }
  return myReturn;
}

RescueInterfaceSurfaceDesc *RescueGeometry::InterfaceSurfaceDesc(RESCUEINT64 zeroBasedNdx)
{
  EnsureLoaded();
  return isds->NthObject(zeroBasedNdx);
}

RESCUEINT64 RescueGeometry::InterfaceSurfaceDescCount64() 
{
  EnsureLoaded();
  return isds->Count64();
}

RESCUEINT32 RescueGeometry::InterfaceSurfaceDescCount()
{
  return RescueContext::Return32For64(InterfaceSurfaceDescCount64(), false);
}

RESCUEBOOL RescueGeometry::DropInterfaceSurfaceDesc(RescueInterfaceSurfaceDesc *toDrop)
{
  EnsureLoaded();
  return (*isds) -= toDrop;
}

void RescueGeometry::SetUnstructuredGrid(RescueUnstructuredGrid *grid)
{
  EnsureLoaded();
  if (lgrGridType == R_STRUCTURED_CPG
  ||  lgrGridType == R_STRUCTURED_RADIAL)
  {
    throw "Illegal attempt to assign an unstructured grid to a structured geometry.";
  }
  if (unstructuredGrid != 0)
  {
    delete unstructuredGrid;
  }
  unstructuredGrid = grid;
}

void RescueGeometry::SetRadialGrid(RescueRadialGrid *grid)
{
  EnsureLoaded();
  if (lgrGridType != R_STRUCTURED_RADIAL)
  {
    throw "Illegal attempt to assign a radial grid to a non radial geometry.";
  }
  if (radialGrid != 0)
  {
    delete radialGrid;
  }
  radialGrid = grid;
}

void RescueGeometry::CalculateMinMax()
{
  EnsureLoaded();
  if (lgrGridType == R_STRUCTURED_RADIAL
  || (lgrType == R_UNCONSTRAINED && radialGrid != 0))
  {
    radialGrid->CalculateMinMax(minX, maxX, minY, maxY, minZ, maxZ);
    dirtyMinMax = false;
  }
  else if (lgrGridType != R_STRUCTURED_CPG
       || (lgrGridType == R_STRUCTURED_CPG && unstructuredGrid != 0))
  {
    unstructuredGrid->CalculateMinMax(minX, maxX, minY, maxY, minZ, maxZ);
    dirtyMinMax = false;
  }
  else
  {
    RESCUEINT64 count = Grid()->Dimensions();
    RESCUEINT64 loop;
    RESCUEINT64 ni = 1;
    RESCUEINT64 nj = 1;
    RESCUEINT64 nk = 1;

    for (loop = 0; loop < count; loop++)
    {
      RescueGridAxis *axis = Grid()->Axis(loop);
      RESCUEINT64 nodes = axis->Count64();
      if (nodes > 1)
      {
        nodes--;
      }
      if (axis->AxisIndex() == RescueGridAxis::I_AXIS)
      {
        ni = nodes;
      }
      else if (axis->AxisIndex() == RescueGridAxis::J_AXIS)
      {
        nj = nodes;
      }
      else if (axis->AxisIndex() == RescueGridAxis::K_AXIS)
      {
        nk = nodes;
      }
    }
    bool firstXIteration = true;
    bool firstYIteration = true;
    bool firstZIteration = true;
    RESCUEINT64 iLoop;
    RESCUEINT64 jLoop;
    RESCUEINT64 kLoop;
    RESCUEINT64 cLoop;
    for (kLoop = 0; kLoop < nk; kLoop++)
    {
      for (jLoop = 0; jLoop < nj; jLoop++)
      {
        for (iLoop = 0; iLoop < ni; iLoop++)
        {
/*
Note the order of these three loops was reversed between v35 and v36.
*/
          if (CellStatus(iLoop, jLoop, kLoop) == R_ACTIVE)
          {
            for (cLoop = 0; cLoop < 8; cLoop++)
            {
              RESCUEFLOAT xValue, yValue, zValue;
              Values(iLoop, jLoop, kLoop, cLoop, xValue, yValue, zValue);
              if (xValue != missingValue)
              {
                if (firstXIteration)
                {
                  firstXIteration = false;
                  minX = xValue;
                  maxX = xValue;
                }
                else
                {
                  if (xValue < minX)
                  {
                    minX = xValue;
                  }
                  if (xValue > maxX)
                  {
                    maxX = xValue;
                  }
                }
              }
              if (yValue != missingValue)
              {
                if (firstYIteration)
                {
                  firstYIteration = false;
                  minY = yValue;
                  maxY = yValue;
                }
                else
                {
                  if (yValue < minY)
                  {
                    minY = yValue;
                  }
                  if (yValue > maxY)
                  {
                    maxY = yValue;
                  }
                }
              }
              if (zValue != missingValue)
              {
                if (firstZIteration)
                {
                  firstZIteration = false;
                  minZ = zValue;
                  maxZ = zValue;
                }
                else
                {
                  if (zValue < minZ)
                  {
                    minZ = zValue;
                  }
                  if (zValue > maxZ)
                  {
                    maxZ = zValue;
                  }
                }
              }
            }
          }
        }
      }
    }
    dirtyMinMax = false;
  }
}

cSetRescueDataContainer *RescueGeometry::DataContainers()
{
  if (propertyContainers == 0)
  {
    if (propertyContainerId != 0)
    {
      propertyContainers = new cSetRescueDataContainer(ParentModel(), propertyContainerId);  // Will Read.
    }
  }
  return propertyContainers;
}

cSetRescueDataContainer *RescueGeometry::DemandDataContainers()
{
  if (propertyContainers == 0)
  {
    if (propertyContainerId != 0)
    {
      propertyContainers = new cSetRescueDataContainer(ParentModel(), propertyContainerId);  // Will Read.
    }
    else
    {
      propertyContainers = new cSetRescueDataContainer(ParentModel());
    }
  }
  return propertyContainers;
}

void RescueGeometry::SetOrientation(RescueOrientationLedger *ledger,
                                    RescueCoordinateSystem::Orientation orientation)
{
  RescueCoordinateSystem::Orientation oNow = Grid()->DisplayOrientation();
  bool swapI = false;
  bool swapJ = false;
  bool swapK = false;
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
      swapK = true;
    }
  }
  else if (orientation == RescueCoordinateSystem::LUF
       ||  orientation == RescueCoordinateSystem::LUB
       ||  orientation == RescueCoordinateSystem::RUF
       ||  orientation == RescueCoordinateSystem::RUB)
  {
    swapK = true;
  }
  if (swapI == true || swapJ == true || swapK == true)
  {
    if (IsLoaded() == false)
    {
      Load();
    }
    if (swapI == true || swapJ == true)
    {
      RESCUEINT64 iNodes = Grid()->Axis(0)->Count64();
      if (swapJ)
      {
        RESCUEINT64 iLoop;
        for (iLoop = 0; iLoop < iNodes; iLoop++)
        {
          RESCUEINT64 fNdx = (iLoop * jNodes);
          RESCUEINT64 eNdx = fNdx + (jNodes - 1);
          while (fNdx < eNdx)
          {
            RescueGeometryObject *temp = vertices[fNdx];
            vertices[fNdx] = vertices[eNdx];
            vertices[eNdx] = temp;
            fNdx++;
            eNdx--;
          }
        }
      }
      if (swapI)
      {
        RESCUEINT64 ifNdx = 0;
        RESCUEINT64 efNdx = ((iNodes * jNodes) - jNodes);
        while (ifNdx < efNdx)
        {
          RESCUEINT64 iLoop;
          for (iLoop = 0; iLoop < jNodes; iLoop++)
          {
            RescueGeometryObject *temp = vertices[ifNdx + iLoop];
            vertices[ifNdx + iLoop] = vertices[efNdx + iLoop];
            vertices[efNdx + iLoop] = temp;
          }
          ifNdx += jNodes;
          efNdx -= jNodes;
        }
      }

    }
   RESCUEINT64 howMany = Grid()->Vertices64();
    RESCUEINT64 iLowBound = Grid()->Axis(0)->LowBound64();
    RESCUEINT64 iCount    = Grid()->Axis(0)->Count64();
    RESCUEINT64 jLowBound = Grid()->Axis(1)->LowBound64();
    RESCUEINT64 jCount    = Grid()->Axis(1)->Count64();
    RESCUEINT64 loop;
    for (loop = 0; loop < howMany; loop++)
    {
      RescueGeometryObject *object = vertices[loop];
      if (object != 0)
      {
        if (object->VertexIs() == R_SPLIT_LINE)
        {
          ((RescueSplitLine *) object)->SwapAxes(swapI, swapJ, swapK, kNodes);
        }
        else
        {
          if (swapK)
          {
            object->SwapKAxis(kNodes);
          }
          if (swapI || swapJ)
          {
            if (object->VertexIs() == R_SPLINE_LINE)
            {
              ((RescuePillar *) object)->SwapIJAxis(swapI, iLowBound, iCount,
                                                    swapJ, jLowBound, jCount);
            }
          }
        }
      }
    }
    RESCUEINT64 kCount = Grid()->Axis(2)->Count64();
    if (swapK)
    {
      RESCUEINT64 kLowBound = Grid()->Axis(2)->LowBound64();
      ledger->SetKFor(this, kLowBound, kCount, swapK);
    }

    if (cellStatus != 0)
    {
      cellStatus->SwapAxes(swapI, iCount, swapJ, jCount, swapK, kCount);
    }
    MarkChanged();
    Unload();

    RESCUEINT64 ordinal = 0;
    RescueProperty *property = properties->NthObject(ordinal++);
    while (property != 0)
    {
      property->SwapAxes(swapI, iCount, swapJ, jCount, swapK, kCount);
      property = properties->NthObject(ordinal++);
    }
  }
  Grid()->SetOrientation(orientation, swapI, swapJ, swapK);
}

void RescueGeometry::EnsureLoaded()
{
  if (IsLoaded() == FALSE)
  {
    Load();
  }
}

void RescueGeometry::FindUniquePropertyNames(cSetString *container)
{
  RESCUEBOOL wasLoaded = IsLoaded();
  EnsureLoaded();
  properties->FindUniquePropertyNames(container);
  cSetRescueDataContainer *containers = DataContainers();
  if (containers != 0)
  {
    containers->FindUniquePropertyNames(container);
  }
  lgrs->FindUniquePropertyNames(container);
  surfaceToGrid->FindUniquePropertyNames(container);
  stairSteps->FindUniquePropertyNames(container);
  if (parametricMap != 0)
  {
    parametricMap->FindUniquePropertyNames(container);
  }
  isds->FindUniquePropertyNames(container);
  if (wasLoaded == FALSE)
  {
    Unload();
  }
}

RESCUEINT32 RescueGeometry::GeometryUnitCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = GeometryUnitCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 RescueGeometry::SurfaceToGridCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = SurfaceToGridCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}



