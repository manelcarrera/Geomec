/********************************************************************

  RescueBlock.cpp

  The block object for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueBlock.h"
#include "RescueModel.h"
#include "RescueBuffer.h"
#include <string.h>

RescueBlock::RescueBlock(const RESCUECHAR *newBlockName,
    RescueModel *newBlockParent)
                        :RescueHistoryObject(newBlockParent->Context())
                        ,blockName(0)
                        ,parentModel(newBlockParent)
                        ,blockUnits(0)
{
  blockName = new RCHString(newBlockName);
  blockUnits = new cSetRescueBlockUnit();

  (*parentModel->blocks) += this;
  isA = R_RescueBlock;
}

RESCUEBOOL RescueBlock::IsNamed(const RESCUECHAR *possibleName)
{
   return ((*blockName) == possibleName);
}

RescueBlock::~RescueBlock()
{
  if (blockName != 0)
  {
    delete blockName;
  }
  if (blockUnits != 0)
  {
    delete blockUnits;
  }
}

RescueBlock::RescueBlock(RescueContext *context, FILE *archiveFile)
                  :RescueHistoryObject(context)
                  ,blockName(0)
                  ,parentModel(0)
                  ,blockUnits(0)
{
  RESCUECHAR myString[255];

  isA = R_RescueBlock;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  blockName = new RCHString(myString);
  context->RescueProgress(myString);
  blockUnits = new cSetRescueBlockUnit();
  (*blockUnits).UnArchive(context, archiveFile);
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

void RescueBlock::Relink(RescueObject *parent)
{
  parentModel = (RescueModel *) parent;
  (*blockUnits).Relink(this);
}


void RescueBlock::Archive(FILE *archiveFile)
{
  parentModel->Context()->RescueProgress((*blockName).String());
  myfprintf(parentModel->Context(), archiveFile, "; Block");
  myfprintf(parentModel->Context(), archiveFile, Identifier());
  myfprintf(parentModel->Context(), archiveFile, (*blockName).String());
  (*blockUnits).Archive(parentModel->Context(), archiveFile);
  if (parentModel->Context()->FileVersion() >= 37)
  {
    myfprintf(parentModel->Context(), archiveFile, "EOD");
  }
}

RescueBlockUnit *RescueBlock::BlockUnitForUnit(RescueUnit *existingUnit)
{
  RescueBlockUnit *myReturn = 0;
  RESCUEINT64 ndx = 0;
  RescueBlockUnit *candidate = (*blockUnits).NthObject(ndx++);

  while (candidate != 0 && myReturn == 0)
  {
    if (candidate->Unit() == existingUnit)
    {
      myReturn = candidate;
    }
    else
    {
      candidate = (*blockUnits).NthObject(ndx++);
    }
  }
  
  return myReturn;
}

RescueBlockUnitSide *RescueBlock::BlockUnitSideIdentifiedBy(RESCUEINT64 id)
{
  RescueBlockUnitSide *myReturn = 0;
  RESCUEINT64 ndx = 0;
  RescueBlockUnit *bu = (*blockUnits).NthObject(ndx++);
  while (bu != 0 && myReturn == 0)
  {
    myReturn = bu->BlockUnitSideIdentifiedBy(id);
    if (myReturn == 0)
    {
      bu = (*blockUnits).NthObject(ndx++);
    }
  }
  return myReturn;
}

RescueMacroVolume *RescueBlock::MacroVolumeIdentifiedBy(RESCUEINT64 id)
{
  RescueMacroVolume *myReturn = 0;
  RESCUEINT64 ndx = 0;
  RescueBlockUnit *bu = (*blockUnits).NthObject(ndx++);
  while (bu != 0 && myReturn == 0)
  {
    myReturn = bu->MacroVolumeIdentifiedBy(id);
    if (myReturn == 0)
    {
      bu = (*blockUnits).NthObject(ndx++);
    }
  }
  return myReturn;
}

RescueProperty *RescueBlock::PropertyIdentifiedBy(RESCUEINT64 id)
{
  RescueProperty *myReturn = 0;
  RESCUEINT64 ndx = 0;
  RescueBlockUnit *bu = (*blockUnits).NthObject(ndx++);
  while (bu != 0 && myReturn == 0)
  {
    myReturn = bu->PropertyIdentifiedBy(id);
    if (myReturn == 0)
    {
      bu = (*blockUnits).NthObject(ndx++);
    }
  }
  return myReturn;
}

void RescueBlock::SetOrientation(RescueOrientationLedger *ledger, 
                                 RescueCoordinateSystem::Orientation orientation)
{
  RESCUEINT64 ndx = 0;
  RescueBlockUnit *bu = (*blockUnits).NthObject(ndx++);
  while (bu != 0)
  {
    bu->SetOrientation(ledger, orientation);
    bu = (*blockUnits).NthObject(ndx++);
  }
}

RESCUEBOOL RescueBlock::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueBlock)
  {
    return TRUE;
  }
  else
  {
    return RescueHistoryObject::IsOfType(thisType);
  }
}

void RescueBlock::FindUniquePropertyNames(cSetString *container)
{
  RESCUEINT64 ndx = 0;
  RescueBlockUnit *bu = (*blockUnits).NthObject(ndx++);
  while (bu != 0)
  {
    bu->FindUniquePropertyNames(container);
    bu = (*blockUnits).NthObject(ndx++);
  }
}

RESCUEINT32 RescueBlock::BlockUnitCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = BlockUnitCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}



