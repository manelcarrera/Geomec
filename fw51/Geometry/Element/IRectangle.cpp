/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IRectangle.cpp: implementation of the IRectangle class.
//
//////////////////////////////////////////////////////////////////////

#include "IRectangle.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {
IRectangle::IRectangle() {}

int IRectangle::NrOfPoints() const { return 4; }

int IRectangle::GetOpositeIndex(const geo::CPoint &p) {
  for (int i = 0; i < NrOfPoints(); i++) {
    if (p == Point(i)) {
      return (i + 2) % NrOfPoints();
    }
  }

  return -1;
}

IElement::TDoubleVec IRectangle::WorldToIso(const geo::IPoint &point) const {
  TDoubleVec vcRet;
  assert(Contains(point, true));

  CVector vecDir = point - Refpoint();
  if (vecDir.isNullVector()) {
    vcRet.push_back(-1);
    vcRet.push_back(-1);
    return vcRet;
  }

  vcRet.push_back(2 * vecDir.DotProduct(Direction1()) / (Direction1().Length() * Direction1().Length()) - 1);
  vcRet.push_back(2 * vecDir.DotProduct(Direction2()) / (Direction2().Length() * Direction2().Length()) - 1);

  assert(vcRet.size() == 2);
  assert(vcRet[0] >= -1.0 && vcRet[0] <= 1.0);
  assert(vcRet[1] >= -1.0 && vcRet[1] <= 1.0);

  return vcRet;
}

} // namespace geo
