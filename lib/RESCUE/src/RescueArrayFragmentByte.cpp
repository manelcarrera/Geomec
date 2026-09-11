/********************************************************************

  RescueArrayFragmentByte.cpp

  A dynamically sized array handler for part of a 2-3 dimensional grid of
  unsigned chars for RESCUE's data model. The related grid gives
  the dimensionality.  We keep lowbound and count for each axis.

  Rod Hanks,  January 2000

*********************************************************************/
#include "RescueArrayFragmentByte.h"
#include "RescueModel.h"
#include "RescueArrayByte.h"

RescueArrayFragmentByte::~RescueArrayFragmentByte()
{
  if (value != 0)
  {
    delete [] value;
  }
}

RESCUEUCHAR RescueArrayFragmentByte::Replace(RESCUEINT64 iNdx, RESCUEINT64 jNdx, RESCUEINT64 kNdx, RESCUEUCHAR data)
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

void RescueArrayFragmentByte::InitializeArray()
{
  valueLength = NodeCount64();
  value = new RESCUEUCHAR [(size_t) valueLength];
  RESCUEUCHAR nullValue = (RESCUEUCHAR) ((RescueArrayByte *) parentArray)->NullValue();
  RESCUEINT64 loop;
  for (loop = 0; loop < valueLength; loop++)
  {
    value[loop] = nullValue;
  }
}

void RescueArrayFragmentByte::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
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
    value = new RESCUEUCHAR [(size_t) valueLength];

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
      RESCUEUCHAR data;
      myfscanf(context, archiveFile, &data);

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
  }
}

void RescueArrayFragmentByte::SetValue(RESCUEUCHAR *valueArray)
{
  valueLength = NodeCount64();

  if (value != 0)
  {
    delete [] value;
  }
  value = new RESCUEUCHAR [(size_t) valueLength];

  RESCUEINT64 loop;
  for (loop = 0; loop < valueLength; loop++)
  {
    value[loop] = valueArray[loop];
  }
}

void RescueArrayFragmentByte::AcceptValue(RESCUEUCHAR *valueArray)
{
  if (value != 0)
  {
    delete [] value;
  }
  value = valueArray;
}



