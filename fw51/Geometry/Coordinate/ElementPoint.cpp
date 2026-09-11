 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ElementPoint.h"

namespace geo {

CElementPoint::CElementPoint()
{
}

CElementPoint::CElementPoint(const IElement& element)
{
  m_stElements.insert(&element);
}

CElementPoint::CElementPoint(const std::set<const IElement*>& stElements)
: m_stElements(stElements)
{
}

CElementPoint::CElementPoint(const IElement& element,
                             const double &dX,
                             const double &dY,
                             const double &dZ)
: CPoint(dX, dY, dZ)
{
  m_stElements.insert(&element);
}

CElementPoint::CElementPoint(const std::set<const IElement*>& stElements,
                             const double& dX,
                             const double& dY,
                             const double& dZ)
: CPoint(dX, dY, dZ),
  m_stElements(stElements)
{
}

CElementPoint::CElementPoint(const IPoint& pt)
: CPoint(pt)
{
}

CElementPoint::CElementPoint(const IElement& element,
                             const IPoint& pt)
: CPoint(pt)
{
  m_stElements.insert(&element);
}

CElementPoint::CElementPoint(const std::set<const IElement*>& stElements,
                             const IPoint& pt)
: CPoint(pt),
  m_stElements(stElements)
{
}

CElementPoint::CElementPoint(const CElementPoint &pt)
: CPoint(pt),
  m_stElements(pt.m_stElements)
{
}

CElementPoint& CElementPoint::operator=(const CElementPoint& rhs)
{
  CPoint::operator=(rhs);
  m_stElements = rhs.m_stElements;

  return *this;
}

const std::set<const IElement*>& CElementPoint::Elements() const
{
  return m_stElements;
}

bool CElementPoint::AddElement(const IElement& element)
{
  return m_stElements.insert(&element).second;
}

bool CElementPoint::RemoveElement(const IElement& element)
{
  std::set<const IElement*>::iterator it = m_stElements.find(&element);
  if(it != m_stElements.end())
  {
    m_stElements.erase(it);
    return true;
  }

  return false;
}

} // namespace geo
