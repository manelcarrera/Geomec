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

        cSetRescueTrimVertex.cpp

 Keeps a list of pointers to some RescueTrimVertex.

        Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueTrimVertex.h"
#include "RescueTrimVertex.h"

RescueTrimVertex *cSetRescueTrimVertex::TrimVertexLocatedBy(RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
{
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE)
  {
    if (objects[ndx]->X() == x)
    {
   if (objects[ndx]->Y() == y)
   {
    if (objects[ndx]->Z() == z)
    {
     found = TRUE;
    }
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

cSetRescueTrimVertex::cSetRescueTrimVertex()
{
  allocated = 10;
  count = 0;
  objects = (RescueTrimVertex **) malloc(sizeof(RescueTrimVertex *) * (size_t) allocated);
}

cSetRescueTrimVertex::~cSetRescueTrimVertex()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueTrimVertex::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescueTrimVertex::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Relink(parent);
  }
}

void cSetRescueTrimVertex::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
    RescueTrimVertex *newObject = new RescueTrimVertex(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueTrimVertex::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueTrimVertex::operator+=(RescueTrimVertex *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RescueTrimVertex **) realloc(objects, sizeof(RescueTrimVertex *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

void cSetRescueTrimVertex::Relinquish(RescueTrimVertex *existingObject)
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

RESCUEBOOL cSetRescueTrimVertex::operator-=(RescueTrimVertex *existingObject)
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

RescueTrimVertex *cSetRescueTrimVertex::ObjectNamed(const RESCUECHAR *mayBeName)
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

RescueTrimVertex *cSetRescueTrimVertex::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescueTrimVertex::operator-=(RESCUEINT64 ndx)
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

RescueTrimVertex *cSetRescueTrimVertex::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueTrimVertex::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueTrimVertex::Count(void)
{
  return (RESCUEINT32) count;
}

RESCUEINT32 cSetRescueTrimVertex::Count(RESCUEBOOL throwIfTrue)
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




