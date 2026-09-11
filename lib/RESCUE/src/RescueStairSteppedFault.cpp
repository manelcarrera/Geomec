/********************************************************************

  RescueStairSteppedFault.cpp

  Records the positions within a RescueGeometry in which

  Rod Hanks,  June 2002

*********************************************************************/
#include "RescueStairSteppedFault.h"
#include "RescueGeometry.h"
#include "RescueIJSurface.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include "cSetRescueDataContainer.h"
#include <string.h>

RescueStairSteppedFault::RescueStairSteppedFault(RescueGeometry *geometryIn, 
                                                 RescueIJSurface *surfaceIn,
                                                 RescueIJKCellPair *arrayIn, 
                                                 RESCUEINT64 arrayLengthIn)
                                                 :RescueObject(geometryIn->ParentModel()->Context())
                                                 ,geometry(geometryIn)
                                                 ,surface(surfaceIn)
                                                 ,pairs(arrayIn)
                                                 ,count(arrayLengthIn)
                                                 ,properties(0)
                                                 ,propertyContainerId(0)
                                                 ,surfaceID(0)
{
  isA = R_RescueStairSteppedFault;
}

RescueStairSteppedFault::~RescueStairSteppedFault()
{
  if (pairs != 0)
  {
  delete [] pairs;
  }
  if (properties != 0)
  {
  delete properties;
  }
}

RESCUEBOOL RescueStairSteppedFault::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FALSE;
  if (properties == 0 && propertyContainerId != 0)
  {
  properties = new cSetRescueDataContainer(geometry->ParentModel(), propertyContainerId);  // Will Read.
  }
  if (properties != 0)
  {
  myReturn = properties->AnyFileTruncated();
  }
  return myReturn;
}

cSetRescueDataContainer *RescueStairSteppedFault::DataContainers()
{
  if (properties == 0)
  {
  if (propertyContainerId != 0)
  {
      properties = new cSetRescueDataContainer(geometry->ParentModel(), propertyContainerId);  // Will Read.
  }
  }
  return properties;
}

cSetRescueDataContainer *RescueStairSteppedFault::DemandDataContainers()
{
  if (properties == 0)
  {
  if (propertyContainerId != 0)
  {
      properties = new cSetRescueDataContainer(geometry->ParentModel(), propertyContainerId);  // Will Read.
  }
  else
  {
      properties = new cSetRescueDataContainer(geometry->ParentModel());
  }
  }
  return properties;
}

void RescueStairSteppedFault::SetList(RescueIJKCellPair *arrayIn, RESCUEINT64 arrayLengthIn)
{
  if (pairs != 0)
  {
  delete [] pairs;
  }
  pairs = arrayIn;
  count = arrayLengthIn;
}

RescueIJKCellPair *RescueStairSteppedFault::NthCellPair(RESCUEINT64 zeroBasedOrdinal)
{
  RescueIJKCellPair *myReturn = 0;
  if (pairs != 0)
  {
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < count)
  {
      myReturn = &pairs[zeroBasedOrdinal];
  }
  }
  return myReturn;
}

void RescueStairSteppedFault::Relink(RescueObject *parent)
{
  geometry = (RescueGeometry *) parent;
  surface = geometry->ParentModel()->IJSurfaceIdentifiedBy(surfaceID);
  surface->AddStairSteps(this);
/*
  It's a shame we can't relink just-in-time but then there wouldn't be a
  list of objects on the surface side.
*/
}

void RescueStairSteppedFault::Archive(FILE *archiveFile)
{
  RescueContext *context = geometry->ParentModel()->Context();
  myfprintf(context, archiveFile, "; Stair Stepped Fault");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, surface->Identifier());
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  myfprintf(context, archiveFile, pairs[loop].i1);
  myfprintf(context, archiveFile, pairs[loop].j1);
  myfprintf(context, archiveFile, pairs[loop].k1);
  myfprintf(context, archiveFile, pairs[loop].i2);
  myfprintf(context, archiveFile, pairs[loop].j2);
  myfprintf(context, archiveFile, pairs[loop].k2);
  if (context->FileVersion() >= 33)
  {
      myfprintf(context, archiveFile, pairs[loop].face1);
      myfprintf(context, archiveFile, pairs[loop].face2);
  }
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "lgrIDs");
  RescueBuffer buf(context, (count + 1) * 8);
  for (loop = 0; loop < count; loop++)
  {
      buf << pairs[loop].lgrid1;
      buf << pairs[loop].lgrid2;
  }
  buf.Archive(archiveFile);
  if (properties == 0 
  &&  propertyContainerId != 0 
  &&  geometry->ParentModel()->propertyActionImmediate == TRUE)
  {
      properties = new cSetRescueDataContainer(geometry->ParentModel(), propertyContainerId);  // Will Read.
  }
  if (properties != 0)
  {
      myfprintf(context, archiveFile, "properties");
      RescueBuffer buf1(context, 10);
      buf1 << properties->Identifier();
      buf1.Archive(archiveFile);

      properties->Archive(); // Goes into it's own file.
  }
  else if (properties == 0 && propertyContainerId != 0)
  {
      myfprintf(context, archiveFile, "properties");
      RescueBuffer buf1(context, 10);
      buf1 << propertyContainerId;
      buf1.Archive(archiveFile);
  }
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueStairSteppedFault::RescueStairSteppedFault(RescueContext *context, FILE *archiveFile)
                        :RescueObject(context)
                        ,geometry(0)
                        ,surface(0)
                        ,pairs(0)
                        ,count(0)
                        ,properties(0)
                        ,propertyContainerId(0)
                        ,surfaceID(0)
{
  isA = R_RescueStairSteppedFault;
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &surfaceID);
  myfscanf(context, archiveFile, &count);
  pairs = new RescueIJKCellPair[(int) count];
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  myfscanf(context, archiveFile, &pairs[loop].i1);
  myfscanf(context, archiveFile, &pairs[loop].j1);
  myfscanf(context, archiveFile, &pairs[loop].k1);
  myfscanf(context, archiveFile, &pairs[loop].i2);
  myfscanf(context, archiveFile, &pairs[loop].j2);
  myfscanf(context, archiveFile, &pairs[loop].k2);
  if (context->ReadFileVersion() >= 33)
  {
      myfscanf(context, archiveFile, &pairs[loop].face1);
      myfscanf(context, archiveFile, &pairs[loop].face2);
  }
  else
  {
      pairs[loop].face1 = 0;
      pairs[loop].face2 = 0;
  }
  pairs[loop].lgrid1 = -1;
  pairs[loop].lgrid2 = -1;
  }
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      if (strcmp(myString, "lgrIDs") == 0)
      {
    RescueBuffer buf(context, archiveFile);
    for (loop = 0; loop < count; loop++)
    {
          buf >> pairs[loop].lgrid1;
          buf >> pairs[loop].lgrid2;
    }
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

RESCUEBOOL RescueStairSteppedFault::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueStairSteppedFault)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}

void RescueStairSteppedFault::FindUniquePropertyNames(cSetString *container)
{
  cSetRescueDataContainer *containers = DataContainers();
  if (containers != 0)
  {
  containers->FindUniquePropertyNames(container);
  }
}





