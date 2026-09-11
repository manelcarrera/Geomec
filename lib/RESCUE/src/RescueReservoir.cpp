/********************************************************************

  RescueReservoir.cpp

  Allows a writer to formally declare a Reservoir as a collection
  of RescueUnits.

  Rod Hanks,  September, 2001

*********************************************************************/
#include "RescueReservoir.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueReservoir::~RescueReservoir()
{
  delete reservoirName;
  delete unitIds;
}

RescueReservoir::RescueReservoir(RescueModel *model, RESCUECHAR *reservoirNameIn)
                  :RescueObject(model->Context())
{
  isA = R_RescueReservoir;
  reservoirName = new RCHString(reservoirNameIn);
  unitIds = new cBagInt();
  (*model->reservoirs) += this;
}

void RescueReservoir::AddUnit(RescueUnit *toAdd)
{
  if (toAdd != 0)
  {
    RESCUEINT64 id = toAdd->Identifier();
    if (unitIds->Contains(id) == FALSE)
    {
      (*unitIds) += id;
    }
  }
}

void RescueReservoir::DeleteUnit(RescueUnit *toDelete)
{
  if (toDelete != 0)
  {
    RESCUEINT64 id = toDelete->Identifier();
    (*unitIds) -= id;
  }
}

RescueUnit *RescueReservoir::NthUnit(RescueModel *model, RESCUEINT64 zeroBasedOrdinal)
{
  RescueUnit *myReturn = 0;
  RESCUEINT64 id = unitIds->NthObject(zeroBasedOrdinal);
  if (id != 0)
  {
    myReturn = model->UnitIdentifiedBy(id);
  }
  return myReturn;
}

void RescueReservoir::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; Reservoir");
  myfprintf(context, archiveFile, Identifier());
  reservoirName->Archive(context, archiveFile);
  RESCUEINT64 howMany = unitIds->Count64();
  myfprintf(context, archiveFile, howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    RESCUEINT64 id = unitIds->NthObject(loop);
    myfprintf(context, archiveFile, id);
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueReservoir::RescueReservoir(RescueContext *context, FILE *archiveFile)
                  :RescueObject(context),reservoirName(0),unitIds(0)
{
  isA = R_RescueReservoir;
  ReadId(context, archiveFile);
  reservoirName = new RCHString(context, archiveFile);
  unitIds = new cBagInt();
  RESCUEINT64 howMany;
  myfscanf(context, archiveFile, &howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
    RESCUEINT64 id;
    myfscanf(context, archiveFile, &id);
    (*unitIds) += id;
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


RESCUEBOOL RescueReservoir::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueReservoir)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}



