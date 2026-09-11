/********************************************************************

  RescueParametricLGRMap.cpp

  For explicit (unconstrained) LGRs, a map between parent and
  child.  Knowing the fondness of Rescue members for arrays as
  opposed to classes I have constructed this map as a set of
  integer arrays.

  cellToMapCount, lgrIndx, lgrJndx, lgrKndx, parentNdx, parentCount
  contain data about LGR (refining) cells.

  parentMapCount, parentIndx, parentJndx, parentKndx, lgrNdx
  contain data about the parent geometry (the one that is one
  step up the ancestor tree, not necessarily the founding BUG).

  Each has an index into the other so that the map can be traversed
  in both directions.

  Rod Hanks,  June 2003

*********************************************************************/
#include <stdio.h>
#include <memory.h>
#include "myHeaders.h"
#include "RescueContext.h"
#include "RescueParametricLGRMap.h"
#include "RescueGeometry.h"
#include "RescueBuffer.h"
#include "cSetRescueDataContainer.h"
#include "RescueModel.h"
#include <string.h>

RescueParametricLGRMap::RescueParametricLGRMap(RescueGeometry *lgr,
                                               RESCUEINT64 expectedRefined, 
                                               RESCUEINT64 expectedParent,
                                               RESCUEINT64 refinedRealloc,
                                               RESCUEINT64 parentRealloc)
                                               :refinedReallocFactor(refinedRealloc)
      ,refinedAllocated(expectedRefined)
                                               ,cellToCellMapCount(0)
                                               ,lgrIndx(0)
                                               ,lgrJndx(0)
                                               ,lgrKndx(0)
                                               ,parentNdx(0)
                                               ,parentCount(0)
                                               ,parentReallocFactor(parentRealloc)
                                               ,parentAllocated(expectedParent)
                                               ,parentMapCount(0)
                                               ,parentIndx(0)
                                               ,parentJndx(0)
                                               ,parentKndx(0)
                                               ,lgrNdx(0)
                                               ,lgrGeometry(lgr)
                                               ,properties(0)
                                               ,propertyContainerId(0)
{
  lgr->parametricMap = this;
  if (expectedRefined > 0)
  {
  lgrIndx = new RESCUEINT64[(size_t) expectedRefined];
  lgrJndx = new RESCUEINT64[(size_t) expectedRefined];
  lgrKndx = new RESCUEINT64[(size_t) expectedRefined];
  parentNdx = new RESCUEINT64[(size_t) expectedRefined];
  parentCount = new RESCUEINT64[(size_t) expectedRefined];
  }
  if (expectedParent > 0)
  {
  parentIndx = new RESCUEINT64[(size_t) parentAllocated];
  parentJndx = new RESCUEINT64[(size_t) parentAllocated];
  parentKndx = new RESCUEINT64[(size_t) parentAllocated];
  lgrNdx = new RESCUEINT64[(size_t) parentAllocated];
  }
  if (refinedRealloc < 1 || parentRealloc < 1)
  {
  throw "reallocation factors cannot be less than one";
  }
}

RescueParametricLGRMap::~RescueParametricLGRMap()
{
  if (lgrIndx != 0)
  {
  delete [] lgrIndx;
  }
  if (lgrJndx != 0)
  {
  delete [] lgrJndx;
  }
  if (lgrKndx != 0)
  {
  delete [] lgrKndx;
  }
  if (parentNdx != 0)
  {
  delete [] parentNdx;
  }
  if (parentCount != 0)
  {
  delete [] parentCount;
  }
  if (parentIndx != 0)
  {
  delete [] parentIndx;
  }
  if (parentJndx != 0)
  {
  delete [] parentJndx;
  }
  if (parentKndx != 0)
  {
  delete [] parentKndx;
  }
  if (lgrNdx != 0)
  {
  delete [] lgrNdx;
  }
  if (properties != 0)
  {
  delete properties;
  }
}

void RescueParametricLGRMap::ClearMap()
{
  cellToCellMapCount = 0;
  parentMapCount = 0;
}

void RescueParametricLGRMap::AddRefinedCell()
{
  if (cellToCellMapCount == refinedAllocated)
  {
  refinedAllocated += refinedReallocFactor;
  size_t sizeNow = (size_t) cellToCellMapCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newLgrIndx = new RESCUEINT64[(size_t) refinedAllocated];
  memcpy(newLgrIndx, lgrIndx, sizeNow);
  delete [] lgrIndx;
  lgrIndx = newLgrIndx;
  if (lgrJndx != 0)
  {
      RESCUEINT64 *newLgrJndx = new RESCUEINT64[(size_t) refinedAllocated];
      memcpy(newLgrJndx, lgrJndx, sizeNow);
      delete [] lgrJndx;
      lgrJndx = newLgrJndx;
  }
  if (lgrKndx != 0)
  {
      RESCUEINT64 *newLgrKndx = new RESCUEINT64[(size_t) refinedAllocated];
      memcpy(newLgrKndx, lgrKndx, sizeNow);
      delete [] lgrKndx;
      lgrKndx = newLgrKndx;
  }
  RESCUEINT64 *newParentNdx = new RESCUEINT64[(size_t) refinedAllocated];
  memcpy(newParentNdx, parentNdx, sizeNow);
  delete [] parentNdx;
  parentNdx = newParentNdx;
  RESCUEINT64 *newParentCount = new RESCUEINT64[(size_t) refinedAllocated];
  memcpy(newParentCount, parentCount, sizeNow);
  delete [] parentCount;
  parentCount = newParentCount;
  }
}

void RescueParametricLGRMap::EnsureParentMapSize(RESCUEINT64 parentCountIn)
{
  if (parentMapCount + parentCountIn >= parentAllocated)
  {
  parentAllocated += (parentCountIn > parentReallocFactor) ? parentCountIn : parentReallocFactor;
  size_t sizeNow = (size_t) parentMapCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newParentIndx = new RESCUEINT64[(size_t) parentAllocated];
  memcpy(newParentIndx, parentIndx, sizeNow);
  delete [] parentIndx;
  parentIndx = newParentIndx;
  if (parentJndx != 0)
  {
      RESCUEINT64 *newParentJndx = new RESCUEINT64[(size_t) parentAllocated];
      memcpy(newParentJndx, parentJndx, sizeNow);
      delete [] parentJndx;
      parentJndx = newParentJndx;
  }
  if (parentKndx != 0)
  {
      RESCUEINT64 *newParentKndx = new RESCUEINT64[(size_t) parentAllocated];
      memcpy(newParentKndx, parentKndx, sizeNow);
      delete [] parentKndx;
      parentKndx = newParentKndx;
  }
  if (lgrNdx != 0)
  {
      RESCUEINT64 *newLgrNdx = new RESCUEINT64[(size_t) parentAllocated];
      memcpy(newLgrNdx, lgrNdx, sizeNow);
      delete [] lgrNdx;
      lgrNdx = newLgrNdx;
  }
  }
}

void RescueParametricLGRMap::SetCellToCell(RESCUEINT32 iNdx, RESCUEINT32 jNdx, RESCUEINT32 kNdx, RESCUEINT32 parentCountIn,
                                           RESCUEINT32 *pIndx, RESCUEINT32 *pJndx, RESCUEINT32 *pKndx)
{
  AddRefinedCell();
  EnsureParentMapSize(parentCountIn);

  lgrIndx[cellToCellMapCount] = iNdx;
  if (lgrJndx != 0)
  {
  lgrJndx[cellToCellMapCount] = jNdx;
  }
  if (lgrKndx != 0)
  {
  lgrKndx[cellToCellMapCount] = kNdx;
  }
  parentNdx[cellToCellMapCount] = parentMapCount;
  parentCount[cellToCellMapCount] = parentCountIn;
  RESCUEINT32 loop;
  for (loop = 0; loop < parentCountIn; loop++)
  {
  parentIndx[parentMapCount + (RESCUEINT64) loop] = pIndx[loop];
  if (parentJndx != 0)
  {
      parentJndx[parentMapCount + (RESCUEINT64) loop] = pJndx[loop];
  }
  if (parentKndx != 0)
  {
      parentKndx[parentMapCount + (RESCUEINT64) loop] = pKndx[loop];
  }
  if (lgrNdx != 0)
  {
      lgrNdx[parentMapCount + (RESCUEINT64) loop] = cellToCellMapCount;
  }
  }
  cellToCellMapCount++;
  parentMapCount += parentCountIn;
}

void RescueParametricLGRMap::SetCellToCell(RESCUEINT64 iNdx, RESCUEINT64 jNdx, RESCUEINT64 kNdx, RESCUEINT64 parentCountIn,
                                           RESCUEINT64 *pIndx, RESCUEINT64 *pJndx, RESCUEINT64 *pKndx)
{
  AddRefinedCell();
  EnsureParentMapSize(parentCountIn);

  lgrIndx[cellToCellMapCount] = iNdx;
  if (lgrJndx != 0)
  {
  lgrJndx[cellToCellMapCount] = jNdx;
  }
  if (lgrKndx != 0)
  {
  lgrKndx[cellToCellMapCount] = kNdx;
  }
  parentNdx[cellToCellMapCount] = parentMapCount;
  parentCount[cellToCellMapCount] = parentCountIn;
  RESCUEINT64 loop;
  for (loop = 0; loop < parentCountIn; loop++)
  {
  parentIndx[parentMapCount + loop] = pIndx[loop];
  if (parentJndx != 0)
  {
      parentJndx[parentMapCount + loop] = pJndx[loop];
  }
  if (parentKndx != 0)
  {
      parentKndx[parentMapCount + loop] = pKndx[loop];
  }
  if (lgrNdx != 0)
  {
      lgrNdx[parentMapCount + loop] = cellToCellMapCount;
  }
  }
  cellToCellMapCount++;
  parentMapCount += parentCountIn;
}

RESCUEINT32 RescueParametricLGRMap::ReadLGRCellToCell(RESCUEINT32 iNdx, RESCUEINT32 jNdx, RESCUEINT32 kNdx, RESCUEINT32 allocated,
                                                      RESCUEINT32 *pIndx, RESCUEINT32 *pJndx, RESCUEINT32 *pKndx)
{
  RESCUEINT32 myReturn = -1;
  RESCUEINT32 loop;
  for (loop = 0; loop < cellToCellMapCount && myReturn < 0; loop++)
  {
  if (iNdx == lgrIndx[loop]
  && (jNdx == -1 || ((lgrJndx == 0) ? true : lgrJndx[loop] == jNdx))
  && (kNdx == -1 || ((lgrKndx == 0) ? true : lgrKndx[loop] == kNdx)))
  {
      if (parentCount[loop] > 2147483647 || parentCount[loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      myReturn = (RESCUEINT32) parentCount[loop];
      RESCUEINT32 subloop;
      for (subloop = 0; subloop < allocated && subloop < myReturn; subloop++)
      {
    if (pIndx != 0)
    {
          if (parentIndx[myReturn + subloop] > 2147483647 || parentIndx[myReturn + subloop] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          pIndx[subloop] = (RESCUEINT32) parentIndx[myReturn + subloop];
    }
    if (pJndx != 0 && parentJndx != 0)
    {
          if (parentJndx[myReturn + subloop] > 2147483647 || parentJndx[myReturn + subloop] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          pJndx[subloop] = (RESCUEINT32) parentJndx[myReturn + subloop];
    }
    if (pKndx != 0 && parentKndx != 0)
    {
          if (parentKndx[myReturn + subloop] > 2147483647 || parentKndx[myReturn + subloop] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          pKndx[subloop] = (RESCUEINT32) parentKndx[myReturn + subloop];
    }
      }
  }
  }
  return myReturn;
}

RESCUEINT64 RescueParametricLGRMap::ReadLGRCellToCell(RESCUEINT64 iNdx, RESCUEINT64 jNdx, RESCUEINT64 kNdx, RESCUEINT64 allocated,
                                                      RESCUEINT64 *pIndx, RESCUEINT64 *pJndx, RESCUEINT64 *pKndx)
{
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < cellToCellMapCount && myReturn < 0; loop++)
  {
  if (iNdx == lgrIndx[loop]
  && (jNdx == -1 || ((lgrJndx == 0) ? true : lgrJndx[loop] == jNdx))
  && (kNdx == -1 || ((lgrKndx == 0) ? true : lgrKndx[loop] == kNdx)))
  {
      myReturn = parentCount[loop];
      RESCUEINT64 subloop;
      for (subloop = 0; subloop < allocated && subloop < myReturn; subloop++)
      {
    if (pIndx != 0)
    {
          pIndx[subloop] = parentIndx[myReturn + subloop];
    }
    if (pJndx != 0 && parentJndx != 0)
    {
          pJndx[subloop] = parentJndx[myReturn + subloop];
    }
    if (pKndx != 0 && parentKndx != 0)
    {
          pKndx[subloop] = parentKndx[myReturn + subloop];
    }
      }
  }
  }
  return myReturn;
}

RESCUEINT32 RescueParametricLGRMap::ReadParentCellToCell(RESCUEINT32 iNdx, RESCUEINT32 jNdx, RESCUEINT32 kNdx, RESCUEINT32 allocated,
                                                         RESCUEINT32 *cIndx, RESCUEINT32 *cJndx, RESCUEINT32 *cKndx)
{
  RESCUEINT32 myReturn = -1;
  RESCUEINT32 loop;
  for (loop = 0; loop < parentMapCount; loop++)
  {
  if (iNdx == parentIndx[loop]
  && (jNdx == -1 || ((parentJndx == 0) ? true : parentJndx[loop] == jNdx))
  && (kNdx == -1 || ((parentKndx == 0) ? true : parentKndx[loop] == kNdx)))
  {
      if (myReturn < 0) myReturn++;
      if (lgrNdx != 0 && allocated > 0)
      {
    if (cIndx != 0)
    {
          if (lgrIndx[lgrNdx[loop]] > 2147483647 || lgrIndx[lgrNdx[loop]] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          cIndx[myReturn] = (RESCUEINT32) lgrIndx[lgrNdx[loop]];
    }
    if (cJndx != 0 && parentJndx != 0 && lgrJndx != 0)
    {
          if (lgrJndx[lgrNdx[loop]] > 2147483647 || lgrJndx[lgrNdx[loop]] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          cJndx[myReturn] = (RESCUEINT32) lgrJndx[lgrNdx[loop]];
    }
    if (cKndx != 0 && parentKndx != 0 && lgrKndx != 0)
    {
          if (lgrKndx[lgrNdx[loop]] > 2147483647 || lgrKndx[lgrNdx[loop]] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          cKndx[myReturn] = (RESCUEINT32) lgrKndx[lgrNdx[loop]];
    }
    allocated--;
      }
      myReturn++;
  }
  }
  return myReturn;
}

RESCUEINT64 RescueParametricLGRMap::ReadParentCellToCell(RESCUEINT64 iNdx, RESCUEINT64 jNdx, RESCUEINT64 kNdx, RESCUEINT64 allocated,
                                                 RESCUEINT64 *cIndx, RESCUEINT64 *cJndx, RESCUEINT64 *cKndx)
{
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < parentMapCount; loop++)
  {
  if (iNdx == parentIndx[loop]
  && (jNdx == -1 || ((parentJndx == 0) ? true : parentJndx[loop] == jNdx))
  && (kNdx == -1 || ((parentKndx == 0) ? true : parentKndx[loop] == kNdx)))
  {
      if (myReturn < 0) myReturn++;
      if (lgrNdx != 0 && allocated > 0)
      {
    if (cIndx != 0)
    {
          cIndx[myReturn] = lgrIndx[lgrNdx[loop]];
    }
    if (cJndx != 0 && parentJndx != 0 && lgrJndx != 0)
    {
          cJndx[myReturn] = lgrJndx[lgrNdx[loop]];
    }
    if (cKndx != 0 && parentKndx != 0 && lgrKndx != 0)
    {
          cKndx[myReturn] = lgrKndx[lgrNdx[loop]];
    }
    allocated--;
      }
      myReturn++;
  }
  }
  return myReturn;
}

void RescueParametricLGRMap::CopyArray(RescuePMLGRArrayName whichArray, RESCUEINT32 srcLowBound, RESCUEINT32 srcCount,
                                       RESCUEINT32 *preAllocated, RESCUEINT32 destOffset)
{
  switch (whichArray)
  {
  case LGR_I_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    if (lgrIndx[srcLowBound + loop] > 2147483647 || lgrIndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) lgrIndx[srcLowBound + loop];
      }
  }
  break;
  case LGR_J_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    if (lgrJndx[srcLowBound + loop] > 2147483647 || lgrJndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) lgrJndx[srcLowBound + loop];
      }
  }
  break;
  case LGR_K_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    if (lgrKndx[srcLowBound + loop] > 2147483647 || lgrKndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) lgrKndx[srcLowBound + loop];
      }
  }
  break;
  case PARENT_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    if (parentNdx[srcLowBound + loop] > 2147483647 || parentNdx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) parentNdx[srcLowBound + loop];
      }
  }
  break;
  case PARENT_COUNT:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    if (parentCount[srcLowBound + loop] > 2147483647 || parentCount[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) parentCount[srcLowBound + loop];
      }
  }
  break;
  case PARENT_I_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < parentMapCount; loop++)
      {
    if (parentIndx[srcLowBound + loop] > 2147483647 || parentIndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) parentIndx[srcLowBound + loop];
      }
  }
  break;
  case PARENT_J_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < parentMapCount; loop++)
      {
    if (parentJndx[srcLowBound + loop] > 2147483647 || parentJndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) parentJndx[srcLowBound + loop];
      }
  }
  break;
  case PARENT_K_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < parentMapCount; loop++)
      {
    if (parentKndx[srcLowBound + loop] > 2147483647 || parentKndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) parentKndx[srcLowBound + loop];
      }
  }
  break;
  case LGR_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < parentMapCount; loop++)
      {
    if (lgrNdx[srcLowBound + loop] > 2147483647 || lgrNdx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) lgrNdx[srcLowBound + loop];
      }
  }
  break;
  }
}

void RescueParametricLGRMap::CopyArray(RescuePMLGRArrayName whichArray, RESCUEINT64 srcLowBound, RESCUEINT64 srcCount,
                                       RESCUEINT64 *preAllocated, RESCUEINT64 destOffset)
{
  switch (whichArray)
  {
  case LGR_I_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    preAllocated[destOffset + loop] = lgrIndx[srcLowBound + loop];
      }
  }
  break;
  case LGR_J_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    preAllocated[destOffset + loop] = lgrJndx[srcLowBound + loop];
      }
  }
  break;
  case LGR_K_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    preAllocated[destOffset + loop] = lgrKndx[srcLowBound + loop];
      }
  }
  break;
  case PARENT_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    preAllocated[destOffset + loop] = parentNdx[srcLowBound + loop];
      }
  }
  break;
  case PARENT_COUNT:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < cellToCellMapCount; loop++)
      {
    preAllocated[destOffset + loop] = parentCount[srcLowBound + loop];
      }
  }
  break;
  case PARENT_I_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < parentMapCount; loop++)
      {
    preAllocated[destOffset + loop] = parentIndx[srcLowBound + loop];
      }
  }
  break;
  case PARENT_J_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < parentMapCount; loop++)
      {
    preAllocated[destOffset + loop] = parentJndx[srcLowBound + loop];
      }
  }
  break;
  case PARENT_K_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < parentMapCount; loop++)
      {
    preAllocated[destOffset + loop] = parentKndx[srcLowBound + loop];
      }
  }
  break;
  case LGR_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount && srcLowBound + loop < parentMapCount; loop++)
      {
    preAllocated[destOffset + loop] = lgrNdx[srcLowBound + loop];
      }
  }
  break;
  }
}

void RescueParametricLGRMap::Accept(RESCUEINT32 cellToCellCountIn, 
                  RESCUEINT32 *lgrIndxIn,
                  RESCUEINT32 *lgrJndxIn,
                  RESCUEINT32 *lgrKndxIn,
                  RESCUEINT32 *parentNdxIn,
                  RESCUEINT32 *parentCountIn,
                  RESCUEINT32 parentMapCountIn,
                  RESCUEINT32 *parentIndxIn,
                  RESCUEINT32 *parentJndxIn,
                  RESCUEINT32 *parentKndxIn,
                  RESCUEINT32 *lgrNdxIn)
{
  cellToCellMapCount = cellToCellCountIn;
  refinedAllocated = cellToCellMapCount;
  if (lgrIndx != 0)
  {
  delete [] lgrIndx;
  }
  lgrIndx = RescueContext::Allocate64For32(lgrIndxIn, cellToCellCountIn, true);
  if (lgrJndx != 0)
  {
  delete [] lgrJndx;
  }
  lgrJndx = RescueContext::Allocate64For32(lgrJndxIn, cellToCellCountIn, true);
  if (lgrKndx != 0)
  {
  delete [] lgrKndx;
  }
  lgrKndx = RescueContext::Allocate64For32(lgrKndxIn, cellToCellCountIn, true);
  if (parentNdx != 0)
  {
  delete [] parentNdx;
  }
  parentNdx = RescueContext::Allocate64For32(parentNdxIn, cellToCellCountIn, true);
  if (parentCount != 0)
  {
  delete [] parentCount;
  }
  parentCount = RescueContext::Allocate64For32(parentCountIn, cellToCellCountIn, true);

  parentMapCount = parentMapCountIn;
  parentAllocated = parentMapCount;
  if (parentIndx != 0)
  {
  delete [] parentIndx;
  }
  parentIndx = RescueContext::Allocate64For32(parentIndxIn, parentMapCountIn, true);
  if (parentJndx != 0)
  {
  delete [] parentJndx;
  }
  parentJndx = RescueContext::Allocate64For32(parentJndxIn, parentMapCountIn, true);
  if (parentKndx != 0)
  {
  delete [] parentKndx;
  }
  parentKndx = RescueContext::Allocate64For32(parentKndxIn, parentMapCountIn, true);
  if (lgrNdx != 0)
  {
  delete [] lgrNdx;
  }
  lgrNdx = RescueContext::Allocate64For32(lgrNdxIn, parentMapCountIn, true);
}

void RescueParametricLGRMap::Accept(RESCUEINT64 cellToCellCountIn, 
                  RESCUEINT64 *lgrIndxIn,
                  RESCUEINT64 *lgrJndxIn,
                  RESCUEINT64 *lgrKndxIn,
                  RESCUEINT64 *parentNdxIn,
                  RESCUEINT64 *parentCountIn,
                  RESCUEINT64 parentMapCountIn,
                  RESCUEINT64 *parentIndxIn,
                  RESCUEINT64 *parentJndxIn,
                  RESCUEINT64 *parentKndxIn,
                  RESCUEINT64 *lgrNdxIn)
{
  cellToCellMapCount = cellToCellCountIn;
  refinedAllocated = cellToCellMapCount;
  if (lgrIndx != 0)
  {
  delete [] lgrIndx;
  }
  lgrIndx = lgrIndxIn;
  if (lgrJndx != 0)
  {
  delete [] lgrJndx;
  }
  lgrJndx = lgrJndxIn;
  if (lgrKndx != 0)
  {
  delete [] lgrKndx;
  }
  lgrKndx = lgrKndxIn;
  if (parentNdx != 0)
  {
  delete [] parentNdx;
  }
  parentNdx = parentNdxIn;
  if (parentCount != 0)
  {
  delete [] parentCount;
  }
  parentCount = parentCountIn;

  parentMapCount = parentMapCountIn;
  parentAllocated = parentMapCount;
  if (parentIndx != 0)
  {
  delete [] parentIndx;
  }
  parentIndx = parentIndxIn;
  if (parentJndx != 0)
  {
  delete [] parentJndx;
  }
  parentJndx = parentJndxIn;
  if (parentKndx != 0)
  {
  delete [] parentKndx;
  }
  parentKndx = parentKndxIn;
  if (lgrNdx != 0)
  {
  delete [] lgrNdx;
  }
  lgrNdx = lgrNdxIn;
}

void RescueParametricLGRMap::Set(RESCUEINT32 cellToCellCountIn,
                                 RESCUEINT32 *lgrIndxIn, 
                                 RESCUEINT32 *lgrJndxIn,
                                 RESCUEINT32 *lgrKndxIn,
                                 RESCUEINT32 *parentNdxIn,
                                 RESCUEINT32 *parentCountIn,
                                 RESCUEINT32 parentMapCountIn,
                                 RESCUEINT32 *parentIndxIn,
                                 RESCUEINT32 *parentJndxIn,
                                 RESCUEINT32 *parentKndxIn,
                                 RESCUEINT32 *lgrNdxIn)
{
  if (lgrJndxIn == 0 && lgrJndx != 0)
  {
  delete [] lgrJndx;
  lgrJndx = 0;
  }
  if (lgrKndxIn == 0 && lgrKndx != 0)
  {
  delete [] lgrKndx;
  lgrKndx = 0;
  }
  cellToCellMapCount = cellToCellCountIn;
  if (cellToCellCountIn > refinedAllocated)
  {
  refinedAllocated = cellToCellCountIn;
  delete [] lgrIndx;
  lgrIndx = new RESCUEINT64[(size_t) refinedAllocated];
  if (lgrJndx != 0)
  {
      delete [] lgrJndx;
      lgrJndx = 0;
  }
  if (lgrJndxIn != 0) lgrJndx = new RESCUEINT64[(size_t) refinedAllocated];
  if (lgrKndx != 0)
  {
      delete [] lgrKndx;
      lgrKndx = 0;
  }
  if (lgrKndxIn != 0) lgrKndx = new RESCUEINT64[(size_t) refinedAllocated];
  delete [] parentNdx;
  parentNdx = new RESCUEINT64[(size_t) refinedAllocated];
  delete [] parentCount;
  parentCount = new RESCUEINT64[(size_t) refinedAllocated];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < cellToCellCountIn; loop++)
  {
  lgrIndx[loop] = lgrIndxIn[loop];
  if (lgrJndx != 0 && lgrJndxIn != 0)
  {
      lgrJndx[loop] = lgrJndxIn[loop];
  }
  if (lgrKndx != 0 && lgrKndxIn != 0)
  {
      lgrKndx[loop] = lgrKndxIn[loop];
  }
  parentNdx[loop] = parentNdxIn[loop];
  parentCount[loop] = parentCountIn[loop];
  }

  if (parentJndxIn == 0 && parentJndx != 0)
  {
  delete [] parentJndx;
  parentJndx = 0;
  }
  if (parentKndxIn == 0 && parentKndx != 0)
  {
  delete [] parentKndx;
  parentKndx = 0;
  }
  parentMapCount = parentMapCountIn;
  if (parentMapCountIn > parentAllocated)
  {
  parentAllocated = parentMapCountIn;
  delete [] parentIndx;
  parentIndx = new RESCUEINT64[(size_t) parentAllocated];
  if (parentJndx != 0)
  {
      delete [] parentJndx;
      parentJndx = 0;
  }
  if (parentJndxIn != 0) parentJndx = new RESCUEINT64[(size_t) parentAllocated];
  if (parentKndx != 0)
  {
      delete [] parentKndx;
      parentKndx = 0;
  }
  if (parentKndxIn != 0) parentKndx = new RESCUEINT64[(size_t) parentAllocated];
  delete [] lgrNdx;
  lgrNdx = new RESCUEINT64[(size_t) parentAllocated];
  }
  for (loop = 0; loop < parentMapCountIn; loop++)
  {
  parentIndx[loop] = parentIndxIn[loop];
  if (parentJndx != 0 && parentJndxIn != 0)
  {
      parentJndx[loop] = parentJndxIn[loop];
  }
  if (parentKndx != 0 && parentKndxIn != 0)
  {
      parentKndx[loop] = parentKndxIn[loop];
  }
  lgrNdx[loop] = lgrNdxIn[loop];
  }
  
}

void RescueParametricLGRMap::Set(RESCUEINT64 cellToCellCountIn,
                                 RESCUEINT64 *lgrIndxIn, 
                                 RESCUEINT64 *lgrJndxIn,
                                 RESCUEINT64 *lgrKndxIn,
                                 RESCUEINT64 *parentNdxIn,
                                 RESCUEINT64 *parentCountIn,
                                 RESCUEINT64 parentMapCountIn,
                                 RESCUEINT64 *parentIndxIn,
                                 RESCUEINT64 *parentJndxIn,
                                 RESCUEINT64 *parentKndxIn,
                                 RESCUEINT64 *lgrNdxIn)
{
  if (lgrJndxIn == 0 && lgrJndx != 0)
  {
  delete [] lgrJndx;
  lgrJndx = 0;
  }
  if (lgrKndxIn == 0 && lgrKndx != 0)
  {
  delete [] lgrKndx;
  lgrKndx = 0;
  }
  cellToCellMapCount = cellToCellCountIn;
  if (cellToCellCountIn > refinedAllocated)
  {
  refinedAllocated = cellToCellCountIn;
  delete [] lgrIndx;
  lgrIndx = new RESCUEINT64[(size_t) refinedAllocated];
  if (lgrJndx != 0)
  {
      delete [] lgrJndx;
      lgrJndx = 0;
  }
  if (lgrJndxIn != 0) lgrJndx = new RESCUEINT64[(size_t) refinedAllocated];
  if (lgrKndx != 0)
  {
      delete [] lgrKndx;
      lgrKndx = 0;
  }
  if (lgrKndxIn != 0) lgrKndx = new RESCUEINT64[(size_t) refinedAllocated];
  delete [] parentNdx;
  parentNdx = new RESCUEINT64[(size_t) refinedAllocated];
  delete [] parentCount;
  parentCount = new RESCUEINT64[(size_t) refinedAllocated];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < cellToCellCountIn; loop++)
  {
  lgrIndx[loop] = lgrIndxIn[loop];
  if (lgrJndx != 0 && lgrJndxIn != 0)
  {
      lgrJndx[loop] = lgrJndxIn[loop];
  }
  if (lgrKndx != 0 && lgrKndxIn != 0)
  {
      lgrKndx[loop] = lgrKndxIn[loop];
  }
  parentNdx[loop] = parentNdxIn[loop];
  parentCount[loop] = parentCountIn[loop];
  }

  if (parentJndxIn == 0 && parentJndx != 0)
  {
  delete [] parentJndx;
  parentJndx = 0;
  }
  if (parentKndxIn == 0 && parentKndx != 0)
  {
  delete [] parentKndx;
  parentKndx = 0;
  }
  parentMapCount = parentMapCountIn;
  if (parentMapCountIn > parentAllocated)
  {
  parentAllocated = parentMapCountIn;
  delete [] parentIndx;
  parentIndx = new RESCUEINT64[(size_t) parentAllocated];
  if (parentJndx != 0)
  {
      delete [] parentJndx;
      parentJndx = 0;
  }
  if (parentJndxIn != 0) parentJndx = new RESCUEINT64[(size_t) parentAllocated];
  if (parentKndx != 0)
  {
      delete [] parentKndx;
      parentKndx = 0;
  }
  if (parentKndxIn != 0) parentKndx = new RESCUEINT64[(size_t) parentAllocated];
  delete [] lgrNdx;
  lgrNdx = new RESCUEINT64[(size_t) parentAllocated];
  }
  for (loop = 0; loop < parentMapCountIn; loop++)
  {
  parentIndx[loop] = parentIndxIn[loop];
  if (parentJndx != 0 && parentJndxIn != 0)
  {
      parentJndx[loop] = parentJndxIn[loop];
  }
  if (parentKndx != 0 && parentKndxIn != 0)
  {
      parentKndx[loop] = parentKndxIn[loop];
  }
  lgrNdx[loop] = lgrNdxIn[loop];
  }
  
}

void RescueParametricLGRMap::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, cellToCellMapCount);
  myfprintf(context, archiveFile, lgrIndx, cellToCellMapCount, FALSE);
  if (lgrJndx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, lgrJndx, cellToCellMapCount, FALSE);
  }
  if (lgrKndx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, lgrKndx, cellToCellMapCount, FALSE);
  }
  myfprintf(context, archiveFile, parentNdx, cellToCellMapCount, FALSE);
  myfprintf(context, archiveFile, parentCount, cellToCellMapCount, FALSE);
  myfprintf(context, archiveFile, parentMapCount);
  myfprintf(context, archiveFile, parentIndx, parentMapCount, FALSE);
  if (parentJndx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, parentJndx, parentMapCount, FALSE);
  }
  if (parentKndx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, parentKndx, parentMapCount, FALSE);
  }
  if (lgrNdx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, lgrNdx, parentMapCount, FALSE);
  }
  if (context->FileVersion() >= 37)
  {
  if (properties == 0 
  &&  propertyContainerId != 0 
  &&  lgrGeometry->ParentModel()->propertyActionImmediate == TRUE)
  {
      properties = new cSetRescueDataContainer(lgrGeometry->ParentModel(), propertyContainerId);  // Will Read.
  }
  if (properties != 0)
  {
      myfprintf(context, archiveFile, "properties");
      RescueBuffer buf1(context, 10);
      buf1 << properties->Identifier();
      buf1.Archive(archiveFile);

      properties->Archive(); // Goes into it's own file.
  }
  else if (properties == 0 && propertyContainerId != 0)
  {
      myfprintf(context, archiveFile, "properties");
      RescueBuffer buf1(context, 10);
      buf1 << propertyContainerId;
      buf1.Archive(archiveFile);
  }
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueParametricLGRMap::RescueParametricLGRMap(RescueContext *context, FILE *archiveFile):cellToCellMapCount(0)
                                                                                         ,lgrIndx(0)
                                                                                         ,lgrJndx(0)
                                                                                         ,lgrKndx(0)
                                                                                         ,parentNdx(0)
                                                                                         ,parentCount(0)
                                                                                         ,parentMapCount(0)
                                                                                         ,parentIndx(0)
                                                                                         ,parentJndx(0)
                                                                                         ,parentKndx(0)
                                                                                         ,lgrNdx(0)
             ,properties(0)
                                                                                         ,propertyContainerId(0)
{
  myfscanf(context, archiveFile, &cellToCellMapCount);
  refinedAllocated = cellToCellMapCount;
  lgrIndx = new RESCUEINT64 [(size_t) refinedAllocated];
  myfscanf(context, archiveFile, lgrIndx, cellToCellMapCount, FALSE);
  RESCUEINT64 flag;
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  lgrJndx = new RESCUEINT64 [(size_t) refinedAllocated];
  myfscanf(context, archiveFile, lgrJndx, cellToCellMapCount, FALSE);
  }
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  lgrKndx = new RESCUEINT64 [(size_t) refinedAllocated];
  myfscanf(context, archiveFile, lgrKndx, cellToCellMapCount, FALSE);
  }
  parentNdx = new RESCUEINT64 [(size_t) refinedAllocated];
  myfscanf(context, archiveFile, parentNdx, cellToCellMapCount, FALSE);
  parentCount = new RESCUEINT64 [(size_t) refinedAllocated];
  myfscanf(context, archiveFile, parentCount, cellToCellMapCount, FALSE);

  myfscanf(context, archiveFile, &parentMapCount);
  parentAllocated = parentMapCount;
  parentIndx = new RESCUEINT64 [(size_t) parentAllocated];
  myfscanf(context, archiveFile, parentIndx, parentMapCount, FALSE);
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  parentJndx = new RESCUEINT64 [(size_t) parentAllocated];
  myfscanf(context, archiveFile, parentJndx, parentMapCount, FALSE);
  }
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  parentKndx = new RESCUEINT64 [(size_t) parentAllocated];
  myfscanf(context, archiveFile, parentKndx, parentMapCount, FALSE);
  }
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  lgrNdx = new RESCUEINT64 [(size_t) parentAllocated];
  myfscanf(context, archiveFile, lgrNdx, parentMapCount, FALSE);
  }
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      if (strcmp(myString, "properties") == 0)
      {
    RescueBuffer buf(context, archiveFile);
    buf >> propertyContainerId;
      }
      else
      {
    RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
  }
  }
}

void RescueParametricLGRMap::Relink(RescueGeometry *geometryIn)
{
  lgrGeometry = geometryIn;
}

RESCUEBOOL RescueParametricLGRMap::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FALSE;
  if (properties == 0 && propertyContainerId != 0)
  {
  properties = new cSetRescueDataContainer(lgrGeometry->ParentModel(), propertyContainerId);  // Will Read.
  }
  if (properties != 0)
  {
  myReturn = properties->AnyFileTruncated();
  }
  return myReturn;
}

cSetRescueDataContainer *RescueParametricLGRMap::DataContainers()
{
  if (properties == 0)
  {
  if (propertyContainerId != 0)
  {
      properties = new cSetRescueDataContainer(lgrGeometry->ParentModel(), propertyContainerId);  // Will Read.
  }
  }
  return properties;
}

cSetRescueDataContainer *RescueParametricLGRMap::DemandDataContainers()
{
  if (properties == 0)
  {
  if (propertyContainerId != 0)
  {
      properties = new cSetRescueDataContainer(lgrGeometry->ParentModel(), propertyContainerId);  // Will Read.
  }
  else
  {
      properties = new cSetRescueDataContainer(lgrGeometry->ParentModel());
  }
  }
  return properties;
}

void RescueParametricLGRMap::FindUniquePropertyNames(cSetString *container)
{
  cSetRescueDataContainer *containers = DataContainers();
  if (containers != 0)
  {
  containers->FindUniquePropertyNames(container);
  }
}

RESCUEINT32 RescueParametricLGRMap::CellToCellMapCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CellToCellMapCount64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 *RescueParametricLGRMap::LGRIndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(LGRIndx64(), CellToCellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueParametricLGRMap::LGRJndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(LGRJndx64(), CellToCellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueParametricLGRMap::LGRKndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(LGRKndx64(), CellToCellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueParametricLGRMap::ParentNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(ParentNdx64(), CellToCellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueParametricLGRMap::ParentCount(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(ParentCount64(), CellToCellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 RescueParametricLGRMap::ParentMapCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = ParentMapCount64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 *RescueParametricLGRMap::ParentINdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(ParentINdx64(), ParentMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueParametricLGRMap::ParentJNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(ParentJNdx64(), ParentMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueParametricLGRMap::ParentKNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(ParentKNdx64(), ParentMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueParametricLGRMap::LGRNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(LGRNdx64(), ParentMapCount(throwIfTooBig), false, throwIfTooBig);
}




