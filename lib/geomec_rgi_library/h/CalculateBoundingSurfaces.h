#ifndef _CalculateBoundingSurfaces_h_
#define _CalculateBoundingSurfaces_h_

#include "SharedBodyTriangle.h"

namespace GeomecRGI
{

class CCalculateBoundingSurfaces
{
  public:
  CCalculateBoundingSurfaces(geo::IElementSet& elementSet);

  const TSharedBodyTriangleMap& getSharedBodyTriangleMap() const;

  protected:
  TSharedBodyTriangleMap m_SharedBodyTriangleMap;

  private:
  CCalculateBoundingSurfaces(const CCalculateBoundingSurfaces& rhs);
  CCalculateBoundingSurfaces& operator = (
      const CCalculateBoundingSurfaces& rhs);
};

} // namespace GeomecRGI

#endif  // _CalculateBoundingSurfaces_h_
