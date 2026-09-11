/*************************************************************************

    cSetRescuePoint.cpp

 Keeps a list of pointers to some RescuePoint.

    Rod Hanks               Oct 2003

****************************************************************************/
#include "cSetRescuePoint.h"
#include "RescueModel.h"
#include "RescuePoint.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

cSetRescuePoint::cSetRescuePoint() {
  allocated = 10;
  count = 0;
  objects = (RescuePoint **)malloc(sizeof(RescuePoint *) * (size_t)allocated);
}

cSetRescuePoint::~cSetRescuePoint() {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescuePoint::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescuePoint::UnArchive(RescueContext *context, FILE *archiveFile) {
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++) {
    RescuePoint *newObject = new RescuePoint(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescuePoint::EmptySelf(void) {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescuePoint::operator+=(RescuePoint *newObject) {
  if (allocated == count) {
    allocated += 10;
    objects = (RescuePoint **)realloc(objects, sizeof(RescuePoint *) * (size_t)allocated);
  }
  objects[count++] = newObject;
}

RESCUEINT64 cSetRescuePoint::IndexOf(RescuePoint *existingObject) {
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 ndx = 0;
  while (ndx < count && myReturn < 0) {
    if (existingObject == objects[ndx]) {
      myReturn = ndx;
    } else {
      ndx++;
    }
  }
  return myReturn;
}

RESCUEBOOL cSetRescuePoint::operator-=(RescuePoint *existingObject) {
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

RESCUEBOOL cSetRescuePoint::operator-=(RESCUEINT64 ndx) {
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

RescuePoint *cSetRescuePoint::NthObject(RESCUEINT64 ordinal) {
  if (ordinal < 0 || ordinal >= count) {
    return 0;
  } else {
    return objects[ordinal];
  }
}

RESCUEINT64 cSetRescuePoint::Count64(void) { return count; }

RESCUEINT32 cSetRescuePoint::Count(void) { return (RESCUEINT32)count; }

RESCUEINT32 cSetRescuePoint::Count(RESCUEBOOL throwIfTrue) {
  if (count > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)count;
  }
}
