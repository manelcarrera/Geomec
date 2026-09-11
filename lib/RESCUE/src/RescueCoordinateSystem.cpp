/********************************************************************

  RescueCoordinateSystem.cpp

  Coordinate system axis for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueCoordinateSystem.h"
#include "RescueVertex.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueCoordinateSystemAxis *RescueCoordinateSystem::AxisIdentifiedBy(RESCUEINT64 identifier)
{
  RescueCoordinateSystemAxis *myReturn = 0;

  RESCUEINT64 ndx = 0;
  while (ndx < 3 && myReturn == 0)
  {
  if (axes[ndx]->Identifier() == identifier)
  {
      myReturn = axes[ndx];
  }
  else
  {
      ndx++;
  }
  }
  return myReturn;
}

RESCUEBOOL RescueCoordinateSystem::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueCoordinateSystem)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}

RescueCoordinateSystem::RescueCoordinateSystem(RescueContext *contextIn,
                         RESCUECHAR *name,
                      RescueCoordinateSystem::Orientation displayOrientationIn,
      RescueVertex *vertexIn,
  const RESCUECHAR *Xproperty, const RESCUECHAR *Xuom,
  const RESCUECHAR *Yproperty, const RESCUECHAR *Yuom,
  const RESCUECHAR *Zproperty, const RESCUECHAR *Zuom,
                         RESCUEINT64 insysIn,
                         RESCUEINT64 inzoneIn,
                         RESCUEINT64 inUnitIn,
                         RESCUEINT64 insphIn,
                         RESCUEDOUBLE semiMajorAxisIn,
                         RESCUEDOUBLE eccentricityIn,
                         RESCUEDOUBLE standardLatIn,
                         RESCUEDOUBLE secondLatIn,
                         RESCUEDOUBLE centralLongIn,
                         RESCUEDOUBLE latOriginIn,
                         RESCUEDOUBLE FALSEEastingIn,
                         RESCUEDOUBLE FALSENorthingIn)
                         :RescueHistoryObject(contextIn)
                         ,context(contextIn)
                         ,coordinateSystemName(0)
                         ,displayOrientation(displayOrientationIn)
                         ,vertex(vertexIn)
                         ,insys(insysIn)
                         ,inzone(inzoneIn)
                         ,inUnit(inUnitIn)
                         ,insph(insphIn)
                         ,semiMajorAxis(semiMajorAxisIn)
                         ,eccentricity(eccentricityIn)
                         ,standardLat(standardLatIn)
                         ,secondLat(secondLatIn)
                         ,centralLong(centralLongIn)
                         ,latOrigin(latOriginIn)
                         ,FALSEEasting(FALSEEastingIn)
                         ,FALSENorthing(FALSENorthingIn)
                         ,epsgId(0)
                         ,parentModel(0)
{
  isA = R_RescueCoordinateSystem;
  coordinateSystemName = new RCHString(name);
  axes[0] = new RescueCoordinateSystemAxis(this, Xproperty, Xuom);
  axes[1] = new RescueCoordinateSystemAxis(this, Yproperty, Yuom);
  axes[2] = new RescueCoordinateSystemAxis(this, Zproperty, Zuom);
}

RescueCoordinateSystem::RescueCoordinateSystem(RescueContext *contextIn, FILE *archiveFile)
                              :RescueHistoryObject(contextIn)
                              ,context(contextIn)
                              ,coordinateSystemName(0)
                              ,vertex(0)
                              ,epsgId(0)
                              ,parentModel(0)
{
  RESCUECHAR name[255];
  RESCUEINT64 vertexFlag;

  isA = R_RescueCoordinateSystem;
  if (context->ReadFileVersion() >= 20)
  {
  ReadId(context, archiveFile);
  }
  myfgets(context, name, 255, archiveFile);
  coordinateSystemName = new RCHString(name);

  RESCUEINT64 orientationFlag;
  myfscanf(context, archiveFile, &orientationFlag);
  displayOrientation = (RescueCoordinateSystem::Orientation) orientationFlag;

  RESCUEINT64 loop;
  for (loop = 0; loop < 3; loop++)
  {
  axes[loop] = new RescueCoordinateSystemAxis(context, archiveFile);
  axes[loop]->Relink(this);
  }
  myfscanf(context, archiveFile, &vertexFlag);
  if (vertexFlag == 1)
  {
  vertex = new RescueVertex(context, archiveFile);
  }
  if (context->ReadFileVersion() < 6)
  {
  insys = 0;
  inzone = 0;
  inUnit = 0;
  insph = 0;
  semiMajorAxis = 0.0;
  eccentricity = 0.0;
  standardLat = 0.0;
  secondLat = 0.0;
  centralLong = 0.0;
  latOrigin = 0.0;
  FALSEEasting = 0.0;
  FALSENorthing = 0.0;
  }
  else
  {
  myfscanf(context, archiveFile, &insys);
  myfscanf(context, archiveFile, &inzone);
  myfscanf(context, archiveFile, &inUnit);
  myfscanf(context, archiveFile, &insph);
  myfscanf(context, archiveFile, &semiMajorAxis);
  myfscanf(context, archiveFile, &eccentricity);
  myfscanf(context, archiveFile, &standardLat);
  myfscanf(context, archiveFile, &secondLat);
  myfscanf(context, archiveFile, &centralLong);
  myfscanf(context, archiveFile, &latOrigin);
  myfscanf(context, archiveFile, &FALSEEasting);
  myfscanf(context, archiveFile, &FALSENorthing);
  }
  if (context->ReadFileVersion() >= 37)
  {
      RESCUECHAR myString[255];

      myfgets(context, myString, 255, archiveFile);
      while (strcmp(myString, "EOD") != 0)
      {
    if (strcmp(myString, "epsgId") == 0)
    {
          RescueBuffer buf(context, archiveFile);
          buf >> epsgId;
    }
    else
    {
          RescueBuffer buf(context, archiveFile);
    }
    myfgets(context, myString, 255, archiveFile);
      }
  }
}

void RescueCoordinateSystem::RegisterWith(RescueModel *parentModelIn)
{
  if (parentModel == 0)
  {
  parentModel = parentModelIn;
  context = parentModel->Context();
  parentModel->RegisterObject(this);
  if (vertex != 0)
  {
      vertex->RegisterWith(parentModel);
  }
  }
}
  
void RescueCoordinateSystem::SetVertex(RescueVertex *vertexIn)
{
  vertex = vertexIn;
  if (parentModel != 0)
  {
  vertex->RegisterWith(parentModel);
  }
}

RescueCoordinateSystemAxis *RescueCoordinateSystem::Axis(RESCUEINT64 zeroBasedOrdinal)
{
  if (zeroBasedOrdinal < 0 || zeroBasedOrdinal > 2)
  {
  return 0;
  }
  else
  {
  return axes[zeroBasedOrdinal];
  }
}

RESCUEINT64 RescueCoordinateSystem::NdxOf(RescueCoordinateSystemAxis *axisIn)
{
  RESCUEINT64 loop;
  RESCUEINT64 myReturn = -1;
  for (loop = 0; loop < 3 && myReturn < 0; loop++)
  {
  if (axisIn == axes[loop])
  {
      myReturn = loop;
  }
  }
  return myReturn;
}

RescueCoordinateSystem::~RescueCoordinateSystem()
{
  RESCUEINT64 loop;

  if (coordinateSystemName != 0)
  {
  delete coordinateSystemName;
  }
  for (loop = 0; loop < 3; loop++)
  {
  delete axes[loop];
  }
  if (parentModel != 0)
  {
  parentModel->UnRegisterObject(this);
  }
}

void RescueCoordinateSystem::Archive(FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; Coordinate System");
  if (context->FileVersion() >= 20)
  {
  myfprintf(context, archiveFile, Identifier());
  }
  myfprintf(context, archiveFile, coordinateSystemName->String());
  myfprintf(context, archiveFile, (RESCUEINT64) displayOrientation);
  RESCUEINT64 loop;
  for (loop = 0; loop < 3; loop++)
  {
  axes[loop]->Archive(archiveFile);
  }
  if (vertex == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  vertex->Archive(context, archiveFile);
  }
  myfprintf(context, archiveFile, insys);
  myfprintf(context, archiveFile, inzone);
  myfprintf(context, archiveFile, inUnit);
  myfprintf(context, archiveFile, insph);
  myfprintf(context, archiveFile, semiMajorAxis);
  myfprintf(context, archiveFile, eccentricity);
  myfprintf(context, archiveFile, standardLat);
  myfprintf(context, archiveFile, secondLat);
  myfprintf(context, archiveFile, centralLong);
  myfprintf(context, archiveFile, latOrigin);
  myfprintf(context, archiveFile, FALSEEasting);
  myfprintf(context, archiveFile, FALSENorthing);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "epsgId");
  RescueBuffer buf(context, sizeof(RESCUEINT32) + 10);
  buf << epsgId;
  buf.Archive(archiveFile);
  myfprintf(context, archiveFile, "EOD");
  }
}



