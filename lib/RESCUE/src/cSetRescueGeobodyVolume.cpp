/*************************************************************************

        cSetRescueGeobodyVolume.cpp

 Keeps a list of pointers to some RescueGeobodyVolume.

        Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueGeobodyVolume.h"
#include "RescueGeobodyVolume.h"

void cSetRescueGeobodyVolume::DropWireframeMemory()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->DropWireframeMemory();
  }
}

void cSetRescueGeobodyVolume::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->UnArchiveWireframeData(model, archiveFile);
  }
}

void cSetRescueGeobodyVolume::RelinkWireframeData(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->RelinkWireframeData(parent);
  }
}

void cSetRescueGeobodyVolume::ArchiveWireframeData(FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->ArchiveWireframeData(archiveFile);
  }
}

cSetRescueGeobodyVolume::cSetRescueGeobodyVolume()
{
  allocated = 10;
  count = 0;
  objects = (RescueGeobodyVolume **) malloc(sizeof(RescueGeobodyVolume *) * (size_t) allocated);
}

cSetRescueGeobodyVolume::~cSetRescueGeobodyVolume()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueGeobodyVolume::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueGeobodyVolume::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Relink(parent);
  }
}

void cSetRescueGeobodyVolume::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
    RescueGeobodyVolume *newObject = new RescueGeobodyVolume(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueGeobodyVolume::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueGeobodyVolume::operator+=(RescueGeobodyVolume *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RescueGeobodyVolume **) realloc(objects, sizeof(RescueGeobodyVolume *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueGeobodyVolume::operator-=(RescueGeobodyVolume *existingObject)
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

RescueGeobodyVolume *cSetRescueGeobodyVolume::ObjectNamed(const RESCUECHAR *mayBeName)
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

RescueGeobodyVolume *cSetRescueGeobodyVolume::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescueGeobodyVolume::operator-=(RESCUEINT64 ndx)
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

RescueGeobodyVolume *cSetRescueGeobodyVolume::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueGeobodyVolume::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueGeobodyVolume::Count(void)
{
  return (RESCUEINT32) count;
}

void cSetRescueGeobodyVolume::FindUniquePropertyNames(cSetString *container)
{
  int loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->FindUniquePropertyNames(container);
  }
}

RESCUEINT32 cSetRescueGeobodyVolume::Count(RESCUEBOOL throwIfTrue)
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




