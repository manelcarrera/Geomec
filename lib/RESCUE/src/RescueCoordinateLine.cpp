/********************************************************************

  RescueCoordinateLine.cpp

  An object used by RescueGeometry for condition 3, ij vertices
  where a coordinate line is given.

  Rod Hanks,  January 1997

*********************************************************************/
#include "RescueCoordinateLine.h"
#include "RescueBuffer.h"
#include "RescueGeometry.h"
#include "RescueGrid.h"
#include "RescueGridAxis.h"
#include <string.h>

RescueCoordinateLine::RescueCoordinateLine(RescueGeometry *geometry, RESCUEFLOAT xTopIn, RESCUEFLOAT yTopIn,
                                           RESCUEFLOAT zTopIn, RESCUEFLOAT xBottomIn, RESCUEFLOAT yBottomIn,
                                           RESCUEFLOAT zBottomIn, RESCUEFLOAT *middleValues)
    : middleZs(0) {
  xTop = xTopIn;
  yTop = yTopIn;
  zTop = zTopIn;
  xBottom = xBottomIn;
  yBottom = yBottomIn;
  zBottom = zBottomIn;
  middleLayers = geometry->Grid()->Axis(2)->Count64() - 2;
  if (middleValues == 0) {
    RESCUEINT64 howMany = geometry->Grid()->Axis(2)->Count64();
    middleZs = new RESCUEFLOAT[(size_t)(howMany - 2)];
    RESCUEFLOAT step = (zBottom - zTop) / (RESCUEFLOAT)(howMany - 1);
    RESCUEINT64 loop;
    for (loop = 0; loop < howMany - 2; loop++) {
      middleZs[loop] = zTop + ((loop + 1) * step);
    }
  } else {
    middleZs = new RESCUEFLOAT[(size_t)middleLayers];
    RESCUEINT64 loop;
    for (loop = 0; loop < middleLayers; loop++) {
      middleZs[loop] = middleValues[loop];
    }
  }
}

void RescueCoordinateLine::ZValue(RESCUEINT64 k, RESCUEFLOAT newZValue) {
  if (k == 0) {
    zTop = newZValue;
  } else if (k == middleLayers + 1) {
    zBottom = newZValue;
  } else {
    middleZs[k - 1] = newZValue;
  }
}

void RescueCoordinateLine::ZStack(RescueGeometry *geometry, RESCUEFLOAT *newZValues) {
  RESCUEINT64 ndx = 0;
  zTop = newZValues[ndx++];
  RESCUEINT64 loop;
  for (loop = 0; loop < middleLayers; loop++) {
    middleZs[loop] = newZValues[ndx++];
  }
  zBottom = newZValues[ndx];
}

void RescueCoordinateLine::Values(RescueGeometry *geometry, RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k,
                                  RESCUEINT64 corner, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z) {
  geometry->CellToNode(i, j, k, corner, i, j, k);
  /*
   Calculate node-based i,j,k values.
  */
  Values(geometry, i, j, k, x, y, z);
}

void RescueCoordinateLine::Values(RescueGeometry *geometry, RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT &x,
                                  RESCUEFLOAT &y, RESCUEFLOAT &z) {
  if (k == 0) {
    x = xTop;
    y = yTop;
    z = zTop;
  } else if (k == middleLayers + 1) {
    x = xBottom;
    y = yBottom;
    z = zBottom;
  } else if (k > 0 && k <= middleLayers) {
    RESCUEFLOAT ratio;
    z = middleZs[k - 1];
    if (zBottom != zTop) {
      ratio = (z - zTop) / (zBottom - zTop);
    } else {
      ratio = (RESCUEFLOAT)0.0;
    }
    x = xTop + ((xBottom - xTop) * ratio);
    y = yTop + ((yBottom - yTop) * ratio);
  }
}

void RescueCoordinateLine::Values(RESCUEINT64 k, RESCUEFLOAT &z) {
  if (k == 0) {
    z = zTop;
  } else if (k == middleLayers + 1) {
    z = zBottom;
  } else if (k > 0 && k <= middleLayers) {
    z = middleZs[k - 1];
  }
}

void RescueCoordinateLine::Archive(RescueContext *context, RESCUEINT64 klayers, FILE *archiveFile,
                                   RESCUEBOOL compress) {
  myfprintf(context, archiveFile, xTop);
  myfprintf(context, archiveFile, yTop);
  myfprintf(context, archiveFile, zTop);
  myfprintf(context, archiveFile, xBottom);
  myfprintf(context, archiveFile, yBottom);
  myfprintf(context, archiveFile, zBottom);
  myfprintf(context, archiveFile, middleLayers);
  myfprintf(context, archiveFile, middleZs, middleLayers, compress);
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueCoordinateLine::RescueCoordinateLine(RescueContext *context, FILE *archiveFile, RESCUEBOOL compress) {
  myfscanf(context, archiveFile, &xTop);
  myfscanf(context, archiveFile, &yTop);
  myfscanf(context, archiveFile, &zTop);
  myfscanf(context, archiveFile, &xBottom);
  myfscanf(context, archiveFile, &yBottom);
  myfscanf(context, archiveFile, &zBottom);
  myfscanf(context, archiveFile, &middleLayers);
  middleZs = new RESCUEFLOAT[(size_t)middleLayers];
  myfscanf(context, archiveFile, middleZs, middleLayers, compress);
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueCoordinateLine::SwapKAxis(RESCUEINT64 kNodes) {
  RescueContext::SwapArray(middleZs, middleLayers);
  float xTemp = xTop;
  float yTemp = yTop;
  float zTemp = zTop;
  xTop = xBottom;
  yTop = yBottom;
  zTop = zBottom;
  xBottom = xTemp;
  yBottom = yTemp;
  zBottom = zTemp;
}
