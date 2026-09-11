/********************************************************************

  RescueArrayFloat.cpp

  A dynamically sized array handler for N dimensional grids of
  floating points for RESCUE's data model. The related grid gives
  the dimensionality.

  Rod Hanks,  May 1996

*********************************************************************/
#include <cmath>
#include <string.h>
#include "RescueArrayFloat.h"
#include "RescueModel.h"
#include "RescueArrayFragmentFloat.h"
#include "cSetRescueArrayFragment.h"
#include "RescueBuffer.h"

RescueArrayFloat::~RescueArrayFloat()
{
  if (value != 0)
  {
    delete [] value;
  }
}

RESCUEFLOAT *RescueArrayFloat::DemandValue()
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  return value;
}

void RescueArrayFloat::AllValues(RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  if (value != 0)
  {
    RESCUEFLOAT *pos = &buffer[offset];
    RESCUEFLOAT *src = value;
    while (bufferLength > 0)
    {
      *pos++ = *src++;
      bufferLength--;
    }
  }
/*
  If value == 0, valueLength should be zero too.
*/
}

RescueArrayFloat::RescueArrayFloat(RescueContext *context, FILE *archiveFile)
                            :RescueArray(context, archiveFile)
                            ,value(0),valueLength(0)
{
  isA = R_RescueArrayFloat;
  InitMinMax();
  if (context->ReadFileVersion() >= 37)
  {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0)
    {
      if (strcmp(myString, "dirtyMinMax") == 0)
      {
        RescueBuffer buf(context, archiveFile);
        buf >> dirtyMinMax;
      }
      else if (strcmp(myString, "minMax") == 0)
      {
        ReadMinMax(context, archiveFile);
      }
      else
      {
        RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
    }
  }
  myfscanf(context, archiveFile, &nullValue);
  if (context->ReadFileVersion() < 12)
  {
    myfscanf(context, archiveFile, &valueLength);
    if (valueLength != 0)
    {
      value = new RESCUEFLOAT [(size_t) valueLength];
      myfscanf(context, archiveFile, value, valueLength, FALSE);
    }
  }
}

void RescueArrayFloat::Archive(FILE *archiveFile)
{
  RescueArray::Archive(archiveFile);
  myfprintf(ParentModel()->Context(), archiveFile, nullValue);
  if (ParentModel()->Context()->FileVersion() == 9)
  {
    RESCUEBOOL loadedNow = IsLoaded();
    if (loadedNow == FALSE)
    {
      ReadData(parentModel->oldPathName);
    }
    if (value == 0)
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      RESCUEINT64 items = grid->NodeCount(cellCentered);

      myfprintf(ParentModel()->Context(), archiveFile, items);
      myfprintf(ParentModel()->Context(), archiveFile, value, items, FALSE);
    }
    if (loadedNow == FALSE)
    {
      DropMemory();
    }
  }
}

void RescueArrayFloat::CalculateMinMaxData(FILE *fragmentFile, RESCUEINT64 fileVersion)
{
  if (value == 0)
  {
    minValue = 0;
    maxValue = 0;
    if (HasFragments())
    {
      RESCUEINT64 count = 0;
      if (fragmentFile != 0)
      {
        myfscanf(ParentModel()->Context(), fragmentFile, &count);
      }
      if (count == 0)
      {
        count = grid->NodeCount(cellCentered);
      }
      if (count != 0)
      {
        RESCUEINT64 axisDecrement = 0;
        if (CellCentered())
        {
          axisDecrement = 1;
        }
        RESCUEINT64 fragCount = fragments->Count64();
        RescueArrayFragmentFloat *driverFrag = (RescueArrayFragmentFloat *) fragments->NthObject(0);
        driverFrag->PrepareRead();

        RescueGridAxis *axis = grid->Axis(0);
        RESCUEINT64 origILowBound = axis->LowBound64();
        RESCUEINT64 origICount = axis->Count64() - axisDecrement;

        axis = grid->Axis(1);
        RESCUEINT64 origJLowBound = axis->LowBound64();
        RESCUEINT64 origJCount = axis->Count64() - axisDecrement;
        RESCUEINT64 origKLowBound = -1;
        if (grid->Dimensions() >= 3)
        {
          axis = grid->Axis(2);
          origKLowBound = axis->LowBound64();
        }
        RESCUEINT64 iFollower = 0;
        RESCUEINT64 jFollower = 0;
        RESCUEINT64 kFollower = 0;

        RESCUEINT64 loop;
        for (loop = 0; loop < count; loop++)
        {
          RESCUEFLOAT data = (fragmentFile == 0) ? nullValue 
                              : driverFrag->ReadFloat(fragmentFile, fileVersion > 1);

          RESCUEINT64 frag;
          for (frag = 0; frag < fragCount; frag++)
          {
            data = ((RescueArrayFragmentFloat *) 
              fragments->NthObject(frag))->Replace(origILowBound + iFollower, 
                                                   origJLowBound + jFollower, 
                                                   origKLowBound + kFollower, data);
          }

          if (loop == 0)
          {
            minValue = data;
            maxValue = data;
          }
          else
          {
            if (data < minValue)
            {
              minValue = data;
            }
            if (data > maxValue)
            {
              maxValue = data;
            }
          }

          iFollower++;
          if (iFollower >= origICount)
          {
            iFollower = 0;
            jFollower++;
            if (jFollower >= origJCount)
            {
              jFollower = 0;
              kFollower++;
            }
          }
        }
        driverFrag->EndRead();
      }
    }
  }
  else
  {
    RESCUEUINT64 items = (RESCUEUINT64) grid->NodeCount(cellCentered);
    RESCUEFLOAT *ptr = value;
    minValue = (RESCUEFLOAT) HUGE_VAL;
    maxValue = (RESCUEFLOAT) -HUGE_VAL;
    while (items > 0)
    {
      if (*ptr != nullValue) {
 if (*ptr < minValue)
   {
     minValue = *ptr;
   }
 if (*ptr > maxValue)
   {
     maxValue = *ptr;
   }
      }
      ptr++;
      items--;
    }
  }
}

void RescueArrayFloat::ReadMinMax(RescueContext *context, FILE *archiveFile)
{
  RescueBuffer buf(context, archiveFile);
  buf >> minValue;
  buf >> maxValue;
}

void RescueArrayFloat::WriteMinMax(RescueContext *context, FILE *archiveFile)
{
  RescueBuffer buf(context, 50);
  buf << minValue;
  buf << maxValue;
  buf.Archive(archiveFile);
}

void RescueArrayFloat::InitMinMax(void)
{
  minValue = 0;
  maxValue = 0;
}

void RescueArrayFloat::ArchiveData(FILE *archiveFile, FILE *fragmentFile, RESCUEINT64 fileVersion)
{
  if (value == 0)
  {
    if (HasFragments())
    {
      RESCUEINT64 count = 0;
      if (fragmentFile != 0)
      {
        myfscanf(ParentModel()->Context(), fragmentFile, &count);
      }
      if (count == 0)
      {
        count = grid->NodeCount(cellCentered);
      }
      myfprintf(ParentModel()->Context(), archiveFile, count);
      if (count != 0)
      {
        RESCUEINT64 axisDecrement = 0;
        if (CellCentered())
        {
          axisDecrement = 1;
        }
        RESCUEINT64 fragCount = fragments->Count64();
        RescueArrayFragmentFloat *driverFrag = (RescueArrayFragmentFloat *) fragments->NthObject(0);
        driverFrag->PrepareRead();

        RescueGridAxis *axis = grid->Axis(0);
        RESCUEINT64 origILowBound = axis->LowBound64();
        RESCUEINT64 origICount = axis->Count64() - axisDecrement;

        axis = grid->Axis(1);
        RESCUEINT64 origJLowBound = axis->LowBound64();
        RESCUEINT64 origJCount = axis->Count64() - axisDecrement;
        RESCUEINT64 origKLowBound = -1;
        if (grid->Dimensions() >= 3)
        {
          axis = grid->Axis(2);
          origKLowBound = axis->LowBound64();
        }
        RESCUEINT64 iFollower = 0;
        RESCUEINT64 jFollower = 0;
        RESCUEINT64 kFollower = 0;

        PrepareWrite();

        RESCUEINT64 loop;
        for (loop = 0; loop < count; loop++)
        {
          RESCUEFLOAT data = (fragmentFile == 0) ? nullValue 
                              : driverFrag->ReadFloat(fragmentFile, fileVersion > 1);

          RESCUEINT64 frag;
          for (frag = 0; frag < fragCount; frag++)
          {
            data = ((RescueArrayFragmentFloat *) 
              fragments->NthObject(frag))->Replace(origILowBound + iFollower, 
                                                   origJLowBound + jFollower, 
                                                   origKLowBound + kFollower, data);
          }
          WriteFloat(archiveFile, fileVersion > 1, data);
          iFollower++;
          if (iFollower >= origICount)
          {
            iFollower = 0;
            jFollower++;
            if (jFollower >= origJCount)
            {
              jFollower = 0;
              kFollower++;
            }
          }
        }
        driverFrag->EndRead();
        EndWrite(archiveFile);
      }
    }
    else
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEINT64) 0);
    }
  }
  else
  {
    RESCUEINT64 items = grid->NodeCount(cellCentered);

    myfprintf(ParentModel()->Context(), archiveFile, items);
    myfprintf(ParentModel()->Context(), archiveFile, value, items, TRUE);
  }
}

void RescueArrayFloat::PrepareWrite()
{
  writeBehindBuffer = 0;
  writeBehindPos = 0;
  writeBehindAllocated = 0;
  writeBehindCount = 0;
  writeBehindFloat = (RESCUEFLOAT) 0;
  writeBehindState = NO_STATE;
}

void RescueArrayFloat::EndWrite(FILE *archiveFile)
{
  FlushWrite(archiveFile);
  if (writeBehindBuffer != 0)
  {
    free(writeBehindBuffer);
  }
}

void RescueArrayFloat::FlushWrite(FILE *archiveFile)
{
  if (writeBehindState == BUFFERING_SAME)
  {
    if (writeBehindCount <= 250)
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEUCHAR) writeBehindCount);
      myfprintf(ParentModel()->Context(), archiveFile, writeBehindFloat);
    }
    else
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEUCHAR) 255);
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEINT64) writeBehindCount);
      myfprintf(ParentModel()->Context(), archiveFile, writeBehindFloat);
    }
  }
  else if (writeBehindState == BUFFERING_DIFFERENT)
  {
    if (writeBehindPos == 1)
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEUCHAR) writeBehindPos);
    }
    else if (writeBehindPos <= 250)
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEUCHAR) 253);
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEUCHAR) writeBehindPos);
    }
    else
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEUCHAR) 254);
      myfprintf(ParentModel()->Context(), archiveFile, writeBehindPos);
    }
    RESCUEUINT64 loop;
    for (loop = 0; loop < writeBehindPos; loop++)
    {
      myfprintf(ParentModel()->Context(), archiveFile, writeBehindBuffer[loop]);
    }
  }
}

void RescueArrayFloat::AllocateWriteBehind()
{
  if (writeBehindPos + 1 >= writeBehindAllocated)
  {
    writeBehindAllocated += 250;
    if (writeBehindBuffer == 0)
    {
      writeBehindBuffer = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT) * (size_t) writeBehindAllocated);
    }
    else
    {
      writeBehindBuffer = (RESCUEFLOAT *) realloc(writeBehindBuffer, sizeof(RESCUEFLOAT) * (size_t) writeBehindAllocated);
    }
  }
}

void RescueArrayFloat::WriteFloat(FILE *archiveFile, RESCUEBOOL compress, RESCUEFLOAT data)
{
  if (compress == FALSE)
  {
    myfprintf(ParentModel()->Context(), archiveFile, data);
  }
  else
  {
    if (writeBehindState == NO_STATE)
    {
      writeBehindFloat = data;
      writeBehindCount = 1;
      writeBehindState = BUFFERING_SAME;
    }
    else if (writeBehindState == BUFFERING_SAME)
    {
      if (writeBehindFloat == data)
      {
        writeBehindCount++;
      }
      else if (writeBehindCount == 1)
      {
        writeBehindPos = 0;
        AllocateWriteBehind();
        writeBehindState = BUFFERING_DIFFERENT;
        writeBehindBuffer[writeBehindPos++] = writeBehindFloat;
        writeBehindBuffer[writeBehindPos++] = data;
      }
      else
      {
        FlushWrite(archiveFile);
        writeBehindFloat = data;
        writeBehindCount = 1;
        writeBehindState = BUFFERING_SAME;
      }
    }
    else if (writeBehindState == BUFFERING_DIFFERENT)
    {
      if (writeBehindBuffer[writeBehindPos - 1] == data)
      {
        writeBehindPos--;
        FlushWrite(archiveFile);
        writeBehindFloat = data;
        writeBehindCount = 2;
        writeBehindState = BUFFERING_SAME;
      }
      else
      {
        AllocateWriteBehind();
        writeBehindBuffer[writeBehindPos++] = data;
      }
    }
  }
}

void RescueArrayFloat::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  myfscanf(ParentModel()->Context(), archiveFile, &valueLength);
  if (valueLength != 0)
  {
    value = new RESCUEFLOAT [(size_t) valueLength];
    myfscanf(ParentModel()->Context(), archiveFile, value, valueLength, fileVersion > 1);
  }
}

void RescueArrayFloat::SetValue(RESCUEFLOAT nullValueIn, RESCUEFLOAT *valueArray)
{
  DropFragments();
  valueLength = grid->NodeCount(cellCentered);

  if (value != 0)
  {
    delete [] value;
  }
  nullValue = nullValueIn;
  value = new RESCUEFLOAT [(size_t) valueLength];

  RESCUEINT64 loop;
  for (loop = 0; loop < valueLength; loop++)
  {
    value[loop] = valueArray[loop];
  }
}

void RescueArrayFloat::AcceptValue(RESCUEFLOAT nullValueIn, RESCUEFLOAT *valueArray)
{
  DropFragments();
  if (value != 0)
  {
    delete [] value;
  }
  nullValue = nullValueIn;
  value = valueArray;
  valueLength = grid->NodeCount(cellCentered);
}

RescueArrayFragment *RescueArrayFloat::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                              RESCUEINT32 jLowBound, RESCUEINT32 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount);
  ((RescueArrayFragmentFloat *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  ((RescueArrayFragmentFloat *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = 0;
  if (grid->Dimensions() == 3)
  {
    RescueGridAxis *kAxis = grid->Axis(2);
    myReturn = new RescueArrayFragmentFloat(this, 3, iLowBound, iCount, 
                                                     jLowBound, jCount,
                                                     kAxis->LowBound64(), kAxis->Count64());
  }
  else
  {
    myReturn = new RescueArrayFragmentFloat(this, 2, iLowBound, iCount,
                                                     jLowBound, jCount, -1, -1);
  }
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                              RESCUEINT32 jLowBound, RESCUEINT32 jCount,
                                              RESCUEINT32 kLowBound, RESCUEINT32 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount, 
                                                  (RESCUEINT64) kLowBound, (RESCUEINT64) kCount);
  ((RescueArrayFragmentFloat *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                              RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  ((RescueArrayFragmentFloat *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                              RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = new RescueArrayFragmentFloat(this, 3, iLowBound, iCount, 
                                                                        jLowBound, jCount,
                                                                        kLowBound, kCount);
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                            RESCUEINT32 jLowBound, RESCUEINT32 jCount,
                                            RESCUEINT32 kLowBound, RESCUEINT32 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount, 
                                                  (RESCUEINT64) kLowBound, (RESCUEINT64) kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                            RESCUEINT32 jLowBound, RESCUEINT32 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                            RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                            RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayFloat::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                            RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  LoadFragment(myReturn);
  return myReturn;
}

void RescueArrayFloat::LoadAndSwapArray()
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  RescueContext::SwapArray(value, valueLength);
  MarkChanged();
  Unload();
}

void RescueArrayFloat::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes)
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  RescueContext::SwapAxes(value, swapI, iNodes, swapJ, jNodes);
  MarkChanged();
  Unload();
}
  
void RescueArrayFloat::SwapAxes(bool swapI, RESCUEINT64 iNodes, 
                                bool swapJ, RESCUEINT64 jNodes,
                                bool swapK, RESCUEINT64 kNodes)
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  RescueContext::SwapAxes(value, swapI, iNodes, swapJ, jNodes, swapK, kNodes);
  MarkChanged();
  Unload();
}
  
RESCUEBOOL RescueArrayFloat::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueArrayFloat)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}


