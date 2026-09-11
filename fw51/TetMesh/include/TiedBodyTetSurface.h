#pragma once

#include "GeometryExports.h"

#include "TetSurface.h"

namespace geo {

class CSurfaceDesc;
class CBodyGroup;

class CTiedBodyTetSurface : public CTetSurface {
public:
  CTiedBodyTetSurface(CSurfaceDesc &desc, CBodyGroup &body_group);

  virtual ~CTiedBodyTetSurface();

  virtual bool isTiedSurface() const { return true; }
};

} // namespace geo
