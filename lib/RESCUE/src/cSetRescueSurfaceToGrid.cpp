/*************************************************************************

    cSetRescueSurfaceToGrid.cpp

 Keeps a list of pointers to some RescueSurfaceToGrid.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueSurfaceToGrid.h"
#include "RescueSurfaceToGrid.h"

cSetRescueSurfaceToGrid::cSetRescueSurfaceToGrid()
{
  allocated = 10;
  count = 0;
  objects = (RescueSurfaceToGrid **) malloc(sizeof(RescueSurfaceToGrid *) * (size_t) allocated);
}

cSetRescueSurfaceToGrid::~cSetRescueSurfaceToGrid()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  free(objects);
}

RESCUEBOOL cSetRescueSurfaceToGrid::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
  myReturn = objects[loop]->AnyFileTruncated();
  }
  return myReturn;
}

void cSetRescueSurfaceToGrid::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueSurfaceToGrid::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Relink(parent);
  }
}

void cSetRescueSurfaceToGrid::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
  RescueSurfaceToGrid *newObject = new RescueSurfaceToGrid(context, archiveFile);
  (*this) += newObject;
  }
}

void cSetRescueSurfaceToGrid::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  count = 0;
}

void cSetRescueSurfaceToGrid::operator+=(RescueSurfaceToGrid *newObject)
{
  if (allocated == count)
  {
  allocated += 10;
  objects = (RescueSurfaceToGrid **) realloc(objects, sizeof(RescueSurfaceToGrid *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueSurfaceToGrid::operator-=(RescueSurfaceToGrid *existingObject)
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

RescueSurfaceToGrid *cSetRescueSurfaceToGrid::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescueSurfaceToGrid::operator-=(RESCUEINT64 ndx)
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

RescueSurfaceToGrid *cSetRescueSurfaceToGrid::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueSurfaceToGrid::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueSurfaceToGrid::Count(void)
{
  return (RESCUEINT32) count;
}

void cSetRescueSurfaceToGrid::FindUniquePropertyNames(cSetString *container)
{
  int loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->FindUniquePropertyNames(container);
  }
}

RESCUEINT32 cSetRescueSurfaceToGrid::Count(RESCUEBOOL throwIfTrue)
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




