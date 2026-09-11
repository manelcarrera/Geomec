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

    cSetRescueProperty.cpp

 Keeps a list of pointers to some RescueProperty.

    Rod Hanks               January 18th, 1995  /  August 1996

****************************************************************************/
#include "cSetRescueProperty.h"
#include "RescueModel.h"
#include "RescueProperty.h"

cSetRescueProperty::cSetRescueProperty() {
  allocated = 10;
  count = 0;
  objects = (RescueProperty **)malloc(sizeof(RescueProperty *) * (size_t)allocated);
}

cSetRescueProperty::~cSetRescueProperty() {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  free(objects);
}

void cSetRescueProperty::RelinquishAll() { count = 0; }

void cSetRescueProperty::Relinquish(RescueProperty *existingObject) {
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

    count--;
    for (loop = ndx; loop < count; loop++) {
      objects[loop] = objects[loop + 1];
    }
  }
}

RESCUEBOOL cSetRescueProperty::AnyFileTruncated() {
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == FALSE; loop++) {
    myReturn = objects[loop]->AnyFileTruncated();
  }
  return myReturn;
}

void cSetRescueProperty::DropWireframeMemory() {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->DropWireframeMemory();
  }
}

void cSetRescueProperty::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile) {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->UnArchiveWireframeData(model, archiveFile);
  }
}

void cSetRescueProperty::RelinkWireframeData(RescueObject *parent) {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->RelinkWireframeData(parent);
  }
}

void cSetRescueProperty::ArchiveWireframeData(FILE *archiveFile) {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->ArchiveWireframeData(archiveFile);
  }
}

void cSetRescueProperty::Archive(RescueContext *context, FILE *archiveFile) {
  RESCUEINT64 countToArchive = count;
  if (context->FileVersion() == 9) {
    countToArchive = 0;
    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++) {
      if (objects[loop]->parentBlockUnit != 0) {
        if (objects[loop]->geometry == objects[loop]->parentBlockUnit->GridGeometry(0)) {
          countToArchive++;
        }
      }
    }
  } else if (context->FileVersion() == 19) {
    countToArchive = 0;
    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++) {
      if (objects[loop]->Data()->IsA() == R_RescueArray2dVector ||
          objects[loop]->Data()->IsA() == R_RescueArray3dVector) {
      } else {
        countToArchive++;
      }
    }
  }
  myfprintf(context, archiveFile, countToArchive);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueProperty::Relink(RescueObject *parent) {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Relink(parent);
  }
}

void cSetRescueProperty::UnArchive(RescueContext *context, FILE *archiveFile) {
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(context, archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++) {
    RescueProperty *newObject = new RescueProperty(context, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueProperty::EmptySelf(void) {
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++) {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueProperty::operator+=(RescueProperty *newObject) {
  if (allocated == count) {
    allocated += 10;
    objects = (RescueProperty **)realloc(objects, sizeof(RescueProperty *) * (size_t)allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueProperty::operator-=(RescueProperty *existingObject) {
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

RescueProperty *cSetRescueProperty::ObjectNamed(const RESCUECHAR *mayBeName) {
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

void cSetRescueProperty::Dispose() {
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++) {
    objects[loop]->Dispose();
  }
}

RescueProperty *cSetRescueProperty::ObjectIdentifiedBy(RESCUEINT64 identifier) {
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

RESCUEBOOL cSetRescueProperty::operator-=(RESCUEINT64 ndx) {
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

RescueProperty *cSetRescueProperty::NthObject(RESCUEINT64 ordinal) {
  if (ordinal < 0 || ordinal >= count) {
    return 0;
  } else {
    return objects[ordinal];
  }
}

RESCUEINT32 cSetRescueProperty::Count(void) { return (RESCUEINT32)count; }

RESCUEINT64 cSetRescueProperty::Count64(void) { return count; }

void cSetRescueProperty::FindUniquePropertyNames(cSetString *container) {
  int loop;
  for (loop = 0; loop < count; loop++) {
    RCHString *propertyName = objects[loop]->Data()->PropertyName();
    container->AddIfUnique(propertyName->String());
  }
}

RESCUEINT32 cSetRescueProperty::Count(RESCUEBOOL throwIfTrue) {
  if (count > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)count;
  }
}
