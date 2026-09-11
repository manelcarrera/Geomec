/*************************************************************************

    cSetRescueTriangleVertex.cpp

 Keeps a list of pointers to some RescueTriangleVertex.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueTriangleVertex.h"
#include "RescueTriangleVertex.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

cSetRescueTriangleVertex::cSetRescueTriangleVertex()
{
  allocated = 10;
  count = 0;
  objects = (RescueTriangleVertex **) malloc(sizeof(RescueTriangleVertex *) * (size_t) allocated);
}

cSetRescueTriangleVertex::~cSetRescueTriangleVertex()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  free(objects);
}

void cSetRescueTriangleVertex::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->ndx = loop;
  objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescueTriangleVertex::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Relink(parent);
  }
}

void cSetRescueTriangleVertex::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
  RescueTriangleVertex *newObject = new RescueTriangleVertex(context, archiveFile);
  (*this) += newObject;
  }
}

void cSetRescueTriangleVertex::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  count = 0;
}

void cSetRescueTriangleVertex::operator+=(RescueTriangleVertex *newObject)
{
  if (allocated == count)
  {
  allocated += 10;
  objects = (RescueTriangleVertex **) realloc(objects, sizeof(RescueTriangleVertex *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEINT64 cSetRescueTriangleVertex::IndexOf(RescueTriangleVertex *existingObject)
{
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 ndx = 0;
  while (ndx < count && myReturn < 0)
  {
  if (existingObject == objects[ndx])
  {
      myReturn = ndx;
  }
  else
  {
      ndx++;
  }
  }
  return myReturn;
}

RESCUEBOOL cSetRescueTriangleVertex::operator-=(RescueTriangleVertex *existingObject)
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

RESCUEBOOL cSetRescueTriangleVertex::operator-=(RESCUEINT64 ndx)
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

RescueTriangleVertex *cSetRescueTriangleVertex::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueTriangleVertex::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueTriangleVertex::Count(void)
{
  return (RESCUEINT32) count;
}

RESCUEINT32 cSetRescueTriangleVertex::Count(RESCUEBOOL throwIfTrue)
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




