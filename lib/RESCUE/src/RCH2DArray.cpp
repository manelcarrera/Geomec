/************************************************************************

  RCH2DArray.cpp

  A RESCUEFLOAT array that can be allocated as a single array, but accessed via
  2 dimensions.  Doesn't free the array unless it created it.

  Rod Hanks   January, 1997

***********************************************************************/
#include "RCH2DArray.h"

RCH2DArray::RCH2DArray(RESCUEINT64 maxIin, RESCUEINT64 maxJin, RESCUEFLOAT *valuesIn)
              :maxI(maxIin)
              ,maxJ(maxJin)
              ,values(valuesIn)
{
  if (values == 0)
  {
  values = new RESCUEFLOAT[(size_t) (maxI * maxJ)];
  freeValues = TRUE;
  }
  else
  {
  freeValues = FALSE;
  }
}

RCH2DArray::RCH2DArray(RESCUEINT32 maxIin, RESCUEINT32 maxJin, RESCUEFLOAT *valuesIn)
              :maxI((RESCUEINT64) maxIin)
              ,maxJ((RESCUEINT64) maxJin)
              ,values(valuesIn)
{
  if (values == 0)
  {
  values = new RESCUEFLOAT[(size_t) (maxI * maxJ)];
  freeValues = TRUE;
  }
  else
  {
  freeValues = FALSE;
  }
}

void RCH2DArray::Set(RESCUEINT64 i, RESCUEINT64 j, RESCUEFLOAT value)
{
  Ndx(i, j) = value;
}

RESCUEFLOAT &RCH2DArray::Ndx(RESCUEINT64 i, RESCUEINT64 j)
{
  RESCUEINT64 ndx = (j * maxI)
         +  i;
  return values[ndx];
}

RESCUEFLOAT RCH2DArray::Value(RESCUEINT64 i, RESCUEINT64 j)
{
  RESCUEINT64 ndx = (j * maxI)
         +  i;
  return values[ndx];
}

RESCUEINT32 RCH2DArray::ArrayLength(RESCUEBOOL throwIfTrue)
{
  RESCUEINT64 count = ArrayLength64();
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




