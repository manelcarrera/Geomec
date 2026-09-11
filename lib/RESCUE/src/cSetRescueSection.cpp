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

    cSetRescueSection.cpp

 Keeps a list of pointers to some RescueSection.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueSection.h"
#include "RescueSection.h"

cSetRescueSection::cSetRescueSection()
{
  allocated = 10;
  count = 0;
  objects = (RescueSection **) malloc(sizeof(RescueSection *) * (size_t) allocated);
}

cSetRescueSection::~cSetRescueSection()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  free(objects);
}

RESCUEBOOL cSetRescueSection::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
  myReturn = objects[loop]->AnyFileTruncated();
  }
  return myReturn;
}

void cSetRescueSection::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueSection::DropWireframeMemory()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->DropWireframeMemory();
  }
}

void cSetRescueSection::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->UnArchiveWireframeData(model, archiveFile);
  }
}

void cSetRescueSection::RelinkWireframeData(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->RelinkWireframeData(parent);
  }
}

void cSetRescueSection::ArchiveWireframeData(FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->ArchiveWireframeData(archiveFile);
  }
}

void cSetRescueSection::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Relink(parent);
  }
}

void cSetRescueSection::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
  RescueSection *newObject = new RescueSection(context, archiveFile);
  (*this) += newObject;
  }
}

void cSetRescueSection::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  count = 0;
}

void cSetRescueSection::operator+=(RescueSection *newObject)
{
  if (allocated == count)
  {
  allocated += 10;
  objects = (RescueSection **) realloc(objects, sizeof(RescueSection *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueSection::operator-=(RescueSection *existingObject)
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

RescueSection *cSetRescueSection::ObjectNamed(const RESCUECHAR *mayBeName)
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

RescueSection *cSetRescueSection::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescueSection::operator-=(RESCUEINT64 ndx)
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

RescueSection *cSetRescueSection::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueSection::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueSection::Count(void)
{
  return (RESCUEINT32) count;
}

void cSetRescueSection::FindUniquePropertyNames(cSetString *container)
{
  int loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->FindUniquePropertyNames(container);
  }
}

RESCUEINT32 cSetRescueSection::Count(RESCUEBOOL throwIfTrue)
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





