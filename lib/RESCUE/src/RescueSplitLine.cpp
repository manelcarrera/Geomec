/********************************************************************

  RescueSplitLine.h

  An object used by RescueGeometry for condition 5, ij vertices
  where individual corners of cells may each have their own xyz
  triplet.  Each k-layer node may therefore have up to 8 triplets.
  To save space, a default triplet is re-used if possible.

  Rod Hanks,  January 1997

*********************************************************************/
#include "RescueSplitLine.h"
#include "RescueGrid.h"
#include "RescueBuffer.h"
#include <string.h>

RescueSplitLine::RescueSplitLine(RescueGeometry *geometry, 
                                 RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT step, RESCUEFLOAT origin,
                                 RESCUEFLOAT *stackValuesIn)
                                    :addresses(stackValuesIn)
{
  if (addresses == 0)
  {
    RESCUEINT64 kLayers = geometry->Grid()->Axis(2)->Count64();
    addresses = new RESCUEFLOAT[(size_t) (kLayers * 8 * 3)];
    RESCUEINT64 kloop, cornerLoop;
    RESCUEFLOAT z = step;
    RESCUEINT64 ndx = 0;
    for (kloop = 0; kloop < kLayers; kloop++)
    {
      for (cornerLoop = 0; cornerLoop < 8; cornerLoop++)
      {
        addresses[ndx++] = x;
        addresses[ndx++] = y;
        addresses[ndx++] = z;
      }
      z += origin;
    }
  }
}

RescueSplitLine::RescueSplitLine(RescueGeometry *geometry, RESCUEINT64 i, RESCUEINT64 j, RescueGeometryObject *previous)
{
  RESCUEINT64 kLayers = geometry->Grid()->Axis(2)->Count64();
  addresses = new RESCUEFLOAT[(size_t) (kLayers * 8 * 3)];
  RESCUEFLOAT *address = addresses;
  RESCUEINT64 kLoop;
  RESCUEINT64 cornerLoop;
  for ( kLoop = 0; kLoop < kLayers; kLoop++)
  {
    RESCUEFLOAT x;
    RESCUEFLOAT y;
    RESCUEFLOAT z;

    for (cornerLoop = 0; cornerLoop < 8; cornerLoop++)
    {
      previous->Values(geometry, i, j, kLoop, x, y, z);
      *address = x;
      address++;
      *address = y;
      address++;
      *address = z;
      address++;
    }
  }
}

void RescueSplitLine::ZValue(RESCUEINT64 k, RESCUEFLOAT newZValue)
{
  RESCUEINT64 ndx = ((k * 8 * 3) + (2));
/*
  Address of first z for the layer.
*/
  RESCUEINT64 cornerLoop;
  for (cornerLoop = 0; cornerLoop < 8; cornerLoop++)
  {
    addresses[ndx] = newZValue;
    ndx += 3;
  }
}

void RescueSplitLine::ZStack(RescueGeometry *geometry, RESCUEFLOAT *newZValues)
{
  RESCUEINT64 howMany = geometry->Grid()->Axis(2)->Count64();
  RESCUEINT64 kLoop, cornerLoop;
  RESCUEINT64 ndx = 2;
/*
  Point to the first z.
*/
  for (kLoop = 0; kLoop < howMany; kLoop++)
  {
    for (cornerLoop = 0; cornerLoop < 8; cornerLoop++)
    {
      addresses[ndx] = newZValues[kLoop];
      ndx += 3;
    }
  }
}

void RescueSplitLine::SetCornerNode(RescueGeometry *geometry,
                                    RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner,
                                    RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
{
  RESCUEINT64 iPrime;
  RESCUEINT64 jPrime;
  RESCUEINT64 kPrime;

 geometry->CellToNode(i, j, k, corner, iPrime, jPrime, kPrime);
/*
  kPrime gives which k-layer.
*/
  RESCUEINT64 address = GenerateAddress(i, j, k, iPrime, jPrime, kPrime);
  addresses[address++] = x;
  addresses[address++] = y;
  addresses[address++] = z;
}

void RescueSplitLine::SetXValue(RescueGeometry *geometry,
                                    RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner,RESCUEFLOAT x)
{
  RESCUEINT64 iPrime;
  RESCUEINT64 jPrime;
  RESCUEINT64 kPrime;

 geometry->CellToNode(i, j, k, corner, iPrime, jPrime, kPrime);
/*
  kPrime gives which k-layer.
*/
  RESCUEINT64 address = GenerateAddress(i, j, k, iPrime, jPrime, kPrime);
  addresses[address] = x;
}

void RescueSplitLine::SetYValue(RescueGeometry *geometry,
                                    RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, RESCUEFLOAT y)
{
  RESCUEINT64 iPrime;
  RESCUEINT64 jPrime;
  RESCUEINT64 kPrime;

  geometry->CellToNode(i, j, k, corner, iPrime, jPrime, kPrime);
/*
  kPrime gives which k-layer.
*/
  RESCUEINT64 address = GenerateAddress(i, j, k, iPrime, jPrime, kPrime);
  address++;
  addresses[address] = y;
}

void RescueSplitLine::SetZValue(RescueGeometry *geometry,
                                    RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, RESCUEFLOAT z)
{
  RESCUEINT64 iPrime;
  RESCUEINT64 jPrime;
  RESCUEINT64 kPrime;

  geometry->CellToNode(i, j, k, corner, iPrime, jPrime, kPrime);
/*
  kPrime gives which k-layer.
*/
  RESCUEINT64 address = GenerateAddress(i, j, k, iPrime, jPrime, kPrime);
  address++;
  address++;
  addresses[address] = z;
}

void RescueSplitLine::Values(RescueGeometry *geometry, 
             RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner, 
             RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  RESCUEINT64 iPrime;
  RESCUEINT64 jPrime;
  RESCUEINT64 kPrime;

 geometry->CellToNode(i, j, k, corner, iPrime, jPrime, kPrime);
/*
  kPrime gives which k-layer.
*/
  RESCUEINT64 address = GenerateAddress(i, j, k, iPrime, jPrime, kPrime);
  x = addresses[address++];
  y = addresses[address++];
  z = addresses[address];
}

RESCUEINT64 RescueSplitLine::GenerateAddress(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, 
                                         RESCUEINT64 iPrime, RESCUEINT64 jPrime, RESCUEINT64 kPrime)
{
  RESCUEINT64 ndx = 0;

  if (iPrime == i)
  {
    if (jPrime == j)
    {
      ndx = 0;
    }
    else
    {
      ndx = 1;
    }
  }
  else
  {
    if (jPrime == j)
    {
      ndx = 2;
    }
    else
    {
      ndx = 3;
    }
  }
  if (kPrime != k)
  {
    ndx += 4;
  }
  return (kPrime * 8 * 3) + (ndx * 3);
}

void RescueSplitLine::Archive(RescueContext *context, RESCUEINT64 kLayers, FILE *archiveFile, RESCUEBOOL compress)
{
  myfprintf(context, archiveFile, addresses, kLayers * 3 * 8, compress);
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueSplitLine::RescueSplitLine(RescueContext *context, RESCUEINT64 kLayers, FILE *archiveFile, RESCUEBOOL compress)
{
  addresses = new RESCUEFLOAT[(size_t) (kLayers * 3 * 8)];
  myfscanf(context, archiveFile, addresses, kLayers * 3 * 8, compress);
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

void RescueSplitLine::SwapKAxis(RESCUEINT64 kNodes)
{
#if 0
  if (addresses != 0)
  {
    RescueContext::SwapAxes(addresses, false, 3, false, 8, true, kNodes);
  }
#endif
/*
  Consider it a three dimensional array [xyz][corner][kNodes].
*/
}

void RescueSplitLine::SwapAxes(bool swapI, bool swapJ, bool swapK, RESCUEINT64 kLayers)
{
  RESCUEINT64 totalValues = kLayers * 8 * 3;
  if (totalValues > 2147483647)
  {
    throw "RescueSplitLine can't swap more than 2^31 values";
  }
  RESCUEFLOAT *tempValues = new RESCUEFLOAT[(size_t) totalValues];
  RESCUEINT64 loop;
  for (loop = 0; loop < totalValues; loop++)
  {
    tempValues[loop] = addresses[loop];
  }
 int corner;
 for (corner = 0; corner < 8; corner++)
 {
  int otherCorner = corner;
  if (swapI)
  {
   switch (otherCorner)
   {
    case 0:
    case 2:
    case 4:
    case 6:
     otherCorner++;
     break;
    case 1:
    case 3:
    case 5:
    case 7:
     otherCorner--;
     break;
   }
  }
  if (swapJ)
  {
   switch (otherCorner)
   {
    case 0:
    case 4:
     otherCorner += 3;
     break;
    case 1:
    case 5:
     otherCorner++;
     break;
    case 3:
    case 7:
     otherCorner -= 3;
     break;
    case 2:
    case 6:
     otherCorner--;
     break;
   }
  }
  if (swapK)
  {
   switch (otherCorner)
   {
    case 0:
    case 1:
    case 2:
    case 3:
     otherCorner += 4;
     break;
    case 4:
    case 5:
    case 6:
    case 7:
     otherCorner -= 4;
     break;
   }
  }
    RESCUEINT64 kLayer;
    for (kLayer = 0; kLayer < kLayers; kLayer++)
    {
      RESCUEINT64 otherK = kLayer;
      if (swapK)
      {
        otherK = (kLayers - 1) - kLayer;
      }
      int cornerNdxes[8] = {0, 2, 3, 1, 4, 6, 7, 5};
      RESCUEINT64 oldNdx = (kLayer * 8 * 3) + (cornerNdxes[corner] * 3);
      RESCUEINT64 newNdx = (otherK * 8 * 3) + (cornerNdxes[otherCorner] * 3);
      int xyz;
      for (xyz = 0; xyz < 3; xyz++)
      {
        addresses[newNdx + xyz] = tempValues[oldNdx + xyz];
      }
    }
  }
  delete [] tempValues;
}






