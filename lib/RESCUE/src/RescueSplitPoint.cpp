/********************************************************************

  RescueSplitPoint.cpp

  An object used by RescueTripletArray for split ij vertices
  in surface geometries.

  Rod Hanks,  October 2002

*********************************************************************/
#include <string.h>
#include <float.h>
#include "myHeaders.h"
#include "RescueSplitPoint.h"
#include "RescueBuffer.h"
#include <string.h>

RescueSplitPoint::RescueSplitPoint(RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
{
  addresses = new RESCUEFLOAT[12];
  RESCUEFLOAT *pos = addresses;
  RESCUEINT64 loop;
  for (loop = 0; loop < 4; loop++)
  {
    *pos++ = x;
    *pos++ = y;
    *pos++ = z;
  }
}

void RescueSplitPoint::SetCornerNode(RESCUEINT64 corner, RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
{
  if (corner >= 0 && corner < 4)
  {
    RESCUEINT64 ndx = corner * 3;
    addresses[ndx++] = x;
    addresses[ndx++] = y;
    addresses[ndx++] = z;
  }
}

void RescueSplitPoint::Values(RESCUEINT64 corner, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  if (corner >= 0 && corner < 4)
  {
    RESCUEINT64 ndx = corner * 3;
    x = addresses[ndx++];
    y = addresses[ndx++];
    z = addresses[ndx++];
  }
  else
  {
    x = FLT_MIN;
    y = FLT_MIN;
    z = FLT_MIN;
  }
}

void RescueSplitPoint::Archive(RescueContext *context, FILE *archiveFile, RESCUEBOOL compress)
{
  myfprintf(context, archiveFile, addresses, 12, compress);
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueSplitPoint::RescueSplitPoint(RescueContext *context, FILE *archiveFile, RESCUEBOOL compress)
{
  addresses = new RESCUEFLOAT[12];
  myfscanf(context, archiveFile, addresses, 12, compress);
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



