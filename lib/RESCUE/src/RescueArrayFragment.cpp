/********************************************************************

  RescueArrayFragment.cpp

  A dynamically sized array handler for part of an 2-3 dimensional grid of
  values for RESCUE's data model. The related grid gives
  the dimensionality.  We keep lowbound and count for each grid.

  This abstract superclass can be instantiated as one of it's subclasses.

  Rod Hanks,  December 2000

*********************************************************************/
#include <string.h>
#include "RescueModel.h"
#include "RescueArrayFragment.h"

RESCUEINT64 RescueArrayFragment::NodeCount64()
{
  RESCUEINT64 myReturn = iCount * jCount;
  if (dimensions == 3)
  {
    myReturn *= kCount;
  }
  return myReturn;
}



