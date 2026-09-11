/*************************************************************************

    cBagRescueStairSteppedFault.h

 Keeps a list of pointers to RescueStairSteppedFault.

    Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "cBagRescueStairSteppedFault.h"
#include "RescueModel.h"
#include "RescueStairSteppedFault.h"
#include "myHeaders.h"

cBagRescueStairSteppedFault::cBagRescueStairSteppedFault() { tree = new RescueTree(); }

cBagRescueStairSteppedFault::~cBagRescueStairSteppedFault() { delete tree; }

void cBagRescueStairSteppedFault::operator+=(RescueStairSteppedFault *newObject) { tree->Add(newObject); }

RESCUEBOOL cBagRescueStairSteppedFault::operator-=(RescueStairSteppedFault *existingObject) {
  return tree->Delete(existingObject);
}

RescueStairSteppedFault *cBagRescueStairSteppedFault::NthObject(RESCUEINT64 ordinal) {
  return (RescueStairSteppedFault *)tree->NthObject(ordinal);
}

RESCUEINT32 cBagRescueStairSteppedFault::Count(RESCUEBOOL throwIfTrue) {
  if (tree->Count() > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)tree->Count();
  }
}
