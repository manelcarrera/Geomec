/********************************************************************

  RescueSection.cpp

  The sections for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueSection.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueSection::~RescueSection()
{
  if (sectionName != 0)
  {
    delete sectionName;
  }
}
RescueSection::RescueSection(RescueCoordinateSystem::Orientation orientation,
                RESCUECHAR *newSectionName,
                RescueModel *parentModelIn,
                RescueSurface::SurfaceType typeIn,
                RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                RESCUEFLOAT missingValue)
                             :RescueSurface( parentModelIn, 
                                             orientation,
                                             i_lowbound, i_count,
                                             j_lowbound, j_count,
                                             missingValue, typeIn)
         ,sectionName(0)
{
  isA = R_RescueSection;
  sectionName = new RCHString(newSectionName);
  (*parentModel->sections) += this;
}

RescueSection::RescueSection(RescueCoordinateSystem::Orientation orientation,
                RESCUECHAR *newSectionName,
                RescueModel *parentModelIn,
                RescueSurface::SurfaceType typeIn,
                RESCUEFLOAT missingValue)
                             :RescueSurface( parentModelIn, 
                                             orientation,
                                             0, 0,
                                             0, 0,
                                             missingValue, typeIn)
          ,sectionName(0)
{
  isA = R_RescueSection;
  sectionName = new RCHString(newSectionName);
  (*parentModel->sections) += this;
}

RescueSection::RescueSection(RescueCoordinateSystem::Orientation orientation,
                             RESCUECHAR *newSectionName,
                             RescueModel *parentModelIn,
                             RescueSurface::SurfaceType typeIn,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count, 
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue, RESCUEFLOAT *tripletArray)
                             :RescueSurface( parentModelIn,
                                             orientation,
                                             i_lowbound, i_count,
                                             j_lowbound, j_count,
                                             missingValue, 
                                             tripletArray, typeIn)
         ,sectionName(0)
{
  isA = R_RescueSection;
  sectionName = new RCHString(newSectionName);
  (*parentModel->sections) += this;
}

RescueSection::RescueSection(RescueCoordinateSystem::Orientation orientation,
                             RESCUECHAR *newSectionName,
                             RescueModel *modelIn,
                             SurfaceType typeIn,
                             RescueCoordinateSystemAxis *i_axis,
                             RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RescueCoordinateSystemAxis *j_axis,
                             RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue)
                             :RescueSurface( modelIn,
                                             orientation,
                                             i_axis, i_origin, i_step, i_lowbound, i_count,
                                             j_axis, j_origin, j_step, j_lowbound, j_count,
                                             missingValue, 
                                             typeIn)
         ,sectionName(0)
                             {
  isA = R_RescueSection;
  sectionName = new RCHString(newSectionName);
  (*parentModel->sections) += this;
}

RescueSection::RescueSection(RescueContext *context, FILE *archiveFile)
          :RescueSurface(context, archiveFile),sectionName(0)
{
  RESCUECHAR myString[255];

  isA = R_RescueSection;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  sectionName = new RCHString(myString);
  context->RescueProgress(myString);
  if (context->ReadFileVersion() < 6)
  {
    RESCUEBOOL oldStyleFlag;
    myfscanf(context, archiveFile, &oldStyleFlag);
    if (oldStyleFlag)
    {
      type = FAULT;
    }
    else
    {
      type = LEASE_BOUNDARY;
    }
  }
  else if (context->ReadFileVersion() < 10)
  {
    RESCUEINT64 typeFlag;
    myfscanf(context, archiveFile, &typeFlag);
    type = (SurfaceType) typeFlag;
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

void RescueSection::Relink(RescueObject *parentObject)
{
  parentModel = (RescueModel *) parentObject;
  RescueSurface::Relink(parentModel);
}

void RescueSection::Archive(FILE *archiveFile)
{
  RescueContext *context = ParentModel()->Context();
  context->RescueProgress((*sectionName).String());
  myfprintf(context, archiveFile, "; Section");
  RescueSurface::Archive(archiveFile);
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, (*sectionName).String());
  if (context->FileVersion() < 10)
  {
    myfprintf(context, archiveFile, (RESCUEINT64) type);
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEBOOL RescueSection::IsNamed(const RESCUECHAR *possibleName)
{
   return ((*sectionName) == possibleName);
}

RESCUEBOOL RescueSection::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueSection)
  {
    return TRUE;
  }
  else
  {
    return RescueSurface::IsOfType(thisType);
  }
}



