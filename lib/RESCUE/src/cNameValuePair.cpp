/*************************************************************************

    cNameValuePair.cpp

       Keeps a list of named values.

    Rod Hanks               June 1998

****************************************************************************/
#include "cNameValuePair.h"
#include "RescueModel.h"
#include "RCHString.h"
#include <string.h>

cNameValuePair::cNameValuePair(RescueContext *context, FILE *archiveFile)
{
  myfscanf(context, archiveFile, &count);
  allocated = (count > 10) ? count : 10;
  names = (RCHString **) malloc(sizeof(RCHString *) * (size_t) allocated);
  values = (RCHString **) malloc(sizeof(RCHString *) * (size_t) allocated);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  RESCUEINT64 length;

  myfscanf(context, archiveFile, &length);
  RESCUECHAR *buffer = (RESCUECHAR *) malloc((size_t) (length + 1));
  myfgets(context, buffer, length + 1, archiveFile);
  RCHString *obj = new RCHString();
  obj->Accept(buffer);
  names[loop] = obj;

  myfscanf(context, archiveFile, &length);
  buffer = (RESCUECHAR *) malloc((size_t) (length + 1));
  myfgets(context, buffer, length + 1, archiveFile);
  obj = new RCHString();
  obj->Accept(buffer);
  values[loop] = obj;
  }
}

void cNameValuePair::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  RESCUEINT64 length = names[loop]->length64();
  myfprintf(context, archiveFile, length);
  myfprintf(context, archiveFile, names[loop]->String());
  length = values[loop]->length64();
  myfprintf(context, archiveFile, length);
  myfprintf(context, archiveFile, values[loop]->String());
  }
}

cNameValuePair::cNameValuePair()
{
  allocated = 10;
  count = 0;
  names = (RCHString **) malloc(sizeof(RCHString *) * (size_t) allocated);
  values = (RCHString **) malloc(sizeof(RCHString *) * (size_t) allocated);
}

cNameValuePair::~cNameValuePair()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  delete names[loop];
  delete values[loop];
  }
  free(names);
  free(values);
}

RESCUECHAR *cNameValuePair::NthName(RESCUEINT64 ordinal)
{
  RESCUECHAR *myReturn = 0;
  if (ordinal >= 0 && ordinal < count)
  {
  myReturn = names[ordinal]->String();
  }
  return myReturn;
}

RESCUECHAR *cNameValuePair::NthValue(RESCUEINT64 ordinal)
{
  RESCUECHAR *myReturn = 0;
  if (ordinal >= 0 && ordinal < count)
  {
  myReturn = values[ordinal]->String();
  }
  return myReturn;
}

RESCUEBOOL cNameValuePair::Contains(RESCUECHAR *name)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 ndx = NdxOf(name);
  if (ndx >= 0 && ndx < count)
  {
  if (*(names[ndx]) == name)
  {
      myReturn = TRUE;
  }
  }
  return myReturn;
}

void cNameValuePair::SetNameValuePair(const RESCUECHAR *name,
          const RESCUECHAR *value)
{
  RESCUEBOOL handled = FALSE;
  RESCUEINT64 ndx = NdxOf(name);
  if (ndx >= 0 && ndx < count)
  {
  if (*(names[ndx]) == name)
  {
      values[ndx]->Replace(value);
      handled = TRUE;
  }
  }
  if (handled == FALSE)
  {
  if (count == allocated)
  {
      allocated += count / 2;
      names = (RCHString **) realloc(names, sizeof(RCHString *) * (size_t) allocated);
      values = (RCHString **) realloc(values, sizeof(RCHString *) * (size_t) allocated);
  }
  if (ndx < count)
  {
      RESCUEINT64 loop;
      for (loop = count; loop > ndx; loop--)
      {
    names[loop] = names[loop - 1];
    values[loop] = values[loop - 1];
      }
  }
  names[ndx] = new RCHString(name);
  values[ndx] = new RCHString(value);
  count++;
  }
}

RESCUECHAR *cNameValuePair::GetNameValuePair(const RESCUECHAR *name)
{
  RESCUECHAR *myReturn = 0;
  RESCUEINT64 ndx = NdxOf(name);
  if (ndx >= 0 && ndx < count)
  {
  if (*(names[ndx]) == name)
  {
      myReturn = values[ndx]->String();
  }
  }
  return myReturn;
}

RESCUEINT64 cNameValuePair::NdxOf(const RESCUECHAR *name)
{
  RESCUEINT64 ndx = 0;
  RESCUEINT64 upperLimit = count;
  RESCUEINT64 lowerLimit = -1;
  RESCUEINT64 result = -1;

  while ((upperLimit - lowerLimit) > 1  && result != 0)
  {
  ndx = (lowerLimit + upperLimit) >> 1;
  result = strcmp(name, names[ndx]->String());
  if (result < 0)
  {
      upperLimit = ndx;
  }
  else if (result > 0)
  {
      lowerLimit = ndx;
  }
  }
  if (result > 0)
  {
  ndx++;
  }
  return ndx;
}

RESCUEINT32 cNameValuePair::Count(RESCUEBOOL throwIfTrue)
{
  if (count > 2147483647)
  {
  if (throwIfTrue)
  {
      throw "Model is too large to be accessed in 32 bit mode.";
  }
  return 0;
  }
  else
  {
  return (RESCUEINT32) count;
  }
}



