/********************************************************************

  RescueColorTableList.cpp

  An association between a color table and a property type.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "myHeaders.h"
#include "RescueColorTableList.h"
#include "RescueColorTable.h"
#include "RescueBuffer.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#ifdef _WIN32
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#if !defined(WIN32) && !defined(WIN64) && !defined(_WINDOWS)
#define _strdup strdup
#endif  // !defined(WIN32) && !defined(WIN64)

RescueColorTableList::RescueColorTableList(RescueContext *context, RESCUEINT64 probableCount)
                          :RescueObject(context)
{
  allocated = probableCount;
  count = 0;
  typeNames = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * (size_t) allocated);
  tables = (RescueColorTable **) malloc(sizeof(RescueColorTable *) * (size_t) allocated);
}

RescueColorTableList::~RescueColorTableList()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  free(typeNames[loop]);
  delete tables[loop];
  }
  free(typeNames);
  free(tables);
}

RescueColorTable *RescueColorTableList::TableFor(RESCUECHAR *typeName)
{
  RescueColorTable *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == 0; loop++)
  {
  if (strcasecmp(typeName, typeNames[loop]) == 0)
  {
      myReturn = tables[loop];
  }
  }
  return myReturn;
}

RESCUEBOOL RescueColorTableList::DeleteTableFor(RESCUECHAR *typeName)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 ndx = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
  if (strcasecmp(typeName, typeNames[loop]) == 0)
  {
      myReturn = TRUE;
      ndx = loop;
  }
  }
  if (myReturn == TRUE)
  {
  tables[ndx]->RescueDeleteFile();

  free(typeNames[ndx]);
  delete tables[ndx];
  RESCUEINT64 numberToMove = (count - ndx) - 1;
  if (numberToMove > 0)
  {
      memmove(&typeNames[(int) ndx], &typeNames[(int) (ndx + 1)], (size_t) numberToMove * sizeof(RESCUECHAR *));
      memmove(&tables[(int) ndx]   , &tables[(int) (ndx + 1)]   , (size_t) numberToMove * sizeof(RescueColorTable *));
  }
  count--;
  }
  return myReturn;
}

RESCUEBOOL RescueColorTableList::AddTableFor(RESCUECHAR *typeName, RescueColorTable *table)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 ndx = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++)
  {
  if (strcasecmp(typeName, typeNames[loop]) == 0)
  {
      myReturn = TRUE;
      ndx = loop;
  }
  }
  if (myReturn == TRUE)
  {
  delete tables[ndx];
  tables[ndx] = table;
  }
  else
  {
  if (count >= allocated)
  {
      allocated += MAX(10, allocated / 2);
      typeNames = (RESCUECHAR **) realloc(typeNames, sizeof(RESCUECHAR *) * (size_t) allocated);
      tables = (RescueColorTable **) realloc(tables, sizeof(RescueColorTable *) * (size_t) allocated);
  }
  typeNames[count] = _strdup(typeName);
  tables[count++] = table;
  }
  return myReturn;
}

void RescueColorTableList::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  RESCUEINT64 length = (RESCUEINT64) strlen(typeNames[loop]);
  myfprintf(context, archiveFile, length);
  myfprintf(context, archiveFile, typeNames[loop]);
  tables[loop]->Archive(archiveFile);
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueColorTableList::RescueColorTableList(RescueContext *context, FILE *archiveFile)
                  :RescueObject(context)
{
  myfscanf(context, archiveFile, &count);
  allocated = MAX(10, count);
  typeNames = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * (size_t) allocated);
  tables = (RescueColorTable **) malloc(sizeof(RescueColorTable *) * (size_t) allocated);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  RESCUEINT64 length;
  myfscanf(context, archiveFile, &length);
  length++;
  typeNames[loop] = (RESCUECHAR *) malloc((size_t) length);
  myfgets(context, typeNames[loop], length, archiveFile);
  tables[loop] = new RescueColorTable(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
  }
  }
}

void RescueColorTableList::Relink(RescueObject *parentModel)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
  tables[loop]->Relink(parentModel);
  }
}


