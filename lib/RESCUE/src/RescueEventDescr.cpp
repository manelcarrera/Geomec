/********************************************************************

  RescueEventDescr.cpp

  This object describes an unconformity or a diapir.

  Rod Hanks,  June 2002

*********************************************************************/
#include "RescueEventDescr.h"
#include "RescueBuffer.h"
#include "RescueLogicalOrder.h"
#include "RescueModel.h"
#include <string.h>

RescueEventDescr::RescueEventDescr(RescueContext *context, RESCUECHAR *nameIn, EventType flagIn)
    : RescueLogicalOrderEntry(context), eventType(flagIn), eventDescrName(new RCHString(nameIn)),
      sections(new cBagRescueSection()), sectionIds(0) {
  isA = R_RescueEventDescr;
}

RescueEventDescr::~RescueEventDescr() {
  delete sections;
  delete eventDescrName;
}

RESCUEBOOL RescueEventDescr::FullEvents(RescueModel *model) {
  RescueLogicalOrder *order = model->LogicalOrder();
  RescueObject *eventAbove = order->UnconformityEventAbove(this);
  RescueObject *eventBelow = order->UnconformityEventBelow(this);
  if (eventAbove != 0 && eventBelow != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

void RescueEventDescr::Archive(RescueModel *model, RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, "; EventDescr");
  myfprintf(context, archiveFile, Identifier());
  if (context->FileVersion() >= 25) {
    eventDescrName->Archive(context, archiveFile);
  }
  if (context->FileVersion() >= 26 && context->FileVersion() < 30) {
    RescueLogicalOrder *order = model->LogicalOrder();
    RescueObject *eventAbove = order->UnconformityEventAbove(this);
    if (eventAbove == 0) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    } else {
      myfprintf(context, archiveFile, (RESCUEINT64)1);
      myfprintf(context, archiveFile, "; RescueEvent");
      myfprintf(context, archiveFile, context->NextId());
      if (eventAbove->IsOfType(R_RescueHorizon)) {
        myfprintf(context, archiveFile, eventAbove->Identifier());
        myfprintf(context, archiveFile, (RESCUEINT64)0);
      } else {
        myfprintf(context, archiveFile, (RESCUEINT64)0);
        myfprintf(context, archiveFile, eventAbove->Identifier());
      }
    }
    RescueObject *eventBelow = order->UnconformityEventBelow(this);
    if (eventBelow == 0) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    } else {
      myfprintf(context, archiveFile, (RESCUEINT64)1);
      myfprintf(context, archiveFile, "; RescueEvent");
      myfprintf(context, archiveFile, context->NextId());
      if (eventBelow->IsOfType(R_RescueHorizon)) {
        myfprintf(context, archiveFile, eventBelow->Identifier());
        myfprintf(context, archiveFile, (RESCUEINT64)0);
      } else {
        myfprintf(context, archiveFile, (RESCUEINT64)0);
        myfprintf(context, archiveFile, eventBelow->Identifier());
      }
    }
  }
  myfprintf(context, archiveFile, sections->Count64());

  RESCUEINT64 loop;
  for (loop = 0; loop < sections->Count64(); loop++) {
    RescueSection *group = sections->NthObject(loop);
    myfprintf(context, archiveFile, group->Identifier());
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueEventDescr::RescueEventDescr(RescueContext *context, FILE *archiveFile)
    : RescueLogicalOrderEntry(context), eventType(RescueEventDescr::UNCONFORMITY), sections(new cBagRescueSection()),
      sectionIds(0), horizonAboveID(0), horizonBelowID(0), eventDescrAboveID(0), eventDescrBelowID(0) {
  isA = R_RescueEventDescr;
  ReadId(context, archiveFile);
  if (context->ReadFileVersion() >= 25) {
    eventDescrName = new RCHString(context, archiveFile);
  } else {
    eventDescrName = new RCHString();
  }
  if (context->ReadFileVersion() >= 26 && context->ReadFileVersion() < 30) {
    RESCUEINT64 flag;
    myfscanf(context, archiveFile, &flag);
    if (flag != 0) {
      myfscanf(context, archiveFile, &flag); // eventAbove:identifier
      myfscanf(context, archiveFile, &flag); // eventAbove:horizon identifier
      if (flag != 0)
        horizonAboveID = flag;
      myfscanf(context, archiveFile, &flag); // eventAbove:unconformity identifier
      if (flag != 0)
        eventDescrAboveID = flag;
    }
    myfscanf(context, archiveFile, &flag);
    if (flag != 0) {
      myfscanf(context, archiveFile, &flag); // eventBelow:identifier
      myfscanf(context, archiveFile, &flag); // eventBelow:horizon identifier
      if (flag != 0)
        horizonBelowID = flag;
      myfscanf(context, archiveFile, &flag); // eventBelow:unconformity identifier
      if (flag != 0)
        eventDescrBelowID = flag;
    }
  }
  RESCUEINT64 loop, count;

  myfscanf(context, archiveFile, &count);
  for (loop = 0; loop < count; loop++) {
    RESCUEINT64 id;

    if (sectionIds == 0) {
      sectionIds = new cBagInt();
    }
    myfscanf(context, archiveFile, &id);
    (*sectionIds) += id;
  }
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueEventDescr::Relink(RescueObject *object) {
  RescueModel *parentModel = (RescueModel *)object;
  if (sectionIds != 0) {
    RESCUEINT64 loop;
    for (loop = 0; loop < sectionIds->Count64(); loop++) {
      (*sections) += parentModel->SectionIdentifiedBy(sectionIds->NthObject(loop));
    }
    delete sectionIds;
    sectionIds = 0;
  }
}

RESCUEBOOL RescueEventDescr::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueEventDescr) {
    return TRUE;
  } else {
    return RescueLogicalOrderEntry::IsOfType(thisType);
  }
}
