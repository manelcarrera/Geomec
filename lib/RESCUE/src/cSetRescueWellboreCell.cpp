/****************************************************************************

Copyright 1995 Petrotechnical Open Software Corporation

POSC grants permission to copy or reproduce this material in its original
form for internal use only.

This software is subject to the provision of the "POSC Software License
Agreement" which states in part:

1) Licensee accepts a non-exclusive, non-transferable, license  to use,
display, modify and distribute works derived from the licensed documentation
and Software Product.

2) Licensee shall have no right to distribute in its original form any
Software Product or documentation licensed under this agreement.

****************************************************************************/
/*************************************************************************

    cSetRescueWellboreCell.cpp

 Keeps a list of pointers to some RescueWellboreCell.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueWellboreCell.h"
#include "RescueWellboreCell.h"

cSetRescueWellboreCell::cSetRescueWellboreCell()
{
  allocated = 10;
  count = 0;
  objects = (RescueWellboreCell **) malloc(sizeof(RescueWellboreCell *) * (size_t) allocated);
}

cSetRescueWellboreCell::~cSetRescueWellboreCell()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  free(objects);
}

void cSetRescueWellboreCell::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueWellboreCell::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Relink(parent);
  }
}

void cSetRescueWellboreCell::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
  RescueWellboreCell *newObject = new RescueWellboreCell(context, archiveFile);
  (*this) += newObject;
  }
}

void cSetRescueWellboreCell::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  count = 0;
}

void cSetRescueWellboreCell::operator+=(RescueWellboreCell *newObject)
{
  if (allocated == count)
  {
  allocated += 10;
  objects = (RescueWellboreCell **) realloc(objects, sizeof(RescueWellboreCell *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueWellboreCell::operator-=(RescueWellboreCell *existingObject)
{
  RESCUEBOOL found = FALSE;
  RESCUEINT64 ndx = 0;

  while (ndx < count && found == FALSE)
  {
  if (existingObject == objects[ndx])
  {
      found = TRUE;
  }
  else
  {
      ndx++;
  }
  }
  if (found)
  {
  RESCUEINT64 loop;

  delete objects[ndx];
  count--;
  for (loop = ndx; loop < count; loop++)
  {
      objects[loop] = objects[loop + 1];
  }
  }
  return found;
}

RescueWellboreCell *cSetRescueWellboreCell::ObjectNamed(const RESCUECHAR *mayBeName)
{
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE)
  {
  if (objects[ndx]->IsNamed(mayBeName))
  {
      found = TRUE;
  }
  else
  {
      ndx++;
  }
  }
  if (found)
  {
  return objects[ndx];
  }
  else
  {
  return 0;
  }
}

RescueWellboreCell *cSetRescueWellboreCell::ObjectIdentifiedBy(RESCUEINT64 identifier)
{
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE)
  {
  if (objects[ndx]->IsIdentifiedBy(identifier))
  {
      found = TRUE;
  }
  else
  {
      ndx++;
  }
  }
  if (found)
  {
  return objects[ndx];
  }
  else
  {
  return 0;
  }
}

RESCUEBOOL cSetRescueWellboreCell::operator-=(RESCUEINT64 ndx)
{
  if (ndx >= 0 && ndx < count)
  {
  RESCUEINT64 loop;

  delete objects[ndx];
  count--;
  for (loop = ndx; loop < count; loop++)
  {
      objects[loop] = objects[loop + 1];
  }
  return TRUE;
  }
  else
  {
  return FALSE;
  }
}

void cSetRescueWellboreCell::InsertAt(RESCUEINT64 ndx, RescueWellboreCell *newObject)
{
  if (ndx >= allocated)
  {
  allocated = ndx + 5;
  objects = (RescueWellboreCell **) realloc(objects, sizeof(RescueWellboreCell *) * (size_t) allocated);
  }
  RESCUEINT64 loop;
  for (loop = count; loop < ndx; loop++)
  {
  objects[ndx] = 0;
  }
  objects[ndx] = newObject;
  if (count <= ndx)
  {
  count = ndx + 1;
  }
}
/* 
  This one is required for the Oracle archive/unarchive methods.
  It assumes we will eventually fill in all the places.
*/
RescueWellboreCell *cSetRescueWellboreCell::NthObject(RESCUEINT64 ordinal)
{
  if (ordinal < 0 || ordinal >= count)
  {
  return 0;
  }
  else
  {
  return objects[ordinal];
  }
}

RESCUEINT64 cSetRescueWellboreCell::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueWellboreCell::Count(void)
{
  return (RESCUEINT32) count;
}

RESCUEINT32 cSetRescueWellboreCell::Count(RESCUEBOOL throwIfTrue)
{
  if (count > 2147483647)
  {
  if (throwIfTrue)
  {
      throw "Model is too large to be accessed in 32 bit mode.";
  }
  return 0;
  }
  else
  {
  return (RESCUEINT32) count;
  }
}



