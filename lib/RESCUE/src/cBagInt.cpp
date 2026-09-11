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

    cBagInt.h

 Keeps a list of integers.

    Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "myHeaders.h"
#include "cBagInt.h"
#include "RescueModel.h"

cBagInt::cBagInt()
{
  allocated = 10;
  count = 0;
  objects = (RESCUEINT64 *) malloc(sizeof(RESCUEINT64) * (size_t) allocated);
}

cBagInt::~cBagInt()
{
  free(objects);
}

void cBagInt::operator+=(RESCUEINT64 newObject)
{
  if (allocated == count)
  {
  allocated += 10;
  objects = (RESCUEINT64 *) realloc(objects, sizeof(RESCUEINT64) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cBagInt::operator-=(RESCUEINT64 existingObject)
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

RESCUEINT64 cBagInt::NthObject(RESCUEINT64 ordinal)
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

RESCUEBOOL cBagInt::Contains(RESCUEINT64 example)
{
  RESCUEBOOL myReturn = FALSE;
  if (count > 0)
  {
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
      if (objects[loop] == example)
      {
    myReturn = TRUE;
      }
  }
  }
  return myReturn;
}




