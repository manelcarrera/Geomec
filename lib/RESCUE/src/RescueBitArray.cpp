/********************************************************************

  RescueBitArray.cpp

  A dynamically sized array handler for N dimensional grids of
  bit fields for RESCUE's data model.

  This class is not accessed directly by the
  application.  See RescueGeometry for an example of usage.

  For the moment, we are assuming that sizeof(RESCUEINT64) is an even
  multiple of bitFieldWidth.

  Rod Hanks,  December 1998

*********************************************************************/
#include "RescueBitArray.h"
#include "RescueBuffer.h"
#include "RescueContext.h"
#include "myHeaders.h"
#include <string.h>

RESCUEINT32 RescueBitArray::BitField(RESCUEINT64 zeroBasedNdx) {
  if (zeroBasedNdx >= 0 && zeroBasedNdx < bitFieldCount) {
    RESCUEINT64 ndx = zeroBasedNdx / fieldsPerInt;
    RESCUEINT64 whichInNdx = zeroBasedNdx % fieldsPerInt;
    unsigned RESCUEINT64 storage = data[ndx];
    storage = storage >> (unsigned RESCUEINT64)(whichInNdx * bitFieldWidth);
    return (RESCUEINT32)(storage & mask);
  } else {
    return 0;
  }
}

void RescueBitArray::SetBitField(RESCUEINT64 zeroBasedNdx, RESCUEINT32 value) {
  if (zeroBasedNdx >= 0 && zeroBasedNdx < bitFieldCount) {
    value = value & mask;
    RESCUEINT64 ndx = zeroBasedNdx / (RESCUEINT64)fieldsPerInt;
    RESCUEINT64 whichInNdx = zeroBasedNdx % (RESCUEINT64)fieldsPerInt;
    unsigned RESCUEINT32 storage = (unsigned RESCUEINT32) ~(mask << (unsigned RESCUEINT32)(whichInNdx * bitFieldWidth));
    data[ndx] = data[ndx] & storage;
    storage = (unsigned RESCUEINT32)(value << (unsigned RESCUEINT32)(whichInNdx * bitFieldWidth));
    data[ndx] = data[ndx] | storage;
  }
}

void RescueBitArray::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes) {
  if (swapI) {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes; jLoop++) {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx) {
        RESCUEINT32 temp = BitField(fNdx);
        SetBitField(fNdx, BitField(eNdx));
        SetBitField(eNdx, temp);
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
        RESCUEINT32 temp = BitField((ifNdx * iNodes) + iLoop);
        SetBitField((ifNdx * iNodes) + iLoop, BitField((efNdx * iNodes) + iLoop));
        SetBitField((efNdx * iNodes) + iLoop, temp);
      }
      ifNdx++;
      efNdx--;
    }
  }
}

void RescueBitArray::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes, bool swapK,
                              RESCUEINT64 kNodes) {
  if (swapI) {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes * kNodes; jLoop++) {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx) {
        RESCUEINT32 temp = BitField(fNdx);
        SetBitField(fNdx, BitField(eNdx));
        SetBitField(eNdx, temp);
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
          RESCUEINT32 temp = BitField(offset + (ifNdx * iNodes) + iLoop);
          SetBitField(offset + (ifNdx * iNodes) + iLoop, BitField(offset + (efNdx * iNodes) + iLoop));
          SetBitField(offset + (efNdx * iNodes) + iLoop, temp);
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
        RESCUEINT32 temp = BitField((kfNdx * iNodes * jNodes) + ijLoop);
        SetBitField((kfNdx * iNodes * jNodes) + ijLoop, BitField((keNdx * iNodes * jNodes) + ijLoop));
        SetBitField((keNdx * iNodes * jNodes) + ijLoop, temp);
      }
      kfNdx++;
      keNdx--;
    }
  }
}

RescueBitArray::RescueBitArray(RescueContext *contextIn, RESCUEINT64 totalElements, RESCUEINT32 bitFieldWidthIn)
    : RescueObject(contextIn) {
  CommonInitialization(contextIn, totalElements, bitFieldWidthIn);
}

void RescueBitArray::CommonInitialization(RescueContext *contextIn, RESCUEINT64 totalElements,
                                          RESCUEINT32 bitFieldWidthIn) {
  isA = R_RescueBitArray;
  context = contextIn;
  bitFieldCount = totalElements;
  bitFieldWidth = bitFieldWidthIn;
  fieldsPerInt = (sizeof(unsigned RESCUEINT32) * 8) / bitFieldWidth;
  intCount = (bitFieldCount / fieldsPerInt) + 1;
  data = new unsigned RESCUEINT32[(int)intCount];
  RESCUEINT64 loop;
  for (loop = 0; loop < intCount; loop++) {
    data[loop] = 0;
  }
  mask = 0;
  RESCUEINT32 maskBit = 1;
  RESCUEINT32 count = bitFieldWidth - 1;
  mask = mask | maskBit;
  while (count > 0) {
    maskBit = maskBit << 1;
    mask = mask | maskBit;
    count--;
  }
}

RescueBitArray::RescueBitArray(RescueContext *contextIn, FILE *archiveFile) : RescueObject(contextIn) {
  myfscanf(contextIn, archiveFile, &bitFieldCount);
  myfscanf(contextIn, archiveFile, &bitFieldWidth);
  CommonInitialization(contextIn, bitFieldCount, bitFieldWidth);
  myfscanf(contextIn, archiveFile, data, intCount);
  if (contextIn->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(contextIn, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(contextIn, archiveFile);
      myfgets(contextIn, myString, 255, archiveFile);
    }
  }
}

RescueBitArray::RescueBitArray(RescueContext *contextIn, RescueBuffer &buf) : RescueObject(contextIn) {
  buf >> bitFieldCount;
  buf >> bitFieldWidth;
  CommonInitialization(contextIn, bitFieldCount, bitFieldWidth);
  RESCUEINT64 storedCount;
  buf >> storedCount;
  RESCUEINT64 loop;
  for (loop = 0; loop < storedCount; loop++) {
    buf >> data[loop];
  }
}

void RescueBitArray::Archive(FILE *archiveFile) {
  myfprintf(context, archiveFile, "; RescueBitArray");
  myfprintf(context, archiveFile, bitFieldCount);
  myfprintf(context, archiveFile, bitFieldWidth);
  myfprintf(context, archiveFile, data, intCount);
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

void RescueBitArray::Archive(RescueBuffer &buf) {
  buf << bitFieldCount;
  buf << bitFieldWidth;
  buf << intCount;
  RESCUEINT64 loop;
  for (loop = 0; loop < intCount; loop++) {
    buf << data[loop];
  }
  buf << "EOD";
}

RESCUEBOOL RescueBitArray::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueBitArray) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}
