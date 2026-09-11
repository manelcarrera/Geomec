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

    cSetRescuePolyLineNode.cpp

 Keeps a list of pointers to some RescuePolyLineNode.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescuePolyLineNode.h"
#include "RescuePolyLineNode.h"

cSetRescuePolyLineNode::cSetRescuePolyLineNode(RescuePolyLine *parentIn)
{
  allocated = 10;
  count = 0;
  objects = (RescuePolyLineNode **) malloc(sizeof(RescuePolyLineNode *) * (size_t) allocated);
  parent = parentIn;
}

cSetRescuePolyLineNode::~cSetRescuePolyLineNode()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  free(objects);
}

void cSetRescuePolyLineNode::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  if (context->FileVersion() >= 27)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) objects[loop]->IsA());
  }
  objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescuePolyLineNode::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  objects[loop]->Relink(parent);
  }
}

void cSetRescuePolyLineNode::UnArchive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
  if (context->ReadFileVersion() >= 27)
  {
      RESCUEINT64 flag;
      myfscanf(context, archiveFile, &flag);
      RescuePolyLineNode *newObject = new RescuePolyLineNode(context, archiveFile);
      (*this) += newObject;
      if (flag == R_RescuePolyLineNodeAvatar)
      {
    RESCUEINT64 fakeID;
    myfscanf(context, archiveFile, &fakeID);
      }
/*
  If it is an avatar consume the id that was written after the object.
*/
  }
  else
  {
      RescuePolyLineNode *newObject = new RescuePolyLineNode(context, archiveFile);
      (*this) += newObject;
  }
  }
}

void cSetRescuePolyLineNode::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
  delete objects[loop];
  }
  count = 0;
}

RESCUEBOOL cSetRescuePolyLineNode::operator+=(RescuePolyLineNode *newObject)
{
  RESCUEBOOL myReturn = TRUE;
  if (newObject->IsA() != R_RescuePolyLineNode
  &&  newObject->IsA() != R_RescuePolyLineNodeAvatar)
  {
  myReturn = FALSE;
  }
  else
  {
  if (allocated == count)
  {
      allocated += 10;
      objects = (RescuePolyLineNode **) realloc(objects, sizeof(RescuePolyLineNode *) * (size_t) allocated);
  }
  objects[count++] = newObject;
  }
  return myReturn;
}

RESCUEBOOL cSetRescuePolyLineNode::operator-=(RescuePolyLineNode *existingObject)
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

RescuePolyLineNode *cSetRescuePolyLineNode::ObjectNamed(const RESCUECHAR *mayBeName)
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

RescuePolyLineNode *cSetRescuePolyLineNode::ObjectIdentifiedBy(RESCUEINT64 identifier)
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

RESCUEBOOL cSetRescuePolyLineNode::operator-=(RESCUEINT64 ndx)
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

RescuePolyLineNode *cSetRescuePolyLineNode::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescuePolyLineNode::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescuePolyLineNode::Count(void)
{
  return (RESCUEINT32) count;
}

RESCUEINT32 cSetRescuePolyLineNode::Count(RESCUEBOOL throwIfTrue)
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




