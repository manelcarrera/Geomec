/*************************************************************************

    cSetRescueTriangleFace.cpp

 Keeps a list of pointers to some RescueTriangleFace.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "cSetRescueTriangleFace.h"
#include "RescueModel.h"
#include "RescueTriangleFace.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

cSetRescueTriangleFace::cSetRescueTriangleFace() {
  allocated = 10;
  count = 0;
  objects = (RescueTriangleFace **)malloc(sizeof(RescueTriangleFace *) * (size_t)allocated);
}

cSetRescueTriangleFace::~cSetRescueTriangleFace() {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueTriangleFace::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescueTriangleFace::Relink(RescueObject *parent) {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Relink(parent);
  }
}

void cSetRescueTriangleFace::UnArchive(RescueContext *context, FILE *archiveFile) {
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++) {
    RescueTriangleFace *newObject = new RescueTriangleFace(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueTriangleFace::EmptySelf(void) {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueTriangleFace::operator+=(RescueTriangleFace *newObject) {
  if (allocated == count) {
    allocated += 10;
    objects = (RescueTriangleFace **)realloc(objects, sizeof(RescueTriangleFace *) * (size_t)allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueTriangleFace::operator-=(RescueTriangleFace *existingObject) {
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

RESCUEBOOL cSetRescueTriangleFace::operator-=(RESCUEINT64 ndx) {
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

RescueTriangleFace *cSetRescueTriangleFace::NthObject(RESCUEINT64 ordinal) {
  if (ordinal < 0 || ordinal >= count) {
    return 0;
  } else {
    return objects[ordinal];
  }
}

RESCUEINT64 cSetRescueTriangleFace::Count64(void) { return count; }

RESCUEINT32 cSetRescueTriangleFace::Count(void) { return (RESCUEINT32)count; }

RESCUEINT64 cSetRescueTriangleFace::IndexOf(RescueTriangleFace *existingObject) {
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

RESCUEINT32 cSetRescueTriangleFace::IndexOf(RescueTriangleFace *existingObject, RESCUEBOOL throwIfTrue) {
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 ndx = 0;
  while (ndx < count && myReturn < 0) {
    if (existingObject == objects[ndx]) {
      myReturn = ndx;
    } else {
      ndx++;
    }
  }
  if (myReturn > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)myReturn;
  }
}

RESCUEINT32 cSetRescueTriangleFace::Count(RESCUEBOOL throwIfTrue) {
  if (count > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)count;
  }
}
