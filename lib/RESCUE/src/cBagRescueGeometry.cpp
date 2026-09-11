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

        cBagRescueGeometry.h

 Keeps a list of pointers to RescueGeometry.

        Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "myHeaders.h"
#include "cBagRescueGeometry.h"
#include "RescueGeometry.h"
#include "RescueModel.h"

cBagRescueGeometry::cBagRescueGeometry()
{
  tree = new RescueTree();
}

cBagRescueGeometry::~cBagRescueGeometry()
{
  delete tree;
}

void cBagRescueGeometry::operator+=(RescueGeometry *newObject)
{
  tree->Add(newObject);
}

RESCUEBOOL cBagRescueGeometry::operator-=(RescueGeometry * existingObject)
{
  return tree->Delete(existingObject);
}

RescueGeometry *cBagRescueGeometry::NthObject(RESCUEINT64 ordinal)
{
  return (RescueGeometry *) tree->NthObject(ordinal);
}

RESCUEINT32 cBagRescueGeometry::Count(RESCUEBOOL throwIfTrue)
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



