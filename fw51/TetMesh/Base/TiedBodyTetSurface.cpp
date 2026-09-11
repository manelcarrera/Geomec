#include "TiedBodyTetSurface.h"

#include "TiedPointBodyTriangle.h"

namespace geo
{

CTiedBodyTetSurface::CTiedBodyTetSurface(CSurfaceDesc& desc, CBodyGroup& body_group)
  : CTetSurface(desc, body_group)
{
}

CTiedBodyTetSurface::~CTiedBodyTetSurface()
{
  for (int i = 0; i < FaceSize(); ++i)
    delete &Face(i);
}

}
