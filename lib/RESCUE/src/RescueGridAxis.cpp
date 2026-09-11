/********************************************************************

  RescueGridAxis.cpp

  Grid axis for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueGridAxis.h"
#include "RescueBuffer.h"
#include "RescueCoordinateSystem.h"
#include "RescueGrid.h"
#include <string.h>

RescueGridAxis::RescueGridAxis(RescueContext *context, FILE *archiveFile)
    : RescueObject(context), parentGrid(0), relatedAxis(0) {
  isA = R_RescueGridAxis;
  myfscanf(context, archiveFile, &relatedAxisID);
  myfscanf(context, archiveFile, &lowBound);
  myfscanf(context, archiveFile, &count);
  myfscanf(context, archiveFile, &origin);
  myfscanf(context, archiveFile, &step);
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueGridAxis::Relink(RescueGrid *parent, RescueCoordinateSystem *cs) {
  parentGrid = parent;
  relatedAxis = cs->AxisIdentifiedBy(relatedAxisID);
}

void RescueGridAxis::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, "; Grid Axis");
  if (relatedAxis == 0) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  } else {
    myfprintf(context, archiveFile, relatedAxis->Identifier());
  }
  myfprintf(context, archiveFile, lowBound);
  myfprintf(context, archiveFile, count);
  myfprintf(context, archiveFile, origin);
  myfprintf(context, archiveFile, step);
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueGridAxis::GridNdx RescueGridAxis::AxisIndex() {
  GridNdx myReturn = UNKNOWN;
  RESCUEINT64 howMany = parentGrid->Dimensions();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == UNKNOWN; loop++) {
    if (parentGrid->axes->NthObject(loop) == this) {
      myReturn = (GridNdx)loop;
    }
  }
  return myReturn;
}

void RescueGridAxis::Swap() {
  origin = origin + (step * (count - 1));
  step = -step;
}

RESCUEBOOL RescueGridAxis::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueGridAxis) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}

RESCUEINT32 RescueGridAxis::LowBound(RESCUEBOOL throwIfTooBig) {
  RESCUEINT64 output = LowBound64();
  if (throwIfTooBig) {
    if (output > 2147483647 || output < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32)output;
}

RESCUEINT32 RescueGridAxis::Count(RESCUEBOOL throwIfTooBig) {
  RESCUEINT64 output = Count64();
  if (throwIfTooBig) {
    if (output > 2147483647 || output < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32)output;
}
