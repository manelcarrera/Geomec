#pragma once


#include "GeometryExports.h"

#include "BodyTriangle.h"
#include "IInterfaceElement.h"


namespace geo
{


class GEOMETRY_EXPORT CTiedPointBodyTriangle : public CBodyTriangle
{
  std::vector<geo::CPoint> m_vcPoints;
  std::vector<const geo::IInterfaceElement *> m_vcElements;

  int m_nOffset;

public:
  CTiedPointBodyTriangle(IBody &parent, int nIndex);

  void AddTying(int nIndex, const geo::IInterfaceElement& element, const geo::CPoint& point, int nOffset);

  const geo::IInterfaceElement *GetTying(int nIndex, geo::CPoint& point, int& nOffset) const;
};

}
