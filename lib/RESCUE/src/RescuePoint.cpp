/********************************************************************

  RescuePoint.cpp

  A point on a Pillar grid.  We don't need the overhead
  so this is not a RescueObject.

  Rod Hanks,  Oct, 2003

*********************************************************************/
#include "RescuePoint.h"
#include "RescueModel.h"

void RescuePoint::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, x);
  myfprintf(context, archiveFile, y);
  myfprintf(context, archiveFile, z);
}

RescuePoint::RescuePoint(RescueContext *context, FILE *archiveFile)
{
  myfscanf(context, archiveFile, &x);
  myfscanf(context, archiveFile, &y);
  myfscanf(context, archiveFile, &z);
}


