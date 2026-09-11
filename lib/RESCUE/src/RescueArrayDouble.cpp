/********************************************************************

  RescueArrayDouble.cpp

  A dynamically sized array handler for N dimensional grids of
  floating points for RESCUE's data model. The related grid gives
  the dimensionality.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueArrayDouble.h"
#include "RescueArrayFragmentDouble.h"
#include "RescueBuffer.h"
#include "RescueModel.h"
#include "cSetRescueArrayFragment.h"
#include <string.h>

RescueArrayDouble::~RescueArrayDouble() {
  if (value != 0) {
    delete[] value;
  }
}

RESCUEDOUBLE *RescueArrayDouble::DemandValue() {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  return value;
}

void RescueArrayDouble::AllValues(RESCUEDOUBLE *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength) {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  if (value != 0) {
    RESCUEDOUBLE *pos = &buffer[offset];
    RESCUEDOUBLE *src = value;
    while (bufferLength > 0) {
      *pos++ = *src++;
      bufferLength--;
    }
  }
  /*
    If value == 0, valueLength should be zero too.
  */
}

RescueArrayDouble::RescueArrayDouble(RescueContext *context, FILE *archiveFile)
    : RescueArray(context, archiveFile), value(0), valueLength(0) {
  isA = R_RescueArrayDouble;
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
  myfscanf(ParentModel()->Context(), archiveFile, &nullValue);
  if (ParentModel()->Context()->ReadFileVersion() < 12) {
    myfscanf(ParentModel()->Context(), archiveFile, &valueLength);
    if (valueLength != 0) {
      value = new RESCUEDOUBLE[(size_t)valueLength];
      myfscanf(ParentModel()->Context(), archiveFile, value, valueLength, FALSE);
    }
  }
}

void RescueArrayDouble::Archive(FILE *archiveFile) {
  RescueArray::Archive(archiveFile);
  myfprintf(ParentModel()->Context(), archiveFile, nullValue);
  if (ParentModel()->Context()->FileVersion() == 9) {
    RESCUEBOOL loadedNow = IsLoaded();
    if (loadedNow == FALSE) {
      ReadData(parentModel->oldPathName);
    }
    if (value == 0) {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEINT64)0);
    } else {
      valueLength = grid->NodeCount(cellCentered);

      myfprintf(ParentModel()->Context(), archiveFile, valueLength);
      myfprintf(ParentModel()->Context(), archiveFile, value, valueLength, FALSE);
    }
    if (loadedNow == FALSE) {
      DropMemory();
    }
  }
}

void RescueArrayDouble::CalculateMinMaxData(FILE *fragmentFile, RESCUEINT64 fileVersion) {
  if (value == 0) {
    minValue = 0;
    maxValue = 0;
  } else {
    RESCUEUINT64 items = (RESCUEUINT64)grid->NodeCount(cellCentered);
    RESCUEDOUBLE *ptr = value;
    bool firstIteration = true;
    while (items > 0) {
      if (*ptr != nullValue) {
        if (firstIteration) {
          minValue = *ptr;
          maxValue = *ptr;
          firstIteration = false;
        } else {
          if (*ptr < minValue) {
            minValue = *ptr;
          }
          if (*ptr > maxValue) {
            maxValue = *ptr;
          }
        }
      }
      ptr++;
      items--;
    }
  }
}

void RescueArrayDouble::ReadMinMax(RescueContext *context, FILE *archiveFile) {
  RescueBuffer buf(context, archiveFile);
  buf >> minValue;
  buf >> maxValue;
}

void RescueArrayDouble::WriteMinMax(RescueContext *context, FILE *archiveFile) {
  RescueBuffer buf(context, 50);
  buf << minValue;
  buf << maxValue;
  buf.Archive(archiveFile);
}

void RescueArrayDouble::InitMinMax(void) {
  minValue = 0;
  maxValue = 0;
}

void RescueArrayDouble::ArchiveData(FILE *archiveFile, FILE *fragmentFile, RESCUEINT64 fileVersion) {
  if (value == 0) {
    myfprintf(ParentModel()->Context(), archiveFile, (RESCUEINT64)0);
  } else {
    valueLength = grid->NodeCount(cellCentered);

    myfprintf(ParentModel()->Context(), archiveFile, valueLength);
    myfprintf(ParentModel()->Context(), archiveFile, value, valueLength, TRUE);
  }
}

void RescueArrayDouble::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion) {
  myfscanf(ParentModel()->Context(), archiveFile, &valueLength);
  if (valueLength != 0) {
    value = new RESCUEDOUBLE[(size_t)valueLength];
    myfscanf(ParentModel()->Context(), archiveFile, value, valueLength, fileVersion > 1);
  }
}

void RescueArrayDouble::SetValue(RESCUEDOUBLE nullValueIn, RESCUEDOUBLE *valueArray) {
  valueLength = grid->NodeCount(cellCentered);

  if (value != 0) {
    delete[] value;
  }
  nullValue = nullValueIn;
  value = new RESCUEDOUBLE[(size_t)valueLength];

  RESCUEINT64 loop;
  for (loop = 0; loop < valueLength; loop++) {
    value[loop] = valueArray[loop];
  }
}

void RescueArrayDouble::AcceptValue(RESCUEDOUBLE nullValueIn, RESCUEDOUBLE *valueArray) {
  if (value != 0) {
    delete[] value;
  }
  nullValue = nullValueIn;
  value = valueArray;
  valueLength = grid->NodeCount(cellCentered);
}

void RescueArrayDouble::LoadAndSwapArray() {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  RescueContext::SwapArray(value, valueLength);
  MarkChanged();
  Unload();
}

void RescueArrayDouble::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes) {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  RescueContext::SwapAxes(value, swapI, iNodes, swapJ, jNodes);
  MarkChanged();
  Unload();
}

void RescueArrayDouble::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes, bool swapK,
                                 RESCUEINT64 kNodes) {
  if (IsLoaded() == FALSE) {
    RescueArray::Load();
  }
  RescueContext::SwapAxes(value, swapI, iNodes, swapJ, jNodes, swapK, kNodes);
  MarkChanged();
  Unload();
}

RESCUEBOOL RescueArrayDouble::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueArrayDouble) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}

RescueArrayFragment *RescueArrayDouble::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount, RESCUEINT32 jLowBound,
                                               RESCUEINT32 jCount) {
  RescueArrayFragment *myReturn =
      CreatePrimitive((RESCUEINT64)iLowBound, (RESCUEINT64)iCount, (RESCUEINT64)jLowBound, (RESCUEINT64)jCount);
  ((RescueArrayFragmentDouble *)myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount, RESCUEINT64 jLowBound,
                                               RESCUEINT64 jCount) {
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  ((RescueArrayFragmentDouble *)myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                                        RESCUEINT64 jLowBound, RESCUEINT64 jCount) {
  RescueArrayFragment *myReturn = 0;
  if (grid->Dimensions() == 3) {
    RescueGridAxis *kAxis = grid->Axis(2);
    myReturn = new RescueArrayFragmentDouble(this, 3, iLowBound, iCount, jLowBound, jCount, kAxis->LowBound64(),
                                             kAxis->Count64());
  } else {
    myReturn = new RescueArrayFragmentDouble(this, 2, iLowBound, iCount, jLowBound, jCount, -1, -1);
  }
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount, RESCUEINT32 jLowBound,
                                               RESCUEINT32 jCount, RESCUEINT32 kLowBound, RESCUEINT32 kCount) {
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64)iLowBound, (RESCUEINT64)iCount, (RESCUEINT64)jLowBound,
                                                  (RESCUEINT64)jCount, (RESCUEINT64)kLowBound, (RESCUEINT64)kCount);
  ((RescueArrayFragmentDouble *)myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount, RESCUEINT64 jLowBound,
                                               RESCUEINT64 jCount, RESCUEINT64 kLowBound, RESCUEINT64 kCount) {
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  ((RescueArrayFragmentDouble *)myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                                        RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                                        RESCUEINT64 kLowBound, RESCUEINT64 kCount) {
  RescueArrayFragment *myReturn =
      new RescueArrayFragmentDouble(this, 3, iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount, RESCUEINT32 jLowBound,
                                             RESCUEINT32 jCount, RESCUEINT32 kLowBound, RESCUEINT32 kCount) {
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64)iLowBound, (RESCUEINT64)iCount, (RESCUEINT64)jLowBound,
                                                  (RESCUEINT64)jCount, (RESCUEINT64)kLowBound, (RESCUEINT64)kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount, RESCUEINT32 jLowBound,
                                             RESCUEINT32 jCount) {
  RescueArrayFragment *myReturn =
      CreatePrimitive((RESCUEINT64)iLowBound, (RESCUEINT64)iCount, (RESCUEINT64)jLowBound, (RESCUEINT64)jCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount, RESCUEINT64 jLowBound,
                                             RESCUEINT64 jCount, RESCUEINT64 kLowBound, RESCUEINT64 kCount) {
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayDouble::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount, RESCUEINT64 jLowBound,
                                             RESCUEINT64 jCount) {
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  LoadFragment(myReturn);
  return myReturn;
}
