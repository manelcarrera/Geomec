/*************************************************************************

    cBagRescueTriangleVertex.h

 Keeps a list of pointers to RescueTriangleVertex.

    Rod Hanks               December 15th, 1995  / August 1996

****************************************************************************/
#include "cBagRescueTriangleVertex.h"
#include "RescueModel.h"
#include "RescueTriangleVertex.h"
#include "RescueTriangulatedSurface.h"
#include "myHeaders.h"
#include <stdio.h>

cBagRescueTriangleVertex::cBagRescueTriangleVertex() { tree = new RescueTree(); }

cBagRescueTriangleVertex::~cBagRescueTriangleVertex() { delete tree; }

void cBagRescueTriangleVertex::operator+=(RescueTriangleVertex *newObject) { tree->Add(newObject); }

RESCUEBOOL cBagRescueTriangleVertex::operator-=(RescueTriangleVertex *existingObject) {
  return tree->Delete(existingObject);
}

RescueTriangleVertex *cBagRescueTriangleVertex::NthObject(RESCUEINT64 ordinal) {
  return (RescueTriangleVertex *)tree->NthObject(ordinal);
}

void cBagRescueTriangleVertex::Archive(RescueContext *context, FILE *archiveFile) {
  RESCUEINT64 howMany = tree->Count();
  myfprintf(context, archiveFile, howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    RescueTriangleVertex *vertex = (RescueTriangleVertex *)tree->NthObject(loop);
    myfprintf(context, archiveFile, vertex->ndx);
  }
}

void cBagRescueTriangleVertex::UnArchive(RescueContext *context, FILE *archiveFile,
                                         RescueTriangulatedSurface *parentSurface) {
  RESCUEINT64 howMany;
  myfscanf(context, archiveFile, &howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    RESCUEINT64 ndx;
    myfscanf(context, archiveFile, &ndx);
    RescueTriangleVertex *vertex = parentSurface->NthVertex(ndx);
    (*this) += vertex;
  }
}

RESCUEINT32 cBagRescueTriangleVertex::Count(RESCUEBOOL throwIfTrue) {
  if (tree->Count() > 2147483647) {
    if (throwIfTrue) {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  } else {
    return (RESCUEINT32)tree->Count();
  }
}
