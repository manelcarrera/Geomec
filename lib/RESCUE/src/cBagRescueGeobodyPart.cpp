/*************************************************************************

        cBagRescueGeobodyPart.h

 Keeps a list of pointers to RescueGeobodyPart.

        Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "myHeaders.h"
#include "cBagRescueGeobodyPart.h"
#include "RescueGeobodyPart.h"
#include "RescueModel.h"

cBagRescueGeobodyPart::cBagRescueGeobodyPart()
{
  tree = new RescueTree();
}

cBagRescueGeobodyPart::~cBagRescueGeobodyPart()
{
  delete tree;
}

void cBagRescueGeobodyPart::operator+=(RescueGeobodyPart *newObject)
{
  tree->Add(newObject);
}

RESCUEBOOL cBagRescueGeobodyPart::operator-=(RescueGeobodyPart * existingObject)
{
  return tree->Delete(existingObject);
}

RescueGeobodyPart *cBagRescueGeobodyPart::NthObject(RESCUEINT64 ordinal)
{
  return (RescueGeobodyPart *) tree->NthObject(ordinal);
}

RESCUEINT32 cBagRescueGeobodyPart::Count(RESCUEBOOL throwIfTrue)
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




