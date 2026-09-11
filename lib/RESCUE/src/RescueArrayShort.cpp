/********************************************************************

  RescueArrayShort.cpp

  A dynamically sized array handler for N dimensional grids of
  floating points for RESCUE's data model. The related grid gives
  the dimensionality.

  Rod Hanks,  May 1996

*********************************************************************/
#include <string.h>
#include "RescueArrayShort.h"
#include "RescueModel.h"
#include "RescueLookup.h"
#include "RescueArrayFragmentShort.h"
#include "cSetRescueArrayFragment.h"
#include "RescueBuffer.h"

RescueArrayShort::~RescueArrayShort()
{
  if (value != 0)
  {
    delete [] value;
  }
}

RESCUEUSHORT *RescueArrayShort::DemandValue()
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  return value;
}

void RescueArrayShort::AllValues(RESCUEUSHORT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  if (value != 0)
  {
    RESCUEUSHORT *pos = &buffer[offset];
    RESCUEUSHORT *src = value;
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

void RescueArrayShort::Relink(RescueObject *parent)
{
  lookupTable = ((RescueModel *) parent)->LookupIdentifiedBy(lookupId);
  RescueArray::Relink(parent);
}

RescueArrayShort::RescueArrayShort(RescueContext *context, FILE *archiveFile)
                            :RescueArray(context, archiveFile)
                            ,value(0)
{
  isA = R_RescueArrayShort;
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
  myfscanf(context, archiveFile, &lookupId);
  if (context->ReadFileVersion() < 12)
  {
    myfscanf(context, archiveFile, &valueLength);
    if (valueLength != 0)
    {
      value = new RESCUEUSHORT [(size_t) valueLength];
      myfscanf(context, archiveFile, value, valueLength);
    }
  }
}

void RescueArrayShort::Archive(FILE *archiveFile)
{
  RescueArray::Archive(archiveFile);
  myfprintf(ParentModel()->Context(), archiveFile, nullValue);
  myfprintf(ParentModel()->Context(), archiveFile, lookupTable->Identifier());
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
      myfprintf(ParentModel()->Context(), archiveFile, value, valueLength);
    }
    if (loadedNow == FALSE)
    {
      DropMemory();
    }
  }
}

void RescueArrayShort::CalculateMinMaxData(FILE *fragmentFile, RESCUEINT64 fileVersion)
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
          RESCUEUSHORT data = nullValue;
          if (fragmentFile != 0)
          {
            myfscanf(ParentModel()->Context(), fragmentFile, &data);
          }

          RESCUEINT64 frag;
          for (frag = 0; frag < fragCount; frag++)
          {
            data = ((RescueArrayFragmentShort *) 
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
      }
    }
  }
  else
  {
    RESCUEUINT64 items = (RESCUEUINT64) grid->NodeCount(cellCentered);
    RESCUEUSHORT *ptr = value;
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

void RescueArrayShort::ReadMinMax(RescueContext *context, FILE *archiveFile)
{
  RescueBuffer buf(context, archiveFile);
  buf >> minValue;
  buf >> maxValue;
}

void RescueArrayShort::WriteMinMax(RescueContext *context, FILE *archiveFile)
{
  RescueBuffer buf(context, 50);
  buf << minValue;
  buf << maxValue;
  buf.Archive(archiveFile);
}

void RescueArrayShort::InitMinMax(void)
{
  minValue = 0;
  maxValue = 0;
}

void RescueArrayShort::ArchiveData(FILE *archiveFile, FILE *fragmentFile, RESCUEINT64 fileVersion)
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
          RESCUEUSHORT data = nullValue;
          if (fragmentFile != 0)
          {
            myfscanf(ParentModel()->Context(), fragmentFile, &data);
          }

          RESCUEINT64 frag;
          for (frag = 0; frag < fragCount; frag++)
          {
            data = ((RescueArrayFragmentShort *) 
              fragments->NthObject(frag))->Replace(origILowBound + iFollower, 
                                                   origJLowBound + jFollower, 
                                                   origKLowBound + kFollower, data);
          }
          myfprintf(ParentModel()->Context(), archiveFile, data);
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
    myfprintf(ParentModel()->Context(), archiveFile, value, count);
  }
}

void RescueArrayShort::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  myfscanf(ParentModel()->Context(), archiveFile, &valueLength);
  if (valueLength != 0)
  {
    value = new RESCUEUSHORT [(size_t) valueLength];
    myfscanf(ParentModel()->Context(), archiveFile, value, valueLength);
  }
}

void RescueArrayShort::SetValue(RESCUEUSHORT nullValueIn, RESCUEUSHORT *valueArray)
{
  DropFragments();
  valueLength = grid->NodeCount(cellCentered);

  if (value != 0)
  {
    delete [] value;
  }
  nullValue = nullValueIn;
  value = new RESCUEUSHORT [(size_t) valueLength];

  RESCUEINT64 loop;
  for (loop = 0; loop < valueLength; loop++)
  {
    value[loop] = valueArray[loop];
  }
}

void RescueArrayShort::AcceptValue(RESCUEUSHORT nullValueIn, RESCUEUSHORT *valueArray)
{
  DropFragments();
  if (value != 0)
  {
    delete [] value;
  }
  nullValue = nullValueIn;
  value = valueArray;
}

RescueArrayFragment *RescueArrayShort::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                              RESCUEINT32 jLowBound, RESCUEINT32 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount);
  ((RescueArrayFragmentShort *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  ((RescueArrayFragmentShort *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = 0;
  if (grid->Dimensions() == 3)
  {
    RescueGridAxis *kAxis = grid->Axis(2);
    myReturn = new RescueArrayFragmentShort(this, 3, iLowBound, iCount, 
                                                     jLowBound, jCount,
                                                     kAxis->LowBound64(), kAxis->Count64());
  }
  else
  {
    myReturn = new RescueArrayFragmentShort(this, 2, iLowBound, iCount,
                                                     jLowBound, jCount, -1, -1);
  }
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::Create(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                              RESCUEINT32 jLowBound, RESCUEINT32 jCount,
                                              RESCUEINT32 kLowBound, RESCUEINT32 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount, 
                                                  (RESCUEINT64) kLowBound, (RESCUEINT64) kCount);
  ((RescueArrayFragmentShort *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::Create(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                              RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  ((RescueArrayFragmentShort *) myReturn)->InitializeArray();
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::CreatePrimitive(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                              RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                              RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = new RescueArrayFragmentShort(this, 3, iLowBound, iCount, 
                                                                        jLowBound, jCount,
                                                                        kLowBound, kCount);
  DemandFragments();
  (*fragments) += myReturn;
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                            RESCUEINT32 jLowBound, RESCUEINT32 jCount,
                                            RESCUEINT32 kLowBound, RESCUEINT32 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount, 
                                                  (RESCUEINT64) kLowBound, (RESCUEINT64) kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::Load(RESCUEINT32 iLowBound, RESCUEINT32 iCount,
                                            RESCUEINT32 jLowBound, RESCUEINT32 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive((RESCUEINT64) iLowBound, (RESCUEINT64) iCount, 
                                                  (RESCUEINT64) jLowBound, (RESCUEINT64) jCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                            RESCUEINT64 jLowBound, RESCUEINT64 jCount,
                                            RESCUEINT64 kLowBound, RESCUEINT64 kCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount, kLowBound, kCount);
  LoadFragment(myReturn);
  return myReturn;
}

RescueArrayFragment *RescueArrayShort::Load(RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                            RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  RescueArrayFragment *myReturn = CreatePrimitive(iLowBound, iCount, jLowBound, jCount);
  LoadFragment(myReturn);
  return myReturn;
}
  
void RescueArrayShort::LoadAndSwapArray()
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  RescueContext::SwapArray(value, valueLength);
  MarkChanged();
  Unload();
}
  
void RescueArrayShort::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes)
{
  if (IsLoaded() == FALSE)
  {
    RescueArray::Load();
  }
  RescueContext::SwapAxes(value, swapI, iNodes, swapJ, jNodes);
  MarkChanged();
  Unload();
}
  
void RescueArrayShort::SwapAxes(bool swapI, RESCUEINT64 iNodes, 
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
  
RESCUEBOOL RescueArrayShort::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueArrayShort)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}


