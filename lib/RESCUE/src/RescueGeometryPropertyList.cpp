/********************************************************************

  RescueGeometryPropertyList.cpp

  Returns a table-and-column style listing of properties in a
  RescueGeometry (and possibly it's LGR's) that places properties
  within context of groupings and times.

  This class is ephemeral.  It is accurate at the moment it is 
  created but any editing events after that point are not reflected
  in it's values.  It is not persistent.

  The order is either : by group, by time step, by geometry, by property
               or     : by time step, by group, by geometry, by property

  Properties that are in more than one group and/or timestep may appear
  more than once.

  Properties that are not in a timestep will not appear.

  The geometry you pass to the constructor must be a root geometry,
  not an LGR.  LGRs that are listed in the Geometry column are not
  necessarily included in the RescueTimeStep group of the same row,
  but they do have a property which is included in that group so
  presumably they do have something to do with that time step.

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
#include "RescueGeometryPropertyList.h"

RescueGeometryPropertyList::RescueGeometryPropertyList(RescueGeometry *geometry, 
                                                       RESCUEBOOL byTimeStepIn,
                                                       RESCUEBOOL includeLGRsIn, 
                                                       RescuePropertyGroup *nullOrGroup,
                                                       RESCUECHAR *nullOrTimeStepName)
                                                       :includeLGRs(includeLGRsIn)
                                                       ,byTimeStep(byTimeStepIn)
{
  count = 0;
  allocated = 100;
  properties = (RescueProperty **) malloc(sizeof(RescueProperty *) * allocated);
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

RescueGeometryPropertyList::~RescueGeometryPropertyList()
{
  free(properties);
  free(geometries);
  free(timeSteps);
  free(groups);
}

void RescueGeometryPropertyList::AddGeometry(RescueGeometry *geometry, RESCUECHAR *nullOrTimeStepName)
{
  RescueBlockUnit *bu = geometry->ParentBlockUnit();
  if (bu != 0)
  {
    RESCUEINT32 gOrd = 0;
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
      RESCUEINT32 gOrd = 0;
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
        RESCUEINT32 gOrd = 0;
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

void RescueGeometryPropertyList::AddGroup(RescuePropertyGroup *toAdd, 
                                          RescueGeometry *geometry,
                                          RESCUECHAR *nullOrTimeStepName)
{
  RESCUEINT32 tsNdx = 0;
  RescueTimeStepGroup *ts = toAdd->NthTimeStepGroup(tsNdx++);
  while (ts != 0)
  {
    if ((nullOrTimeStepName == 0) ? true : (*ts->TimeStepName()) == nullOrTimeStepName)
    {
      RESCUEINT32 pNdx = 0;
      RescueProperty *property = ts->NthRescueProperty(pNdx++);
      while (property != 0)
      {
        RescueGeometry *pGeom = property->Geometry();
        if (pGeom == geometry || (includeLGRs && pGeom->ParentBUG() == geometry))
        {
          AddRow(property, pGeom, ts, toAdd);
        }
        property = ts->NthRescueProperty(pNdx++);
      }
    }
    ts = toAdd->NthTimeStepGroup(tsNdx++);
  }
}

void RescueGeometryPropertyList::AddRow(RescueProperty *property, 
                                        RescueGeometry *geom,
                                        RescueTimeStepGroup *ts, 
                                        RescuePropertyGroup *group)
{
  RESCUEINT64 ndx = NdxOf(property, geom, ts, group);
  RESCUEBOOL handled = FALSE;
  if (ndx >= 0 && ndx < count)
  {
    if (CompareRow(ndx, property, geom, ts, group) == 0)
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
      properties = (RescueProperty **) realloc(properties, sizeof(RescueProperty *) * allocated);
      geometries = (RescueGeometry **) realloc(geometries, sizeof(RescueGeometry *) * allocated);
      timeSteps = (RescueTimeStepGroup **) realloc(timeSteps, sizeof(RescueTimeStepGroup *) * allocated);
      groups = (RescuePropertyGroup **) realloc(groups, sizeof(RescuePropertyGroup *) * allocated);
    }
    if (ndx < count)
    {
      RESCUEINT64 loop;
      for (loop = count; loop > ndx; loop--)
      {
        properties[loop] = properties[loop - 1];
        geometries[loop] = geometries[loop - 1];
        timeSteps[loop] = timeSteps[loop - 1];
        groups[loop] = groups[loop - 1];
      }
    }
    properties[ndx] = property;
    geometries[ndx] = geom;
    timeSteps[ndx] = ts;
    groups[ndx] = group;
    count++;
  }
}

RESCUEINT64 RescueGeometryPropertyList::NdxOf(RescueProperty *property, 
                                            RescueGeometry *geom,
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
    result = CompareRow(ndx, property, geom, ts, group);
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

RESCUEINT64 RescueGeometryPropertyList::CompareRow(RESCUEINT64 ndx, 
                                                 RescueProperty *property, 
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
  if (myReturn == 0)
  {
    myReturn = IDCompare(property->Identifier(), properties[ndx]->Identifier());
  }
  return myReturn;
}

RESCUEINT64 RescueGeometryPropertyList::IDCompare(RESCUEINT64 id1, RESCUEINT64 id2)
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

RescueProperty *RescueGeometryPropertyList::NthRowProperty(RESCUEINT32 zbn)
{
  RescueProperty *myReturn = 0;
  if (zbn >= 0 && zbn < count)
  {
    myReturn = properties[zbn];
  }
  return myReturn;
}

RescueGeometry *RescueGeometryPropertyList::NthRowGeometry(RESCUEINT32 zbn)
{
  RescueGeometry *myReturn = 0;
  if (zbn >= 0 && zbn < count)
  {
    myReturn = geometries[zbn];
  }
  return myReturn;
}

RescueTimeStepGroup *RescueGeometryPropertyList::NthRowTimeStepGroup(RESCUEINT32 zbn)
{
  RescueTimeStepGroup *myReturn = 0;
  if (zbn >= 0 && zbn < count)
  {
    myReturn = timeSteps[zbn];
  }
  return myReturn;
}

RescuePropertyGroup *RescueGeometryPropertyList::NthRowPropertyGroup(RESCUEINT32 zbn)
{
  RescuePropertyGroup *myReturn = 0;
  if (zbn >= 0 && zbn < count)
  {
    myReturn = groups[zbn];
  }
  return myReturn;
}


