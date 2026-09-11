/********************************************************************

  RescueUnstructuredGrid.cpp

  A geometry handler for RESCUE style corner point grids.
  See description of each array in the private section of
  the header.

  Rod Hanks,  June 2003

*********************************************************************/
#include <stdio.h>
#include <memory.h>
#include "myHeaders.h"
#include "RescueContext.h"
#include "RescueUnstructuredGrid.h"
#include "RescueGeometry.h"
#include "RescueGrid.h"

RescueUnstructuredGrid::~RescueUnstructuredGrid()
{
  if (vertexX != 0)
  {
  delete [] vertexX;
  }
  if (vertexY != 0)
  {
  delete [] vertexY;
  }
  if (vertexZ != 0)
  {
  delete [] vertexZ;
  }
  if (faceVertexNdx != 0)
  {
  delete [] faceVertexNdx;
  }
  if (faceVertexCount != 0)
  {
  delete [] faceVertexCount;
  }
  if (faceArray != 0)
  {
  delete [] faceArray;
  }
  if (faceCellNdx != 0)
  {
  delete [] faceCellNdx;
  }
  if (faceCellCount != 0)
  {
  delete [] faceCellCount;
  }
  if (cellCenterNdx != 0)
  {
  delete [] cellCenterNdx;
  }
  if (kLayer != 0)
  {
  delete [] kLayer;
  }
  if (faceList != 0)
  {
  delete [] faceList;
  }
}

void RescueUnstructuredGrid::AddVertices(RESCUEINT64 count, RESCUEFLOAT *vertexXin, RESCUEFLOAT *vertexYin, RESCUEFLOAT *vertexZin)
{
  if (vertexCount + count > vertexAlloc)
  {
  vertexAlloc += ((count > vertexRealloc) ? count : vertexRealloc);
  size_t sizeNow = (size_t) vertexCount * sizeof(RESCUEFLOAT);
  RESCUEFLOAT *newVertexX = new RESCUEFLOAT[(size_t) vertexAlloc];
  memcpy(newVertexX, vertexX, sizeNow);
  delete [] vertexX;
  vertexX = newVertexX;
  RESCUEFLOAT *newVertexY = new RESCUEFLOAT[(size_t) vertexAlloc];
  memcpy(newVertexY, vertexY, sizeNow);
  delete [] vertexY;
  vertexY = newVertexY;
  RESCUEFLOAT *newVertexZ = new RESCUEFLOAT[(size_t) vertexAlloc];
  memcpy(newVertexZ, vertexZ, sizeNow);
  delete [] vertexZ;
  vertexZ = newVertexZ;
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  vertexX[loop + vertexCount] = vertexXin[loop];
  vertexY[loop + vertexCount] = vertexYin[loop];
  vertexZ[loop + vertexCount] = vertexZin[loop];
  }
  vertexCount += count;
}

void RescueUnstructuredGrid::AddFace(RESCUEINT32 count, RESCUEINT32 *vertexIndices)
{
  if (faceCount + 1 > faceAlloc)
  {
  faceAlloc += faceRealloc;
  size_t sizeNow = (size_t) faceCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newFaceVertexNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newFaceVertexNdx, faceVertexNdx, sizeNow);
  delete [] faceVertexNdx;
  faceVertexNdx = newFaceVertexNdx;
  RESCUEINT64 *newFaceVertexCount = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newFaceVertexCount, faceVertexCount, sizeNow);
  delete [] faceVertexCount;
  faceVertexCount = newFaceVertexCount;
  }
  faceVertexNdx[faceCount] = faceVertexTupleCount;
  faceVertexCount[faceCount++] = count;

  if (faceVertexTupleCount + count > faceVertexAlloc)
  {
  faceVertexAlloc += ((count > faceVertexRealloc) ? count : faceVertexRealloc);
  size_t sizeNow = (size_t) faceVertexTupleCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newFaceArray = new RESCUEINT64[(size_t) faceVertexAlloc];
  memcpy(newFaceArray, faceArray, sizeNow);
  delete [] faceArray;
  faceArray = newFaceArray;
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  faceArray[faceVertexTupleCount + loop] = vertexIndices[loop];
  }
  faceVertexTupleCount += count;
}

void RescueUnstructuredGrid::AddFace(RESCUEINT64 count, RESCUEINT64 *vertexIndices)
{
  if (faceCount + 1 > faceAlloc)
  {
  faceAlloc += faceRealloc;
  size_t sizeNow = (size_t) faceCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newFaceVertexNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newFaceVertexNdx, faceVertexNdx, sizeNow);
  delete [] faceVertexNdx;
  faceVertexNdx = newFaceVertexNdx;
  RESCUEINT64 *newFaceVertexCount = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newFaceVertexCount, faceVertexCount, sizeNow);
  delete [] faceVertexCount;
  faceVertexCount = newFaceVertexCount;
  }
  faceVertexNdx[faceCount] = faceVertexTupleCount;
  faceVertexCount[faceCount++] = count;

  if (faceVertexTupleCount + count > faceVertexAlloc)
  {
  faceVertexAlloc += ((count > faceVertexRealloc) ? count : faceVertexRealloc);
  size_t sizeNow = (size_t) faceVertexTupleCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newFaceArray = new RESCUEINT64[(size_t) faceVertexAlloc];
  memcpy(newFaceArray, faceArray, sizeNow);
  delete [] faceArray;
  faceArray = newFaceArray;
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  faceArray[faceVertexTupleCount + loop] = vertexIndices[loop];
  }
  faceVertexTupleCount += count;
}

void RescueUnstructuredGrid::AddCell(RESCUEINT32 cellCenterNdxIn, RESCUEINT32 kLayerIn,
                                     RESCUEINT32 count, RESCUEINT32 *faceIndices)
{
  if (cellCount + 1 > cellAlloc)
  {
  cellAlloc += cellRealloc;
  size_t sizeNow = (size_t) cellCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, faceCellNdx, sizeNow);
  delete [] faceCellNdx;
  faceCellNdx = newNdx;
  newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, faceCellCount, sizeNow);
  delete [] faceCellCount;
  faceCellCount = newNdx;
  newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, cellCenterNdx, sizeNow);
  delete [] cellCenterNdx;
  cellCenterNdx = newNdx;
  newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, kLayer, sizeNow);
  delete [] kLayer;
  kLayer = newNdx;
  }
  faceCellNdx[cellCount] = faceListCount;
  faceCellCount[cellCount] = count;
  cellCenterNdx[cellCount] = cellCenterNdxIn;
  kLayer[cellCount++] = kLayerIn;

  if (faceListCount + count > faceListAlloc)
  {
  faceListAlloc += ((count > faceListRealloc) ? count : faceListRealloc);
  size_t sizeNow = (size_t) faceListCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, faceList, sizeNow);
  delete [] faceList;
  faceList = newNdx;
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  faceList[faceListCount + loop] = faceIndices[loop];
  }
  faceListCount += loop;
}

void RescueUnstructuredGrid::AddCell(RESCUEINT64 cellCenterNdxIn, RESCUEINT64 kLayerIn,
                                     RESCUEINT64 count, RESCUEINT64 *faceIndices)
{
  if (cellCount + 1 > cellAlloc)
  {
  cellAlloc += cellRealloc;
  size_t sizeNow = (size_t) cellCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, faceCellNdx, sizeNow);
  delete [] faceCellNdx;
  faceCellNdx = newNdx;
  newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, faceCellCount, sizeNow);
  delete [] faceCellCount;
  faceCellCount = newNdx;
  newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, cellCenterNdx, sizeNow);
  delete [] cellCenterNdx;
  cellCenterNdx = newNdx;
  newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, kLayer, sizeNow);
  delete [] kLayer;
  kLayer = newNdx;
  }
  faceCellNdx[cellCount] = faceListCount;
  faceCellCount[cellCount] = count;
  cellCenterNdx[cellCount] = cellCenterNdxIn;
  kLayer[cellCount++] = kLayerIn;

  if (faceListCount + count > faceListAlloc)
  {
  faceListAlloc += ((count > faceListRealloc) ? count : faceListRealloc);
  size_t sizeNow = (size_t) faceListCount * sizeof(RESCUEINT64);
  RESCUEINT64 *newNdx = new RESCUEINT64[(size_t) faceAlloc];
  memcpy(newNdx, faceList, sizeNow);
  delete [] faceList;
  faceList = newNdx;
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  faceList[faceListCount + loop] = faceIndices[loop];
  }
  faceListCount += loop;
}

RESCUEFLOAT RescueUnstructuredGrid::NthVertexX(RESCUEINT64 zeroBasedOrdinal)
{
  RESCUEFLOAT myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < vertexCount)
  {
  myReturn = vertexX[zeroBasedOrdinal];
  }
  return myReturn;
}

RESCUEFLOAT RescueUnstructuredGrid::NthVertexY(RESCUEINT64 zeroBasedOrdinal)
{
  RESCUEFLOAT myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < vertexCount)
  {
  myReturn = vertexY[zeroBasedOrdinal];
  }
  return myReturn;
}

RESCUEFLOAT RescueUnstructuredGrid::NthVertexZ(RESCUEINT64 zeroBasedOrdinal)
{
  RESCUEFLOAT myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < vertexCount)
  {
  myReturn = vertexZ[zeroBasedOrdinal];
  }
  return myReturn;
}

void RescueUnstructuredGrid::CopyVertices(RESCUEINT64 lowNdx, RESCUEINT64 count, 
                                          RESCUEFLOAT *preAllocatedX, RESCUEFLOAT *preAllocatedY, RESCUEFLOAT *preAllocatedZ, 
                                          RESCUEINT64 offset)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count && lowNdx + loop < vertexCount; loop++)
  {
  if (preAllocatedX != 0)
  {
      preAllocatedX[offset + loop] = vertexX[lowNdx + loop];
  }
  if (preAllocatedY != 0)
  {
      preAllocatedY[offset + loop] = vertexY[lowNdx + loop];
  }
  if (preAllocatedZ != 0)
  {
      preAllocatedZ[offset + loop] = vertexZ[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::AcceptVertices(RESCUEINT64 count, RESCUEFLOAT *xs, RESCUEFLOAT *ys, RESCUEFLOAT *zs)
{
  delete [] vertexX;
  delete [] vertexY;
  delete [] vertexZ;
  vertexCount = count;
  vertexAlloc = count;
  vertexX = xs;
  vertexY = ys;
  vertexZ = zs;
}

void RescueUnstructuredGrid::SetVertices(RESCUEINT64 count, RESCUEFLOAT *xs, RESCUEFLOAT *ys, RESCUEFLOAT *zs)
{
  if (count > vertexAlloc)
  {
  delete [] vertexX;
  delete [] vertexY;
  delete [] vertexZ;
  vertexAlloc = count;
  vertexX = new RESCUEFLOAT[(size_t) vertexAlloc];
  vertexY = new RESCUEFLOAT[(size_t) vertexAlloc];
  vertexZ = new RESCUEFLOAT[(size_t) vertexAlloc];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  vertexX[loop] = xs[loop];
  vertexY[loop] = ys[loop];
  vertexZ[loop] = zs[loop];
  }
  vertexCount = count;
}

RESCUEINT32 RescueUnstructuredGrid::NthFace(RESCUEINT32 zeroBasedOrdinal, RESCUEINT32 count, RESCUEINT32 *preAllocated)
{
  RESCUEINT32 myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < faceCount)
  {
  if (faceVertexCount[zeroBasedOrdinal] > 2147483647 || faceVertexCount[zeroBasedOrdinal] < -2147483647)
  {
      throw "Model is too large to be written in 32 bit mode.";
  }
  myReturn = (RESCUEINT32) faceVertexCount[zeroBasedOrdinal];
  if (count > 0 && preAllocated != 0)
  {
      RESCUEINT64 arrayNdx = faceVertexNdx[zeroBasedOrdinal];
      RESCUEINT64 loop;
      for (loop = 0; loop < count && loop < myReturn; loop++)
      {
    if (faceArray[arrayNdx] > 2147483647 || faceArray[arrayNdx] < -2147483647)
    {
          throw "Model is too large to be written in 32 bit mode.";
    }
    preAllocated[loop] = (RESCUEINT32) faceArray[arrayNdx++];
      }
  }
  }
  return myReturn;
}

RESCUEINT64 RescueUnstructuredGrid::NthFace(RESCUEINT64 zeroBasedOrdinal, RESCUEINT64 count, RESCUEINT64 *preAllocated)
{
  RESCUEINT64 myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < faceCount)
  {
  myReturn = faceVertexCount[zeroBasedOrdinal];
  if (count > 0 && preAllocated != 0)
  {
      RESCUEINT64 arrayNdx = faceVertexNdx[zeroBasedOrdinal];
      RESCUEINT64 loop;
      for (loop = 0; loop < count && loop < myReturn; loop++)
      {
    preAllocated[loop] = faceArray[arrayNdx++];
      }
  }
  }
  return myReturn;
}

void RescueUnstructuredGrid::CopyFaces(RESCUEINT32 lowNdx, RESCUEINT32 count, RESCUEINT32 *preAllocatedNdx, 
                                       RESCUEINT32 *preAllocatedCount, RESCUEINT32 offset)
{
  RESCUEINT32 loop;
  for (loop = 0; loop < count && lowNdx + loop < faceCount; loop++)
  {
  if (preAllocatedNdx != 0)
  {
      if (faceVertexNdx[lowNdx + loop] > 2147483647 || faceVertexNdx[lowNdx + loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      preAllocatedNdx[offset + loop] = (RESCUEINT32) faceVertexNdx[lowNdx + loop];
  }
  if (preAllocatedCount != 0)
  {
      if (faceVertexCount[lowNdx + loop] > 2147483647 || faceVertexCount[lowNdx + loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      preAllocatedCount[offset + loop] = (RESCUEINT32) faceVertexCount[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::CopyFaces(RESCUEINT64 lowNdx, RESCUEINT64 count, RESCUEINT64 *preAllocatedNdx, 
                                       RESCUEINT64 *preAllocatedCount, RESCUEINT64 offset)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count && lowNdx + loop < faceCount; loop++)
  {
  if (preAllocatedNdx != 0)
  {
      preAllocatedNdx[offset + loop] = faceVertexNdx[lowNdx + loop];
  }
  if (preAllocatedCount != 0)
  {
      preAllocatedCount[offset + loop] = faceVertexCount[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::CopyFaceVertexTuples(RESCUEINT32 lowNdx, RESCUEINT32 count, RESCUEINT32 *preAllocatedNdx, RESCUEINT32 offset)
{
  RESCUEINT32 loop;
  for (loop = 0; loop < count && lowNdx + loop < faceVertexTupleCount; loop++)
  {
  if (preAllocatedNdx != 0)
  {
      if (faceArray[lowNdx + loop] > 2147483647 || faceArray[lowNdx + loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      preAllocatedNdx[offset + loop] = (RESCUEINT32) faceArray[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::CopyFaceVertexTuples(RESCUEINT64 lowNdx, RESCUEINT64 count, RESCUEINT64 *preAllocatedNdx, RESCUEINT64 offset)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count && lowNdx + loop < faceVertexTupleCount; loop++)
  {
  if (preAllocatedNdx != 0)
  {
      preAllocatedNdx[offset + loop] = faceArray[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::AcceptFaces(RESCUEINT32 faceCountIn, RESCUEINT32 *faceVertexNdxIn, RESCUEINT32 *faceVertexCountIn,
                                         RESCUEINT32 faceVertexTupleCountIn, RESCUEINT32 *faceArrayIn)
{
  delete [] faceVertexNdx;
  delete [] faceVertexCount;
  delete [] faceArray;
  faceCount = faceCountIn;
  faceAlloc = faceCount;
  faceVertexNdx = RescueContext::Allocate64For32(faceVertexNdxIn, faceCountIn, true);
  faceVertexCount = RescueContext::Allocate64For32(faceVertexCountIn, faceCountIn, true);
  faceVertexTupleCount = faceVertexTupleCountIn;
  faceVertexAlloc = faceVertexTupleCount;
  faceArray = RescueContext::Allocate64For32(faceArrayIn, faceVertexTupleCountIn, true);
}

void RescueUnstructuredGrid::AcceptFaces(RESCUEINT64 faceCountIn, RESCUEINT64 *faceVertexNdxIn, RESCUEINT64 *faceVertexCountIn,
                                         RESCUEINT64 faceVertexTupleCountIn, RESCUEINT64 *faceArrayIn)
{
  delete [] faceVertexNdx;
  delete [] faceVertexCount;
  delete [] faceArray;
  faceCount = faceCountIn;
  faceAlloc = faceCount;
  faceVertexNdx = faceVertexNdxIn;
  faceVertexCount = faceVertexCountIn;
  faceVertexTupleCount = faceVertexTupleCountIn;
  faceVertexAlloc = faceVertexTupleCount;
  faceArray = faceArrayIn;
}

void RescueUnstructuredGrid::SetFaces(RESCUEINT32 faceCountIn, RESCUEINT32 *faceVertexNdxIn, RESCUEINT32 *faceVertexCountIn,
                                      RESCUEINT32 faceVertexTupleCountIn, RESCUEINT32 *faceArrayIn)
{
  if (faceCountIn > faceAlloc)
  {
  delete [] faceVertexNdx;
  delete [] faceVertexCount;
  faceAlloc = faceCountIn;
  faceVertexNdx = new RESCUEINT64[(size_t) faceAlloc];
  faceVertexCount = new RESCUEINT64[(size_t) faceAlloc];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < faceCountIn; loop++)
  {
  faceVertexNdx[loop] = faceVertexNdxIn[loop];
  faceVertexCount[loop] = faceVertexCountIn[loop];
  }
  faceCount = faceCountIn;

  if (faceVertexTupleCountIn > faceVertexAlloc)
  {
  delete [] faceArray;
  faceVertexAlloc = faceVertexTupleCountIn;
  faceArray = new RESCUEINT64[(size_t) faceVertexAlloc];
  }
  for (loop = 0; loop < faceVertexTupleCountIn; loop++)
  {
  faceArray[loop] = faceArrayIn[loop];
  }
  faceVertexTupleCount = faceVertexTupleCountIn;
}

void RescueUnstructuredGrid::SetFaces(RESCUEINT64 faceCountIn, RESCUEINT64 *faceVertexNdxIn, RESCUEINT64 *faceVertexCountIn,
                                      RESCUEINT64 faceVertexTupleCountIn, RESCUEINT64 *faceArrayIn)
{
  if (faceCountIn > faceAlloc)
  {
  delete [] faceVertexNdx;
  delete [] faceVertexCount;
  faceAlloc = faceCountIn;
  faceVertexNdx = new RESCUEINT64[(size_t) faceAlloc];
  faceVertexCount = new RESCUEINT64[(size_t) faceAlloc];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < faceCountIn; loop++)
  {
  faceVertexNdx[loop] = faceVertexNdxIn[loop];
  faceVertexCount[loop] = faceVertexCountIn[loop];
  }
  faceCount = faceCountIn;

  if (faceVertexTupleCountIn > faceVertexAlloc)
  {
  delete [] faceArray;
  faceVertexAlloc = faceVertexTupleCountIn;
  faceArray = new RESCUEINT64[(size_t) faceVertexAlloc];
  }
  for (loop = 0; loop < faceVertexTupleCountIn; loop++)
  {
  faceArray[loop] = faceArrayIn[loop];
  }
  faceVertexTupleCount = faceVertexTupleCountIn;
}

RESCUEINT32 RescueUnstructuredGrid::NthCell(RESCUEINT32 zeroBasedOrdinal, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z, RESCUEINT32 &kLayerIn,
                  RESCUEINT32 count, RESCUEINT32 *faceIndices)
{
  RESCUEINT32 myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < cellCount)
  {
  if (faceCellCount[zeroBasedOrdinal] > 2147483647 || faceCellCount[zeroBasedOrdinal] < -2147483647)
  {
      throw "Model is too large to be written in 32 bit mode.";
  }
  myReturn = (RESCUEINT32) faceCellCount[zeroBasedOrdinal];
  RESCUEINT64 centerNdx = cellCenterNdx[zeroBasedOrdinal];
  x = vertexX[centerNdx];
  y = vertexY[centerNdx];
  z = vertexZ[centerNdx];
  if (kLayer[zeroBasedOrdinal] > 2147483647 || kLayer[zeroBasedOrdinal] < -2147483647)
  {
      throw "Model is too large to be written in 32 bit mode.";
  }
  kLayerIn = (RESCUEINT32) kLayer[zeroBasedOrdinal];
  RESCUEINT64 faceNdx = faceCellNdx[zeroBasedOrdinal];

  RESCUEINT64 loop;
  for (loop = 0; loop < count && loop < myReturn; loop++)
  {
      if (faceList[faceNdx] > 2147483647 || faceList[faceNdx] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      faceIndices[loop] = (RESCUEINT32) faceList[faceNdx++];
  }
  }
  return myReturn;
}

RESCUEINT64 RescueUnstructuredGrid::NthCell(RESCUEINT64 zeroBasedOrdinal, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z, RESCUEINT64 &kLayerIn,
                  RESCUEINT64 count, RESCUEINT64 *faceIndices)
{
  RESCUEINT64 myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < cellCount)
  {
  myReturn = faceCellCount[zeroBasedOrdinal];
  RESCUEINT64 centerNdx = cellCenterNdx[zeroBasedOrdinal];
  x = vertexX[centerNdx];
  y = vertexY[centerNdx];
  z = vertexZ[centerNdx];
  kLayerIn = kLayer[zeroBasedOrdinal];
  RESCUEINT64 faceNdx = faceCellNdx[zeroBasedOrdinal];

  RESCUEINT64 loop;
  for (loop = 0; loop < count && loop < myReturn; loop++)
  {
      faceIndices[loop] = faceList[faceNdx++];
  }
  }
  return myReturn;
}

void RescueUnstructuredGrid::CopyCells(RESCUEINT32 lowNdx, RESCUEINT32 count, 
                                       RESCUEINT32 *faceCellNdxArrayIn, RESCUEINT32 *faceCellCountArrayIn, 
                                       RESCUEINT32 *cellCenterNdxArrayIn, RESCUEINT32 *kLayerArrayIn,
                                       RESCUEINT32 offset)
{
  RESCUEINT32 loop;
  for (loop = 0; loop < count && lowNdx + loop < cellCount; loop++)
  {
  if (faceCellNdxArrayIn != 0)
  {
      if (faceCellNdx[lowNdx + loop]> 2147483647 || faceCellNdx[lowNdx + loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      faceCellNdxArrayIn[offset + loop] = (RESCUEINT32) faceCellNdx[lowNdx + loop];
  }
  if (faceCellCountArrayIn != 0)
  {
      if (faceCellCount[lowNdx + loop]> 2147483647 || faceCellCount[lowNdx + loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      faceCellCountArrayIn[offset + loop] = (RESCUEINT32) faceCellCount[lowNdx + loop];
  }
  if (cellCenterNdxArrayIn != 0)
  {
      if (cellCenterNdx[lowNdx + loop]> 2147483647 || cellCenterNdx[lowNdx + loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      cellCenterNdxArrayIn[offset + loop] = (RESCUEINT32) cellCenterNdx[lowNdx + loop];
  }
  if (kLayerArrayIn != 0)
  {
      if (kLayer[lowNdx + loop]> 2147483647 || kLayer[lowNdx + loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      kLayerArrayIn[offset + loop] = (RESCUEINT32) kLayer[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::CopyCells(RESCUEINT64 lowNdx, RESCUEINT64 count, 
                                       RESCUEINT64 *faceCellNdxArrayIn, RESCUEINT64 *faceCellCountArrayIn, 
                                       RESCUEINT64 *cellCenterNdxArrayIn, RESCUEINT64 *kLayerArrayIn,
                                       RESCUEINT64 offset)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count && lowNdx + loop < cellCount; loop++)
  {
  if (faceCellNdxArrayIn != 0)
  {
      faceCellNdxArrayIn[offset + loop] = faceCellNdx[lowNdx + loop];
  }
  if (faceCellCountArrayIn != 0)
  {
      faceCellCountArrayIn[offset + loop] = faceCellCount[lowNdx + loop];
  }
  if (cellCenterNdxArrayIn != 0)
  {
      cellCenterNdxArrayIn[offset + loop] = cellCenterNdx[lowNdx + loop];
  }
  if (kLayerArrayIn != 0)
  {
      kLayerArrayIn[offset + loop] = kLayer[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::CopyCellFaceTuples(RESCUEINT32 lowNdx, RESCUEINT32 count, RESCUEINT32 *faceListArray, RESCUEINT32 offset)
{
  RESCUEINT32 loop;
  for (loop = 0; loop < count && lowNdx + loop < faceListCount; loop++)
  {
  if (faceListArray != 0)
  {
      if (faceList[lowNdx + loop] > 2147483647 || faceList[lowNdx + loop] < -2147483647)
      {
    throw "Model is too large to be written in 32 bit mode.";
      }
      faceListArray[offset + loop] = (RESCUEINT32) faceList[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::CopyCellFaceTuples(RESCUEINT64 lowNdx, RESCUEINT64 count, RESCUEINT64 *faceListArray, RESCUEINT64 offset)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count && lowNdx + loop < faceListCount; loop++)
  {
  if (faceListArray != 0)
  {
      faceListArray[offset + loop] = faceList[lowNdx + loop];
  }
  }
}

void RescueUnstructuredGrid::AcceptCells(RESCUEINT32 cellCountIn, RESCUEINT32 *faceCellNdxIn, RESCUEINT32 *faceCellCountIn, 
                                         RESCUEINT32 *cellCenterNdxIn,
                                         RESCUEINT32 *kLayerIn, RESCUEINT32 faceListCountIn, RESCUEINT32 *faceListIn)
{
  delete [] faceCellNdx;
  delete [] faceCellCount;
  delete [] cellCenterNdx;
  delete [] kLayer;
  cellCount = cellCountIn;
  cellAlloc = cellCount;
  faceCellNdx = RescueContext::Allocate64For32(faceCellNdxIn, cellCountIn, true);
  faceCellCount = RescueContext::Allocate64For32(faceCellCountIn, cellCountIn, true);
  cellCenterNdx = RescueContext::Allocate64For32(cellCenterNdxIn, cellCountIn, true);
  kLayer = RescueContext::Allocate64For32(kLayerIn, cellCountIn, true);
  delete [] faceList;
  faceListCount = faceListCountIn;
  faceListAlloc = faceListCount;
  faceList = RescueContext::Allocate64For32(faceListIn, faceListCountIn, true);

  SetupGrid();
}

void RescueUnstructuredGrid::AcceptCells(RESCUEINT64 cellCountIn, RESCUEINT64 *faceCellNdxIn, RESCUEINT64 *faceCellCountIn, 
                                         RESCUEINT64 *cellCenterNdxIn,
                                         RESCUEINT64 *kLayerIn, RESCUEINT64 faceListCountIn, RESCUEINT64 *faceListIn)
{
  delete [] faceCellNdx;
  delete [] faceCellCount;
  delete [] cellCenterNdx;
  delete [] kLayer;
  cellCount = cellCountIn;
  cellAlloc = cellCount;
  faceCellNdx = faceCellNdxIn;
  faceCellCount = faceCellCountIn;
  cellCenterNdx = cellCenterNdxIn;
  kLayer = kLayerIn;
  delete [] faceList;
  faceListCount = faceListCountIn;
  faceListAlloc = faceListCount;
  faceList = faceListIn;

  SetupGrid();
}

void RescueUnstructuredGrid::SetCells(RESCUEINT32 cellCountIn, RESCUEINT32 *faceCellNdxIn, RESCUEINT32 *faceCellCountIn, RESCUEINT32 *cellCenterNdxIn,
                                      RESCUEINT32 *kLayerIn, RESCUEINT32 faceListCountIn, RESCUEINT32 *faceListIn)
{
  if (cellCountIn > cellAlloc)
  {
  delete [] faceCellNdx;
  delete [] faceCellCount;
  delete [] cellCenterNdx;
  delete [] kLayer;
  cellAlloc = cellCountIn;
  faceCellNdx = new RESCUEINT64[(size_t) cellAlloc];
  faceCellCount = new RESCUEINT64[(size_t) cellAlloc];
  cellCenterNdx = new RESCUEINT64[(size_t) cellAlloc];
  kLayer = new RESCUEINT64[(size_t) cellAlloc];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < cellCountIn; loop++)
  {
  faceCellNdx[loop] = faceCellNdxIn[loop];
  faceCellCount[loop] = faceCellCountIn[loop];
  cellCenterNdx[loop] = cellCenterNdxIn[loop];
  kLayer[loop] = kLayerIn[loop];
  }
  cellCount = cellCountIn;

  if (faceListCountIn > faceListAlloc)
  {
  delete [] faceList;
  faceListAlloc = faceListCountIn;
  faceList = new RESCUEINT64[(size_t) faceListAlloc];
  }
  for (loop = 0; loop < faceListCountIn; loop++)
  {
  faceList[loop] = faceListIn[loop];
  }
  faceListCount = faceListCountIn;

  SetupGrid();
}

void RescueUnstructuredGrid::SetCells(RESCUEINT64 cellCountIn, RESCUEINT64 *faceCellNdxIn, RESCUEINT64 *faceCellCountIn, RESCUEINT64 *cellCenterNdxIn,
                                      RESCUEINT64 *kLayerIn, RESCUEINT64 faceListCountIn, RESCUEINT64 *faceListIn)
{
  if (cellCountIn > cellAlloc)
  {
  delete [] faceCellNdx;
  delete [] faceCellCount;
  delete [] cellCenterNdx;
  delete [] kLayer;
  cellAlloc = cellCountIn;
  faceCellNdx = new RESCUEINT64[(size_t) cellAlloc];
  faceCellCount = new RESCUEINT64[(size_t) cellAlloc];
  cellCenterNdx = new RESCUEINT64[(size_t) cellAlloc];
  kLayer = new RESCUEINT64[(size_t) cellAlloc];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < cellCountIn; loop++)
  {
  faceCellNdx[loop] = faceCellNdxIn[loop];
  faceCellCount[loop] = faceCellCountIn[loop];
  cellCenterNdx[loop] = cellCenterNdxIn[loop];
  kLayer[loop] = kLayerIn[loop];
  }
  cellCount = cellCountIn;

  if (faceListCountIn > faceListAlloc)
  {
  delete [] faceList;
  faceListAlloc = faceListCountIn;
  faceList = new RESCUEINT64[(size_t) faceListAlloc];
  }
  for (loop = 0; loop < faceListCountIn; loop++)
  {
  faceList[loop] = faceListIn[loop];
  }
  faceListCount = faceListCountIn;

  SetupGrid();
}

void RescueUnstructuredGrid::SetupGrid()
{
  parentGeometry->Grid()->Axis(0)->SetCount(cellCount + 1);
}

RescueUnstructuredGrid::RescueUnstructuredGrid(RescueGeometry *lgrGeometry,
                                               RESCUEINT64 expectedVertexCount,            
                                               RESCUEINT64 expectedFaceCount,
                                               RESCUEINT64 expectedFaceVertexTuples,
                                               RESCUEINT64 expectedCellCount,
                                               RESCUEINT64 expectedCellFaceTuples,
                                               RESCUEINT64 expectedVertexRealloc,
                                               RESCUEINT64 expectedFaceRealloc,
                                               RESCUEINT64 expectedFaceVertexRealloc,
                                               RESCUEINT64 expectedCellRealloc,
                                               RESCUEINT64 expectedCellFaceRealloc)
{
  parentGeometry = lgrGeometry;
  lgrGeometry->SetUnstructuredGrid(this);

  vertexCount = 0;
  vertexAlloc = expectedVertexCount;
  vertexRealloc = ((expectedVertexRealloc <= 0) ? 1 : expectedVertexRealloc);
  vertexX = new RESCUEFLOAT[(size_t) vertexAlloc];
  vertexY = new RESCUEFLOAT[(size_t) vertexAlloc];
  vertexZ = new RESCUEFLOAT[(size_t) vertexAlloc];

  faceCount = 0;
  faceAlloc = expectedFaceCount;
  faceRealloc = ((expectedFaceRealloc <= 0) ? 1 : expectedFaceRealloc);
  faceVertexNdx = new RESCUEINT64[(size_t) faceAlloc];
  faceVertexCount = new RESCUEINT64[(size_t) faceAlloc];

  faceVertexTupleCount = 0;
  faceVertexAlloc = expectedFaceVertexTuples;
  faceVertexRealloc = ((expectedFaceVertexRealloc <= 0) ? 1 : expectedFaceVertexRealloc);
  faceArray = new RESCUEINT64[(size_t) faceVertexAlloc];

  cellCount = 0;
  cellAlloc = expectedCellCount;
  cellRealloc = ((expectedCellRealloc <= 0) ? 1 : expectedCellRealloc);
  faceCellNdx = new RESCUEINT64[(size_t) cellAlloc];
  faceCellCount = new RESCUEINT64[(size_t) cellAlloc];
  cellCenterNdx = new RESCUEINT64[(size_t) cellAlloc];
  kLayer = new RESCUEINT64[(size_t) cellAlloc];

  faceListCount = 0;
  faceListAlloc = expectedCellFaceTuples;
  faceListRealloc = ((expectedCellFaceRealloc <= 0) ? 1 : expectedCellFaceRealloc);
  faceList = new RESCUEINT64[(size_t) faceListAlloc];
}

void RescueUnstructuredGrid::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, vertexCount);
  myfprintf(context, archiveFile, vertexRealloc);
  myfprintf(context, archiveFile, vertexX, vertexCount, FALSE);
  myfprintf(context, archiveFile, vertexY, vertexCount, FALSE);
  myfprintf(context, archiveFile, vertexZ, vertexCount, FALSE);

  myfprintf(context, archiveFile, faceCount);
  myfprintf(context, archiveFile, faceRealloc);
  myfprintf(context, archiveFile, faceVertexNdx, faceCount, FALSE);
  myfprintf(context, archiveFile, faceVertexCount, faceCount, FALSE);

  myfprintf(context, archiveFile, faceVertexTupleCount);
  myfprintf(context, archiveFile, faceVertexRealloc);
  myfprintf(context, archiveFile, faceArray, faceVertexTupleCount, FALSE);

  myfprintf(context, archiveFile, cellCount);
  myfprintf(context, archiveFile, cellRealloc);
  myfprintf(context, archiveFile, faceCellNdx, cellCount, FALSE);
  myfprintf(context, archiveFile, faceCellCount, cellCount, FALSE);
  myfprintf(context, archiveFile, cellCenterNdx, cellCount, FALSE);
  myfprintf(context, archiveFile, kLayer, cellCount, FALSE);

  myfprintf(context, archiveFile, faceListCount);
  myfprintf(context, archiveFile, faceListRealloc);
  myfprintf(context, archiveFile, faceList, faceListCount);
}


RescueUnstructuredGrid::RescueUnstructuredGrid(RescueContext *context, FILE *archiveFile)
{
  myfscanf(context, archiveFile, &vertexCount);
  myfscanf(context, archiveFile, &vertexRealloc);
  vertexAlloc = vertexCount;
  vertexX = new RESCUEFLOAT[(size_t) vertexCount];
  myfscanf(context, archiveFile, vertexX, vertexCount, FALSE);
  vertexY = new RESCUEFLOAT[(size_t) vertexCount];
  myfscanf(context, archiveFile, vertexY, vertexCount, FALSE);
  vertexZ = new RESCUEFLOAT[(size_t) vertexCount];
  myfscanf(context, archiveFile, vertexZ, vertexCount, FALSE);

  myfscanf(context, archiveFile, &faceCount);
  myfscanf(context, archiveFile, &faceRealloc);
  faceAlloc = faceCount;
  faceVertexNdx = new RESCUEINT64[(size_t) faceCount];
  myfscanf(context, archiveFile, faceVertexNdx, faceCount, FALSE);
  faceVertexCount = new RESCUEINT64[(size_t) faceCount];
  myfscanf(context, archiveFile, faceVertexCount, faceCount, FALSE);

  myfscanf(context, archiveFile, &faceVertexTupleCount);
  myfscanf(context, archiveFile, &faceVertexRealloc);
  faceVertexAlloc = faceVertexTupleCount;
  faceArray = new RESCUEINT64[(size_t) faceVertexTupleCount];
  myfscanf(context, archiveFile, faceArray, faceVertexTupleCount, FALSE);

  myfscanf(context, archiveFile, &cellCount);
  myfscanf(context, archiveFile, &cellRealloc);
  cellAlloc = cellCount;
  faceCellNdx = new RESCUEINT64[(size_t) cellAlloc];
  myfscanf(context, archiveFile, faceCellNdx, cellCount, FALSE);
  faceCellCount = new RESCUEINT64[(size_t) cellAlloc];
  myfscanf(context, archiveFile, faceCellCount, cellCount, FALSE);
  cellCenterNdx = new RESCUEINT64[(size_t) cellAlloc];
  myfscanf(context, archiveFile, cellCenterNdx, cellCount, FALSE);
  kLayer = new RESCUEINT64[(size_t) cellAlloc];
  myfscanf(context, archiveFile, kLayer, cellCount, FALSE);

  myfscanf(context, archiveFile, &faceListCount);
  faceListAlloc = faceListCount;
  myfscanf(context, archiveFile, &faceListRealloc);
  faceList = new RESCUEINT64[(size_t) faceListAlloc];
  myfscanf(context, archiveFile, faceList, faceListCount);
}

void RescueUnstructuredGrid::Relink(RescueObject *object)
{
  parentGeometry = (RescueGeometry *) object;
}

void RescueUnstructuredGrid::CalculateMinMax(RESCUEFLOAT &minX, RESCUEFLOAT &maxX, 
                                       RESCUEFLOAT &minY, RESCUEFLOAT &maxY, 
                                       RESCUEFLOAT &minZ, RESCUEFLOAT &maxZ)
{
  RESCUEFLOAT missingValue = parentGeometry->missingValue;
  RESCUEINT64 howMany = VertexCount64();
  bool firstXIteration = true;
  bool firstYIteration = true;
  bool firstZIteration = true;
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
  if (vertexX[loop] != missingValue)
  {
      if (firstXIteration)
      {
    minX = vertexX[loop];
    maxX = vertexX[loop];
    firstXIteration = false;
      }
      else
      {
    if (vertexX[loop] < minX)
    {
          minX = vertexX[loop];
    }
    if (vertexX[loop] > maxX)
    {
          maxX = vertexX[loop];
    }
      }
  }
  if (vertexY[loop] != missingValue)
  {
      if (firstYIteration)
      {
    minY = vertexY[loop];
    maxY = vertexY[loop];
    firstYIteration = false;
      }
      else
      {
    if (vertexY[loop] < minY)
    {
          minY = vertexY[loop];
    }
    if (vertexY[loop] > maxY)
    {
          maxY = vertexY[loop];
    }
      }
  }
  if (vertexZ[loop] != missingValue)
  {
      if (firstZIteration)
      {
    minZ = vertexZ[loop];
    maxZ = vertexZ[loop];
    firstZIteration = false;
      }
      else
      {
    if (vertexZ[loop] < minZ)
    {
          minZ = vertexZ[loop];
    }
    if (vertexZ[loop] > maxZ)
    {
          maxZ = vertexZ[loop];
    }
      }
  }
  }
}

RESCUEINT32 RescueUnstructuredGrid::CellCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = CellCount64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 *RescueUnstructuredGrid::FaceCellNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceCellNdx(), CellCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueUnstructuredGrid::FaceCellCount(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceCellCount(), CellCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueUnstructuredGrid::CellCenterNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(CellCenterNdx(), CellCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueUnstructuredGrid::KLayer(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(KLayer(), CellCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 RescueUnstructuredGrid::FaceCount(RESCUEBOOL throwIfTooBig)
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

RESCUEINT32 *RescueUnstructuredGrid::FaceVertexNdx(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceVertexNdx(), FaceCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 *RescueUnstructuredGrid::FaceVertexCount(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceVertexCount(), FaceCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 RescueUnstructuredGrid::FaceVertexTupleCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = FaceVertexTupleCount64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 *RescueUnstructuredGrid::FaceArray(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceArray(), FaceVertexTupleCount(throwIfTooBig), false, throwIfTooBig);
}

RESCUEINT32 RescueUnstructuredGrid::FaceListCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = FaceListCount64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 *RescueUnstructuredGrid::FaceList(RESCUEBOOL throwIfTooBig)
{
  return RescueContext::Allocate32For64(FaceList(), FaceListCount(throwIfTooBig), false, throwIfTooBig);
}



