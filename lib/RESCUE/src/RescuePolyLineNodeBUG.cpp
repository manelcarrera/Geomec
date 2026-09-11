/********************************************************************

  RescuePolyLineNodeBUG.h

  The description of a point in terms of it's position on some geometry's
  layer.

  Rod Hanks,  December 1999

*********************************************************************/
#include "RescuePolyLineNodeBUG.h"
#include "RescueGeometry.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include "RescueOrientationLedger.h"
#include <string.h>

RescuePolyLineNodeBUG::RescuePolyLineNodeBUG(RescueContext *context, FILE *archiveFile)
                                  :RescueObject(context)
{
  isA = R_RescuePolyLineNodeBUG;
  RESCUEINT64 temp;
  myfscanf(context, archiveFile, &temp);
  layerType = (RescuePolyLineNode::RescueLayerType) temp;
  myfscanf(context, archiveFile, &layerNdxParam);
  myfscanf(context, archiveFile, &geometryID);
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

void RescuePolyLineNodeBUG::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; PolyLineNodeBUG");
  myfprintf(context, archiveFile, (RESCUEINT64) layerType);
  myfprintf(context, archiveFile, layerNdxParam);
  myfprintf(context, archiveFile, geometry->Identifier());
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

void RescuePolyLineNodeBUG::Relink(RescueObject *parent)
{
  geometry = ((RescueModel *) parent)->GeometryIdentifiedBy(geometryID);
}

void RescuePolyLineNodeBUG::SetOrientation(RescueOrientationLedger *ledger)
{
  RESCUEINT64 kLowBound;
  RESCUEINT64 kCount;
  bool swapK;

  if (ledger->GetKFor(geometry, kLowBound, kCount, swapK))
  {
    if (swapK)
    {
      float highBound = (float) ((kLowBound + kCount) - 1);
      layerNdxParam = (float) kLowBound + (highBound - layerNdxParam);
    }
  }
}

RESCUEBOOL RescuePolyLineNodeBUG::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescuePolyLineNodeBUG)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}



