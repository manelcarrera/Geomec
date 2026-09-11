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

    cSetRescuePolyLineNodeUV.cpp

 Keeps a list of pointers to some RescuePolyLineNodeUV.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescuePolyLineNodeUV.h"
#include "RescuePolyLineNodeUV.h"

cSetRescuePolyLineNodeUV::cSetRescuePolyLineNodeUV()
{
  allocated = 10;
  count = 0;
  objects = (RescuePolyLineNodeUV **) malloc(sizeof(RescuePolyLineNodeUV *) * (size_t) allocated);
}

cSetRescuePolyLineNodeUV::~cSetRescuePolyLineNodeUV()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  free(objects);
}

void cSetRescuePolyLineNodeUV::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescuePolyLineNodeUV::SetOrientation(RescueOrientationLedger *ledger)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->SetOrientation(ledger);
  }
}

void cSetRescuePolyLineNodeUV::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Relink(parent);
  }
}

void cSetRescuePolyLineNodeUV::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
  RescuePolyLineNodeUV *newObject = new RescuePolyLineNodeUV(context, archiveFile);
  (*this) += newObject;
  }
}

void cSetRescuePolyLineNodeUV::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
  if (objects[loop] != 0)
  {
      delete objects[loop];
  }
  }
  count = 0;
}

void cSetRescuePolyLineNodeUV::InsertAt(RESCUEINT64 ndx, RescuePolyLineNodeUV *newObject)
{
  if (ndx >= allocated)
  {
  allocated = ndx + 5;
  objects = (RescuePolyLineNodeUV **) realloc(objects, sizeof(RescuePolyLineNodeUV *) * (size_t) allocated);
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
void cSetRescuePolyLineNodeUV::operator+=(RescuePolyLineNodeUV *newObject)
{
  if (allocated == count)
  {
  allocated += 10;
  objects = (RescuePolyLineNodeUV **) realloc(objects, sizeof(RescuePolyLineNodeUV *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescuePolyLineNodeUV::operator-=(RescuePolyLineNodeUV *existingObject)
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

RescuePolyLineNodeUV *cSetRescuePolyLineNodeUV::ObjectNamed(const RESCUECHAR *mayBeName)
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

RescuePolyLineNodeUV *cSetRescuePolyLineNodeUV::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescuePolyLineNodeUV::operator-=(RESCUEINT64 ndx)
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

RescuePolyLineNodeUV *cSetRescuePolyLineNodeUV::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescuePolyLineNodeUV::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescuePolyLineNodeUV::Count(void)
{
  return (RESCUEINT32) count;
}

RESCUEINT32 cSetRescuePolyLineNodeUV::Count(RESCUEBOOL throwIfTrue)
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





