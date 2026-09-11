/************************************************************************************

 Extra.cpp
 
 These functions perform input and output to ASCII files. I borrowed them from
  my normal "bag of tricks". 
 
 Rod Hanks  Feb, 1995
 
*************************************************************************************/
#include "myHeaders.h"
#include <string.h>

#ifdef __unix__
#undef ISNAND
#define ISNAN
#endif

#ifdef ISNAND
#include <ieeefp.h>
#endif
#ifdef ISNAN
#include <math.h>
#endif
#include <float.h>
#include <stdlib.h>
#include "RescueModel.h"

void RescueSwapEndian(void *source, RESCUEINT64 count)
{
  RESCUEUCHAR *data = (RESCUEUCHAR *) source;
  RESCUEINT64 howMany = count / 2;
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    RESCUEUCHAR temp = data[loop];
    RESCUEINT64 otherNdx = count - loop - 1;
    data[loop] = data[otherNdx];
    data[otherNdx] = temp;
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEUINT64 output)
{
  if (context->BinaryFlag())
  {
    if (context->Write32())
    {
#ifdef _WIN32
      if (output > 4294967296)
#else
      if ((output >> 32) != 0)
#endif
      {
        throw "Model is too large to be written in 32 bit mode.";
      }
      RESCUEUINT32 smallOutput = (RESCUEUINT32) output;
      if (context->LittleEndian())
      {
        RescueSwapEndian(&smallOutput, sizeof(smallOutput));
      }
      fwrite(&smallOutput, sizeof(smallOutput), 1, stream);
    }
    else
    {
      if (context->LittleEndian())
      {
        RescueSwapEndian(&output, sizeof(output));
      }
      fwrite(&output, sizeof(output), 1, stream);
    }
  }
  else
  {
    fprintf(stream, "%llu\n", output);
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEUINT64 *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfprintf(context, stream, output[loop]);
 }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEINT64 *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfprintf(context, stream, output[loop]);
 }
}

#ifndef RESCUEINT_INT32_SAME
void myfprintf(RescueContext *context, FILE *stream, RESCUEINT32 *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfprintf(context, stream, output[loop]);
 }
}

void myfprintf(RescueContext *context, FILE *stream, unsigned RESCUEINT32 *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfprintf(context, stream, output[loop]);
 }
}
#endif

void myfprintf(RescueContext *context, FILE *stream, RESCUEUSHORT output)
{
  if (context->BinaryFlag())
  {
    if (context->LittleEndian())
    {
      RescueSwapEndian(&output, sizeof(output));
    }
    fwrite(&output, sizeof(RESCUEUSHORT), 1, stream);
  }
  else
  {
    fprintf(stream, "%hu\n", output);
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEUSHORT *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfprintf(context, stream, output[loop]);
 }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEUCHAR output)
{
  if (context->BinaryFlag())
  {
     fwrite(&output, sizeof(RESCUEUCHAR), 1, stream);
  }
  else
  {
    fprintf(stream, "%hu\n", (RESCUEUSHORT) output);
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEUCHAR *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfprintf(context, stream, output[loop]);
 }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEFLOAT *output, RESCUEINT64 arraySize, RESCUEBOOL compress)
{
 RESCUEINT64 loop;
  if (compress)
  {
    RESCUEINT64 ndx = 1;
    RESCUEINT64 pos = 0;
    while (ndx <= arraySize)
    {
      while ((ndx >= arraySize) ? FALSE : (output[pos] == output[ndx]))
      {
        ndx++;
      }
      RESCUEUINT64 howMany = (RESCUEUINT64) (ndx - pos);
      if (howMany == 1
      &&  ndx < arraySize)
      {
        while ((ndx >= arraySize) ? FALSE : (output[ndx - 1] != output[ndx]))
        {
          ndx++;
        }
        if (ndx < arraySize)
        {
          ndx -= 1;
        }
        howMany = (RESCUEUINT64) (ndx - pos);
        if (howMany == 1)
        {
          myfprintf(context, stream, (RESCUEUCHAR) howMany);
        }
        else if (howMany <= 250)
        {
          myfprintf(context, stream, (RESCUEUCHAR) 253);
          myfprintf(context, stream, (RESCUEUCHAR) howMany);
        }
        else
        {
          myfprintf(context, stream, (RESCUEUCHAR) 254);
          myfprintf(context, stream, howMany);
        }
        RESCUEINT64 loop;
        for (loop = pos; loop < ndx; loop++)
        {
          myfprintf(context, stream, output[loop]);
        }
/*
  What to do when there are lots of values that are different (prevents compression
  from making the file too much BIGGER.
*/
      }
      else if (howMany <= 250)
/*
  I'm saving the top 5 for flags.  I already have a use for three of them.
*/
      {
        myfprintf(context, stream, (RESCUEUCHAR) howMany);
        myfprintf(context, stream, output[pos]);
      }
      else
      {
        myfprintf(context, stream, (RESCUEUCHAR) 255);
/*
  255 flag says a count plus a value follows.
*/
        myfprintf(context, stream, howMany);
        myfprintf(context, stream, output[pos]);
      }
      pos = ndx++;
    }
  }
  else
  {
   for (loop = 0; loop < arraySize; loop++)
   {
    myfprintf(context, stream, output[loop]);
   }
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEFLOAT output)
{
  if (context->BinaryFlag())
  {
    if (context->LittleEndian())
    {
      RescueSwapEndian(&output, sizeof(output));
    }
    fwrite(&output, sizeof(RESCUEFLOAT), 1, stream);
  }
  else
  {
  #ifdef ISNAND
   if (isnand(output))
  #else
  #ifdef ISNAN
          if (isnan(output))
  #else
   if (_isnan(output))
  #endif
  #endif
   {
    fprintf(stream, "0\n");
   }
   else
   {
    fprintf(stream, "%.9g\n", output);
   }
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEDOUBLE *output, RESCUEINT64 arraySize, RESCUEBOOL compress)
{
 RESCUEINT64 loop;
  if (compress)
  {
    RESCUEINT64 ndx = 1;
    RESCUEINT64 pos = 0;
    while (ndx <= arraySize)
    {
      while ((ndx >= arraySize) ? FALSE : (output[pos] == output[ndx]))
      {
        ndx++;
      }
      RESCUEUINT64 howMany = (RESCUEUINT64) (ndx - pos);
      if (howMany == 1
      &&  ndx < arraySize)
      {
        while ((ndx >= arraySize) ? FALSE : (output[ndx - 1] != output[ndx]))
        {
          ndx++;
        }
        if (ndx < arraySize)
        {
          ndx -= 1;
        }
        howMany = (RESCUEUINT64) (ndx - pos);
        if (howMany == 1)
        {
          myfprintf(context, stream, (RESCUEUCHAR) howMany);
        }
        else if (howMany <= 250)
        {
          myfprintf(context, stream, (RESCUEUCHAR) 253);
          myfprintf(context, stream, (RESCUEUCHAR) howMany);
        }
        else
        {
          myfprintf(context, stream, (RESCUEUCHAR) 254);
          myfprintf(context, stream, howMany);
        }
        RESCUEINT64 loop;
        for (loop = pos; loop < ndx; loop++)
        {
          myfprintf(context, stream, output[loop]);
        }
/*
  What to do when there are lots of values that are different (prevents compression
  from making the file too much BIGGER.
*/
      }
      else if (howMany <= 250)
/*
  I'm saving the top 5 for flags.  I already have a use for three of them.
*/
      {
        myfprintf(context, stream, (RESCUEUCHAR) howMany);
        myfprintf(context, stream, output[pos]);
      }
      else
      {
        myfprintf(context, stream, (RESCUEUCHAR) 255);
/*
  255 flag says a count plus a value follows.
*/
        myfprintf(context, stream, howMany);
        myfprintf(context, stream, output[pos]);
      }
      pos = ndx++;
    }
  }
  else
  {
   for (loop = 0; loop < arraySize; loop++)
   {
    myfprintf(context, stream, output[loop]);
   }
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEINT64 *output, RESCUEINT64 arraySize, RESCUEBOOL compress)
{
 RESCUEINT64 loop;
  if (compress)
  {
    RESCUEINT64 ndx = 1;
    RESCUEINT64 pos = 0;
    while (ndx <= arraySize)
    {
      while ((ndx >= arraySize) ? FALSE : (output[pos] == output[ndx]))
      {
        ndx++;
      }
      RESCUEUINT64 howMany = (RESCUEUINT64) (ndx - pos);
      if (howMany == 1
      &&  ndx < arraySize)
      {
        while ((ndx >= arraySize) ? FALSE : (output[ndx - 1] != output[ndx]))
        {
          ndx++;
        }
        if (ndx < arraySize)
        {
          ndx -= 1;
        }
        howMany = (RESCUEUINT64) (ndx - pos);
        if (howMany == 1)
        {
          myfprintf(context, stream, (RESCUEUCHAR) howMany);
        }
        else if (howMany <= 250)
        {
          myfprintf(context, stream, (RESCUEUCHAR) 253);
          myfprintf(context, stream, (RESCUEUCHAR) howMany);
        }
        else
        {
          myfprintf(context, stream, (RESCUEUCHAR) 254);
          myfprintf(context, stream, howMany);
        }
        RESCUEINT64 loop;
        for (loop = pos; loop < ndx; loop++)
        {
          myfprintf(context, stream, output[loop]);
        }
/*
  What to do when there are lots of values that are different (prevents compression
  from making the file too much BIGGER.
*/
      }
      else if (howMany <= 250)
/*
  I'm saving the top 5 for flags.  I already have a use for three of them.
*/
      {
        myfprintf(context, stream, (RESCUEUCHAR) howMany);
        myfprintf(context, stream, output[pos]);
      }
      else
      {
        myfprintf(context, stream, (RESCUEUCHAR) 255);
/*
  255 flag says a count plus a value follows.
*/
        myfprintf(context, stream, howMany);
        myfprintf(context, stream, output[pos]);
      }
      pos = ndx++;
    }
  }
  else
  {
   for (loop = 0; loop < arraySize; loop++)
   {
    myfprintf(context, stream, output[loop]);
   }
  }
}

#ifndef RESCUEINT_INT32_SAME
void myfprintf(RescueContext *context, FILE *stream, RESCUEINT32 *output, RESCUEINT64 arraySize, RESCUEBOOL compress)
{
 RESCUEINT64 loop;
  if (compress)
  {
    RESCUEINT64 ndx = 1;
    RESCUEINT64 pos = 0;
    while (ndx <= arraySize)
    {
      while ((ndx >= arraySize) ? FALSE : (output[pos] == output[ndx]))
      {
        ndx++;
      }
      RESCUEUINT64 howMany = (RESCUEUINT64) (ndx - pos);
      if (howMany == 1
      &&  ndx < arraySize)
      {
        while ((ndx >= arraySize) ? FALSE : (output[ndx - 1] != output[ndx]))
        {
          ndx++;
        }
        if (ndx < arraySize)
        {
          ndx -= 1;
        }
        howMany = (RESCUEUINT64) (ndx - pos);
        if (howMany == 1)
        {
          myfprintf(context, stream, (RESCUEUCHAR) howMany);
        }
        else if (howMany <= 250)
        {
          myfprintf(context, stream, (RESCUEUCHAR) 253);
          myfprintf(context, stream, (RESCUEUCHAR) howMany);
        }
        else
        {
          myfprintf(context, stream, (RESCUEUCHAR) 254);
          myfprintf(context, stream, howMany);
        }
        RESCUEINT64 loop;
        for (loop = pos; loop < ndx; loop++)
        {
          myfprintf(context, stream, output[loop]);
        }
/*
  What to do when there are lots of values that are different (prevents compression
  from making the file too much BIGGER.
*/
      }
      else if (howMany <= 250)
/*
  I'm saving the top 5 for flags.  I already have a use for three of them.
*/
      {
        myfprintf(context, stream, (RESCUEUCHAR) howMany);
        myfprintf(context, stream, output[pos]);
      }
      else
      {
        myfprintf(context, stream, (RESCUEUCHAR) 255);
/*
  255 flag says a count plus a value follows.
*/
        myfprintf(context, stream, howMany);
        myfprintf(context, stream, output[pos]);
      }
      pos = ndx++;
    }
  }
  else
  {
   for (loop = 0; loop < arraySize; loop++)
   {
    myfprintf(context, stream, output[loop]);
   }
  }
}
#endif

void myfprintf(RescueContext *context, FILE *stream, RESCUEDOUBLE output)
{
  if (context->BinaryFlag())
  {
    if (context->LittleEndian())
    {
      RescueSwapEndian(&output, sizeof(output));
    }
    fwrite(&output, sizeof(RESCUEDOUBLE), 1, stream);
  }
  else
  {
  #ifdef ISNAND
   if (isnand(output))
  #else
  #ifdef ISNAN
          if (isnan(output))
  #else
   if (_isnan(output))
  #endif
  #endif
   {
    fprintf(stream, "0\n");
   }
   else
   {
    fprintf(stream, "%.15lg\n", output);
   }
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEINT64 output)
{
  if (context->BinaryFlag())
  {
    if (context->Write32())
    {
      if (output > 2147483647 || output < -2147483647)
      {
        throw "Model is too large to be written in 32 bit mode.";
      }
      RESCUEINT32 smallOutput = (RESCUEINT32) output;
      if (context->LittleEndian())
      {
        RescueSwapEndian(&smallOutput, sizeof(smallOutput));
      }
      fwrite(&smallOutput, sizeof(smallOutput), 1, stream);
    }
    else
    {
      if (context->LittleEndian())
      {
        RescueSwapEndian(&output, sizeof(output));
      }
      fwrite(&output, sizeof(output), 1, stream);
    }
  }
  else
  {
      fprintf(stream, "%lld\n", output);
  }
}

#ifndef RESCUEINT_INT32_SAME
void myfprintf(RescueContext *context, FILE *stream, RESCUEINT32 output)
{
  if (context->BinaryFlag())
  {
    if (context->LittleEndian())
    {
      RescueSwapEndian(&output, sizeof(output));
    }
    fwrite(&output, sizeof(RESCUEINT32), 1, stream);
  }
  else
  {
  fprintf(stream, "%d\n", output);
  }
}

void myfprintf(RescueContext *context, FILE *stream, unsigned RESCUEINT32 output)
{
  if (context->BinaryFlag())
  {
    if (context->LittleEndian())
    {
      RescueSwapEndian(&output, sizeof(output));
    }
    fwrite(&output, sizeof(unsigned RESCUEINT32), 1, stream);
  }
  else
  {
  fprintf(stream, "%u\n", output);
  }
}
#endif

void myfprintf(RescueContext *context, FILE *stream, const RESCUECHAR *output)
{
  if (context->BinaryFlag())
  {
    if (*output != ';')
    {
      RESCUESHORT len = (RESCUESHORT) strlen(output) + 1;
      if (context->LittleEndian())
      {
        RESCUESHORT swapLen = len;
        RescueSwapEndian(&swapLen, sizeof(swapLen));
        fwrite(&swapLen, sizeof(swapLen), 1, stream);
      }
      else
      {
        fwrite(&len, sizeof(len), 1, stream);
      }
      fwrite(output, sizeof(RESCUECHAR), len, stream);
    }
/*
  Don't output comments, since we don't read them
  back in.
*/
  }
  else
  {
  fprintf(stream, "%s\n", output);
  }
}

void myfprintf(RescueContext *context, FILE *stream, RESCUEBOOL output)
{
  if (context->BinaryFlag())
  {
    if (context->LittleEndian())
    {
      RescueSwapEndian(&output, sizeof(output));
    }
    fwrite(&output, sizeof(RESCUEBOOL), 1, stream);
  }
  else
  {
  fprintf(stream, "%d\n", output);
  }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEUCHAR *output)
{
  if (context->BinaryFlag())
  {
    fread(output, sizeof(RESCUEUCHAR), 1, stream);
  }
  else
  {
   RESCUECHAR buffer[50];
    RESCUEUSHORT dummy;
   
   myfgets(context, buffer, 50, stream);
   if (sscanf(buffer, "%hu", &dummy) != 1)
   {
    *output = 0;
   }
    else
    {
      *output = (RESCUEUCHAR) dummy;
    }
  }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEUCHAR *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfscanf(context, stream, output++);
 }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEUINT64 *output)
{
  if (context->BinaryFlag())
  {
    size_t localSize = sizeof(RESCUEUINT64);
    if (localSize == context->UIntSize())
    {
      fread(output, localSize, 1, stream);
      if (context->UIntSwap() == TRUE)
      {
        RescueSwapEndian(output, localSize);
      }
    }
    else if (context->UIntSize() == 4)
    {
#ifdef _WIN32
      unsigned __int32 localValue;
#else
      unsigned int localValue;
#endif
      fread(&localValue, 4, 1, stream);
      if (context->UIntSwap() == TRUE)
      {
        RescueSwapEndian(&localValue, 4);
      }
      *output = (RESCUEUINT64) localValue;
    }
    else if (context->UIntSize() == 8)
    {
#ifdef _WIN32
      unsigned __int64 localValue;
#else
      unsigned long long localValue;
#endif
      fread(&localValue, 8, 1, stream);
      if (context->UIntSwap() == TRUE)
      {
        RescueSwapEndian(&localValue, 8);
      }
#ifdef _WIN32
      if (localValue > 4294967296)
#else
      if ((localValue >> 32) != 0)
#endif
      {
        throw "64 bit file has values too large to read.";
      }
      *output = (RESCUEUINT64) localValue;
    }
  }
  else
  {
   RESCUECHAR buffer[50];
   
   myfgets(context, buffer, 50, stream);
   if (sscanf(buffer, "%lld", output) != 1)
   {
    *output = 0;
   }
  }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEUINT64 *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfscanf(context, stream, output++);
 }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEINT64 *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfscanf(context, stream, output++);
 }
}

#ifndef RESCUEINT_INT32_SAME
void myfscanf(RescueContext *context, FILE *stream, RESCUEINT32 *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfscanf(context, stream, output++);
 }
}

void myfscanf(RescueContext *context, FILE *stream, unsigned RESCUEINT32 *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfscanf(context, stream, output++);
 }
}
#endif

void myfscanf(RescueContext *context, FILE *stream, RESCUEUSHORT *output)
{
  if (context->BinaryFlag())
  {
    fread(output, sizeof(RESCUEUSHORT), 1, stream);
    if (context->LittleEndian())
    {
      RescueSwapEndian(output, sizeof(*output));
    }
  }
  else
  {
   RESCUECHAR buffer[50];
   
   myfgets(context, buffer, 50, stream);
   if (sscanf(buffer, "%hu", output) != 1)
   {
    *output = 0;
   }
  }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEUSHORT *output, RESCUEINT64 arraySize)
{
 RESCUEINT64 loop;

 for (loop = 0; loop < arraySize; loop++)
 {
  myfscanf(context, stream, output++);
 }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEDOUBLE *output, RESCUEINT64 arraySize, RESCUEBOOL compress)
{
  if (compress)
  {
    RESCUEINT64 pos = 0;
    while (pos < arraySize)
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
        unsigned RESCUEINT64 loop;
        for (loop = pos; loop < pos + howMany; loop++)
        {
          myfscanf(context, stream, &output[loop]);
        }
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
        RESCUEDOUBLE value;
        myfscanf(context, stream, &value);
        unsigned RESCUEINT64 loop;
        for (loop = pos; loop < pos + howMany; loop++)
        {
          output[loop] = value;
        }
      }
      pos += howMany;
    }
  }
  else
  {
   RESCUEINT64 loop;
   for (loop = 0; loop < arraySize; loop++)
   {
    myfscanf(context, stream, output++);
   }
  }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEDOUBLE *output)
{
  if (context->BinaryFlag())
  {
    fread(output, sizeof(RESCUEDOUBLE), 1, stream);
    if (context->LittleEndian())
    {
      RescueSwapEndian(output, sizeof(*output));
    }
  }
  else
  {
   RESCUECHAR buffer[50];
   
   myfgets(context, buffer, 50, stream);
   if (sscanf(buffer, "%lg", output) != 1)
   {
    *output = 0.0;
   }
  }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEFLOAT *output, RESCUEINT64 arraySize, RESCUEBOOL compress)
{
  if (compress)
  {
    RESCUEINT64 pos = 0;
    while (pos < arraySize)
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
        RESCUEUINT64 loop;
        for (loop = pos; loop < pos + howMany; loop++)
        {
          myfscanf(context, stream, &output[loop]);
        }
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
        RESCUEFLOAT value;
        myfscanf(context, stream, &value);
        RESCUEUINT64 loop;
        for (loop = pos; loop < pos + howMany; loop++)
        {
          output[loop] = value;
        }
      }
      pos += howMany;
    }
  }
  else
  {
   RESCUEINT64 loop;
   for (loop = 0; loop < arraySize; loop++)
   {
    myfscanf(context, stream, output++);
   }
  }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEINT64 *output, RESCUEINT64 arraySize, RESCUEBOOL compress)
{
  if (compress)
  {
    RESCUEINT64 pos = 0;
    while (pos < arraySize)
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
        RESCUEUINT64 loop;
        for (loop = pos; loop < pos + howMany; loop++)
        {
          myfscanf(context, stream, &output[loop]);
        }
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
        RESCUEINT64 value;
        myfscanf(context, stream, &value);
        unsigned RESCUEINT64 loop;
        for (loop = pos; loop < pos + howMany; loop++)
        {
          output[loop] = value;
        }
      }
      pos += howMany;
    }
  }
  else
  {
   RESCUEINT64 loop;
   for (loop = 0; loop < arraySize; loop++)
   {
    myfscanf(context, stream, output++);
   }
  }
}

#ifndef RESCUEINT_INT32_SAME
void myfscanf(RescueContext *context, FILE *stream, RESCUEINT32 *output, RESCUEINT64 arraySize, RESCUEBOOL compress)
{
  if (compress)
  {
    RESCUEINT64 pos = 0;
    while (pos < arraySize)
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
        RESCUEUINT64 loop;
        for (loop = pos; loop < pos + howMany; loop++)
        {
          myfscanf(context, stream, &output[loop]);
        }
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
        RESCUEINT32 value;
        myfscanf(context, stream, &value);
        unsigned RESCUEINT64 loop;
        for (loop = pos; loop < pos + howMany; loop++)
        {
          output[loop] = value;
        }
      }
      pos += howMany;
    }
  }
  else
  {
   RESCUEINT64 loop;
   for (loop = 0; loop < arraySize; loop++)
   {
    myfscanf(context, stream, output++);
   }
  }
}
#endif

void myfscanf(RescueContext *context, FILE *stream, RESCUEFLOAT *output)
{
  if (context->BinaryFlag())
  {
    if (context->ReadFileVersion() < 6)
    {
      RESCUEDOUBLE dOutput;
      fread(&dOutput, sizeof(RESCUEDOUBLE), 1, stream);
      if (context->LittleEndian())
      {
        RescueSwapEndian(&dOutput, sizeof(dOutput));
      }
      *output = (RESCUEFLOAT) dOutput;
    }
    else
    {
      fread(output, sizeof(RESCUEFLOAT), 1, stream);
      if (context->LittleEndian())
      {
        RescueSwapEndian(output, sizeof(*output));
      }
    }
  }
  else
  {
   RESCUECHAR buffer[50];
   
   myfgets(context, buffer, 50, stream);
   if (sscanf(buffer, "%g", output) != 1)
   {
    *output = (RESCUEFLOAT) 0.0;
   }
  }
}

void myfscanf(RescueContext *context, FILE *stream, RESCUEBOOL *output)
{
  if (context->BinaryFlag())
  {
    fread(output, sizeof(RESCUEBOOL), 1, stream);
    if (context->LittleEndian())
    {
      RescueSwapEndian(output, sizeof(*output));
    }
  }
  else
  {
   RESCUECHAR buffer[50];
   
   myfgets(context, buffer, 50, stream);
    *output = (buffer[0] == '1');
  }
}

#ifndef RESCUEINT_INT32_SAME
void myfscanf(RescueContext *context, FILE *stream, RESCUEINT32 *output)
{
  if (context->BinaryFlag())
  {
    fread(output, sizeof(RESCUEINT32), 1, stream);
    if (context->LittleEndian())
    {
      RescueSwapEndian(output, sizeof(*output));
    }
  }
  else
  {
   RESCUECHAR buffer[50];
   
   myfgets(context, buffer, 50, stream);
   *output = atoi(buffer);
  }
}

void myfscanf(RescueContext *context, FILE *stream, unsigned RESCUEINT32 *output)
{
  if (context->BinaryFlag())
  {
    fread(output, sizeof(unsigned RESCUEINT32), 1, stream);
    if (context->LittleEndian())
    {
      RescueSwapEndian(output, sizeof(*output));
    }
  }
  else
  {
   RESCUECHAR buffer[50];
   
   myfgets(context, buffer, 50, stream);
   *output = atoi(buffer);
  }
}
#endif

void myfscanf(RescueContext *context, FILE *stream, RESCUEINT64 *output)
{
  if (context->BinaryFlag())
  {
    size_t localSize = sizeof(RESCUEINT64);
    if (localSize == context->IntSize())
    {
      fread(output, localSize, 1, stream);
      if (context->IntSwap() == TRUE)
      {
        RescueSwapEndian(output, localSize);
      }
    }
    else if (context->IntSize() == 4)
    {
#ifdef _WIN32
      __int32 localValue;
#else
      int localValue;
#endif
      fread(&localValue, 4, 1, stream);
      if (context->IntSwap() == TRUE)
      {
        RescueSwapEndian(&localValue, 4);
      }
      *output = (RESCUEINT64) localValue;
    }
    else if (context->IntSize() == 8)
    {
#ifdef _WIN32
      __int64 localValue;
#else
      long long localValue;
#endif
      fread(&localValue, 8, 1, stream);
      if (context->IntSwap() == TRUE)
      {
        RescueSwapEndian(&localValue, 8);
      }
      if (localValue > 2147483647 || localValue < -2147483647)
      {
        throw "64 bit file has values too large to read.";
      }
      *output = (RESCUEINT64) localValue;
    }
  }
  else
  {
   RESCUECHAR buffer[50];
   
   myfgets(context, buffer, 50, stream);
   *output = atoi(buffer);
  }
}
/*
  I removed this because it makes the issue of reading files written with
  different RESCUEUINT64 sizes more difficult.
*/

RESCUECHAR *myfgets(RescueContext *context, RESCUECHAR *buffer, RESCUEINT64 maxBytes, FILE *stream)
{
  if (context->BinaryFlag())
  {
    RESCUESHORT length;
    RESCUESHORT extra = 0;
    fread(&length, sizeof(RESCUESHORT), 1, stream);
    if (context->LittleEndian())
    {
      RescueSwapEndian(&length, sizeof(length));
    }
    if (length > maxBytes)
    {
      extra = (RESCUESHORT) (length - maxBytes);
      length = (RESCUESHORT) maxBytes;
    }
    fread(buffer, sizeof(RESCUECHAR), length, stream);
    if (extra > 0)
    {
      buffer[maxBytes - 1] = 0;
      while (extra > 0)
      {
        getc(stream);
        extra--;
      }
    }
    return buffer;
  }
  else
  {
   RESCUECHAR *pos = buffer;
   RESCUEBOOL foundDelim = FALSE;
    RESCUEBOOL commentLine = FALSE;
   RESCUEINT64 next;
   
   maxBytes--;
   next = fgetc(stream);
   while (feof(stream) == 0 
       && ferror(stream) == 0
       && maxBytes > 0
       && foundDelim == FALSE)
   {
    if (next == 13
    ||  next == 10
    ||  next == 0)
    {
     RESCUEINT64 last = next;

     next = getc(stream);
     while ((next == 13 || next == 10 || next == 0) && next != last)
     {
      last = next;
      next = getc(stream);
     }
        if (commentLine)
        {
          commentLine = FALSE;
          foundDelim = FALSE;
        }
        else
        {
          ungetc((int) next, stream);
          foundDelim = TRUE;
        }
    }
      else if (pos == buffer && next == ';')
      {
        commentLine = TRUE;
        next = fgetc(stream);
      }
      else if (commentLine)
      {
        next = fgetc(stream);
      }
    else
    {
     *pos++ = (RESCUECHAR) next;
     maxBytes--;
     next = fgetc(stream);
    }
   }
   *pos = 0;
   if (feof(stream) == 0 && ferror(stream) == 0)
   {
    return buffer;
   }
   else
   {
    return NULL;
   }
  }
}







