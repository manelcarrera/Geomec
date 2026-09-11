/********************************************************************

  RescueTimeStepManager.cpp

  Finds all of the RescueTimeStepGroup instances in the model and
  arranges them in order.  Time steps are in alphabetical order,
  except for the special name "Default Group", which is always first.
  Because compliant RescueModels have only timesteps named
  "Default Group" and timesteps in ISO format (YYYY-MM-DDTHH:MM:SS Z) 
  with the time always in Greenwich mean time (IE Z is always 0),
  this puts the timesteps of compliant models in proper order.

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
#include "RescueDataContainer.h"
#include "RescueOrientationLedger.h"
#include "RescueGeobodyVolume.h"
#include "RescueTimeStepGroup.h"
#include "RescueTimeStepManager.h"
#include "RescueInterfaceSurfaceDesc.h"
#include "RCHString.h"
#include "cSetString.h"

#if !defined(WIN32) && !defined(WIN64) && !defined(_WINDOWS)
#define _strdup strdup
#endif  // !defined(WIN32) && !defined(WIN64)

RescueTimeStepManager::RescueTimeStepManager(RescuePropertyGroup *group)
{
  ownerGroup = group;

  pCount = 0;
  pAllocated = 20;
  propertyNames = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * pAllocated);

  count = 0;
  allocated = 20;
  timeStepNames = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * allocated);
  steps         = (RescueTimeStepGroup **) malloc(sizeof(RescueTimeStepGroup *) * allocated);
  AddPropertyGroup(group);
}

RescueTimeStepGroup *RescueTimeStepManager::GroupForTime(RESCUECHAR *timeToFind)
{
  RescueTimeStepGroup *myReturn = 0;
  int loop;
  for (loop = 0; loop < count && myReturn == 0; loop++)
  {
    if (strcmp(timeStepNames[loop], timeToFind) == 0)
    {
      myReturn = steps[loop];
    }
  }
  return myReturn;
}

RescueTimeStepManager::~RescueTimeStepManager()
{
  int loop;
  for (loop = 0; loop < count; loop++)
  {
    free(timeStepNames[loop]);
    delete steps[loop];
  }
  free(timeStepNames);
  free(steps);
}

void RescueTimeStepManager::AddPropertyGroup(RescuePropertyGroup *toAdd)
{
  int ndx = 0;
  RescueTimeStepGroup *tsg = toAdd->NthTimeStepGroup(ndx++);
  while (tsg != 0)
  {
    AddTimeStep(tsg->TimeStepName()->NonNullString(), tsg);
    tsg = toAdd->NthTimeStepGroup(ndx++);
  }
}

RescueTimeStepGroup *RescueTimeStepManager::MakeTimeStepGroup(RESCUEINT32 year, RESCUEINT32 month, RESCUEINT32 day,
                                                                   RESCUEINT32 hour, RESCUEINT32 minute, RESCUEINT32 second)
{
  RESCUECHAR buffer[30];
  sprintf(buffer, "%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d Z", year, month, day, hour, minute, second);
  RescueTimeStepGroup *tsGroup = new RescueTimeStepGroup(buffer, ownerGroup);
  AddTimeStep(buffer, tsGroup);
  return tsGroup;
}

RescueTimeStepGroup *RescueTimeStepManager::MakeTimeStepGroup(RESCUEINT32 year, RESCUEINT32 month, RESCUEINT32 day)
{
  RESCUECHAR buffer[30];
  sprintf(buffer, "%4.4d-%2.2d-%2.2dT00:00:00 Z", year, month, day);
  RescueTimeStepGroup *tsGroup = new RescueTimeStepGroup(buffer, ownerGroup);
  AddTimeStep(buffer, tsGroup);
  return tsGroup;
}

RescueTimeStepGroup *RescueTimeStepManager::MakeTimeStepGroup(const RESCUECHAR *timeName)
{
  RescueTimeStepGroup *myReturn = 0;
  if (strcmp(timeName, "Default Group") == 0)
  {
    myReturn = new RescueTimeStepGroup(timeName, ownerGroup);
  }
  else
  {
    int month;
    int day;
    int year;
    int hour;
    int minute;
    int second;
    int howMany = sscanf(timeName, "%d-%d-%dT%d:%d:%d Z", &year, &month, &day, &hour, &minute, &second);
    if (howMany == 6)
    {
      if (month >= 1 && month <= 12)
      {
        if (day >= 1 && day <= 31) //Okay it's a lazy check.
        {
          if (hour >= 0 && hour <= 24)
          {
            if (minute >= 0 && hour <= 60)
            {
              if (second >= 0 && second <= 60)
              {
                myReturn = new RescueTimeStepGroup(timeName, ownerGroup);
              }
            }
          }
        }
      }
    }
  }
  if (myReturn != 0)
  {
    AddTimeStep(timeName, myReturn);
  }
  return myReturn;
}

cSetString *RescueTimeStepManager::UniqueTimestepNames(RESCUECHAR *from, RESCUECHAR *to)
{
  cSetString *myReturn = new cSetString();
  int ndx;
  for (ndx = 0; ndx < count; ndx++)
  {
    if (strcmp(timeStepNames[ndx], from) >= 0)
    {
      if (strcmp(timeStepNames[ndx], to) <= 0)
      {
        (*myReturn) += new RCHString(timeStepNames[ndx]);
      }
    }
  }
  return myReturn;
}

void RescueTimeStepManager::AddTimeStep(const char *timeStepName, RescueTimeStepGroup *toAdd)
{
  RESCUEINT64 ndx = NdxOf(timeStepName);
  RESCUEBOOL handled = FALSE;
  if (ndx >= 0 && ndx < count)
  {
    if (strcmp(timeStepNames[ndx], timeStepName) == 0)
    {
      handled = TRUE;  // Ignore subsequent group with same name.
    }
  }
  if (handled == FALSE)
  {
    if (count == allocated)
    {
      allocated += count / 2;
      timeStepNames = (RESCUECHAR **) realloc(timeStepNames, sizeof(RESCUECHAR *) * allocated);
      steps = (RescueTimeStepGroup **) realloc(steps, sizeof(RescueTimeStepGroup *) * allocated);
    }
    if (ndx < count)
    {
      RESCUEINT64 loop;
      for (loop = count; loop > ndx; loop--)
      {
        timeStepNames[loop] = timeStepNames[loop - 1];
        steps[loop] = steps[loop - 1];
      }
    }
    timeStepNames[ndx] = _strdup(timeStepName);
    steps[ndx] = toAdd;
    count++;
  }

  int pNdx = 0;
  RescueProperty *property = toAdd->NthRescueProperty(pNdx++);
  while (property != 0)
  {
    bool found = false;
    int loop;
    for (loop = 0; loop < pCount && found == false; loop++)
    {
      if (strcmp(propertyNames[loop], property->Data()->PropertyName()->NonNullString()) == 0)
      {
        found = true;
      }
    }
    if (found == false)
    {
      if (pCount == pAllocated)
      {
        pAllocated += pCount / 2;
        propertyNames = (RESCUECHAR **) realloc(propertyNames, sizeof(RESCUECHAR *) * pAllocated);
      }
      propertyNames[pCount++] = _strdup(property->Data()->PropertyName()->NonNullString());
    }
    property = toAdd->NthRescueProperty(pNdx++);
  }
}

RESCUEINT32 RescueTimeStepManager::NdxOf(const RESCUECHAR *timeStepName)
{
  RESCUEINT32 ndx = 0;
  RESCUEINT32 upperLimit = count;
  RESCUEINT32 lowerLimit = -1;
  RESCUEINT32 result = -1;

  while ((upperLimit - lowerLimit) > 1  && result != 0)
  {
    ndx = (lowerLimit + upperLimit) >> 1;
    result = CompareRow(ndx, timeStepName);
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

RESCUEINT32 RescueTimeStepManager::CompareRow(RESCUEINT64 ndx, const RESCUECHAR *timeStepName)
{
  RESCUEINT32 myReturn = 0;
  if (strcmp(timeStepName, "Default Group") == 0)
  {
    if (strcmp(timeStepNames[ndx], "Default Group") == 0)
    {
      myReturn = 0;
    }
    else
    {
      myReturn = -1;
    }
  }
  else if (strcmp(timeStepNames[ndx], "Default Group") == 0)
  {
    myReturn = 1;
  }
  else if (strcmp(timeStepName, "Initialization Group") == 0)
  {
    if (strcmp(timeStepNames[ndx], "Initialization Group") == 0)
    {
      myReturn = 0;
    }
    else if (strcmp(timeStepNames[ndx], "Default Group") == 0)
    {
      myReturn = 1;
    }
    else
    {
      myReturn = -1;
    }
  }
  else if (strcmp(timeStepNames[ndx], "Initialization Group") == 0)
  {
    myReturn = 1;
  }
  else
  {
    myReturn = strcmp(timeStepName, timeStepNames[ndx]);
  }
  return myReturn;
}


