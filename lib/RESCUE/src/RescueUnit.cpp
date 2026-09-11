/********************************************************************

  RescueUnit.cpp

  The unit object for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueUnit.h"
#include "RescueBuffer.h"
#include "RescueEventDescr.h"
#include "RescueLogicalOrder.h"
#include "RescueModel.h"
#include <string.h>

RESCUEBOOL RescueUnit::DeleteBlockUnit(RescueBlockUnit *existingBlockUnit) {
  existingBlockUnit->Dispose();
  return ((*blockUnits) -= existingBlockUnit);
}

RescueUnit::~RescueUnit() {
  if (unitName != 0) {
    delete unitName;
  }
  if (blockUnits != 0) {
    delete blockUnits;
  }
  if (groups != 0) {
    delete groups;
  }
}

RescueUnit::RescueUnit(RESCUECHAR *newUnitName, RescueModel *newParentModel)
    : RescueLogicalOrderEntry(newParentModel->Context()), unitName(0), parentModel(newParentModel), blockUnits(0),
      groups(0) {
  unitName = new RCHString(newUnitName);
  blockUnits = new cBagRescueBlockUnit();
  groups = new cSetRescuePropertyGroup();

  isA = R_RescueUnit;
  (*parentModel->units) += this;
}

RescueUnit::RescueUnit(RescueContext *context, FILE *archiveFile)
    : RescueLogicalOrderEntry(context), unitName(0), parentModel(0), blockUnits(0), groups(0) {
  RESCUECHAR myString[255];

  blockUnits = new cBagRescueBlockUnit();
  groups = new cSetRescuePropertyGroup();
  isA = R_RescueUnit;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  unitName = new RCHString(myString);
  context->RescueProgress(myString);
  if (context->ReadFileVersion() < 30) {
    myfscanf(context, archiveFile, &horizonAboveID);
    myfscanf(context, archiveFile, &horizonBelowID);
    if (context->ReadFileVersion() >= 25) {
      myfscanf(context, archiveFile, &eventDescrAboveID);
      myfscanf(context, archiveFile, &eventDescrBelowID);
    } else {
      eventDescrAboveID = 0;
      eventDescrBelowID = 0;
    }
  }
  (*groups).UnArchive(context, archiveFile);
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

RESCUEBOOL RescueUnit::IsNamed(const RESCUECHAR *possibleName) { return ((*unitName) == possibleName); }

void RescueUnit::Relink(RescueObject *parent) {
  parentModel = (RescueModel *)parent;
  (*groups).Relink(this);
}

void RescueUnit::Archive(FILE *archiveFile) {
  RescueContext *context = ParentModel()->Context();
  context->RescueProgress((*unitName).String());
  myfprintf(context, archiveFile, "; Unit");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, (*unitName).String());
  if (context->FileVersion() < 30) {
    RescueLogicalOrder *order = ParentModel()->LogicalOrder();
    RescueHorizon *horizonAboveMe = order->HorizonAbove(this);
    if (horizonAboveMe == 0) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    } else {
      myfprintf(context, archiveFile, horizonAboveMe->Identifier());
    }
    RescueHorizon *horizonBelowMe = order->HorizonBelow(this);
    if (horizonBelowMe == 0) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    } else {
      myfprintf(context, archiveFile, horizonBelowMe->Identifier());
    }
    if (context->FileVersion() >= 25) {
      RescueEventDescr *eventDescrAboveMe = order->EventAbove(this);
      if (eventDescrAboveMe == 0) {
        myfprintf(context, archiveFile, (RESCUEINT64)0);
      } else if (context->FileVersion() == 25 && eventDescrAboveMe->FullEvents(parentModel) == FALSE) {
        myfprintf(context, archiveFile, (RESCUEINT64)0);
      } else {
        myfprintf(context, archiveFile, eventDescrAboveMe->Identifier());
      }
      RescueEventDescr *eventDescrBelowMe = order->EventBelow(this);
      if (eventDescrBelowMe == 0) {
        myfprintf(context, archiveFile, (RESCUEINT64)0);
      } else if (context->FileVersion() == 25 && eventDescrAboveMe->FullEvents(parentModel) == FALSE) {
        myfprintf(context, archiveFile, (RESCUEINT64)0);
      } else {
        myfprintf(context, archiveFile, eventDescrBelowMe->Identifier());
      }
    }
  }
  (*groups).Archive(context, archiveFile);
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEBOOL RescueUnit::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueUnit) {
    return TRUE;
  } else {
    return RescueLogicalOrderEntry::IsOfType(thisType);
  }
}
