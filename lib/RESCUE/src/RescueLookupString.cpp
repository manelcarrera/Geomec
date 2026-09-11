/********************************************************************

  RescueLookupString.cpp

  An item in a table of lookups for properties which translates to
  a string. 

  Rod Hanks,  May 1997

*********************************************************************/
#include "RescueLookupString.h"
#include "RescueBuffer.h"
#include <string.h>

RescueLookupString::~RescueLookupString()
{
  if (translation != 0)
  {
  delete translation;
  }
}

RescueLookupString::RescueLookupString(RescueContext *context, const RESCUECHAR *translationString)
                      :RescueLookupItem(context),translation(0)
{
  translation = new RCHString(translationString);
  isA = R_RescueLookupString;
}


void RescueLookupString::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; Lookup String");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, translation->String());
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueLookupString::RescueLookupString(RescueContext *context, FILE *archiveFile)
                                  :RescueLookupItem(context)
{
  RESCUECHAR myString[255];

  isA = R_RescueLookupString;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  translation = new RCHString(myString);
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

RESCUEBOOL RescueLookupString::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueLookupString)
  {
  return TRUE;
  }
  else
  {
  return RescueLookupItem::IsOfType(thisType);
  }
}




