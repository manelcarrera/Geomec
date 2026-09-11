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

        cSetString.h

 Keeps a list of integers.

        Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "myHeaders.h"
#include "cSetString.h"
#include "RescueModel.h"
#include "RCHString.h"

cSetString::cSetString()
{
  allocated = 10;
  count = 0;
  objects = (RCHString **) malloc(sizeof(RCHString *) * (size_t) allocated);
}

cSetString::~cSetString()
{
  EmptySelf();
  free(objects);
}

void cSetString::EmptySelf()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetString::AddIfUnique(const RESCUECHAR *toAdd)
{
  if (toAdd != 0)
  {
    RESCUEBOOL found = FALSE;
    if (count > 0)
    {
      RESCUEINT64 loop;
      for (loop = 0; loop < count && found == FALSE; loop++)
      {
        if ((*objects[loop]) == toAdd)
        {
          found = TRUE;
        }
      }
    }
    if (found == FALSE)
    {
      RCHString *toAddObj = new RCHString(toAdd);
      (*this) += toAddObj;
    }
  }
}

void cSetString::operator+=(RCHString *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RCHString **) realloc(objects, sizeof(RCHString *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetString::operator-=(RCHString *existingObject)
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
  return found;
}

RCHString *cSetString::NthObject(RESCUEINT64 ordinal)
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

RESCUEBOOL cSetString::Contains(RCHString *example)
{
  RESCUEBOOL myReturn = FALSE;
  if (count > 0)
  {
    RESCUEINT64 loop;
    for (loop = 0; loop < count && myReturn == FALSE; loop++)
    {
      if ((*objects[loop]) == (*example))
      {
        myReturn = TRUE;
      }
    }
  }
  return myReturn;
}

RESCUEINT32 cSetString::Count(RESCUEBOOL throwIfTrue)
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



