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

    cBagRescuePolyLineStub.h

 Keeps a list of pointers to RescuePolyLineStub.

    Rod Hanks               March 2001

****************************************************************************/
#include "myHeaders.h"
#include "cBagRescuePolyLineStub.h"
#include "RescuePolyLineStub.h"
#include "RescueModel.h"

cBagRescuePolyLineStub::cBagRescuePolyLineStub()
{
  tree = new RescueTree();
}

cBagRescuePolyLineStub::~cBagRescuePolyLineStub()
{
  delete tree;
}

void cBagRescuePolyLineStub::operator+=(RescuePolyLineStub *newObject)
{
  tree->Add(newObject);
}

RESCUEBOOL cBagRescuePolyLineStub::operator-=(RescuePolyLineStub * existingObject)
{
  return tree->Delete(existingObject);
}

RescuePolyLineStub *cBagRescuePolyLineStub::NthObject(RESCUEINT64 ordinal)
{
  return (RescuePolyLineStub *) tree->NthObject(ordinal);
}

RESCUEINT32 cBagRescuePolyLineStub::Count(RESCUEBOOL throwIfTrue)
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




