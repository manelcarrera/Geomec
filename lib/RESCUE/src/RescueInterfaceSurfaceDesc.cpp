/********************************************************************

  RescueInterfaceSurfaceDesc.cpp

  A map between faces of refining cells and neighboring cells of
  other grids.  The other grids are either the BUG or some LGR
  which refines that BUG.

  Rod Hanks,  June 2003

*********************************************************************/
#include <stdio.h>
#include <memory.h>
#include "myHeaders.h"
#include "RescueContext.h"
#include "RescueInterfaceSurfaceDesc.h"
#include "cSetRescueInterfaceSurfaceDesc.h"
#include "RescueGeometry.h"
#include "RescueBuffer.h"
#include "cSetRescueDataContainer.h"
#include "RescueModel.h"
#include <string.h>

RescueInterfaceSurfaceDesc::RescueInterfaceSurfaceDesc(RescueGeometry *lgrGeometryIn,
                                                       RESCUEBOOL isExteriorIn,
                                                       RESCUEINT64 expectedFaces,
                                                       RESCUEINT64 expectedCells,
                                                       RESCUEINT64 facesRealloc,
                                                       RESCUEINT64 cellsRealloc)
                                                       :lgrGeometry(lgrGeometryIn)
       ,facesAllocated(expectedFaces)
       ,cellsAllocated(expectedCells)
       ,facesReallocFactor(facesRealloc)
       ,cellsReallocFactor(cellsRealloc)
       ,isExterior(isExteriorIn)
                                                       ,faceCount(0)
                                                       ,faceIndx(0)
                                                       ,faceJndx(0)
                                                       ,faceKndx(0)
                                                       ,faceNdx(0)
                                                       ,cellNdx(0)
                                                       ,cellCount(0)
                                                       ,cellMapCount(0)
                                                       ,cellRef(0)
                                                       ,cellIndx(0)
                                                       ,cellJndx(0)
                                                       ,cellKndx(0)
                                                       ,faceRowNdx(0)
                                                       ,properties(0)
                                                       ,propertyContainerId(0)
{
  (*lgrGeometry->isds) += this;
  if (expectedFaces > 0)
  {
  faceIndx = new RESCUEINT64[(size_t) expectedFaces];
  faceJndx = new RESCUEINT64[(size_t) expectedFaces];
  faceKndx = new RESCUEINT64[(size_t) expectedFaces];
  faceNdx = new RESCUEINT64[(size_t) expectedFaces];
  cellNdx = new RESCUEINT64[(size_t) expectedFaces];
  cellCount = new RESCUEINT64[(size_t) expectedFaces];
  }
  if (expectedCells > 0)
  {
  cellRef = new RESCUEINT64[(size_t) expectedCells];
  cellIndx = new RESCUEINT64[(size_t) expectedCells];
  cellJndx = new RESCUEINT64[(size_t) expectedCells];
  cellKndx = new RESCUEINT64[(size_t) expectedCells];
  faceRowNdx = new RESCUEINT64[(size_t) expectedCells];
  }
  if (facesRealloc < 1 || cellsRealloc < 1)
  {
  throw "reallocation factors cannot be less than one";
  }
}

RescueInterfaceSurfaceDesc::~RescueInterfaceSurfaceDesc()
{
  if (faceIndx != 0)
  {
  delete [] faceIndx;
  }
  if (faceJndx != 0)
  {
  delete [] faceJndx;
  }
  if (faceKndx != 0)
  {
  delete [] faceKndx;
  }
  if (faceNdx != 0)
  {
  delete [] faceNdx;
  }
  if (cellNdx != 0)
  {
  delete [] cellNdx;
  }
  if (cellCount != 0)
  {
  delete [] cellCount;
  }
  if (cellRef != 0)
  {
  delete [] cellRef;
  }
  if (cellIndx != 0)
  {
  delete [] cellIndx;
  }
  if (cellJndx != 0)
  {
  delete [] cellJndx;
  }
  if (cellKndx != 0)
  {
  delete [] cellKndx;
  }
  if (faceRowNdx != 0)
  {
  delete [] faceRowNdx;
  }
  if (properties != 0)
  {
  delete properties;
  }
}

void RescueInterfaceSurfaceDesc::SetFaceToCell(RESCUEINT32 iNdx, RESCUEINT32 jNdx, RESCUEINT32 kNdx, RESCUEINT32 faceNdxIn,
                                               RESCUEINT32 cellCountIn, RESCUEINT32 *cellRefIn, 
                                               RESCUEINT32 *cIndxIn, RESCUEINT32 *cJndxIn, RESCUEINT32 *cKndxIn)
{
  if (faceCount == facesAllocated)
  {
  facesAllocated += facesReallocFactor;
  size_t sizeNow = (size_t) faceCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newFaceIndx = new RESCUEINT64[(size_t) facesAllocated];
  memcpy(newFaceIndx, faceIndx, sizeNow);
  delete [] faceIndx;
  faceIndx = newFaceIndx;
  if (faceJndx != 0)
  {
      RESCUEINT64 *newFaceJndx = new RESCUEINT64[(size_t) facesAllocated];
      memcpy(newFaceJndx, faceJndx, sizeNow);
      delete [] faceJndx;
      faceJndx = newFaceJndx;
  }
  if (faceKndx != 0)
  {
      RESCUEINT64 *newFaceKndx = new RESCUEINT64[(size_t) facesAllocated];
      memcpy(newFaceKndx, faceKndx, sizeNow);
      delete [] faceKndx;
      faceKndx = newFaceKndx;
  }
  RESCUEINT64 *newFaceNdx = new RESCUEINT64[(size_t) facesAllocated];
  memcpy(newFaceNdx, faceNdx, sizeNow);
  delete [] faceNdx;
  faceNdx = newFaceNdx;
  RESCUEINT64 *newCellNdx = new RESCUEINT64[(size_t) facesAllocated];
  memcpy(newCellNdx, cellNdx, sizeNow);
  delete [] cellNdx;
  cellNdx = newCellNdx;
  RESCUEINT64 *newCellCount = new RESCUEINT64[(size_t) facesAllocated];
  memcpy(newCellCount, cellCount, sizeNow);
  delete [] cellCount;
  cellCount = newCellCount;
  }
  if (cellMapCount + cellCountIn >= cellsAllocated)
  {
  cellsAllocated += (cellCountIn > cellsReallocFactor) ? cellCountIn : cellsReallocFactor;
  size_t sizeNow = (size_t) cellMapCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newCellRef = new RESCUEINT64[(size_t) cellsAllocated];
  memcpy(newCellRef, cellRef, sizeNow);
  delete [] cellRef;
  cellRef = newCellRef;
  RESCUEINT64 *newCellIndx = new RESCUEINT64[(size_t) cellsAllocated];
  memcpy(newCellIndx, cellIndx, sizeNow);
  delete [] cellIndx;
  cellIndx = newCellIndx;
  if (cellJndx != 0)
  {
      RESCUEINT64 *newCellJndx = new RESCUEINT64[(size_t) cellsAllocated];
      memcpy(newCellJndx, cellJndx, sizeNow);
      delete [] cellJndx;
      cellJndx = newCellJndx;
  }
  if (cellKndx != 0)
  {
      RESCUEINT64 *newCellKndx = new RESCUEINT64[(size_t) cellsAllocated];
      memcpy(newCellKndx, cellKndx, sizeNow);
      delete [] cellKndx;
      cellKndx = newCellKndx;
  }
  RESCUEINT64 *newFaceRowNdx = new RESCUEINT64[(size_t) cellsAllocated];
  memcpy(newFaceRowNdx, faceRowNdx, sizeNow);
  delete [] faceRowNdx;
  faceRowNdx = newFaceRowNdx;
  }
  faceIndx[faceCount] = iNdx;
  if (faceJndx != 0)
  {
  faceJndx[faceCount] = jNdx;
  }
  if (faceKndx != 0)
  {
  faceKndx[faceCount] = kNdx;
  }
  faceNdx[faceCount] = faceNdxIn;
  cellNdx[faceCount] = cellMapCount;
  cellCount[faceCount] = cellCountIn;
  RESCUEINT64 loop;
  for (loop = 0; loop < cellCountIn; loop++)
  {
  cellRef[cellMapCount + loop] = cellRefIn[loop];
  cellIndx[cellMapCount + loop] = cIndxIn[loop];
  if (cellJndx != 0 && cJndxIn != 0)
  {
      cellJndx[cellMapCount + loop] = cJndxIn[loop];
  }
  if (cellKndx != 0 && cKndxIn != 0)
  {
      cellKndx[cellMapCount + loop] = cKndxIn[loop];
  }
  faceRowNdx[cellMapCount + loop] = faceCount;
  }
  faceCount++;
  cellMapCount += cellCountIn;
}

void RescueInterfaceSurfaceDesc::SetFaceToCell(RESCUEINT64 iNdx, RESCUEINT64 jNdx, RESCUEINT64 kNdx, RESCUEINT64 faceNdxIn,
                                               RESCUEINT64 cellCountIn, RESCUEINT64 *cellRefIn, 
                                               RESCUEINT64 *cIndxIn, RESCUEINT64 *cJndxIn, RESCUEINT64 *cKndxIn)
{
  if (faceCount == facesAllocated)
  {
  facesAllocated += facesReallocFactor;
  size_t sizeNow = (size_t) faceCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newFaceIndx = new RESCUEINT64[(size_t) facesAllocated];
  memcpy(newFaceIndx, faceIndx, sizeNow);
  delete [] faceIndx;
  faceIndx = newFaceIndx;
  if (faceJndx != 0)
  {
      RESCUEINT64 *newFaceJndx = new RESCUEINT64[(size_t) facesAllocated];
      memcpy(newFaceJndx, faceJndx, sizeNow);
      delete [] faceJndx;
      faceJndx = newFaceJndx;
  }
  if (faceKndx != 0)
  {
      RESCUEINT64 *newFaceKndx = new RESCUEINT64[(size_t) facesAllocated];
      memcpy(newFaceKndx, faceKndx, sizeNow);
      delete [] faceKndx;
      faceKndx = newFaceKndx;
  }
  RESCUEINT64 *newFaceNdx = new RESCUEINT64[(size_t) facesAllocated];
  memcpy(newFaceNdx, faceNdx, sizeNow);
  delete [] faceNdx;
  faceNdx = newFaceNdx;
  RESCUEINT64 *newCellNdx = new RESCUEINT64[(size_t) facesAllocated];
  memcpy(newCellNdx, cellNdx, sizeNow);
  delete [] cellNdx;
  cellNdx = newCellNdx;
  RESCUEINT64 *newCellCount = new RESCUEINT64[(size_t) facesAllocated];
  memcpy(newCellCount, cellCount, sizeNow);
  delete [] cellCount;
  cellCount = newCellCount;
  }
  if (cellMapCount + cellCountIn >= cellsAllocated)
  {
  cellsAllocated += (cellCountIn > cellsReallocFactor) ? cellCountIn : cellsReallocFactor;
  size_t sizeNow = (size_t) cellMapCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newCellRef = new RESCUEINT64[(size_t) cellsAllocated];
  memcpy(newCellRef, cellRef, sizeNow);
  delete [] cellRef;
  cellRef = newCellRef;
  RESCUEINT64 *newCellIndx = new RESCUEINT64[(size_t) cellsAllocated];
  memcpy(newCellIndx, cellIndx, sizeNow);
  delete [] cellIndx;
  cellIndx = newCellIndx;
  if (cellJndx != 0)
  {
      RESCUEINT64 *newCellJndx = new RESCUEINT64[(size_t) cellsAllocated];
      memcpy(newCellJndx, cellJndx, sizeNow);
      delete [] cellJndx;
      cellJndx = newCellJndx;
  }
  if (cellKndx != 0)
  {
      RESCUEINT64 *newCellKndx = new RESCUEINT64[(size_t) cellsAllocated];
      memcpy(newCellKndx, cellKndx, sizeNow);
      delete [] cellKndx;
      cellKndx = newCellKndx;
  }
  RESCUEINT64 *newFaceRowNdx = new RESCUEINT64[(size_t) cellsAllocated];
  memcpy(newFaceRowNdx, faceRowNdx, sizeNow);
  delete [] faceRowNdx;
  faceRowNdx = newFaceRowNdx;
  }
  faceIndx[faceCount] = iNdx;
  if (faceJndx != 0)
  {
  faceJndx[faceCount] = jNdx;
  }
  if (faceKndx != 0)
  {
  faceKndx[faceCount] = kNdx;
  }
  faceNdx[faceCount] = faceNdxIn;
  cellNdx[faceCount] = cellMapCount;
  cellCount[faceCount] = cellCountIn;
  RESCUEINT64 loop;
  for (loop = 0; loop < cellCountIn; loop++)
  {
  cellRef[cellMapCount + loop] = cellRefIn[loop];
  cellIndx[cellMapCount + loop] = cIndxIn[loop];
  if (cellJndx != 0 && cJndxIn != 0)
  {
      cellJndx[cellMapCount + loop] = cJndxIn[loop];
  }
  if (cellKndx != 0 && cKndxIn != 0)
  {
      cellKndx[cellMapCount + loop] = cKndxIn[loop];
  }
  faceRowNdx[cellMapCount + loop] = faceCount;
  }
  faceCount++;
  cellMapCount += cellCountIn;
}

RESCUEINT32 RescueInterfaceSurfaceDesc::ReadFaceToCell(RESCUEINT32 iNdx, RESCUEINT32 jNdx, 
                                                       RESCUEINT32 kNdx, RESCUEINT32 faceNdxIn,
                                                       RESCUEINT32 allocated, RESCUEINT32 *cellRefIn, 
                                                       RESCUEINT32 *cIndx, RESCUEINT32 *cJndx, RESCUEINT32 *cKndx)
{
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < faceCount && myReturn < 0; loop++)
  {
  if (iNdx == faceIndx[loop]
  && (jNdx == -1 || ((faceJndx == 0) ? true : faceJndx[loop] == jNdx))
  && (kNdx == -1 || ((faceKndx == 0) ? true : faceKndx[loop] == kNdx))
  &&  faceNdxIn == faceNdx[loop])
  {
      myReturn = cellCount[loop];
      RESCUEINT64 subloop;
      for (subloop = 0; subloop < allocated && subloop < myReturn; subloop++)
      {
    if (cellRefIn != 0)
    {
          if (cellRef[myReturn + subloop] > 2147483647 || cellRef[myReturn + subloop] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          cellRefIn[subloop] = (RESCUEINT32) cellRef[myReturn + subloop];
    }
    if (cIndx != 0)
    {
          if (cellIndx[myReturn + subloop] > 2147483647 || cellIndx[myReturn + subloop] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          cIndx[subloop] = (RESCUEINT32) cellIndx[myReturn + subloop];
    }
    if (cJndx != 0 && cellJndx != 0)
    {
          if (cellJndx[myReturn + subloop] > 2147483647 || cellJndx[myReturn + subloop] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          cJndx[subloop] = (RESCUEINT32) cellJndx[myReturn + subloop];
    }
    if (cKndx != 0 && cellKndx != 0)
    {
          if (cellKndx[myReturn + subloop] > 2147483647 || cellKndx[myReturn + subloop] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          cKndx[subloop] = (RESCUEINT32) cellKndx[myReturn + subloop];
    }
      }
  }
  }
  return (int) myReturn;
}

RESCUEINT64 RescueInterfaceSurfaceDesc::ReadFaceToCell(RESCUEINT64 iNdx, RESCUEINT64 jNdx, RESCUEINT64 kNdx, RESCUEINT64 faceNdxIn,
                                               RESCUEINT64 allocated, RESCUEINT64 *cellRefIn, 
                                               RESCUEINT64 *cIndx, RESCUEINT64 *cJndx, RESCUEINT64 *cKndx)
{
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < faceCount && myReturn < 0; loop++)
  {
  if (iNdx == faceIndx[loop]
  && (jNdx == -1 || ((faceJndx == 0) ? true : faceJndx[loop] == jNdx))
  && (kNdx == -1 || ((faceKndx == 0) ? true : faceKndx[loop] == kNdx))
  &&  faceNdxIn == faceNdx[loop])
  {
      myReturn = cellCount[loop];
      RESCUEINT64 subloop;
      for (subloop = 0; subloop < allocated && subloop < myReturn; subloop++)
      {
    if (cellRefIn != 0)
    {
          cellRefIn[subloop] = cellRef[myReturn + subloop];
    }
    if (cIndx != 0)
    {
          cIndx[subloop] = cellIndx[myReturn + subloop];
    }
    if (cJndx != 0 && cellJndx != 0)
    {
          cJndx[subloop] = cellJndx[myReturn + subloop];
    }
    if (cKndx != 0 && cellKndx != 0)
    {
          cKndx[subloop] = cellKndx[myReturn + subloop];
    }
      }
  }
  }
  return myReturn;
}

RESCUEINT32 RescueInterfaceSurfaceDesc::ReadCellToFace(RESCUEINT32 cRef, RESCUEINT32 cIndx, RESCUEINT32 cJndx, RESCUEINT32 cKndx, 
                                                       RESCUEINT32 allocated,
                                                       RESCUEINT32 *iNdx, RESCUEINT32 *jNdx, RESCUEINT32 *kNdx, RESCUEINT32 *faceNdxIn)
{
  RESCUEINT32 myReturn = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < cellMapCount; loop++)
  {
  if (cRef == cellRef[loop]
  &&  cIndx == cellIndx[loop]
  && (cJndx == -1 || ((cellJndx == 0) ? true : cellJndx[loop] == cJndx))
  && (cKndx == -1 || ((cellKndx == 0) ? true : cellKndx[loop] == cKndx)))
  {
      if (myReturn < 0) myReturn++;
      if (faceRowNdx != 0 && allocated > 0)
      {
    if (iNdx != 0)
    {
          if (faceIndx[faceRowNdx[loop]] > 2147483647 || faceIndx[faceRowNdx[loop]] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          iNdx[myReturn] = (RESCUEINT32) faceIndx[faceRowNdx[loop]];
    }
    if (jNdx != 0 && faceJndx != 0)
    {
          if (faceJndx[faceRowNdx[loop]] > 2147483647 || faceJndx[faceRowNdx[loop]] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          jNdx[myReturn] = (RESCUEINT32) faceJndx[faceRowNdx[loop]];
    }
    if (kNdx != 0 && faceKndx != 0)
    {
          if (faceKndx[faceRowNdx[loop]] > 2147483647 || faceKndx[faceRowNdx[loop]] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          kNdx[myReturn] = (RESCUEINT32) faceKndx[faceRowNdx[loop]];
    }
    if (faceNdxIn != 0)
    {
          if (faceNdx[faceRowNdx[loop]] > 2147483647 || faceNdx[faceRowNdx[loop]] < -2147483647)
          {
      throw "Model is too large to be written in 32 bit mode.";
          }
          faceNdxIn[myReturn] = (RESCUEINT32) faceNdx[faceRowNdx[loop]];
    }
    allocated--;
      }
      myReturn++;
  }
  }
  return myReturn;
}

RESCUEINT64 RescueInterfaceSurfaceDesc::ReadCellToFace(RESCUEINT64 cRef, RESCUEINT64 cIndx, RESCUEINT64 cJndx, RESCUEINT64 cKndx, 
                                               RESCUEINT64 allocated,
                                               RESCUEINT64 *iNdx, RESCUEINT64 *jNdx, RESCUEINT64 *kNdx, RESCUEINT64 *faceNdxIn)
{
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < cellMapCount; loop++)
  {
  if (cRef == cellRef[loop]
  &&  cIndx == cellIndx[loop]
  && (cJndx == -1 || ((cellJndx == 0) ? true : cellJndx[loop] == cJndx))
  && (cKndx == -1 || ((cellKndx == 0) ? true : cellKndx[loop] == cKndx)))
  {
      if (myReturn < 0) myReturn++;
      if (faceRowNdx != 0 && allocated > 0)
      {
    if (iNdx != 0)
    {
          iNdx[myReturn] = faceIndx[faceRowNdx[loop]];
    }
    if (jNdx != 0 && faceJndx != 0)
    {
          jNdx[myReturn] = faceJndx[faceRowNdx[loop]];
    }
    if (kNdx != 0 && faceKndx != 0)
    {
          kNdx[myReturn] = faceKndx[faceRowNdx[loop]];
    }
    if (faceNdxIn != 0)
    {
          faceNdxIn[myReturn] = faceNdx[faceRowNdx[loop]];
    }
    allocated--;
      }
      myReturn++;
  }
  }
  return myReturn;
}

void RescueInterfaceSurfaceDesc::CopyArray(RescueISDArrayName whichArray, RESCUEINT32 srcLowBound, RESCUEINT32 srcCount,
                                           RESCUEINT32 *preAllocated, RESCUEINT32 destOffset)
{
  switch (whichArray)
  {
  case FACE_I_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (faceIndx[srcLowBound + loop] > 2147483647 || faceIndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) faceIndx[srcLowBound + loop];
      }
  }
  break;
  case FACE_J_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (faceJndx[srcLowBound + loop] > 2147483647 || faceJndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) faceJndx[srcLowBound + loop];
      }
  }
  break;
  case FACE_K_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (faceKndx[srcLowBound + loop] > 2147483647 || faceKndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) faceKndx[srcLowBound + loop];
      }
  }
  break;
  case FACE_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (faceNdx[srcLowBound + loop] > 2147483647 || faceNdx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) faceNdx[srcLowBound + loop];
      }
  }
  break;
  case CELL_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (cellNdx[srcLowBound + loop] > 2147483647 || cellNdx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) cellNdx[srcLowBound + loop];
      }
  }
  break;
  case CELL_COUNT:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (cellCount[srcLowBound + loop] > 2147483647 || cellCount[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) cellCount[srcLowBound + loop];
      }
  }
  break;
  case CELL_REF:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (cellRef[srcLowBound + loop] > 2147483647 || cellRef[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) cellRef[srcLowBound + loop];
      }
  }
  break;
  case CELL_I_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (cellIndx[srcLowBound + loop] > 2147483647 || cellIndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) cellIndx[srcLowBound + loop];
      }
  }
  break;
  case CELL_J_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (cellJndx[srcLowBound + loop] > 2147483647 || cellJndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) cellJndx[srcLowBound + loop];
      }
  }
  break;
  case CELL_K_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (cellKndx[srcLowBound + loop] > 2147483647 || cellKndx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) cellKndx[srcLowBound + loop];
      }
  }
  break;
  case FACE_ROW_NDX:
  {
      RESCUEINT32 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    if (faceRowNdx[srcLowBound + loop] > 2147483647 || faceRowNdx[srcLowBound + loop] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[destOffset + loop] = (RESCUEINT32) faceRowNdx[srcLowBound + loop];
      }
  }
  break;
  }
}

void RescueInterfaceSurfaceDesc::CopyArray(RescueISDArrayName whichArray, RESCUEINT64 srcLowBound, RESCUEINT64 srcCount,
                                           RESCUEINT64 *preAllocated, RESCUEINT64 destOffset)
{
  switch (whichArray)
  {
  case FACE_I_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = faceIndx[srcLowBound + loop];
      }
  }
  break;
  case FACE_J_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = faceJndx[srcLowBound + loop];
      }
  }
  break;
  case FACE_K_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = faceKndx[srcLowBound + loop];
      }
  }
  break;
  case FACE_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = faceNdx[srcLowBound + loop];
      }
  }
  break;
  case CELL_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = cellNdx[srcLowBound + loop];
      }
  }
  break;
  case CELL_COUNT:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = cellCount[srcLowBound + loop];
      }
  }
  break;
  case CELL_REF:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = cellRef[srcLowBound + loop];
      }
  }
  break;
  case CELL_I_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = cellIndx[srcLowBound + loop];
      }
  }
  break;
  case CELL_J_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = cellJndx[srcLowBound + loop];
      }
  }
  break;
  case CELL_K_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = cellKndx[srcLowBound + loop];
      }
  }
  break;
  case FACE_ROW_NDX:
  {
      RESCUEINT64 loop;
      for (loop = 0; loop < srcCount; loop++)
      {
    preAllocated[destOffset + loop] = faceRowNdx[srcLowBound + loop];
      }
  }
  break;
  }
}

void RescueInterfaceSurfaceDesc::Set(RESCUEINT32 faceCountIn,
                                      RESCUEINT32 *faceIndxIn,
                                      RESCUEINT32 *faceJndxIn,
                                      RESCUEINT32 *faceKndxIn,      
                                      RESCUEINT32 *faceNdxIn,      
                                      RESCUEINT32 *cellNdxIn,      
                                      RESCUEINT32 *cellCountIn,
                                      RESCUEINT32 cellMapCountIn,
                                      RESCUEINT32 *cellRefIn,
                                      RESCUEINT32 *cellIndxIn,
                                      RESCUEINT32 *cellJndxIn,
                                      RESCUEINT32 *cellKndxIn,
                                      RESCUEINT32 *faceRowNdxIn)
{
  if (faceJndxIn == 0 && faceJndx != 0)
  {
  delete [] faceJndx;
  faceJndx = 0;
  }
  if (faceKndxIn == 0 && faceKndx != 0)
  {
  delete [] faceKndx;
  faceKndx = 0;
  }
  faceCount = faceCountIn;
  if (faceCountIn > facesAllocated)
  {
  facesAllocated = faceCountIn;
  delete [] faceIndx;
  faceIndx = new RESCUEINT64[(size_t) facesAllocated];
  if (faceJndxIn != 0)
  {
      delete [] faceJndx;
      faceJndx = new RESCUEINT64[(size_t) facesAllocated];
  }
  if (faceKndx != 0)
  {
      delete [] faceKndx;
      faceKndx = new RESCUEINT64[(size_t) facesAllocated];
  }
  delete [] faceNdx;
  faceNdx = new RESCUEINT64[(size_t) facesAllocated];
  delete [] cellNdx;
  cellNdx = new RESCUEINT64[(size_t) facesAllocated];
  delete [] cellCount;
  cellCount = new RESCUEINT64[(size_t) facesAllocated];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < faceCount; loop++)
  {
  faceIndx[loop] = faceIndxIn[loop];
  if (faceJndxIn != 0 && faceJndx != 0)
  {
      faceJndx[loop] = faceJndxIn[loop];
  }
  if (faceKndx != 0 && faceKndxIn != 0)
  {
      faceKndx[loop] = faceKndxIn[loop];
  }
  faceNdx[loop] = faceNdxIn[loop];
  cellNdx[loop] = cellNdxIn[loop];
  cellCount[loop] = cellCountIn[loop];
  }
  if (cellJndx != 0 && cellJndxIn == 0)
  {
  delete [] cellJndx;
  cellJndx = 0;
  }
  if (cellKndx != 0 && cellKndxIn == 0)
  {
  delete [] cellKndx;
  cellKndx = 0;
  }
  cellMapCount = cellMapCountIn;
  if (cellMapCountIn > cellsAllocated)
  {
  cellsAllocated = cellMapCountIn;
  delete [] cellRef;
  cellRef = new RESCUEINT64[(size_t) cellsAllocated];
  delete [] cellIndx;
  cellIndx = new RESCUEINT64[(size_t) cellsAllocated];
  if (cellJndx != 0)
  {
      delete [] cellJndx;
      cellJndx = new RESCUEINT64[(size_t) cellsAllocated];
  }
  if (cellKndx != 0)
  {
      delete [] cellKndx;
      cellKndx = new RESCUEINT64[(size_t) cellsAllocated];
  }
  delete [] faceRowNdx;
  faceRowNdx = new RESCUEINT64[(size_t) cellsAllocated];
  }
  for (loop = 0; loop < cellMapCount; loop++)
  {
  cellRef[loop] = cellRefIn[loop];
  cellIndx[loop] = cellIndxIn[loop];
  if (cellJndx != 0)
  {
      cellJndx[loop] = cellJndxIn[loop];
  }
  if (cellKndx != 0)
  {
      cellKndx[loop] = cellKndxIn[loop];
  }
  faceRowNdx[loop] = faceRowNdxIn[loop];
  }
}

void RescueInterfaceSurfaceDesc::Set(RESCUEINT64 faceCountIn,
                                      RESCUEINT64 *faceIndxIn,
                                      RESCUEINT64 *faceJndxIn,
                                      RESCUEINT64 *faceKndxIn,      
                                      RESCUEINT64 *faceNdxIn,      
                                      RESCUEINT64 *cellNdxIn,      
                                      RESCUEINT64 *cellCountIn,
                                      RESCUEINT64 cellMapCountIn,
                                      RESCUEINT64 *cellRefIn,
                                      RESCUEINT64 *cellIndxIn,
                                      RESCUEINT64 *cellJndxIn,
                                      RESCUEINT64 *cellKndxIn,
                                      RESCUEINT64 *faceRowNdxIn)
{
  if (faceJndxIn == 0 && faceJndx != 0)
  {
  delete [] faceJndx;
  faceJndx = 0;
  }
  if (faceKndxIn == 0 && faceKndx != 0)
  {
  delete [] faceKndx;
  faceKndx = 0;
  }
  faceCount = faceCountIn;
  if (faceCountIn > facesAllocated)
  {
  facesAllocated = faceCountIn;
  delete [] faceIndx;
  faceIndx = new RESCUEINT64[(size_t) facesAllocated];
  if (faceJndxIn != 0)
  {
      delete [] faceJndx;
      faceJndx = new RESCUEINT64[(size_t) facesAllocated];
  }
  if (faceKndx != 0)
  {
      delete [] faceKndx;
      faceKndx = new RESCUEINT64[(size_t) facesAllocated];
  }
  delete [] faceNdx;
  faceNdx = new RESCUEINT64[(size_t) facesAllocated];
  delete [] cellNdx;
  cellNdx = new RESCUEINT64[(size_t) facesAllocated];
  delete [] cellCount;
  cellCount = new RESCUEINT64[(size_t) facesAllocated];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < faceCount; loop++)
  {
  faceIndx[loop] = faceIndxIn[loop];
  if (faceJndxIn != 0 && faceJndx != 0)
  {
      faceJndx[loop] = faceJndxIn[loop];
  }
  if (faceKndx != 0 && faceKndxIn != 0)
  {
      faceKndx[loop] = faceKndxIn[loop];
  }
  faceNdx[loop] = faceNdxIn[loop];
  cellNdx[loop] = cellNdxIn[loop];
  cellCount[loop] = cellCountIn[loop];
  }
  if (cellJndx != 0 && cellJndxIn == 0)
  {
  delete [] cellJndx;
  cellJndx = 0;
  }
  if (cellKndx != 0 && cellKndxIn == 0)
  {
  delete [] cellKndx;
  cellKndx = 0;
  }
  cellMapCount = cellMapCountIn;
  if (cellMapCountIn > cellsAllocated)
  {
  cellsAllocated = cellMapCountIn;
  delete [] cellRef;
  cellRef = new RESCUEINT64[(size_t) cellsAllocated];
  delete [] cellIndx;
  cellIndx = new RESCUEINT64[(size_t) cellsAllocated];
  if (cellJndx != 0)
  {
      delete [] cellJndx;
      cellJndx = new RESCUEINT64[(size_t) cellsAllocated];
  }
  if (cellKndx != 0)
  {
      delete [] cellKndx;
      cellKndx = new RESCUEINT64[(size_t) cellsAllocated];
  }
  delete [] faceRowNdx;
  faceRowNdx = new RESCUEINT64[(size_t) cellsAllocated];
  }
  for (loop = 0; loop < cellMapCount; loop++)
  {
  cellRef[loop] = cellRefIn[loop];
  cellIndx[loop] = cellIndxIn[loop];
  if (cellJndx != 0)
  {
      cellJndx[loop] = cellJndxIn[loop];
  }
  if (cellKndx != 0)
  {
      cellKndx[loop] = cellKndxIn[loop];
  }
  faceRowNdx[loop] = faceRowNdxIn[loop];
  }
}

void RescueInterfaceSurfaceDesc::Accept(RESCUEINT32 faceCountIn,
                    RESCUEINT32 *faceIndxIn,
                    RESCUEINT32 *faceJndxIn,
                    RESCUEINT32 *faceKndxIn,
                    RESCUEINT32 *faceNdxIn,
                    RESCUEINT32 *cellNdxIn,
                    RESCUEINT32 *cellCountIn,
                    RESCUEINT32 cellMapCountIn,
                    RESCUEINT32 *cellRefIn,
                    RESCUEINT32 *cellIndxIn,
                    RESCUEINT32 *cellJndxIn,
                    RESCUEINT32 *cellKndxIn,
                    RESCUEINT32 *faceRowNdxIn)
{
  faceCount = faceCountIn;
  facesAllocated = faceCountIn;
  if (faceIndx != 0)
  {
  delete [] faceIndx;
  }
  faceIndx = RescueContext::Allocate64For32(faceIndxIn, faceCountIn, true);
  if (faceJndx != 0)
  {
  delete [] faceJndx;
  }
  faceJndx = RescueContext::Allocate64For32(faceJndxIn, faceCountIn, true);
  if (faceKndx != 0)
  {
  delete [] faceKndx;
  }
  faceKndx = RescueContext::Allocate64For32(faceKndxIn, faceCountIn, true);
  if (faceNdx != 0)
  {
  delete [] faceNdx;
  }
  faceNdx = RescueContext::Allocate64For32(faceNdxIn, faceCountIn, true);
  if (cellNdx != 0)
  {
  delete [] cellNdx;
  }
  cellNdx = RescueContext::Allocate64For32(cellNdxIn, faceCountIn, true);
  if (cellCount != 0)
  {
  delete [] cellCount;
  }
  cellCount = RescueContext::Allocate64For32(cellCountIn, faceCountIn, true);
  cellMapCount = cellMapCountIn;
  cellsAllocated = cellMapCountIn;
  if (cellRef != 0)
  {
  delete [] cellRef;
  }
  cellRef = RescueContext::Allocate64For32(cellRefIn, cellMapCountIn, true);
  if (cellIndx != 0)
  {
  delete [] cellIndx;
  }
  cellIndx = RescueContext::Allocate64For32(cellIndxIn, cellMapCountIn, true);
  if (cellJndx != 0)
  {
  delete [] cellJndx;
  }
  cellJndx = RescueContext::Allocate64For32(cellJndxIn, cellMapCountIn, true);
  if (cellKndx != 0)
  {
  delete [] cellKndx;
  }
  cellKndx = RescueContext::Allocate64For32(cellKndxIn, cellMapCountIn, true);
  if (faceRowNdx != 0)
  {
  delete [] faceRowNdx;
  }
  faceRowNdx = RescueContext::Allocate64For32(faceRowNdxIn, cellMapCountIn, true);
}

void RescueInterfaceSurfaceDesc::Accept(RESCUEINT64 faceCountIn,
                    RESCUEINT64 *faceIndxIn,
                    RESCUEINT64 *faceJndxIn,
                    RESCUEINT64 *faceKndxIn,
                    RESCUEINT64 *faceNdxIn,
                    RESCUEINT64 *cellNdxIn,
                    RESCUEINT64 *cellCountIn,
                    RESCUEINT64 cellMapCountIn,
                    RESCUEINT64 *cellRefIn,
                    RESCUEINT64 *cellIndxIn,
                    RESCUEINT64 *cellJndxIn,
                    RESCUEINT64 *cellKndxIn,
                    RESCUEINT64 *faceRowNdxIn)
{
  faceCount = faceCountIn;
  facesAllocated = faceCountIn;
  if (faceIndx != 0)
  {
  delete [] faceIndx;
  }
  faceIndx = faceIndxIn;
  if (faceJndx != 0)
  {
  delete [] faceJndx;
  }
  faceJndx = faceJndxIn;
  if (faceKndx != 0)
  {
  delete [] faceKndx;
  }
  faceKndx = faceKndxIn;
  if (faceNdx != 0)
  {
  delete [] faceNdx;
  }
  faceNdx = faceNdxIn;
  if (cellNdx != 0)
  {
  delete [] cellNdx;
  }
  cellNdx = cellNdxIn;
  if (cellCount != 0)
  {
  delete [] cellCount;
  }
  cellCount = cellCountIn;
  cellMapCount = cellMapCountIn;
  cellsAllocated = cellMapCountIn;
  if (cellRef != 0)
  {
  delete [] cellRef;
  }
  cellRef = cellRefIn;
  if (cellIndx != 0)
  {
  delete [] cellIndx;
  }
  cellIndx = cellIndxIn;
  if (cellJndx != 0)
  {
  delete [] cellJndx;
  }
  cellJndx = cellJndxIn;
  if (cellKndx != 0)
  {
  delete [] cellKndx;
  }
  cellKndx = cellKndxIn;
  if (faceRowNdx != 0)
  {
  delete [] faceRowNdx;
  }
  faceRowNdx = faceRowNdxIn;
}

void RescueInterfaceSurfaceDesc::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, isExterior);
  myfprintf(context, archiveFile, faceCount);
  myfprintf(context, archiveFile, faceIndx, faceCount, FALSE);
  if (faceJndx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, faceJndx, faceCount, FALSE);
  }
  if (faceKndx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, faceKndx, faceCount, FALSE);
  }
  myfprintf(context, archiveFile, faceNdx, faceCount, FALSE);
  myfprintf(context, archiveFile, cellNdx, faceCount, FALSE);
  myfprintf(context, archiveFile, cellCount, faceCount, FALSE);
  myfprintf(context, archiveFile, cellMapCount);
  myfprintf(context, archiveFile, cellRef, cellMapCount, FALSE);
  myfprintf(context, archiveFile, cellIndx, cellMapCount, FALSE);
  if (cellJndx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, cellJndx, cellMapCount, FALSE);
  }
  if (cellKndx == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 1);
  myfprintf(context, archiveFile, cellKndx, cellMapCount, FALSE);
  }
  myfprintf(context, archiveFile, faceRowNdx, cellMapCount, FALSE);
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


RescueInterfaceSurfaceDesc::RescueInterfaceSurfaceDesc(RescueContext *context, FILE *archiveFile)
    :faceCount(0)
    ,faceIndx(0)
    ,faceJndx(0)
    ,faceKndx(0)
    ,faceNdx(0)
    ,cellNdx(0)
    ,cellCount(0)
    ,cellMapCount(0)
    ,cellRef(0)
    ,cellIndx(0)
    ,cellJndx(0)
    ,cellKndx(0)
                  ,properties(0)
    ,propertyContainerId(0)
{
  myfscanf(context, archiveFile, &isExterior);
  myfscanf(context, archiveFile, &faceCount);
  facesAllocated = faceCount;
  faceIndx = new RESCUEINT64[(size_t) facesAllocated];
  myfscanf(context, archiveFile, faceIndx, faceCount, FALSE);
  RESCUEINT64 flag;
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  faceJndx = new RESCUEINT64 [(size_t) facesAllocated];
  myfscanf(context, archiveFile, faceJndx, faceCount, FALSE);
  }
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  faceKndx = new RESCUEINT64 [(size_t) facesAllocated];
  myfscanf(context, archiveFile, faceKndx, faceCount, FALSE);
  }
  faceNdx = new RESCUEINT64[(size_t) facesAllocated];
  myfscanf(context, archiveFile, faceNdx, faceCount, FALSE);
  cellNdx = new RESCUEINT64[(size_t) facesAllocated];
  myfscanf(context, archiveFile, cellNdx, faceCount, FALSE);
  cellCount = new RESCUEINT64[(size_t) facesAllocated];
  myfscanf(context, archiveFile, cellCount, faceCount, FALSE);
  myfscanf(context, archiveFile, &cellMapCount);
  cellsAllocated = cellMapCount;
  cellRef = new RESCUEINT64[(size_t) cellsAllocated];
  myfscanf(context, archiveFile, cellRef, cellMapCount, FALSE);
  cellIndx = new RESCUEINT64[(size_t) cellsAllocated];
  myfscanf(context, archiveFile, cellIndx, cellMapCount, FALSE);
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  cellJndx = new RESCUEINT64 [(size_t) cellsAllocated];
  myfscanf(context, archiveFile, cellJndx, cellMapCount, FALSE);
  }
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
  cellKndx = new RESCUEINT64 [(size_t) cellsAllocated];
  myfscanf(context, archiveFile, cellKndx, cellMapCount, FALSE);
  }
  faceRowNdx = new RESCUEINT64[(size_t) cellsAllocated];
  myfscanf(context, archiveFile, faceRowNdx, cellMapCount, FALSE);
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

void RescueInterfaceSurfaceDesc::Relink(RescueGeometry *geometryIn)
{
  lgrGeometry = geometryIn;
}

RESCUEBOOL RescueInterfaceSurfaceDesc::AnyFileTruncated()
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

cSetRescueDataContainer *RescueInterfaceSurfaceDesc::DataContainers()
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

cSetRescueDataContainer *RescueInterfaceSurfaceDesc::DemandDataContainers()
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

void RescueInterfaceSurfaceDesc::FindUniquePropertyNames(cSetString *container)
{
  cSetRescueDataContainer *containers = DataContainers();
  if (containers != 0)
  {
  containers->FindUniquePropertyNames(container);
  }
}

RESCUEINT32 RescueInterfaceSurfaceDesc::FaceCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = FaceCount64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::FaceIndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceIndx64(), FaceCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::FaceJndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceJndx64(), FaceCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::FaceKndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceKndx64(), FaceCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::FaceNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceNdx64(), FaceCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::CellNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(CellNdx64(), FaceCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::CellCount(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(CellCount64(), FaceCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 RescueInterfaceSurfaceDesc::CellMapCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CellMapCount64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::CellRef(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(CellRef64(), CellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::CellIndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(CellIndx64(), CellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::CellJndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(CellJndx64(), CellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::CellKndx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(CellKndx64(), CellMapCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueInterfaceSurfaceDesc::FaceRowNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceRowNdx64(), CellMapCount(throwIfTooBig), false, throwIfTooBig);
}






