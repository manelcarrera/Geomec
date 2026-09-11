/********************************************************************

  RescueReferenceSurface.cpp

  Holds a horizontal surface which may be used to reference z values
  on a RescueGeometry (block unit grid).

  This is mostly necessary because some children of RescueSurface,
  notably RescueSection, would NOT be good candidates for reference
  surfaces.

  Rod Hanks,  June 1997

*********************************************************************/
#include "RescueReferenceSurface.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueReferenceSurface::RescueReferenceSurface(RescueModel *model,
                       RescueCoordinateSystem::Orientation orientation,
                       RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                       RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                       RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                       RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                       RESCUEFLOAT missingValue, 
                       RescueSurface::SurfaceType typeIn)
              :RescueSurface(model, orientation, model->CoordinateSystem()->Axis(0), i_origin, i_step,
                             i_lowbound, i_count, model->CoordinateSystem()->Axis(1), j_origin, j_step,
                             j_lowbound, j_count, missingValue, typeIn) 
{
  CommonInitialization(model);
}

RescueReferenceSurface::RescueReferenceSurface(RescueModel *model,
                       RescueCoordinateSystem::Orientation orientation,
                       RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                       RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                       RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                       RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                       RESCUEFLOAT missingValue, 
                       RESCUEFLOAT *values,
                       RescueSurface::SurfaceType typeIn)
               :RescueSurface(model, orientation, model->CoordinateSystem()->Axis(0), 
                              i_origin, i_step, i_lowbound, 
                              i_count, model->CoordinateSystem()->Axis(1), 
                              j_origin, j_step, j_lowbound, j_count,
                              missingValue, values, typeIn) 
{
  CommonInitialization(model);
}

RescueReferenceSurface::RescueReferenceSurface(RescueContext *context, FILE *archiveFile)
                              :RescueSurface(context, archiveFile)
{
  ReadId(context, archiveFile);
  isA = R_RescueReferenceSurface;
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

void RescueReferenceSurface::Archive(FILE *archiveFile)
{
  RescueContext *context = ParentModel()->Context();
  myfprintf(context, archiveFile, "; Reference Surface");
  RescueSurface::Archive(archiveFile);
  myfprintf(context, archiveFile, "; Reference Surface id");
  myfprintf(context, archiveFile, Identifier());
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

void RescueReferenceSurface::CommonInitialization(RescueModel *model)
{
  isA = R_RescueReferenceSurface;
  (*model->referenceSurfaces) += this;
}

RESCUEBOOL RescueReferenceSurface::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueReferenceSurface)
  {
    return TRUE;
  }
  else
  {
    return RescueSurface::IsOfType(thisType);
  }
}



