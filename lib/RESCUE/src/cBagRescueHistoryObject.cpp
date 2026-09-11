/*************************************************************************

        cBagRescueHistoryObject.h

 Keeps a list of pointers to RescueHistoryObject.

        Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "myHeaders.h"
#include "cBagRescueHistoryObject.h"
#include "RescueHistoryObject.h"
#include "RescueModel.h"

cBagRescueHistoryObject::cBagRescueHistoryObject()
{
  tree = new RescueTree();
}

cBagRescueHistoryObject::~cBagRescueHistoryObject()
{
  delete tree;
}

void cBagRescueHistoryObject::operator+=(RescueHistoryObject *newObject)
{
  tree->Add(newObject);
}

RESCUEBOOL cBagRescueHistoryObject::operator-=(RescueHistoryObject * existingObject)
{
  return tree->Delete(existingObject);
}

RescueHistoryObject *cBagRescueHistoryObject::NthObject(RESCUEINT64 ordinal)
{
  return (RescueHistoryObject *) tree->NthObject(ordinal);
}

RESCUEINT32 cBagRescueHistoryObject::Count(RESCUEBOOL throwIfTrue)
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




