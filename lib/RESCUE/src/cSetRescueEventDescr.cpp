/****************************************************************************

Copyright 1995 Petrotechnical Open Software Corporation

POSC grants permission to copy or reproduce this material in its original
form for internal use only.

This software is subject to the provision of the "POSC Software License
Agreement" which states in part:

1) Licensee accepts a non-exclusive, non-transferable, license  to use,
display, modify and distribute works derived from the licensed documentation
and Software Product.

2) Licensee shall have no right to distribute in its original form any
Software Product or documentation licensed under this agreement.

****************************************************************************/
/*************************************************************************

    cSetRescueEventDescr.cpp

 Keeps a list of pointers to some RescueEventDescr.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "cSetRescueEventDescr.h"
#include "RescueEventDescr.h"
#include "RescueModel.h"

cSetRescueEventDescr::cSetRescueEventDescr() {
  allocated = 10;
  count = 0;
  objects = (RescueEventDescr **)malloc(sizeof(RescueEventDescr *) * (size_t)allocated);
}

cSetRescueEventDescr::~cSetRescueEventDescr() {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueEventDescr::Archive(RescueModel *model, RescueContext *context, FILE *archiveFile) {
  RESCUEINT64 archiveCount = count;
  if (context->FileVersion() < 26) {
    archiveCount = 0;

    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++) {
      if (objects[loop]->FullEvents(model)) {
        archiveCount++;
      }
    }
  }
  myfprintf(context, archiveFile, archiveCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    RESCUEBOOL okay = TRUE;
    if (context->FileVersion() < 26) {
      if (objects[loop]->FullEvents(model) == FALSE) {
        okay = FALSE;
      }
    }
    if (okay == TRUE) {
      objects[loop]->Archive(model, context, archiveFile);
    }
  }
}

void cSetRescueEventDescr::Relink(RescueObject *parent) {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Relink(parent);
  }
}

void cSetRescueEventDescr::UnArchive(RescueContext *context, FILE *archiveFile) {
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++) {
    RescueEventDescr *newObject = new RescueEventDescr(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueEventDescr::EmptySelf(void) {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueEventDescr::operator+=(RescueEventDescr *newObject) {
  if (allocated == count) {
    allocated += 10;
    objects = (RescueEventDescr **)realloc(objects, sizeof(RescueEventDescr *) * (size_t)allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueEventDescr::operator-=(RescueEventDescr *existingObject) {
  RESCUEBOOL found = FALSE;
  RESCUEINT64 ndx = 0;

  while (ndx < count && found == FALSE) {
    if (existingObject == objects[ndx]) {
      found = TRUE;
    } else {
      ndx++;
    }
  }
  if (found) {
    RESCUEINT64 loop;

    delete objects[ndx];
    count--;
    for (loop = ndx; loop < count; loop++) {
      objects[loop] = objects[loop + 1];
    }
  }
  return found;
}

RescueEventDescr *cSetRescueEventDescr::ObjectNamed(const RESCUECHAR *mayBeName) {
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE) {
    if (objects[ndx]->IsNamed(mayBeName)) {
      found = TRUE;
    } else {
      ndx++;
    }
  }
  if (found) {
    return objects[ndx];
  } else {
    return 0;
  }
}

RescueEventDescr *cSetRescueEventDescr::ObjectIdentifiedBy(RESCUEINT64 identifier) {
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE) {
    if (objects[ndx]->IsIdentifiedBy(identifier)) {
      found = TRUE;
    } else {
      ndx++;
    }
  }
  if (found) {
    return objects[ndx];
  } else {
    return 0;
  }
}

RESCUEBOOL cSetRescueEventDescr::operator-=(RESCUEINT64 ndx) {
  if (ndx >= 0 && ndx < count) {
    RESCUEINT64 loop;

    delete objects[ndx];
    count--;
    for (loop = ndx; loop < count; loop++) {
      objects[loop] = objects[loop + 1];
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

RescueEventDescr *cSetRescueEventDescr::NthObject(RESCUEINT64 ordinal) {
  if (ordinal < 0 || ordinal >= count) {
    return 0;
  } else {
    return objects[ordinal];
  }
}

RESCUEINT64 cSetRescueEventDescr::Count64(void) { return count; }

RESCUEINT32 cSetRescueEventDescr::Count(void) { return (RESCUEINT32)count; }

RESCUEINT32 cSetRescueEventDescr::Count(RESCUEBOOL throwIfTrue) {
  if (count > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)count;
  }
}
