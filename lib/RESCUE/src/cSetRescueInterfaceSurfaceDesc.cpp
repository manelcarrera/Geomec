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

        cSetRescueInterfaceSurfaceDesc.cpp

 Keeps a list of pointers to some RescueInterfaceSurfaceDesc.

        Rod Hanks               February, 1999

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueInterfaceSurfaceDesc.h"
#include "RescueInterfaceSurfaceDesc.h"

cSetRescueInterfaceSurfaceDesc::cSetRescueInterfaceSurfaceDesc()
{
  allocated = 10;
  count = 0;
  objects = (RescueInterfaceSurfaceDesc **) malloc(sizeof(RescueInterfaceSurfaceDesc *) * (size_t) allocated);
}

cSetRescueInterfaceSurfaceDesc::~cSetRescueInterfaceSurfaceDesc()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueInterfaceSurfaceDesc::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Relink((RescueGeometry *) parent);
  }
}

RESCUEBOOL cSetRescueInterfaceSurfaceDesc::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
    myReturn = objects[loop]->AnyFileTruncated();
  }
  return myReturn;
}

void cSetRescueInterfaceSurfaceDesc::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescueInterfaceSurfaceDesc::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
    RescueInterfaceSurfaceDesc *newObject = new RescueInterfaceSurfaceDesc(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueInterfaceSurfaceDesc::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueInterfaceSurfaceDesc::operator+=(RescueInterfaceSurfaceDesc *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RescueInterfaceSurfaceDesc **) realloc(objects, sizeof(RescueInterfaceSurfaceDesc *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueInterfaceSurfaceDesc::operator-=(RescueInterfaceSurfaceDesc *existingObject)
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

RESCUEBOOL cSetRescueInterfaceSurfaceDesc::operator-=(RESCUEINT64 ndx)
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

RescueInterfaceSurfaceDesc *cSetRescueInterfaceSurfaceDesc::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueInterfaceSurfaceDesc::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueInterfaceSurfaceDesc::Count(void)
{
  return (RESCUEINT32) count;
}

void cSetRescueInterfaceSurfaceDesc::FindUniquePropertyNames(cSetString *container)
{
  int loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->FindUniquePropertyNames(container);
  }
}

RESCUEINT32 cSetRescueInterfaceSurfaceDesc::Count(RESCUEBOOL throwIfTrue)
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







