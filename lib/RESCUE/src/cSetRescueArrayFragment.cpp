/*************************************************************************

        cSetRescueArrayFragment.cpp

 Keeps a list of pointers to some RescueArrayFragment.

        Rod Hanks               December, 2000

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueArrayFragment.h"
#include "RescueArrayFragment.h"

cSetRescueArrayFragment::cSetRescueArrayFragment()
{
  allocated = 10;
  count = 0;
  objects = (RescueArrayFragment **) malloc(sizeof(RescueArrayFragment *) * (size_t) allocated);
}

cSetRescueArrayFragment::~cSetRescueArrayFragment()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueArrayFragment::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueArrayFragment::operator+=(RescueArrayFragment *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RescueArrayFragment **) realloc(objects, sizeof(RescueArrayFragment *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueArrayFragment::operator-=(RescueArrayFragment *existingObject)
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

RESCUEBOOL cSetRescueArrayFragment::operator-=(RESCUEINT64 ndx)
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

RescueArrayFragment *cSetRescueArrayFragment::NthObject(RESCUEINT64 ordinal)
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

RESCUEINT64 cSetRescueArrayFragment::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueArrayFragment::Count(void)
{
  return (RESCUEINT32) count;
}

RESCUEINT32 cSetRescueArrayFragment::Count(RESCUEBOOL throwIfTrue)
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




