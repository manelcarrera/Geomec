/*************************************************************************

    cBagRescueSurfaceToGrid.h

 Keeps a list of pointers to RescueSurfaceToGrid.

    Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "myHeaders.h"
#include "cBagRescueSurfaceToGrid.h"
#include "RescueSurfaceToGrid.h"
#include "RescueModel.h"

cBagRescueSurfaceToGrid::cBagRescueSurfaceToGrid()
{
  tree = new RescueTree();
}

cBagRescueSurfaceToGrid::~cBagRescueSurfaceToGrid()
{
  delete tree;
}

void cBagRescueSurfaceToGrid::operator+=(RescueSurfaceToGrid *newObject)
{
  tree->Add(newObject);
}

RESCUEBOOL cBagRescueSurfaceToGrid::operator-=(RescueSurfaceToGrid * existingObject)
{
  return tree->Delete(existingObject);
}

RescueSurfaceToGrid *cBagRescueSurfaceToGrid::NthObject(RESCUEINT64 ordinal)
{
  return (RescueSurfaceToGrid *) tree->NthObject(ordinal);
}

RESCUEINT32 cBagRescueSurfaceToGrid::Count(RESCUEBOOL throwIfTrue)
{
  if (tree->Count() > 2147483647)
  {
  if (throwIfTrue)
  {
      throw "Model is too large to be accessed in 32 bit mode.";
  }
  return 0;
  }
  else
  {
  return (RESCUEINT32) tree->Count();
  }
}



