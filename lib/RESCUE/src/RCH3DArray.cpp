/************************************************************************

  RCH3DArray.cpp

  A RESCUEFLOAT array that can be allocated as a single array, but accessed via
  3 dimensions.  Doesn't free the array unless it created it.

  Rod Hanks   January, 1997

***********************************************************************/
#include "RCH3DArray.h"

RCH3DArray::RCH3DArray(RESCUEINT64 maxIin, RESCUEINT64 maxJin, RESCUEINT64 maxKin, RESCUEFLOAT *valuesIn)
    : maxI(maxIin), maxJ(maxJin), maxK(maxKin), values(valuesIn) {
  if (values == 0) {
    values = new RESCUEFLOAT[(size_t)(maxI * maxJ * maxK)];
    freeValues = TRUE;
  } else {
    freeValues = FALSE;
  }
}

RCH3DArray::RCH3DArray(RESCUEINT32 maxIin, RESCUEINT32 maxJin, RESCUEINT32 maxKin, RESCUEFLOAT *valuesIn)
    : maxI((RESCUEINT64)maxIin), maxJ((RESCUEINT64)maxJin), maxK((RESCUEINT64)maxKin), values(valuesIn) {
  if (values == 0) {
    values = new RESCUEFLOAT[(size_t)(maxI * maxJ * maxK)];
    freeValues = TRUE;
  } else {
    freeValues = FALSE;
  }
}

void RCH3DArray::Set(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT value) { Ndx(i, j, k) = value; }

void RCH3DArray::Set(RESCUEINT32 i, RESCUEINT32 j, RESCUEINT32 k, RESCUEFLOAT value) { Ndx(i, j, k) = value; }

RESCUEFLOAT &RCH3DArray::Ndx(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k) {
  RESCUEINT64 ndx = (k * (maxJ * maxI)) + (j * maxI) + i;
  return values[ndx];
}

RESCUEFLOAT RCH3DArray::Value(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k) {
  RESCUEINT64 ndx = (k * (maxJ * maxI)) + (j * maxI) + i;
  return values[ndx];
}

RESCUEINT32 RCH3DArray::ArrayLength(RESCUEBOOL throwIfTrue) {
  RESCUEINT64 count = ArrayLength64();
  if (count > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)count;
  }
}
