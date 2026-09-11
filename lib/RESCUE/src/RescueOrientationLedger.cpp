/********************************************************************

  RescueOrientationLedger.cpp

  Private use of Rescue during SetOrientation method.

  Rod Hanks,  June 2006

*********************************************************************/
#include "RescueOrientationLedger.h"
#include "RescueModel.h"

RescueOrientationLedger::RescueOrientationLedger() {
  kList = 0;
  ijList = 0;
}

bool RescueOrientationLedger::GetIJFor(RescueIJSurface *face, RESCUEINT64 &iLowBound, RESCUEINT64 &iCount,
                                       RESCUEINT64 &jLowBound, RESCUEINT64 &jCount, bool &swapI, bool &swapJ) {
  SurfaceIJ *candidate = ijList;
  bool myReturn = false;
  while (candidate != 0 && myReturn == false) {
    if (candidate->face == face) {
      iLowBound = candidate->iLowBound;
      iCount = candidate->iCount;
      jLowBound = candidate->jLowBound;
      jCount = candidate->jCount;
      swapI = candidate->swapI;
      swapJ = candidate->swapJ;
      myReturn = true;
    } else {
      candidate = candidate->next;
    }
  }
  return myReturn;
}

void RescueOrientationLedger::SetIJFor(RescueIJSurface *face, RESCUEINT64 iLowBound, RESCUEINT64 iCount,
                                       RESCUEINT64 jLowBound, RESCUEINT64 jCount, bool swapI, bool swapJ) {
  SurfaceIJ *newIJ = new SurfaceIJ(face, iLowBound, iCount, jLowBound, jCount, swapI, swapJ);
  if (ijList == 0) {
    ijList = newIJ;
  } else {
    SurfaceIJ *ijParent = ijList;
    while (ijParent->next != 0) {
      ijParent = ijParent->next;
    }
    ijParent->next = newIJ;
  }
}

bool RescueOrientationLedger::GetKFor(RescueGeometry *geom, RESCUEINT64 &kLowBound, RESCUEINT64 &kCount, bool &swapK) {
  GeomK *candidate = kList;
  bool myReturn = false;
  while (candidate != 0 && myReturn == false) {
    if (candidate->geom == geom) {
      kLowBound = candidate->kLowBound;
      kCount = candidate->kCount;
      swapK = candidate->swapK;
    } else {
      candidate = candidate->next;
    }
  }
  return myReturn;
}

void RescueOrientationLedger::SetKFor(RescueGeometry *geom, RESCUEINT64 kLowBound, RESCUEINT64 kCount, bool swapK) {
  GeomK *newK = new GeomK(geom, kLowBound, kCount, swapK);
  if (kList == 0) {
    kList = newK;
  } else {
    GeomK *newParent = kList;
    while (newParent->next != 0) {
      newParent = newParent->next;
    }
    newParent->next = newK;
  }
}

RescueOrientationLedger::~RescueOrientationLedger() {
  if (kList != 0) {
    delete kList;
  }
  if (ijList != 0) {
    delete ijList;
  }
}

RescueOrientationLedger::GeomK::GeomK(RescueGeometry *geomIn, RESCUEINT64 kLowBoundIn, RESCUEINT64 kCountIn,
                                      bool swapKIn) {
  geom = geomIn;
  kLowBound = kLowBoundIn;
  kCount = kCountIn;
  swapK = swapKIn;
  next = 0;
}

RescueOrientationLedger::GeomK::~GeomK() {
  if (next != 0) {
    delete next;
  }
}

RescueOrientationLedger::SurfaceIJ::SurfaceIJ(RescueIJSurface *faceIn, RESCUEINT64 iLowBoundIn, RESCUEINT64 iCountIn,
                                              RESCUEINT64 jLowBoundIn, RESCUEINT64 jCountIn, bool swapIIn,
                                              bool swapJIn) {
  face = faceIn;
  iLowBound = iLowBoundIn;
  iCount = iCountIn;
  jLowBound = jLowBoundIn;
  jCount = jCountIn;
  swapI = swapIIn;
  swapJ = swapJIn;
  next = 0;
}

RescueOrientationLedger::SurfaceIJ::~SurfaceIJ() {
  if (next != 0) {
    delete next;
  }
}
