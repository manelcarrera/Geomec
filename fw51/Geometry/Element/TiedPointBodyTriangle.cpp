#include "TiedPointBodyTriangle.h"

namespace geo
{

CTiedPointBodyTriangle::CTiedPointBodyTriangle(IBody &parent, int nIndex)
  : CBodyTriangle(parent, nIndex)
  , m_vcPoints(3)
  , m_vcElements(3, nullptr)
{
}

void CTiedPointBodyTriangle::AddTying(int nIndex, const geo::IInterfaceElement& elt, const geo::CPoint& point, int nOffset)
{
  if (!m_vcElements[nIndex])
  {
  m_vcPoints[nIndex].Set(point.X(), point.Y(), point.Z());
  m_vcElements[nIndex] = &elt;
  m_nOffset = nOffset;
  }
}

const geo::IInterfaceElement *CTiedPointBodyTriangle::GetTying(int nIndex, geo::CPoint& point, int& nOffset) const
{
  point.Set(m_vcPoints[nIndex].X(), m_vcPoints[nIndex].Y(), m_vcPoints[nIndex].Z());
  nOffset = m_nOffset;
  return m_vcElements[nIndex];
}

}
