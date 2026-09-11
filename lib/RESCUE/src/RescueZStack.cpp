/********************************************************************

  RescueZStack.cpp

  An object used by RescueGeometry for condition 2, ij vertices
  where Z is given.

  Rod Hanks,  January 1997

*********************************************************************/
#include "RescueZStack.h"
#include "RescueGrid.h"
#include "RescueGridAxis.h"
#include "RescueBuffer.h"
#include <string.h>

RescueZStack::RescueZStack(RescueGeometry *geometry)
{
  RescueGridAxis *axis = geometry->Grid()->Axis(2);
  RESCUEINT64 howMany = axis->Count64();
  value = new RESCUEFLOAT[(size_t) howMany];
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
  value[loop] = axis->ValueAt(loop);
  }
}

void RescueZStack::ZValue(RESCUEINT64 k, RESCUEFLOAT newZValue)
{
  value[k] = newZValue;
}

void RescueZStack::ZStack(RescueGeometry *geometry, RESCUEFLOAT *newZValues)
{
  RESCUEINT64 howMany = geometry->Grid()->Axis(2)->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
  value[loop] = newZValues[loop];
  }
}

void RescueZStack::Values(RescueGeometry *geometry, 
             RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, 
             RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
 geometry->CellToNode(i, j, k, corner, i, j, k);
/*
 Calculate node-based i,j,k values.
*/
  Values(geometry, i, j, k, x, y, z);
}

void RescueZStack::Values(RescueGeometry *geometry, 
             RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, 
             RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
 x = geometry->Grid()->Axis(0)->ValueAt(i);
 y = geometry->Grid()->Axis(1)->ValueAt(j);
 z = value[k];
}

void RescueZStack::Archive(RescueContext *context, RESCUEINT64 kLayers, FILE *archiveFile, RESCUEBOOL compress)
{
  myfprintf(context, archiveFile, value, kLayers, compress);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueZStack::RescueZStack(RescueContext *context, RESCUEINT64 kLayers, FILE *archiveFile, RESCUEBOOL compress)
{
  value = new RESCUEFLOAT[(size_t) kLayers];
  myfscanf(context, archiveFile, value, kLayers, compress);
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

void RescueZStack::SwapKAxis(RESCUEINT64 kNodes)
{
  if (value != 0)
  {
  RescueContext::SwapArray(value, kNodes);
  }
}

RescueZStack::~RescueZStack()
{
  if (value != 0)
  {
  delete [] value;
  }
}


