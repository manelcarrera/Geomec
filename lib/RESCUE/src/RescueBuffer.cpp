/********************************************************************

  RescueBuffer.cpp

  A buffer area where we read and write named file sections.

  Rod Hanks,  June, 2005

*********************************************************************/
#include "myHeaders.h"
#include <string.h>

#ifdef aix
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
#include "RescueBuffer.h"

extern void RescueSwapEndian(void *source, RESCUEINT64 count);

RescueBuffer::RescueBuffer(RescueContext *contextIn, FILE *archiveFile)
{
  context = contextIn;
  if (context->BinaryFlag())
  {
    myfscanf(context, archiveFile, &count);
    allocated = count + 2;
    buffer = (RESCUEUCHAR *) malloc((size_t) allocated);
    fread((void *) buffer, (unsigned int) count, 1, archiveFile);
    cursor = buffer;
  }
  else
  {
    RESCUEINT64 tCount;
    myfscanf(context, archiveFile, &tCount);
    myfscanf(context, archiveFile, &allocated);
    allocated += 10;
    buffer = (RESCUEUCHAR *) malloc((size_t) allocated);
    count = 0;
    lineCount = 0;
    while (tCount > 0)
    {
      RESCUECHAR str[512];
      myfgets(context, str, 512, archiveFile);
      (*this) << str;
      tCount--;
    }
    cursor = buffer;
  }
}

RescueBuffer::RescueBuffer(RescueContext *contextIn, RESCUEINT64 allocationHint)
{
  context = contextIn;
  allocated = allocationHint;
  count = 0;
  lineCount = 0;
  buffer = (RESCUEUCHAR *) malloc((size_t) allocated);
  cursor = 0;
}

RescueBuffer::~RescueBuffer()
{
  free(buffer);
}

void RescueBuffer::EnsureCapacity(RESCUEINT64 length)
{
  if (count + length > allocated)
  {
    allocated += ((count + length) - allocated) + 50;
  }
  buffer = (RESCUEUCHAR *) realloc(buffer, (size_t) allocated);
}

void RescueBuffer::Archive(FILE *archiveFile)
{
  if (context->BinaryFlag())
  {
    myfprintf(context, archiveFile, count);
    fwrite((void *) buffer, (unsigned int) count, 1, archiveFile);
  }
  else
  {
    myfprintf(context, archiveFile, lineCount);
    myfprintf(context, archiveFile, count);
    fprintf(archiveFile, "%s", buffer);
  }
}

RescueBuffer &RescueBuffer::operator<<(RCHString &more)
{
  RESCUEINT64 length = more.length64();
  if (context->BinaryFlag())
  {
    EnsureCapacity(length + sizeof(length) + 1);
    RESCUEINT64 lenBuf = length;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&lenBuf, sizeof(lenBuf));
    }
    RESCUEUCHAR *pos = buffer + count;
    memcpy(pos, &lenBuf, sizeof(lenBuf));
    pos += sizeof(lenBuf);
    memcpy(pos, more.String(), (unsigned int) length);
    count += sizeof(length) + length;
  }
  else
  {
    EnsureCapacity(length + 3);
    RESCUEUCHAR *pos = buffer + count;
    sprintf((RESCUECHAR *) pos, "%s\n", more.NonNullString());
    count = strlen((RESCUECHAR *) buffer);
    lineCount++;
  }
  return *this;
}

RescueBuffer &RescueBuffer::operator<<(const RESCUECHAR *more)
{
  size_t length = strlen(more);
  if (context->BinaryFlag())
  {
    EnsureCapacity(length + sizeof(length) + 1);
    RESCUEINT32 lenBuf = (RESCUEINT32) length;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&lenBuf, sizeof(lenBuf));
    }
    RESCUEUCHAR *pos = buffer + count;
    memcpy(pos, &lenBuf, sizeof(lenBuf));
    pos += sizeof(lenBuf);
    memcpy(pos, more, (unsigned int) length);
    count += sizeof(length) + length;
  }
  else
  {
    EnsureCapacity(length + 3);
    RESCUEUCHAR *pos = buffer + count;
    sprintf((RESCUECHAR *) pos, "%s\n", more);
    count = strlen((RESCUECHAR *) buffer);
    lineCount++;
  }
  return *this;
}

void RescueBuffer::operator>>(RCHString &more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT32 length;
    memcpy(&length, cursor, sizeof(length));
    if (context->LittleEndian())
    {
      RescueSwapEndian(&length, sizeof(length));
    }
    cursor += sizeof(length);
    RESCUECHAR *outBuf = (RESCUECHAR *) malloc((size_t) (length + 1));
    RESCUECHAR *begin = (RESCUECHAR *) outBuf;
    RESCUECHAR *pos = outBuf;
    while (length > 0)
    {
      *pos++ = *cursor++;
      length--;
    }
    *pos = 0;
    more = begin;   // RCHString creates copy of buffer on operator=()
    free( outBuf ); // and we need to free here allocated memory
  }
  else
  {
    RESCUEUCHAR *endPos = cursor;
    while (*endPos != 0 && *endPos != '\n' && *endPos != '\r')
    {
      endPos++;
    }
    bool oneMore = false;
    if (*endPos != 0)
    {
      if (*endPos != *(endPos + 1))
      {
        if (*(endPos + 1) == '\n' || *(endPos + 1) == '\r')
        {
          oneMore = true;
        }
      }
    }
    *endPos = 0;
    more = (RESCUECHAR *) cursor;
    cursor = endPos + 1;
    if (oneMore)
    {
      cursor++;
    }
  }
}

#ifndef RESCUEINT_INT32_SAME
RescueBuffer &RescueBuffer::operator<<(RESCUEINT32 more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT64 length = sizeof(more);
    EnsureCapacity(length + 1);
    RESCUEUCHAR *pos = buffer + count;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&more, sizeof(more));
    }
    memcpy(pos, &more, (unsigned int) length);
    count += length;
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%d", more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(RESCUEINT32 &more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT32 buffer;
    memcpy(&buffer, cursor, sizeof(buffer));
    if (context->LittleEndian())
    {
      RescueSwapEndian(&buffer, sizeof(buffer));
    }
    more = buffer;
    cursor += sizeof(buffer);
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = atoi(line.NonNullString());
  }
}

RescueBuffer &RescueBuffer::operator<<(unsigned RESCUEINT32 more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT64 length = sizeof(more);
    EnsureCapacity(length + 1);
    RESCUEUCHAR *pos = buffer + count;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&more, sizeof(more));
    }
    memcpy(pos, &more, (unsigned int) length);
    count += length;
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%u", more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(unsigned RESCUEINT32 &more)
{
  if (context->BinaryFlag())
  {
    unsigned RESCUEINT32 buffer;
    memcpy(&buffer, cursor, sizeof(buffer));
    if (context->LittleEndian())
    {
      RescueSwapEndian(&buffer, sizeof(buffer));
    }
    more = buffer;
    cursor += sizeof(buffer);
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = atoi(line.NonNullString());
  }
}
#endif

RescueBuffer &RescueBuffer::operator<<(RESCUEUINT64 more)
{
  if (context->BinaryFlag())
  {
    if (context->Write32())
    {
#ifdef _WIN32
      if (more > 4294967296)
#else
      if ((more >> 32) != 0)
#endif
      {
        throw "Model is too large to be written in 32 bit mode.";
      }
      RESCUEINT32 smallMore = (RESCUEINT32) more;
      RESCUEINT64 length = sizeof(smallMore);
      EnsureCapacity(length + 1);
      RESCUEUCHAR *pos = buffer + count;
      if (context->LittleEndian())
      {
        RescueSwapEndian(&smallMore, sizeof(smallMore));
      }
      memcpy(pos, &smallMore, (unsigned int) length);
      count += length;
    }
    else
    {
      RESCUEINT64 length = sizeof(more);
      EnsureCapacity(length + 1);
      RESCUEUCHAR *pos = buffer + count;
      if (context->LittleEndian())
      {
        RescueSwapEndian(&more, sizeof(more));
      }
      memcpy(pos, &more, (unsigned int) length);
      count += length;
    }
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%llu", more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(RESCUEUINT64 &more)
{
  if (context->BinaryFlag())
  {
    size_t localSize = sizeof(RESCUEUINT64);
    if (localSize == context->UIntSize())
    {
      RESCUEUINT64 buffer;
      memcpy(&buffer, cursor, sizeof(buffer));
      if (context->UIntSwap() == TRUE)
      {
        RescueSwapEndian(&buffer, localSize);
      }
      more = buffer;
      cursor += sizeof(buffer);
    }
    else if (context->UIntSize() == 4)
    {
#ifdef _WIN32
      unsigned __int32 localValue;
#else
      unsigned int localValue;
#endif
      memcpy(&localValue, cursor, sizeof(localValue));
      if (context->UIntSwap() == TRUE)
      {
        RescueSwapEndian(&localValue, 4);
      }
      more = (RESCUEUINT64) localValue;
      cursor += sizeof(localValue);
    }
    else if (context->UIntSize() == 8)
    {
#ifdef _WIN32
      unsigned __int64 localValue;
#else
      unsigned long long localValue;
#endif
      memcpy(&localValue, cursor, sizeof(localValue));
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
      more = (RESCUEUINT64) localValue;
      cursor += sizeof(localValue);
    }
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = atoi(line.NonNullString());
  }
}

RescueBuffer &RescueBuffer::operator<<(RESCUEINT64 more)
{
  if (context->BinaryFlag())
  {
    if (context->Write32())
    {
      if (more > 2147483647 || more < -2147483647)
      {
        throw "Model is too large to be written in 32 bit mode.";
      }
      RESCUEINT32 smallMore = (RESCUEINT32) more;
      RESCUEINT64 length = sizeof(smallMore);
      EnsureCapacity(length + 1);
      RESCUEUCHAR *pos = buffer + count;
      if (context->LittleEndian())
      {
        RescueSwapEndian(&smallMore, sizeof(smallMore));
      }
      memcpy(pos, &smallMore, (unsigned int) length);
      count += length;
    }
    else
    {
      RESCUEINT64 length = sizeof(more);
      EnsureCapacity(length + 1);
      RESCUEUCHAR *pos = buffer + count;
      if (context->LittleEndian())
      {
        RescueSwapEndian(&more, sizeof(more));
      }
      memcpy(pos, &more, (unsigned int) length);
      count += length;
    }
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%lld", more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(RESCUEINT64 &more)
{
  if (context->BinaryFlag())
  {
    size_t localSize = sizeof(RESCUEINT64);
    if (localSize == context->IntSize())
    {
      RESCUEINT64 buffer;
      memcpy(&buffer, cursor, sizeof(buffer));
      if (context->IntSwap() == TRUE)
      {
        RescueSwapEndian(&buffer, localSize);
      }
      more = buffer;
      cursor += sizeof(buffer);
    }
    else if (context->IntSize() == 4)
    {
#ifdef _WIN32
      unsigned __int32 localValue;
#else
      unsigned int localValue;
#endif
      memcpy(&localValue, cursor, sizeof(localValue));
      if (context->IntSwap() == TRUE)
      {
        RescueSwapEndian(&localValue, 4);
      }
      more = (RESCUEINT64) localValue;
      cursor += sizeof(localValue);
    }
    else if (context->IntSize() == 8)
    {
#ifdef _WIN32
      unsigned __int64 localValue;
#else
      unsigned long long localValue;
#endif
      memcpy(&localValue, cursor, sizeof(localValue));
      if (context->IntSwap() == TRUE)
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
      more = (RESCUEINT64) localValue;
      cursor += sizeof(localValue);
    }
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = atoi(line.NonNullString());
  }
}

RescueBuffer &RescueBuffer::operator<<(RESCUEUCHAR more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT64 length = sizeof(more);
    EnsureCapacity(length + 1);
    RESCUEUCHAR *pos = buffer + count;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&more, sizeof(more));
    }
    memcpy(pos, &more, (unsigned int) length);
    count += length;
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%hu", (RESCUEUSHORT) more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(RESCUEUCHAR &more)
{
  if (context->BinaryFlag())
  {
    RESCUEUCHAR buffer;
    memcpy(&buffer, cursor, sizeof(buffer));
    if (context->LittleEndian())
    {
      RescueSwapEndian(&buffer, sizeof(buffer));
    }
    more = buffer;
    cursor += sizeof(buffer);
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = atoi(line.NonNullString());
  }
}

RescueBuffer &RescueBuffer::operator<<(RESCUEUSHORT more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT64 length = sizeof(more);
    EnsureCapacity(length + 1);
    RESCUEUCHAR *pos = buffer + count;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&more, sizeof(more));
    }
    memcpy(pos, &more, (unsigned int) length);
    count += length;
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%hu", more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(RESCUEUSHORT &more)
{
  if (context->BinaryFlag())
  {
    RESCUEUSHORT buffer;
    memcpy(&buffer, cursor, sizeof(buffer));
    if (context->LittleEndian())
    {
      RescueSwapEndian(&buffer, sizeof(buffer));
    }
    more = buffer;
    cursor += sizeof(buffer);
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = atoi(line.NonNullString());
  }
}

RescueBuffer &RescueBuffer::operator<<(RESCUESHORT more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT64 length = sizeof(more);
    EnsureCapacity(length + 1);
    RESCUEUCHAR *pos = buffer + count;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&more, sizeof(more));
    }
    memcpy(pos, &more, (unsigned int) length);
    count += length;
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%hd", more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(RESCUESHORT &more)
{
  if (context->BinaryFlag())
  {
    RESCUESHORT buffer;
    memcpy(&buffer, cursor, sizeof(buffer));
    if (context->LittleEndian())
    {
      RescueSwapEndian(&buffer, sizeof(buffer));
    }
    more = buffer;
    cursor += sizeof(buffer);
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = atoi(line.NonNullString());
  }
}

RescueBuffer &RescueBuffer::operator<<(RESCUEFLOAT more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT64 length = sizeof(more);
    EnsureCapacity(length + 1);
    RESCUEUCHAR *pos = buffer + count;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&more, sizeof(more));
    }
    memcpy(pos, &more, (unsigned int) length);
    count += length;
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%.9g", more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(RESCUEFLOAT &more)
{
  if (context->BinaryFlag())
  {
    RESCUEFLOAT buffer;
    memcpy(&buffer, cursor, sizeof(buffer));
    if (context->LittleEndian())
    {
      RescueSwapEndian(&buffer, sizeof(buffer));
    }
    more = buffer;
    cursor += sizeof(buffer);
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = (RESCUEFLOAT) atof(line.NonNullString());
  }
}

RescueBuffer &RescueBuffer::operator<<(RESCUEDOUBLE more)
{
  if (context->BinaryFlag())
  {
    RESCUEINT64 length = sizeof(more);
    EnsureCapacity(length + 1);
    RESCUEUCHAR *pos = buffer + count;
    if (context->LittleEndian())
    {
      RescueSwapEndian(&more, sizeof(more));
    }
    memcpy(pos, &more, (unsigned int) length);
    count += length;
  }
  else
  {
    RESCUECHAR str[512];
    sprintf(str, "%.15lg", more);
    (*this) << str;
  }
  return *this;
}

void RescueBuffer::operator>>(RESCUEDOUBLE &more)
{
  if (context->BinaryFlag())
  {
    RESCUEDOUBLE buffer;
    memcpy(&buffer, cursor, sizeof(buffer));
    if (context->LittleEndian())
    {
      RescueSwapEndian(&buffer, sizeof(buffer));
    }
    more = buffer;
    cursor += sizeof(buffer);
  }
  else
  {
    RCHString line;
    (*this) >> line;
    more = atof(line.NonNullString());
  }
}




