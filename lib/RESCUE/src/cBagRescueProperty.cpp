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

        cBagRescueProperty.h

 Keeps a list of pointers to RescueProperty.

        Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "myHeaders.h"
#include "cBagRescueProperty.h"
#include "RescueProperty.h"
#include "RescueModel.h"

cBagRescueProperty::cBagRescueProperty()
{
  tree = new RescueTree();
}

cBagRescueProperty::~cBagRescueProperty()
{
  delete tree;
}

void cBagRescueProperty::operator+=(RescueProperty *newObject)
{
  tree->Add(newObject);
}

RESCUEBOOL cBagRescueProperty::operator-=(RescueProperty * existingObject)
{
  return tree->Delete(existingObject);
}

RescueProperty *cBagRescueProperty::NthObject(RESCUEINT64 ordinal)
{
  return (RescueProperty *) tree->NthObject(ordinal);
}

RescueProperty *cBagRescueProperty::PropertyNamed(RESCUECHAR *propertyName)
{
  RescueProperty *myReturn = 0;
  int ordinal = 0;
  RescueProperty *candidate = (RescueProperty *) tree->NthObject(ordinal++);
  while (myReturn == 0 && candidate != 0)
  {
    if (candidate->IsNamed(propertyName))
    {
      myReturn = candidate;
    }
    else
    {
      candidate = (RescueProperty *) tree->NthObject(ordinal++);
    }
  }
  return myReturn;
}

RESCUEINT32 cBagRescueProperty::Count(RESCUEBOOL throwIfTrue)
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



