/********************************************************************

  RescuePolyLineNodeUV.cpp

  The description of a point in terms of it's position on some face's
  two dimensional grid.

  Rod Hanks,  July 1996

*********************************************************************/
#include "RescuePolyLineNodeUV.h"
#include "RescueSurface.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include "RescueOrientationLedger.h"
#include <string.h>

RescuePolyLineNodeUV::RescuePolyLineNodeUV(RescueContext *context, FILE *archiveFile)
                :RescueObject(context)
{
  isA = R_RescuePolyLineNodeUV;
  myfscanf(context, archiveFile, &uValue);
  myfscanf(context, archiveFile, &vValue);
  myfscanf(context, archiveFile, &faceID);
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

void RescuePolyLineNodeUV::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; PolyLineNodeUV");
  myfprintf(context, archiveFile, uValue);
  myfprintf(context, archiveFile, vValue);
  if (context->FileVersion() >= 23)
  {
  myfprintf(context, archiveFile, face->Identifier());
  }
  else
  {
  myfprintf(context, archiveFile, face->ParentSurface()->Identifier());
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescuePolyLineNodeUV::Relink(RescueObject *parent)
{
  if (((RescueModel *) parent)->Context()->ReadFileVersion() >= 23)
  {
  face = ((RescueModel *) parent)->IJSurfaceIdentifiedBy(faceID);
  }
  else
  {
  RescueSurface *parentSurface = ((RescueModel *) parent)->SurfaceIdentifiedBy(faceID);
  face = parentSurface->NthIJSurface(0);
  }
}

void RescuePolyLineNodeUV::SetOrientation(RescueOrientationLedger *ledger)
{
  RESCUEINT64 iLowBound;
  RESCUEINT64 iCount;
  RESCUEINT64 jLowBound;
  RESCUEINT64 jCount;
  bool swapI;
  bool swapJ;

  if (ledger->GetIJFor(face, iLowBound, iCount, jLowBound, jCount, swapI, swapJ))
  {
  if (swapI)
  {
      float highBound = (float) ((iLowBound + iCount) - 1);
      uValue = (float) iLowBound + (highBound - uValue);
  }
  if (swapJ)
  {
      float highBound = (float) ((jLowBound + jCount) - 1);
      vValue = (float) jLowBound + (highBound - vValue);
  }
  }
}

RESCUEBOOL RescuePolyLineNodeUV::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescuePolyLineNodeUV)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}



