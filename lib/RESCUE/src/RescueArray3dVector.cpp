/********************************************************************

  RescueArray3dVector.cpp

  A dynamically sized array handler for N dimensional grids of
  3d floating point vectors for RESCUE's data model. The related grid gives
  the dimensionality.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "RescueArray3dVector.h"
#include "RescueArrayFragment3dVector.h"
#include "RescueBuffer.h"
#include "RescueModel.h"
#include "cSetRescueArrayFragment.h"
#include <string.h>

RescueArray3dVector::~RescueArray3dVector() {
  if (value != 0) {
    delete[] value;
  }
}

Rescue3dVector *RescueArray3dVector::DemandValue() {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  return value;
}

void RescueArray3dVector::AllValues(Rescue3dVector *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength) {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  if (value != 0) {
    Rescue3dVector *pos = &buffer[offset];
    Rescue3dVector *src = value;
    while (bufferLength > 0) {
      *pos++ = *src++;
      bufferLength--;
    }
  }
  /*
    If value == 0, valueLength should be zero too.
  */
}

RescueArray3dVector::RescueArray3dVector(RescueContext *context, FILE *archiveFile)
    : RescueArray(context, archiveFile), value(0), valueLength(0) {
  isA = R_RescueArray3dVector;
  InitMinMax();
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      if (strcmp(myString, "dirtyMinMax") == 0) {
        RescueBuffer buf(context, archiveFile);
        buf >> dirtyMinMax;
      } else if (strcmp(myString, "minMax") == 0) {
        ReadMinMax(context, archiveFile);
      } else {
        RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
    }
  }
  myfscanf(context, archiveFile, &nullValue.coord1);
  myfscanf(context, archiveFile, &nullValue.coord2);
  myfscanf(context, archiveFile, &nullValue.coord3);
}

void RescueArray3dVector::Archive(FILE *archiveFile) {
  RescueArray::Archive(archiveFile);
  myfprintf(ParentModel()->Context(), archiveFile, nullValue.coord1);
  myfprintf(ParentModel()->Context(), archiveFile, nullValue.coord2);
  myfprintf(ParentModel()->Context(), archiveFile, nullValue.coord3);
}

void RescueArray3dVector::CalculateMinMaxData(FILE *fragmentFile, RESCUEINT64 fileVersion) {
  if (value == 0) {
    bool firstIteration = true;
    minValue.coord1 = 0;
    minValue.coord2 = 0;
    maxValue.coord1 = 0;
    maxValue.coord2 = 0;
    if (HasFragments()) {
      RESCUEINT64 count = 0;
      if (fragmentFile != 0) {
        myfscanf(ParentModel()->Context(), fragmentFile, &count);
      }
      if (count == 0) {
        count = grid->NodeCount(cellCentered);
      }
      if (count != 0) {
        RESCUEINT64 axisDecrement = 0;
        if (CellCentered()) {
          axisDecrement = 1;
        }
        RESCUEINT64 fragCount = fragments->Count64();

        RescueGridAxis *axis = grid->Axis(0);
        RESCUEINT64 origILowBound = axis->LowBound64();
        RESCUEINT64 origICount = axis->Count64() - axisDecrement;

        axis = grid->Axis(1);
        RESCUEINT64 origJLowBound = axis->LowBound64();
        RESCUEINT64 origJCount = axis->Count64() - axisDecrement;
        RESCUEINT64 origKLowBound = -1;
        if (grid->Dimensions() >= 3) {
          axis = grid->Axis(2);
          origKLowBound = axis->LowBound64();
        }
        RESCUEINT64 iFollower = 0;
        RESCUEINT64 jFollower = 0;
        RESCUEINT64 kFollower = 0;

        RESCUEINT64 loop;
        for (loop = 0; loop < count; loop++) {
          Rescue3dVector data = nullValue;
          if (fragmentFile != 0) {
            myfscanf(ParentModel()->Context(), fragmentFile, &data.coord1);
            myfscanf(ParentModel()->Context(), fragmentFile, &data.coord2);
          }

          RESCUEINT64 frag;
          for (frag = 0; frag < fragCount; frag++) {
            data = ((RescueArrayFragment3dVector *)fragments->NthObject(frag))
                       ->Replace(origILowBound + iFollower, origJLowBound + jFollower, origKLowBound + kFollower, data);
          }

          if (data.coord1 != nullValue.coord1 || data.coord2 != nullValue.coord2 || data.coord3 != nullValue.coord3) {
            if (firstIteration) {
              minValue = data;
              maxValue = data;
              firstIteration = false;
            } else {
              if (data.coord1 < minValue.coord1) {
                minValue.coord1 = data.coord1;
              }
              if (data.coord2 < minValue.coord2) {
                minValue.coord2 = data.coord2;
              }
              if (data.coord3 < minValue.coord3) {
                minValue.coord3 = data.coord3;
              }
              if (data.coord1 > maxValue.coord1) {
                maxValue.coord1 = data.coord1;
              }
              if (data.coord2 > maxValue.coord2) {
                maxValue.coord2 = data.coord2;
              }
              if (data.coord3 > maxValue.coord3) {
                maxValue.coord3 = data.coord3;
              }
            }
          }

          iFollower++;
          if (iFollower >= origICount) {
            iFollower = 0;
            jFollower++;
            if (jFollower >= origJCount) {
              jFollower = 0;
              kFollower++;
            }
          }
        }
      }
    }
  } else {
    RESCUEUINT64 items = (RESCUEUINT64)grid->NodeCount(cellCentered);
    Rescue3dVector *ptr = value;
    bool firstIteration = true;
    while (items > 0) {
      if (ptr->coord1 != nullValue.coord1 || ptr->coord2 != nullValue.coord2 || ptr->coord3 != nullValue.coord3) {
        if (firstIteration) {
          minValue = *ptr;
          maxValue = *ptr;
          firstIteration = false;
        } else {
          if (ptr->coord1 < minValue.coord1) {
            minValue.coord1 = ptr->coord1;
          }
          if (ptr->coord2 < minValue.coord2) {
            minValue.coord2 = ptr->coord2;
          }
          if (ptr->coord3 < minValue.coord3) {
            minValue.coord3 = ptr->coord3;
          }
          if (ptr->coord1 > maxValue.coord1) {
            maxValue.coord1 = ptr->coord1;
          }
          if (ptr->coord2 > maxValue.coord2) {
            maxValue.coord2 = ptr->coord2;
          }
          if (ptr->coord3 > maxValue.coord3) {
            maxValue.coord3 = ptr->coord3;
          }
        }
      }
      ptr++;
      items--;
    }
  }
}

void RescueArray3dVector::ReadMinMax(RescueContext *context, FILE *archiveFile) {
  RescueBuffer buf(context, archiveFile);
  buf >> minValue.coord1;
  buf >> minValue.coord2;
  buf >> minValue.coord2;
  buf >> maxValue.coord1;
  buf >> maxValue.coord2;
  buf >> maxValue.coord3;
}

void RescueArray3dVector::WriteMinMax(RescueContext *context, FILE *archiveFile) {
  RescueBuffer buf(context, 50);
  buf << minValue.coord1;
  buf << minValue.coord2;
  buf << minValue.coord3;
  buf << maxValue.coord1;
  buf << maxValue.coord2;
  buf << maxValue.coord3;
  buf.Archive(archiveFile);
}

void RescueArray3dVector::InitMinMax(void) {
  minValue.coord1 = 0;
  minValue.coord2 = 0;
  maxValue.coord1 = 0;
  maxValue.coord2 = 0;
}

void RescueArray3dVector::ArchiveData(FILE *archiveFile, FILE *fragmentFile, RESCUEINT64 fileVersion) {
  if (value == 0) {
    if (HasFragments()) {
      RESCUEINT64 count = 0;
      if (fragmentFile != 0) {
        myfscanf(ParentModel()->Context(), fragmentFile, &count);
      }
      if (count == 0) {
        count = grid->NodeCount(cellCentered);
      }
      myfprintf(ParentModel()->Context(), archiveFile, count);
      if (count != 0) {
        RESCUEINT64 axisDecrement = 0;
        if (CellCentered()) {
          axisDecrement = 1;
        }
        RESCUEINT64 fragCount = fragments->Count64();

        RescueGridAxis *axis = grid->Axis(0);
        RESCUEINT64 origILowBound = axis->LowBound64();
        RESCUEINT64 origICount = axis->Count64() - axisDecrement;

        axis = grid->Axis(1);
        RESCUEINT64 origJLowBound = axis->LowBound64();
        RESCUEINT64 origJCount = axis->Count64() - axisDecrement;
        RESCUEINT64 origKLowBound = -1;
        if (grid->Dimensions() >= 3) {
          axis = grid->Axis(2);
          origKLowBound = axis->LowBound64();
        }
        RESCUEINT64 iFollower = 0;
        RESCUEINT64 jFollower = 0;
        RESCUEINT64 kFollower = 0;

        RESCUEINT64 loop;
        for (loop = 0; loop < count; loop++) {
          Rescue3dVector data = nullValue;
          if (fragmentFile != 0) {
            myfscanf(ParentModel()->Context(), fragmentFile, &data.coord1);
            myfscanf(ParentModel()->Context(), fragmentFile, &data.coord2);
            myfscanf(ParentModel()->Context(), fragmentFile, &data.coord3);
          }

          RESCUEINT64 frag;
          for (frag = 0; frag < fragCount; frag++) {
            data = ((RescueArrayFragment3dVector *)fragments->NthObject(frag))
                       ->Replace(origILowBound + iFollower, origJLowBound + jFollower, origKLowBound + kFollower, data);
          }
          myfprintf(ParentModel()->Context(), archiveFile, data.coord1);
          myfprintf(ParentModel()->Context(), archiveFile, data.coord2);
          myfprintf(ParentModel()->Context(), archiveFile, data.coord3);
          iFollower++;
          if (iFollower >= origICount) {
            iFollower = 0;
            jFollower++;
            if (jFollower >= origJCount) {
              jFollower = 0;
              kFollower++;
            }
          }
        }
      }
    } else {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEINT64)0);
    }
  } else {
    RESCUEINT64 items = grid->NodeCount(cellCentered);

    myfprintf(ParentModel()->Context(), archiveFile, items);
    RESCUEINT64 loop;
    for (loop = 0; loop < items; loop++) {
      myfprintf(ParentModel()->Context(), archiveFile, value[loop].coord1);
      myfprintf(ParentModel()->Context(), archiveFile, value[loop].coord2);
      myfprintf(ParentModel()->Context(), archiveFile, value[loop].coord3);
    }
  }
}

void RescueArray3dVector::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion) {
  myfscanf(ParentModel()->Context(), archiveFile, &valueLength);
  if (valueLength != 0) {
    value = new Rescue3dVector[(int)valueLength];
    RESCUEINT64 loop;
    for (loop = 0; loop < valueLength; loop++) {
      myfscanf(ParentModel()->Context(), archiveFile, &value[loop].coord1);
      myfscanf(ParentModel()->Context(), archiveFile, &value[loop].coord2);
      myfscanf(ParentModel()->Context(), archiveFile, &value[loop].coord3);
    }
  }
}

void RescueArray3dVector::SetValue(Rescue3dVector nullValueIn, Rescue3dVector *valueArray) {
  DropFragments();
  valueLength = grid->NodeCount(cellCentered);

  if (value != 0) {
    delete[] value;
  }
  nullValue = nullValueIn;
  value = new Rescue3dVector[(int)valueLength];

  RESCUEINT64 loop;
  for (loop = 0; loop < valueLength; loop++) {
    value[loop] = valueArray[loop];
  }
}

void RescueArray3dVector::AcceptValue(Rescue3dVector nullValueIn, Rescue3dVector *valueArray) {
  DropFragments();
  if (value != 0) {
    delete[] value;
  }
  nullValue = nullValueIn;
  value = valueArray;
  valueLength = grid->NodeCount(cellCentered);
}

RescueArrayFragment *RescueArray3dVector::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount, RESCUEINT32 jLowBound,
                                                 RESCUEINT32 jCount) {
  RescueArrayFragment *myReturn =
      CreatePrimitive((RESCUEINT64)iLowBound, (RESCUEINT64)iCount, (RESCUEINT64)jLowBound, (RESCUEINT64)jCount);
  ((RescueArrayFragment3dVector *)myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount, RESCUEINT64 jLowBound,
                                                 RESCUEINT64 jCount) {
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  ((RescueArrayFragment3dVector *)myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                                          RESCUEINT64 jLowBound, RESCUEINT64 jCount) {
  RescueArrayFragment *myReturn = 0;
  if (grid->Dimensions() == 3) {
    RescueGridAxis *kAxis = grid->Axis(2);
    myReturn = new RescueArrayFragment3dVector(this, 3, iLowBound, iCount, jLowBound, jCount, kAxis->LowBound64(),
                                               kAxis->Count64());
  } else {
    myReturn = new RescueArrayFragment3dVector(this, 2, iLowBound, iCount, jLowBound, jCount, -1, -1);
  }
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount, RESCUEINT32 jLowBound,
                                                 RESCUEINT32 jCount, RESCUEINT32 kLowBound, RESCUEINT32 kCount) {
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64)iLowBound, (RESCUEINT64)iCount, (RESCUEINT64)jLowBound,
                                                  (RESCUEINT64)jCount, (RESCUEINT64)kLowBound, (RESCUEINT64)kCount);
  ((RescueArrayFragment3dVector *)myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount, RESCUEINT64 jLowBound,
                                                 RESCUEINT64 jCount, RESCUEINT64 kLowBound, RESCUEINT64 kCount) {
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  ((RescueArrayFragment3dVector *)myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                                          RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                                          RESCUEINT64 kLowBound, RESCUEINT64 kCount) {
  RescueArrayFragment *myReturn =
      new RescueArrayFragment3dVector(this, 3, iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount, RESCUEINT32 jLowBound,
                                               RESCUEINT32 jCount, RESCUEINT32 kLowBound, RESCUEINT32 kCount) {
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64)iLowBound, (RESCUEINT64)iCount, (RESCUEINT64)jLowBound,
                                                  (RESCUEINT64)jCount, (RESCUEINT64)kLowBound, (RESCUEINT64)kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount, RESCUEINT32 jLowBound,
                                               RESCUEINT32 jCount) {
  RescueArrayFragment *myReturn =
      CreatePrimitive((RESCUEINT64)iLowBound, (RESCUEINT64)iCount, (RESCUEINT64)jLowBound, (RESCUEINT64)jCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount, RESCUEINT64 jLowBound,
                                               RESCUEINT64 jCount, RESCUEINT64 kLowBound, RESCUEINT64 kCount) {
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArray3dVector::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount, RESCUEINT64 jLowBound,
                                               RESCUEINT64 jCount) {
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  LoadFragment(myReturn);
  return myReturn;
}

void RescueArray3dVector::LoadAndSwapArray() {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  RESCUEINT64 fNdx = 0;
  RESCUEINT64 eNdx = valueLength - 1;
  while (fNdx < eNdx) {
    Rescue3dVector temp = value[fNdx];
    value[fNdx] = value[eNdx];
    value[eNdx] = temp;
    fNdx++;
    eNdx--;
  }
  MarkChanged();
  Unload();
}

void RescueArray3dVector::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes, bool swapK,
                                   RESCUEINT64 kNodes) {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  if (swapI) {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes * kNodes; jLoop++) {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx) {
        Rescue3dVector temp = value[fNdx];
        value[fNdx] = value[eNdx];
        value[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ) {
    RESCUEINT64 kLoop;
    for (kLoop = 0; kLoop < kNodes; kLoop++) {
      RESCUEINT64 offset = kLoop * iNodes * jNodes;
      RESCUEINT64 ifNdx = 0;
      RESCUEINT64 efNdx = (jNodes - 1);
      while (ifNdx < efNdx) {
        RESCUEINT64 iLoop;
        for (iLoop = 0; iLoop < iNodes; iLoop++) {
          Rescue3dVector temp = value[offset + (ifNdx * iNodes) + iLoop];
          value[offset + (ifNdx * iNodes) + iLoop] = value[offset + (efNdx * iNodes) + iLoop];
          value[offset + (efNdx * iNodes) + iLoop] = temp;
        }
        ifNdx++;
        efNdx--;
      }
    }
  }
  if (swapK) {
    RESCUEINT64 kfNdx = 0;
    RESCUEINT64 keNdx = kNodes - 1;
    while (kfNdx < keNdx) {
      RESCUEINT64 ijLoop;
      for (ijLoop = 0; ijLoop < iNodes * jNodes; ijLoop++) {
        Rescue3dVector temp = value[(kfNdx * iNodes * jNodes) + ijLoop];
        value[(kfNdx * iNodes * jNodes) + ijLoop] = value[(keNdx * iNodes * jNodes) + ijLoop];
        value[(keNdx * iNodes * jNodes) + ijLoop] = temp;
      }
      kfNdx++;
      keNdx--;
    }
  }
  MarkChanged();
  Unload();
}

void RescueArray3dVector::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes) {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  if (swapI) {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes; jLoop++) {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx) {
        Rescue3dVector temp = value[fNdx];
        value[fNdx] = value[eNdx];
        value[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ) {
    RESCUEINT64 ifNdx = 0;
    RESCUEINT64 efNdx = jNodes - 1;
    while (ifNdx < efNdx) {
      RESCUEINT64 iLoop;
      for (iLoop = 0; iLoop < iNodes; iLoop++) {
        Rescue3dVector temp = value[(ifNdx * iNodes) + iLoop];
        value[(ifNdx * iNodes) + iLoop] = value[(efNdx * iNodes) + iLoop];
        value[(efNdx * iNodes) + iLoop] = temp;
      }
      ifNdx++;
      efNdx--;
    }
  }
  MarkChanged();
  Unload();
}

RESCUEBOOL RescueArray3dVector::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueArray3dVector) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}
