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

    cSetRescuePolyLine.cpp

 Keeps a list of pointers to some RescuePolyLine.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescuePolyLine.h"
#include "RescuePolyLine.h"

RescuePolyLine *cSetRescuePolyLine::PolyLineBetween(RescueTrimVertex *end1, RescueTrimVertex *end2)
{
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE)
  {
  if (objects[ndx]->LeftVertex() == end1)
   {
  if (objects[ndx]->RightVertex() == end2)
  {
      found = TRUE;
  }
   }
   else if (objects[ndx]->LeftVertex() == end2)
   {
  if (objects[ndx]->RightVertex() == end1)
  {
      found = TRUE;
  }
   }
  if (found == FALSE)
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

cSetRescuePolyLine::cSetRescuePolyLine()
{
  allocated = 10;
  count = 0;
  objects = (RescuePolyLine **) malloc(sizeof(RescuePolyLine *) * (size_t) allocated);
}

cSetRescuePolyLine::~cSetRescuePolyLine()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  free(objects);
}

void cSetRescuePolyLine::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Archive(archiveFile);
  }
}

void cSetRescuePolyLine::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Relink(parent);
  }
}

void cSetRescuePolyLine::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
  RescuePolyLine *newObject = new RescuePolyLine(context, archiveFile);
  (*this) += newObject;
  }
}

void cSetRescuePolyLine::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  count = 0;
}

void cSetRescuePolyLine::operator+=(RescuePolyLine *newObject)
{
  if (allocated == count)
  {
  allocated += 10;
  objects = (RescuePolyLine **) realloc(objects, sizeof(RescuePolyLine *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

void cSetRescuePolyLine::Relinquish(RescuePolyLine *existingObject)
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

  count--;
  for (loop = ndx; loop < count; loop++)
  {
      objects[loop] = objects[loop + 1];
  }
  }
}

RESCUEBOOL cSetRescuePolyLine::operator-=(RescuePolyLine *existingObject)
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

RescuePolyLine *cSetRescuePolyLine::ObjectNamed(const RESCUECHAR *mayBeName)
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

RescuePolyLine *cSetRescuePolyLine::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescuePolyLine::operator-=(RESCUEINT64 ndx)
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

RescuePolyLine *cSetRescuePolyLine::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescuePolyLine::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescuePolyLine::Count(void)
{
  return (RESCUEINT32) count;
}

RESCUEINT32 cSetRescuePolyLine::Count(RESCUEBOOL throwIfTrue)
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




