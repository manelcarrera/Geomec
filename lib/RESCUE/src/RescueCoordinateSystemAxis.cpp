/********************************************************************

  RescueCoordinateSystemAxis.cpp

  Coordinate system axis for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueCoordinateSystemAxis.h"
#include "RescueBuffer.h"
#include "RescueCoordinateSystem.h"
#include <string.h>

RescueCoordinateSystemAxis::~RescueCoordinateSystemAxis() {
  if (axisName != 0) {
    delete axisName;
  }
  if (unitOfMeasure != 0) {
    delete unitOfMeasure;
  }
}

RescueCoordinateSystemAxis::RescueCoordinateSystemAxis(RescueCoordinateSystem *parentIn, const RESCUECHAR *nameIn,
                                                       const RESCUECHAR *uomIn)
    : RescueObject(parentIn->Context()), parentCoordinateSystem(parentIn), axisName(0), unitOfMeasure(0) {
  isA = R_RescueCoordinateSystemAxis;
  axisName = new RCHString(nameIn);
  unitOfMeasure = new RCHString(uomIn);
}

RescueCoordinateSystemAxis::RescueCoordinateSystemAxis(RescueContext *context, FILE *archiveFile)
    : RescueObject(context), axisName(0), unitOfMeasure(0)

{
  RESCUECHAR myString[255];

  isA = R_RescueCoordinateSystemAxis;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  axisName = new RCHString(myString);
  myfgets(context, myString, 255, archiveFile);
  unitOfMeasure = new RCHString(myString);
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueCoordinateSystemAxis::Relink(RescueObject *rescueObject) {
  parentCoordinateSystem = (RescueCoordinateSystem *)rescueObject;
}

void RescueCoordinateSystemAxis::Archive(FILE *archiveFile) {
  myfprintf(parentCoordinateSystem->Context(), archiveFile, "; Coordinate system axis");
  myfprintf(parentCoordinateSystem->Context(), archiveFile, Identifier());
  myfprintf(parentCoordinateSystem->Context(), archiveFile, axisName->String());
  myfprintf(parentCoordinateSystem->Context(), archiveFile, unitOfMeasure->String());
  if (parentCoordinateSystem->Context()->FileVersion() >= 37) {
    myfprintf(parentCoordinateSystem->Context(), archiveFile, "EOD");
  }
}

RESCUEBOOL RescueCoordinateSystemAxis::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueCoordinateSystemAxis) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}
