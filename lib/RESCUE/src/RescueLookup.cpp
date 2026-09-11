/********************************************************************

  RescueLookup.h

  A table of lookups for properties. Individual lookup items are
  accessed by their index in the array.

  Rod Hanks,  May 1997

*********************************************************************/
#include "RescueLookup.h"
#include "RescueBuffer.h"
#include "RescueLookupItem.h"
#include "RescueLookupString.h"
#include "RescueLookupTable.h"
#include "RescueModel.h"
#include <string.h>

RescueLookup::RescueLookup(const RESCUECHAR *newName, RESCUEINT64 size, RescueModel *newParentModel)
    : RescueHistoryObject(newParentModel->Context()), name(0), parentModel(newParentModel), count(size) {
  name = new RCHString(newName);
  isA = R_RescueLookup;
  (*parentModel->lookups) += this;
  lookups = (RescueLookupItem **)malloc(sizeof(RescueLookupItem *) * (size_t)count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    lookups[loop] = 0;
  }
}

RescueLookup::~RescueLookup() {
  if (name != 0) {
    delete name;
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    if (lookups[loop] != 0) {
      delete lookups[loop];
    }
  }
  free(lookups);
}

RescueLookupItem *RescueLookup::NthItem(RESCUEINT64 zeroBasedIndex) {
  if (zeroBasedIndex < 0 || zeroBasedIndex >= count) {
    return 0;
  } else {
    return lookups[zeroBasedIndex];
  }
}

void RescueLookup::SetNthItem(RESCUEINT64 zeroBasedIndex, RescueLookupItem *newItem) {
  if (zeroBasedIndex >= 0 && zeroBasedIndex < count) {
    if (lookups[zeroBasedIndex] != 0) {
      delete lookups[zeroBasedIndex];
    }
    lookups[zeroBasedIndex] = newItem;
  }
}

RESCUEBOOL RescueLookup::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueLookup) {
    return TRUE;
  } else {
    return RescueHistoryObject::IsOfType(thisType);
  }
}

void RescueLookup::Archive(FILE *archiveFile) {
  RescueContext *context = parentModel->Context();
  myfprintf(context, archiveFile, "; Lookup");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, name->String());
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    if (lookups[loop] == 0) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    } else {
      myfprintf(context, archiveFile, (RESCUEINT64)lookups[loop]->IsA());
      lookups[loop]->Archive(context, archiveFile);
    }
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueLookup::RescueLookup(RescueContext *context, FILE *archiveFile) : RescueHistoryObject(context), name(0) {
  RESCUECHAR myString[255];

  isA = R_RescueLookup;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  name = new RCHString(myString);
  myfscanf(context, archiveFile, &count);
  lookups = (RescueLookupItem **)malloc(sizeof(RescueLookupItem *) * (size_t)count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    RESCUEINT64 flag;

    myfscanf(context, archiveFile, &flag);
    if (flag == 0) {
      lookups[loop] = 0;
    } else if (flag == R_RescueLookupString) {
      lookups[loop] = new RescueLookupString(context, archiveFile);
    } else if (flag == R_RescueLookupTable) {
      lookups[loop] = new RescueLookupTable(context, archiveFile);
    }
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

void RescueLookup::Relink(RescueObject *parent) { parentModel = (RescueModel *)parent; }

RESCUEINT32 RescueLookup::Count(RESCUEBOOL throwIfTrue) {
  RESCUEINT64 output = Count64();
  if (output > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)output;
  }
}
