/*************************************************************************

    cSetRescuePolyLineNodeUVT.cpp

 Keeps a list of pointers to some RescuePolyLineNodeUVT.

    Rod Hanks               May 2002

****************************************************************************/
#include "cSetRescuePolyLineNodeUVT.h"
#include "RescueModel.h"
#include "RescuePolyLineNodeUVT.h"

cSetRescuePolyLineNodeUVT::cSetRescuePolyLineNodeUVT() {
  allocated = 10;
  count = 0;
  objects = (RescuePolyLineNodeUVT **)malloc(sizeof(RescuePolyLineNodeUVT *) * (size_t)allocated);
}

cSetRescuePolyLineNodeUVT::~cSetRescuePolyLineNodeUVT() {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescuePolyLineNodeUVT::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescuePolyLineNodeUVT::Relink(RescueObject *parent) {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Relink(parent);
  }
}

void cSetRescuePolyLineNodeUVT::UnArchive(RescueContext *context, FILE *archiveFile) {
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++) {
    RescuePolyLineNodeUVT *newObject = new RescuePolyLineNodeUVT(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescuePolyLineNodeUVT::EmptySelf(void) {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    if (objects[loop] != 0) {
      delete objects[loop];
    }
  }
  count = 0;
}

void cSetRescuePolyLineNodeUVT::InsertAt(RESCUEINT64 ndx, RescuePolyLineNodeUVT *newObject) {
  if (ndx >= allocated) {
    allocated = ndx + 5;
    objects = (RescuePolyLineNodeUVT **)realloc(objects, sizeof(RescuePolyLineNodeUVT *) * (size_t)allocated);
  }
  RESCUEINT64 loop;
  for (loop = count; loop < ndx; loop++) {
    objects[ndx] = 0;
  }
  objects[ndx] = newObject;
  if (count <= ndx) {
    count = ndx + 1;
  }
}
/*
  This one is required for the Oracle archive/unarchive methods.
  It assumes we will eventually fill in all the places.
*/
void cSetRescuePolyLineNodeUVT::operator+=(RescuePolyLineNodeUVT *newObject) {
  if (allocated == count) {
    allocated += 10;
    objects = (RescuePolyLineNodeUVT **)realloc(objects, sizeof(RescuePolyLineNodeUVT *) * (size_t)allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescuePolyLineNodeUVT::operator-=(RescuePolyLineNodeUVT *existingObject) {
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

RescuePolyLineNodeUVT *cSetRescuePolyLineNodeUVT::ObjectNamed(const RESCUECHAR *mayBeName) {
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

RescuePolyLineNodeUVT *cSetRescuePolyLineNodeUVT::ObjectIdentifiedBy(RESCUEINT64 identifier) {
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

RESCUEBOOL cSetRescuePolyLineNodeUVT::operator-=(RESCUEINT64 ndx) {
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

RescuePolyLineNodeUVT *cSetRescuePolyLineNodeUVT::NthObject(RESCUEINT64 ordinal) {
  if (ordinal < 0 || ordinal >= count) {
    return 0;
  } else {
    return objects[ordinal];
  }
}

RESCUEINT64 cSetRescuePolyLineNodeUVT::Count64(void) { return count; }

RESCUEINT32 cSetRescuePolyLineNodeUVT::Count(void) { return (RESCUEINT32)count; }

RESCUEINT32 cSetRescuePolyLineNodeUVT::Count(RESCUEBOOL throwIfTrue) {
  if (count > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)count;
  }
}
