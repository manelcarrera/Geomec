/*************************************************************************

    cSetRescueSOctTreeNode.cpp

 Keeps a list of pointers to some RescueSOctTreeNode.

    Rod Hanks               January 2000

****************************************************************************/
#include "cSetRescueSOctTreeNode.h"
#include "RescueModel.h"
#include "RescueSOctTreeNode.h"

cSetRescueSOctTreeNode::cSetRescueSOctTreeNode() {
  allocated = 10;
  count = 0;
  objects = (RescueSOctTreeNode **)malloc(sizeof(RescueSOctTreeNode *) * (size_t)allocated);
}

cSetRescueSOctTreeNode::~cSetRescueSOctTreeNode() {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueSOctTreeNode::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Archive(context, archiveFile);
  }
}

void cSetRescueSOctTreeNode::UnArchive(RescueContext *context, FILE *archiveFile, RescueTriangulatedSurface *surface) {
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++) {
    RescueSOctTreeNode *newObject = new RescueSOctTreeNode(archiveFile, surface);
    (*this) += newObject;
  }
}

void cSetRescueSOctTreeNode::EmptySelf(void) {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueSOctTreeNode::operator+=(RescueSOctTreeNode *newObject) {
  if (allocated == count) {
    allocated += 10;
    objects = (RescueSOctTreeNode **)realloc(objects, sizeof(RescueSOctTreeNode *) * (size_t)allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueSOctTreeNode::operator-=(RescueSOctTreeNode *existingObject) {
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

RESCUEBOOL cSetRescueSOctTreeNode::operator-=(RESCUEINT64 ndx) {
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

RescueSOctTreeNode *cSetRescueSOctTreeNode::NthObject(RESCUEINT64 ordinal) {
  if (ordinal < 0 || ordinal >= count) {
    return 0;
  } else {
    return objects[ordinal];
  }
}

RESCUEINT64 cSetRescueSOctTreeNode::Count64(void) { return count; }

RESCUEINT32 cSetRescueSOctTreeNode::Count(void) { return (RESCUEINT32)count; }

RESCUEINT32 cSetRescueSOctTreeNode::Count(RESCUEBOOL throwIfTrue) {
  if (count > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)count;
  }
}
