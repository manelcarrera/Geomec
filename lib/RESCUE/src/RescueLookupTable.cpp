/********************************************************************

  RescueLookupTable.cpp

  An item in a table of lookups for properties which translates to
  a two column table of floats. 

  Rod Hanks,  May 1997

*********************************************************************/
#include "RescueLookupTable.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

void RescueLookupTable::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; Lookup Table");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, rows);
  myfprintf(context, archiveFile, table, rows * 2, FALSE);
/*
  We don't compress lookup tables because I think by their nature
  there will be a low probability of duplicates.
*/
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueLookupTable::RescueLookupTable(RescueContext *context, FILE *archiveFile)
                  :RescueLookupItem(context),table(0)
{
  isA = R_RescueLookupTable;
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &rows);
  if (rows != 0)
  {
    table = new RESCUEFLOAT [(size_t) (rows * 2)];
    myfscanf(context, archiveFile, table, rows * 2, FALSE);
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

RESCUEBOOL RescueLookupTable::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueLookupTable)
  {
    return TRUE;
  }
  else
  {
    return RescueLookupItem::IsOfType(thisType);
  }
}



