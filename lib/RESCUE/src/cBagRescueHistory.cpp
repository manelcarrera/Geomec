/*************************************************************************

    cBagRescueHistory.h

 Keeps a list of pointers to RescueHistory.

    Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "myHeaders.h"
#include "cBagRescueHistory.h"
#include "RescueHistory.h"
#include "RescueModel.h"

cBagRescueHistory::cBagRescueHistory()
{
  tree = new RescueTree();
}

cBagRescueHistory::~cBagRescueHistory()
{
  delete tree;
}

void cBagRescueHistory::operator+=(RescueHistory *newObject)
{
  tree->Add(newObject);
}

RESCUEBOOL cBagRescueHistory::operator-=(RescueHistory * existingObject)
{
  return tree->Delete(existingObject);
}

RescueHistory *cBagRescueHistory::NthObject(RESCUEINT64 ordinal)
{
  return (RescueHistory *) tree->NthObject(ordinal);
}

RESCUEINT32 cBagRescueHistory::Count(RESCUEBOOL throwIfTrue)
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




