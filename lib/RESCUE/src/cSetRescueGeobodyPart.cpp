/*************************************************************************

        cSetRescueGeobodyPart.cpp

 Keeps a list of pointers to some RescueGeobodyPart.

        Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueGeobodyPart.h"
#include "RescueGeobodyPart.h"

void cSetRescueGeobodyPart::DropWireframeMemory()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->DropWireframeMemory();
  }
}

void cSetRescueGeobodyPart::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->UnArchiveWireframeData(model, archiveFile);
  }
}

void cSetRescueGeobodyPart::RelinkWireframeData(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->RelinkWireframeData(parent);
  }
}

void cSetRescueGeobodyPart::ArchiveWireframeData(FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->ArchiveWireframeData(archiveFile);
  }
}

cSetRescueGeobodyPart::cSetRescueGeobodyPart()
{
  allocated = 10;
  count = 0;
  objects = (RescueGeobodyPart **) malloc(sizeof(RescueGeobodyPart *) * (size_t) allocated);
}

cSetRescueGeobodyPart::~cSetRescueGeobodyPart()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  free(objects);
}

RESCUEBOOL cSetRescueGeobodyPart::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
    myReturn = objects[loop]->AnyFileTruncated();
  }
  return myReturn;
}

RescueGeometry *cSetRescueGeobodyPart::GeometryIdentifiedBy(RESCUEINT64 identifier)
{
  RESCUEINT64 ndx = 0;
  RescueGeometry *myReturn = 0;

  while (ndx < count && myReturn == 0)
  {
    myReturn = objects[ndx++]->GeometryIdentifiedBy(identifier);
  }
  return myReturn;
}

void cSetRescueGeobodyPart::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueGeobodyPart::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Relink(parent);
  }
}

void cSetRescueGeobodyPart::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
    RescueGeobodyPart *newObject = new RescueGeobodyPart(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueGeobodyPart::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueGeobodyPart::operator+=(RescueGeobodyPart *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RescueGeobodyPart **) realloc(objects, sizeof(RescueGeobodyPart *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueGeobodyPart::operator-=(RescueGeobodyPart *existingObject)
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

RescueGeobodyPart *cSetRescueGeobodyPart::ObjectNamed(const RESCUECHAR *mayBeName)
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

RescueGeobodyPart *cSetRescueGeobodyPart::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescueGeobodyPart::operator-=(RESCUEINT64 ndx)
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

RescueGeobodyPart *cSetRescueGeobodyPart::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueGeobodyPart::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueGeobodyPart::Count(void)
{
  return (RESCUEINT32) count;
}

void cSetRescueGeobodyPart::FindUniquePropertyNames(cSetString *container)
{
  int loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->FindUniquePropertyNames(container);
  }
}

RESCUEINT32 cSetRescueGeobodyPart::Count(RESCUEBOOL throwIfTrue)
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



