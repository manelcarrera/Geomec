/********************************************************************

  RescueCoordinatePolyLine.cpp

  An object used by RescueGeometry for condition 4, ij vertices
  where a coordinate poly line is given.

  Rod Hanks,  January 1997

*********************************************************************/
#include "RescueCoordinatePolyLine.h"
#include "RescueBuffer.h"
#include "RescueGrid.h"
#include <string.h>

RescueCoordinatePolyLine::RescueCoordinatePolyLine(RescueGeometry *geometry, RESCUEFLOAT *stackValuesIn)
    : value(stackValuesIn), kLayers(geometry->Grid()->Axis(2)->Count64()) {}

void RescueCoordinatePolyLine::ZValue(RESCUEINT64 k, RESCUEFLOAT newZValue) { value[(2 * kLayers) + k] = newZValue; }

void RescueCoordinatePolyLine::ZStack(RescueGeometry *geometry, RESCUEFLOAT *newZValues) {
  RESCUEINT64 howMany = geometry->Grid()->Axis(2)->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    value[(2 * kLayers) + loop] = newZValues[loop];
  }
}

void RescueCoordinatePolyLine::Values(RescueGeometry *geometry, RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k,
                                      RESCUEINT64 corner, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z) {
  geometry->CellToNode(i, j, k, corner, i, j, k);
  /*
   Calculate node-based i,j,k values.
  */
  Values(geometry, i, j, k, x, y, z);
}

void RescueCoordinatePolyLine::Values(RescueGeometry *geometry, RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k,
                                      RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z) {
  x = value[k];
  y = value[kLayers + k];
  z = value[(kLayers * 2) + k];
}

void RescueCoordinatePolyLine::Values(RESCUEINT64 k, RESCUEFLOAT &z) { z = value[(kLayers * 2) + k]; }

void RescueCoordinatePolyLine::Archive(RescueContext *context, RESCUEINT64 kLayersIn, FILE *archiveFile,
                                       RESCUEBOOL compress) {
  myfprintf(context, archiveFile, value, kLayers * 3, compress);
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueCoordinatePolyLine::RescueCoordinatePolyLine(RescueContext *context, RESCUEINT64 kLayersIn, FILE *archiveFile,
                                                   RESCUEBOOL compress) {
  kLayers = kLayersIn;
  value = new RESCUEFLOAT[(size_t)(kLayers * 3)];
  myfscanf(context, archiveFile, value, kLayers * 3, compress);
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueCoordinatePolyLine::SwapKAxis(RESCUEINT64 kNodes) {
  if (value != 0) {
    RescueContext::SwapAxes(value, false, 3, true, kNodes);
  }
  /*
    There are three kNodes * xyz values, and the xyz values lie
    together, so it is like an [i][j] array in which i = xyz
    and j = knodes (we use Fortran ordering).
  */
}
