/********************************************************************

  RescueColor.cpp

  A (possibly) named rgb.

  Rod Hanks,  February, 1999

*********************************************************************/
#include <stdlib.h>
#include <string.h>
#include "RescueModel.h"
#include "RescueColor.h"
#include "RCHString.h"
#include "RescueBuffer.h"
#include <string.h>

void RescueColor::CommonInitialization()
{
 isA = R_RescueColor;
  colorName = new RCHString();
  red = (RESCUEFLOAT) 0;
  green = (RESCUEFLOAT) 0;
  blue = (RESCUEFLOAT) 0;
}

RescueColor::RescueColor(RescueColor &otherColor)
            :RescueObject(otherColor.context),context(otherColor.context)
{
  CommonInitialization();
  SetColor(otherColor.red, otherColor.green, otherColor.blue, otherColor.colorName->String());
}

RescueColor::RescueColor(RescueContext *contextIn, int red, int green, int blue, RESCUECHAR *name)
            :RescueObject(contextIn),context(contextIn)
{
  CommonInitialization();
  SetColor(red, green, blue, name);
}

RescueColor::RescueColor(RescueContext *contextIn, RESCUEFLOAT red, RESCUEFLOAT green, RESCUEFLOAT blue, RESCUECHAR *name)
            :RescueObject(contextIn),context(contextIn)
{
  CommonInitialization();
  SetColor(red, green, blue, name);
}

RescueColor::~RescueColor()
{
  delete colorName;
}

void RescueColor::SetColor(int redInt, int greenInt, int blueInt, RESCUECHAR *name)
{
  RESCUEFLOAT redIn = (RESCUEFLOAT) ((redInt + 1) / 256.0);
  RESCUEFLOAT greenIn = (RESCUEFLOAT) ((greenInt + 1) / 256.0);
  RESCUEFLOAT blueIn = (RESCUEFLOAT) ((blueInt + 1) / 256.0);
  SetColor(redIn, greenIn, blueIn, name);
}

void RescueColor::SetColor(RESCUEFLOAT redIn, RESCUEFLOAT greenIn, RESCUEFLOAT blueIn, RESCUECHAR *name)
{
  red = redIn;
  green = greenIn;
  blue = blueIn;
  if (name != 0)
  {
  (*colorName) = name;
  }
  else
  {
  (*colorName) = "";
  }
}

void RescueColor::GetColor(int &redOut, int &greenOut, int &blueOut)
{
  redOut = (int) (red * 256.0) - 1;
  greenOut = (int) (green * 256.0) - 1;
  blueOut = (int) (blue * 256.0) - 1;
}

void RescueColor::GetColor(RESCUEFLOAT &redOut, RESCUEFLOAT &greenOut, RESCUEFLOAT &blueOut)
{
  redOut = red;
  greenOut = green;
  blueOut = blue;
}

void RescueColor::Archive(RescueContext *contextIn, FILE *archiveFile)
{
  if (contextIn->FileVersion() >= 15)
  {
  myfprintf(contextIn, archiveFile, (*colorName).String());
  myfprintf(contextIn, archiveFile, red);
  myfprintf(contextIn, archiveFile, green);
  myfprintf(contextIn, archiveFile, blue);
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueColor::RescueColor(RescueContext *contextIn, FILE *archiveFile)
              :RescueObject(contextIn),context(contextIn)
{
  CommonInitialization();
  if (context->ReadFileVersion() >= 15)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  (*colorName) = myString;
  myfscanf(context, archiveFile, &red);
  myfscanf(context, archiveFile, &green);
  myfscanf(context, archiveFile, &blue);
  }
/*
  Actually we don't get called on earlier files, cause this
  way the user can tell if a color has been defined.
*/
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

RESCUEBOOL RescueColor::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueColor)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}


