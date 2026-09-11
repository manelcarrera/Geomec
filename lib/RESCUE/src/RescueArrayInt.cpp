/********************************************************************

  RescueArrayInt.cpp

  A dynamically sized array handler for N dimensional grids of
  inting points for RESCUE's data model. The related grid gives
  the dimensionality.

  Rod Hanks,  May 1996

*********************************************************************/
#include <string.h>
#include "RescueArrayInt.h"
#include "RescueModel.h"
#include "RescueArrayFragmentInt.h"
#include "cSetRescueArrayFragment.h"
#include "RescueBuffer.h"

RescueArrayInt::~RescueArrayInt()
{
  if (value != 0)
  {
    delete [] value;
  }
}

RESCUEINT32 *RescueArrayInt::DemandValue()
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  return value;
}

void RescueArrayInt::AllValues(RESCUEINT32 *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  if (value != 0)
  {
    RESCUEINT32 *pos = &buffer[offset];
    RESCUEINT32 *src = value;
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

RescueArrayInt::RescueArrayInt(RescueContext *context, FILE *archiveFile)
                            :RescueArray(context, archiveFile)
                            ,value(0)
{
  isA = R_RescueArrayInt;
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
      value = new RESCUEINT32 [(size_t) valueLength];
      myfscanf(context, archiveFile, value, valueLength, FALSE);
    }
  }
}

void RescueArrayInt::Archive(FILE *archiveFile)
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
      valueLength = grid->NodeCount(cellCentered);

      myfprintf(ParentModel()->Context(), archiveFile, valueLength);
      myfprintf(ParentModel()->Context(), archiveFile, value, valueLength, FALSE);
    }
    if (loadedNow == FALSE)
    {
      DropMemory();
    }
  }
}

void RescueArrayInt::CalculateMinMaxData(FILE *fragmentFile, RESCUEINT64 fileVersion)
{
  if (value == 0)
  {
    bool firstIteration = true;
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
        RescueArrayFragmentInt *driverFrag = (RescueArrayFragmentInt *) fragments->NthObject(0);
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
          RESCUEINT32 data = (fragmentFile == 0) ? nullValue 
                              : driverFrag->ReadInt(fragmentFile, fileVersion > 1);

          RESCUEINT64 frag;
          for (frag = 0; frag < fragCount; frag++)
          {
            data = ((RescueArrayFragmentInt *) 
              fragments->NthObject(frag))->Replace(origILowBound + iFollower, 
                                                   origJLowBound + jFollower, 
                                                   origKLowBound + kFollower, data);
          }

          if (minValue != nullValue)
          {
            if (firstIteration)
            {
              minValue = data;
              maxValue = data;
              firstIteration = false;
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
    RESCUEINT32 *ptr = value;
    bool firstIteration = true;
    while (items > 0)
    {
      if (*ptr != nullValue)
      {
        if (firstIteration)
        {
          minValue = *ptr;
          maxValue = *ptr;
          firstIteration = false;
        }
        else
        {
          if (*ptr < minValue)
          {
            minValue = *ptr;
          }
          if (*ptr > maxValue)
          {
            maxValue = *ptr;
          }
        }
      }
      ptr++;
      items--;
    }
  }
}

void RescueArrayInt::ReadMinMax(RescueContext *context, FILE *archiveFile)
{
  RescueBuffer buf(context, archiveFile);
  buf >> minValue;
  buf >> maxValue;
}

void RescueArrayInt::WriteMinMax(RescueContext *context, FILE *archiveFile)
{
  RescueBuffer buf(context, 50);
  buf << minValue;
  buf << maxValue;
  buf.Archive(archiveFile);
}

void RescueArrayInt::InitMinMax(void)
{
  minValue = 0;
  maxValue = 0;
}

void RescueArrayInt::ArchiveData(FILE *archiveFile, FILE *fragmentFile, RESCUEINT64 fileVersion)
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
        RescueArrayFragmentInt *driverFrag = (RescueArrayFragmentInt *) fragments->NthObject(0);
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
          RESCUEINT32 data = (fragmentFile == 0) ? nullValue 
                              : driverFrag->ReadInt(fragmentFile, fileVersion > 1);

          RESCUEINT64 frag;
          for (frag = 0; frag < fragCount; frag++)
          {
            data = ((RescueArrayFragmentInt *) 
              fragments->NthObject(frag))->Replace(origILowBound + iFollower, 
                                                   origJLowBound + jFollower, 
                                                   origKLowBound + kFollower, data);
          }
          WriteInt(archiveFile, fileVersion > 1, data);
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
    RESCUEINT64 count = grid->NodeCount(cellCentered);

    myfprintf(ParentModel()->Context(), archiveFile, count);
    myfprintf(ParentModel()->Context(), archiveFile, value, count, TRUE);
  }
}

void RescueArrayInt::PrepareWrite()
{
  writeBehindBuffer = 0;
  writeBehindPos = 0;
  writeBehindAllocated = 0;
  writeBehindCount = 0;
  writeBehindInt = (RESCUEINT32) 0;
  writeBehindState = NO_STATE;
}

void RescueArrayInt::EndWrite(FILE *archiveFile)
{
  FlushWrite(archiveFile);
  if (writeBehindBuffer != 0)
  {
    free(writeBehindBuffer);
  }
}

void RescueArrayInt::FlushWrite(FILE *archiveFile)
{
  if (writeBehindState == BUFFERING_SAME)
  {
    if (writeBehindCount <= 250)
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEUCHAR) writeBehindCount);
      myfprintf(ParentModel()->Context(), archiveFile, writeBehindInt);
    }
    else
    {
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEUCHAR) 255);
      myfprintf(ParentModel()->Context(), archiveFile, (RESCUEINT64) writeBehindCount);
      myfprintf(ParentModel()->Context(), archiveFile, writeBehindInt);
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

void RescueArrayInt::AllocateWriteBehind()
{
  if (writeBehindPos + 1 >= writeBehindAllocated)
  {
    writeBehindAllocated += 250;
    if (writeBehindBuffer == 0)
    {
      writeBehindBuffer = (RESCUEINT32 *) malloc(sizeof(RESCUEINT32) * (size_t) writeBehindAllocated);
    }
    else
    {
      writeBehindBuffer = (RESCUEINT32 *) realloc(writeBehindBuffer, sizeof(RESCUEINT32) * (size_t) writeBehindAllocated);
    }
  }
}

void RescueArrayInt::WriteInt(FILE *archiveFile, RESCUEBOOL compress, RESCUEINT32 data)
{
  if (compress == FALSE)
  {
    myfprintf(ParentModel()->Context(), archiveFile, data);
  }
  else
  {
    if (writeBehindState == NO_STATE)
    {
      writeBehindInt = data;
      writeBehindCount = 1;
      writeBehindState = BUFFERING_SAME;
    }
    else if (writeBehindState == BUFFERING_SAME)
    {
      if (writeBehindInt == data)
      {
        writeBehindCount++;
      }
      else if (writeBehindCount == 1)
      {
        writeBehindPos = 0;
        AllocateWriteBehind();
        writeBehindState = BUFFERING_DIFFERENT;
        writeBehindBuffer[writeBehindPos++] = writeBehindInt;
        writeBehindBuffer[writeBehindPos++] = data;
      }
      else
      {
        FlushWrite(archiveFile);
        writeBehindInt = data;
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
        writeBehindInt = data;
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

void RescueArrayInt::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  myfscanf(ParentModel()->Context(), archiveFile, &valueLength);
  if (valueLength != 0)
  {
    value = new RESCUEINT32 [(size_t) valueLength];
    myfscanf(ParentModel()->Context(), archiveFile, value, valueLength, fileVersion > 1);
  }
}

void RescueArrayInt::SetValue(RESCUEINT32 nullValueIn, RESCUEINT32 *valueArray)
{
  DropFragments();
  RESCUEINT64 items = grid->NodeCount(cellCentered);

  if (value != 0)
  {
    delete [] value;
  }
  nullValue = nullValueIn;
  value = new RESCUEINT32 [(size_t) items];

  RESCUEINT64 loop;
  for (loop = 0; loop < items; loop++)
  {
    value[loop] = valueArray[loop];
  }
}

void RescueArrayInt::AcceptValue(RESCUEINT32 nullValueIn, RESCUEINT32 *valueArray)
{
  DropFragments();
  if (value != 0)
  {
    delete [] value;
  }
  nullValue = nullValueIn;
  value = valueArray;
}

RescueArrayFragment *RescueArrayInt::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                              RESCUEINT32 jLowBound, RESCUEINT32 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT32) iLowBound, (RESCUEINT32) iCount, 
                                                  (RESCUEINT32) jLowBound, (RESCUEINT32) jCount);
  ((RescueArrayFragmentInt *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayInt::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  ((RescueArrayFragmentInt *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayInt::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = 0;
  if (grid->Dimensions() == 3)
  {
    RescueGridAxis *kAxis = grid->Axis(2);
    myReturn = new RescueArrayFragmentInt(this, 3, iLowBound, iCount, 
                                                     jLowBound, jCount,
                                                     kAxis->LowBound64(), kAxis->Count64());
  }
  else
  {
    myReturn = new RescueArrayFragmentInt(this, 2, iLowBound, iCount,
                                                     jLowBound, jCount, -1, -1);
  }
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArrayInt::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                              RESCUEINT32 jLowBound, RESCUEINT32 jCount,
                                              RESCUEINT32 kLowBound, RESCUEINT32 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount, 
                                                  (RESCUEINT64) kLowBound, (RESCUEINT64) kCount);
  ((RescueArrayFragmentInt *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayInt::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                              RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  ((RescueArrayFragmentInt *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayInt::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                              RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = new RescueArrayFragmentInt(this, 3, iLowBound, iCount, 
                                                                        jLowBound, jCount,
                                                                        kLowBound, kCount);
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArrayInt::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                            RESCUEINT32 jLowBound, RESCUEINT32 jCount,
                                            RESCUEINT32 kLowBound, RESCUEINT32 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount, 
                                                  (RESCUEINT64) kLowBound, (RESCUEINT64) kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayInt::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                            RESCUEINT32 jLowBound, RESCUEINT32 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount);
  LoadFragment(myReturn);
  return myReturn;
}
  
RescueArrayFragment *RescueArrayInt::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                            RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                            RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayInt::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                            RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  LoadFragment(myReturn);
  return myReturn;
}
  
void RescueArrayInt::LoadAndSwapArray()
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  RescueContext::SwapArray(value, valueLength);
  MarkChanged();
  Unload();
}
  
void RescueArrayInt::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes)
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  RescueContext::SwapAxes(value, swapI, iNodes, swapJ, jNodes);
  MarkChanged();
  Unload();
}
  
void RescueArrayInt::SwapAxes(bool swapI, RESCUEINT64 iNodes, 
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
  
RESCUEBOOL RescueArrayInt::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueArrayInt)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}


