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

        cSetRescueBlock.cpp

 Keeps a list of pointers to some RescueBlock.

        Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueBlock.h"
#include "RescueBlock.h"

cSetRescueBlock::cSetRescueBlock()
{
  allocated = 10;
  count = 0;
  objects = (RescueBlock **) malloc(sizeof(RescueBlock *) * (size_t) allocated);
}

cSetRescueBlock::~cSetRescueBlock()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueBlock::DropWireframeMemory()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->DropWireframeMemory();
  }
}

void cSetRescueBlock::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->UnArchiveWireframeData(model, archiveFile);
  }
}

void cSetRescueBlock::RelinkWireframeData(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->RelinkWireframeData(parent);
  }
}

void cSetRescueBlock::ArchiveWireframeData(FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->ArchiveWireframeData(archiveFile);
  }
}

void cSetRescueBlock::PrepareModelDelete()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->PrepareModelDelete();
  }
}


RESCUEBOOL cSetRescueBlock::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
    myReturn = objects[loop]->AnyFileTruncated();
  }
  return myReturn;
}

void cSetRescueBlock::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueBlock::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Relink(parent);
  }
}

void cSetRescueBlock::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
    RescueBlock *newObject = new RescueBlock(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueBlock::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueBlock::operator+=(RescueBlock *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RescueBlock **) realloc(objects, sizeof(RescueBlock *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueBlock::operator-=(RescueBlock *existingObject)
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

RescueBlock *cSetRescueBlock::ObjectNamed(const RESCUECHAR *mayBeName)
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

RescueGeometry *cSetRescueBlock::GeometryIdentifiedBy(RESCUEINT64 identifier)
{
  RESCUEINT64 ndx = 0;
  RescueGeometry *myReturn = 0;

  while (ndx < count && myReturn == 0)
  {
    myReturn = objects[ndx++]->GeometryIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueBlock *cSetRescueBlock::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescueBlock::operator-=(RESCUEINT64 ndx)
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

RescueBlock *cSetRescueBlock::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueBlock::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueBlock::Count(void)
{
  return (RESCUEINT32) count;
}

void cSetRescueBlock::FindUniquePropertyNames(cSetString *container)
{
  int loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->FindUniquePropertyNames(container);
  }
}

RESCUEINT32 cSetRescueBlock::Count(RESCUEBOOL throwIfTrue)
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




