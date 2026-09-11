/********************************************************************

  RescueArrayFragmentFloat.cpp

  A dynamically sized array handler for part of a 2-3 dimensional grid of
  floating points for RESCUE's data model. The related grid gives
  the dimensionality.  We keep lowbound and count for each grid.

  Rod Hanks,  December 2000

*********************************************************************/
#include "RescueArrayFragmentFloat.h"
#include "RescueModel.h"
#include "RescueArrayFloat.h"

RescueArrayFragmentFloat::~RescueArrayFragmentFloat()
{
  if (value != 0)
  {
    delete [] value;
  }
}

void RescueArrayFragmentFloat::AllValues(RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
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

RESCUEFLOAT RescueArrayFragmentFloat::Replace(RESCUEINT64 iNdx, RESCUEINT64 jNdx, RESCUEINT64 kNdx, RESCUEFLOAT data)
{
  if (iNdx >= iLowBound)
  {
    iNdx -= iLowBound;
    if (iNdx < iCount)
    {
      if (jNdx >= jLowBound)
      {
        jNdx -= jLowBound;
        if (jNdx < jCount)
        {
          if (kCount == -1)
          {
            RESCUEINT64 ndx = (jNdx * iCount)
                   +  iNdx;
            data = value[ndx];
          }
          else
          {
            if (kNdx >= kLowBound)
            {
              kNdx -= kLowBound;
              if (kNdx < kCount)
              {
                RESCUEINT64 ndx = (kNdx * (jCount * iCount))
                       + (jNdx * iCount)
                       +  iNdx;
                data = value[ndx];
              }
            }
          }
        }
      }
    }
  }
  return data;
}

void RescueArrayFragmentFloat::InitializeArray()
{
  valueLength = NodeCount64();
  value = new RESCUEFLOAT [(size_t) valueLength];
  RESCUEFLOAT nullValue = (RESCUEFLOAT) ((RescueArrayFloat *) parentArray)->NullValue();
  RESCUEINT64 loop;
  for (loop = 0; loop < valueLength; loop++)
  {
    value[loop] = nullValue;
  }
}

void RescueArrayFragmentFloat::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  RescueContext *context = parentArray->ParentModel()->Context();
  RESCUEINT64 count;
  myfscanf(context, archiveFile, &count);
  if (count != 0)
  {
    RESCUEINT64 axisDecrement = 0;
    if (parentArray->CellCentered())
    {
      axisDecrement = 1;
    }
    valueLength = NodeCount64();
    value = new RESCUEFLOAT [(size_t) valueLength];

    PrepareRead();

    RescueGrid *grid = parentArray->Grid();
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
      RESCUEFLOAT data = ReadFloat(archiveFile, fileVersion > 1);

      RESCUEINT64 iNdx = origILowBound + iFollower;
      RESCUEINT64 jNdx = origJLowBound + jFollower;
      RESCUEINT64 kNdx = origKLowBound + kFollower;

      if (iNdx >= iLowBound)
      {
        iNdx -= iLowBound;
        if (iNdx < iCount)
        {
          if (jNdx >= jLowBound)
          {
            jNdx -= jLowBound;
            if (jNdx < jCount)
            {
              if (kCount == -1)
              {
                RESCUEINT64 ndx = (jNdx * iCount)
                       +  iNdx;
                value[ndx] = data;
              }
              else
              {
                if (kNdx >= kLowBound)
                {
                  kNdx -= kLowBound;
                  if (kNdx < kCount)
                  {
                    RESCUEINT64 ndx = (kNdx * (jCount * iCount))
                           + (jNdx * iCount)
                           +  iNdx;
                    value[ndx] = data;
                  }
                }
              }
            }
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
    EndRead();
  }
}

void RescueArrayFragmentFloat::SetValue(RESCUEFLOAT *valueArray)
{
  valueLength = NodeCount64();

  if (value != 0)
  {
    delete [] value;
  }
  value = new RESCUEFLOAT [(size_t) valueLength];

  RESCUEINT64 loop;
  for (loop = 0; loop < valueLength; loop++)
  {
    value[loop] = valueArray[loop];
  }
}

void RescueArrayFragmentFloat::AcceptValue(RESCUEFLOAT *valueArray)
{
  if (value != 0)
  {
    delete [] value;
  }
  value = valueArray;
  valueLength = NodeCount64();
}

void RescueArrayFragmentFloat::PrepareRead()
{
  readAheadBuffer = 0;
  readAheadAllocated = 0;
  readAheadCount = 0;
  readAheadPos = 0;
}

void RescueArrayFragmentFloat::EndRead()
{
  if (readAheadBuffer != 0)
  {
    delete[] readAheadBuffer;
  }
}

RESCUEFLOAT RescueArrayFragmentFloat::ReadFloat(FILE *stream, RESCUEBOOL compress)
{
  RescueContext *context = parentArray->ParentModel()->Context();
  if (compress)
  {
    if (readAheadBuffer == 0 || readAheadPos >= readAheadCount)
    {
      RESCUEUCHAR smallHowMany;
      RESCUEUINT64 howMany;
      myfscanf(context, stream, &smallHowMany);
      if (smallHowMany == 253 || smallHowMany == 254)
      {
        if (smallHowMany == 253)
        {
          myfscanf(context, stream, &smallHowMany);
          howMany = smallHowMany;
        }
        else
        {
          myfscanf(context, stream, &howMany);
        }
        if (readAheadAllocated < howMany)
        {
          if (readAheadBuffer != 0)
          {
            delete[] readAheadBuffer;
          }
          readAheadBuffer = new RESCUEFLOAT[(size_t) howMany];
          readAheadAllocated = howMany;
        }
        unsigned RESCUEINT64 loop;
        for (loop = 0; loop < howMany; loop++)
        {
          myfscanf(context, stream, &readAheadBuffer[loop]);
        }
        readAheadCount = howMany;
        readAheadPos = 0;
      }
      else
      {
        if (smallHowMany == 255)
        {
          myfscanf(context, stream, &howMany);
        }
        else
        {
          howMany = smallHowMany;
        }
        if (readAheadAllocated < howMany)
        {
          if (readAheadBuffer != 0)
          {
            delete[] readAheadBuffer;
          }
          readAheadBuffer = new RESCUEFLOAT[(size_t) howMany];
          readAheadAllocated = howMany;
        }
        RESCUEFLOAT value;
        myfscanf(context, stream, &value);
        unsigned RESCUEINT64 loop;
        for (loop = 0; loop < howMany; loop++)
        {
          readAheadBuffer[loop] = value;
        }
        readAheadCount = howMany;
        readAheadPos = 0;
      }
    }
    return readAheadBuffer[readAheadPos++];
  }
  else
  {
    RESCUEFLOAT myReturn;
    myfscanf(context, stream, &myReturn);
    return myReturn;
  }
}


