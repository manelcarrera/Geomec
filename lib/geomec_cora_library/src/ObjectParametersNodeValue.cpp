
#include "ObjectParametersNodeValue.h"
#include "Triangle.h"

namespace cora
{

// full specializations

template <>
  double CObjectParametersNodeValue <geo::CElementGroup,
  CFaultParametersNode>::getArea(const geo::IElement& element) const
{
  const geo::CInterfaceElement& interfaceElement =
  dynamic_cast <const geo::CInterfaceElement&> (element);
  const geo::IFace* frontFace = interfaceElement.FrontFace();
  const geo::IFace* backFace = interfaceElement.BackFace();

  if ((frontFace != 0) && (backFace != 0))
  {
  assert(abs(frontFace->Area() - backFace->Area()) < EPS);
  }

  if (frontFace != 0)
  {
  return frontFace->Area();
  }
  else if (backFace != 0)
  {
  return backFace->Area();
  }
  else
  {
  assert(false);
  }

  return 0;
}

template <>
  double CObjectParametersNodeValue <geo::IElementSet,
  CNonMeshedSurfaceParametersNode>::getArea(const geo::IElement& element)
      const
{
  const geo::CTriangle& triangle =
  dynamic_cast <const geo::CTriangle&> (element);

  return triangle.Area();
}

} // namespace cora
