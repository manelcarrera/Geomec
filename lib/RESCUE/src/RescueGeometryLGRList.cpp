/********************************************************************

  RescueGeometryLGRList.cpp

  Returns a table-and-column style listing of LGR's in a
  RescueGeometry tree that places LGR's within context of groupings 
  and times.

  This class is ephemeral.  It is accurate at the moment it is 
  created but any editing events after that point are not reflected
  in it's values.  It is not persistent.

  The order is either : by group, by time step, by geometry
               or     : by time step, by group, by geometry

  LGRs that are in more than one group and/or timestep may appear
  more than once.

  LGRs that are not in a timestep will not appear.

  The geometry that you pass must be a root geometry, not an LGR.

  Rod Hanks,  October, 2006

*********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "myHeaders.h"
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
#include "RescueGeometryLGRList.h"

RescueGeometryLGRList::RescueGeometryLGRList(RescueGeometry *geometry, 
                                                       RESCUEBOOL byTimeStepIn,
                                                       RescuePropertyGroup *nullOrGroup,
                                                       RESCUECHAR *nullOrTimeStepName)
                                                       :byTimeStep(byTimeStepIn)
{
  count = 0;
  allocated = 100;
  geometries = (RescueGeometry **) malloc(sizeof(RescueGeometry *) * allocated);
  timeSteps = (RescueTimeStepGroup **) malloc(sizeof(RescueTimeStepGroup *) * allocated);
  groups = (RescuePropertyGroup **) malloc(sizeof(RescuePropertyGroup *) * allocated);

  if (nullOrGroup != 0)
  {
  AddGroup(nullOrGroup, geometry, nullOrTimeStepName);  // byTimeStep is irrelevant
  }
  else
  {
  AddGeometry(geometry, nullOrTimeStepName);
  }
}

RescueGeometryLGRList::~RescueGeometryLGRList()
{
  free(geometries);
  free(timeSteps);
  free(groups);
}

void RescueGeometryLGRList::AddGeometry(RescueGeometry *geometry, RESCUECHAR *nullOrTimeStepName)
{
  RescueBlockUnit *bu = geometry->ParentBlockUnit();
  if (bu != 0)
  {
  int gOrd = 0;
  RescuePropertyGroup *group = bu->NthPropertyGroup(gOrd++);
  while (group != 0)
  {
      AddGroup(group, geometry, nullOrTimeStepName);
      group = bu->NthPropertyGroup(gOrd++);
  }
  }
  else
  {
  RescueGeobodyPart *part = geometry->ParentGeobodyPart();
  if (part != 0)
  {
      int gOrd = 0;
      RescuePropertyGroup *group = part->NthPropertyGroup(gOrd++);
      while (group != 0)
      {
    AddGroup(group, geometry, nullOrTimeStepName);
    group = part->NthPropertyGroup(gOrd++);
      }
  }
  else
  {
      RescueModel *model = geometry->ParentModel();
      if (model != 0)
      {
    int gOrd = 0;
    RescuePropertyGroup *group = model->NthRescuePropertyGroup(gOrd++);
    while (group != 0)
    {
          AddGroup(group, geometry, nullOrTimeStepName);
          group = model->NthRescuePropertyGroup(gOrd++);
    }
      }
  }
  }
}

void RescueGeometryLGRList::AddGroup(RescuePropertyGroup *toAdd, 
                                          RescueGeometry *geometry,
                                          RESCUECHAR *nullOrTimeStepName)
{
  int tsNdx = 0;
  RescueTimeStepGroup *ts = toAdd->NthTimeStepGroup(tsNdx++);
  while (ts != 0)
  {
  if ((nullOrTimeStepName == 0) ? true : (*ts->TimeStepName()) == nullOrTimeStepName)
  {
      int pNdx = 0;
      RescueGeometry *pGeom = ts->NthRescueGeometry(pNdx++);
      while (pGeom != 0)
      {
    AddRow(pGeom, ts, toAdd);
    pGeom = ts->NthRescueGeometry(pNdx++);
      }
  }
  ts = toAdd->NthTimeStepGroup(tsNdx++);
  }
}

void RescueGeometryLGRList::AddRow(RescueGeometry *geom,
                    RescueTimeStepGroup *ts, 
                    RescuePropertyGroup *group)
{
  RESCUEINT64 ndx = NdxOf(geom, ts, group);
  RESCUEBOOL handled = FALSE;
  if (ndx >= 0 && ndx < count)
  {
  if (CompareRow(ndx, geom, ts, group) == 0)
  {
      handled = TRUE;
  }
  }
/*
  We might see some rows more than once.
*/
  if (handled == FALSE)
  {
  if (count == allocated)
  {
      allocated += count / 2;
      geometries = (RescueGeometry **) realloc(geometries, sizeof(RescueGeometry *) * allocated);
      timeSteps = (RescueTimeStepGroup **) realloc(timeSteps, sizeof(RescueTimeStepGroup *) * allocated);
      groups = (RescuePropertyGroup **) realloc(groups, sizeof(RescuePropertyGroup *) * allocated);
  }
  if (ndx < count)
  {
      RESCUEINT64 loop;
      for (loop = count; loop > ndx; loop--)
      {
    geometries[loop] = geometries[loop - 1];
    timeSteps[loop] = timeSteps[loop - 1];
    groups[loop] = groups[loop - 1];
      }
  }
  geometries[ndx] = geom;
  timeSteps[ndx] = ts;
  groups[ndx] = group;
  count++;
  }
}

RESCUEINT64 RescueGeometryLGRList::NdxOf(RescueGeometry *geom,
                      RescueTimeStepGroup *ts, 
                      RescuePropertyGroup *group)
{
  RESCUEINT64 ndx = 0;
  RESCUEINT64 upperLimit = count;
  RESCUEINT64 lowerLimit = -1;
  RESCUEINT64 result = -1;

  while ((upperLimit - lowerLimit) > 1  && result != 0)
  {
  ndx = (lowerLimit + upperLimit) >> 1;
  result = CompareRow(ndx, geom, ts, group);
  if (result < 0)
  {
      upperLimit = ndx;
  }
  else if (result > 0)
  {
      lowerLimit = ndx;
  }
  }
  if (result > 0)
  {
  ndx++;
  }
  return ndx;
}

RESCUEINT64 RescueGeometryLGRList::CompareRow(RESCUEINT64 ndx, 
                                                 RescueGeometry *geom,
                                                 RescueTimeStepGroup *ts, 
                                                 RescuePropertyGroup *group)
{
  RESCUEINT64 myReturn = 0;
  if (byTimeStep)
  {
  myReturn = strcmp(ts->TimeStepName()->NonNullString(), 
                      timeSteps[ndx]->TimeStepName()->NonNullString());
  if (myReturn == 0)
  {
      myReturn = IDCompare(group->Identifier(), groups[ndx]->Identifier());
  }
  }
  else
  {
  myReturn = IDCompare(group->Identifier(), groups[ndx]->Identifier());
  if (myReturn == 0)
  {
      myReturn = strcmp(ts->TimeStepName()->NonNullString(), 
            timeSteps[ndx]->TimeStepName()->NonNullString());
  }
  }
  if (myReturn == 0)
  {
  myReturn = IDCompare(geom->Identifier(), geometries[ndx]->Identifier());
  }
  return myReturn;
}

RESCUEINT64 RescueGeometryLGRList::IDCompare(RESCUEINT64 id1, RESCUEINT64 id2)
{
  RESCUEINT64 myReturn = 0;
  if (id1 < id2)
  {
  myReturn = -1;
  }
  else if (id1 > id2)
  {
  myReturn = 1;
  }
  return myReturn;
}

RescueGeometry *RescueGeometryLGRList::NthRowGeometry(RESCUEINT32 zbn)
{
  RescueGeometry *myReturn = 0;
  if (zbn >= 0 && zbn < count)
  {
  myReturn = geometries[zbn];
  }
  return myReturn;
}

RescueTimeStepGroup *RescueGeometryLGRList::NthRowTimeStepGroup(RESCUEINT32 zbn)
{
  RescueTimeStepGroup *myReturn = 0;
  if (zbn >= 0 && zbn < count)
  {
  myReturn = timeSteps[zbn];
  }
  return myReturn;
}

RescuePropertyGroup *RescueGeometryLGRList::NthRowPropertyGroup(RESCUEINT32 zbn)
{
  RescuePropertyGroup *myReturn = 0;
  if (zbn >= 0 && zbn < count)
  {
  myReturn = groups[zbn];
  }
  return myReturn;
}



