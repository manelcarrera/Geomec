#include "StdAfx.h" 
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "HexaInterfaceElement.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Implementation of the hexainterface element set
//////////////////////////////////////////////////////////////////////
geo::IPoint& CHexaInterfaceElementSet::PointAt(int nIndex)
{
  geo::IPoint* pBogus = 0;
  assert(false);
  return *pBogus;
}

CHexaInterfaceElementSet::CHexaInterfaceElementSet()
{
}

void CHexaInterfaceElementSet::AssertValid() const
{
  for(int i = 0 ; i < m_vcElement.size() ; i++)
  {
    assert(	m_vcElement[i]->IndexingElementSet() == this );
  }
}

int CHexaInterfaceElementSet::RegisterElement(CHexaInterfaceElement& element)
{
  int ret = m_vcElement.size();
  m_vcElement.push_back(&element);
  return ret;
}

int CHexaInterfaceElementSet::RegisterPoint(geo::IPoint& point)
{
  TPointMap::iterator it = m_mpPoint.find(&point);
  if(it == m_mpPoint.end())
  {
    // Register the new point
    int pos = m_vcPoint.size();
    m_mpPoint.insert(TPointMap::value_type(&point, pos));
    m_vcPoint.push_back(&point);
    return pos;
  }

  return it->second;
}

void CHexaInterfaceElementSet::Clear()
{
  // Destroy elements
  for(int i = 0; i < m_vcElement.size(); i++)
    delete m_vcElement[i];

  m_vcElement.clear();
  m_mpPoint.clear();
  m_vcPoint.clear();
}

const geo::IPoint &CHexaInterfaceElementSet::Point(int nIndex) const
{
  return *m_vcPoint[nIndex];
}

int CHexaInterfaceElementSet::PointSize() const
{
  return m_vcPoint.size();
}

const geo::IElement &CHexaInterfaceElementSet::Element(int nIndex) const
{
  return *m_vcElement[nIndex];
}

int CHexaInterfaceElementSet::ElementSize() const 
{
  return m_vcElement.size();
}

std::vector<int> CHexaInterfaceElementSet::ElementsAt(const geo::IPoint &point) const
{
  assert(false);
  return std::vector<int>();
}

  // get the node indices connected to the given element
std::vector<int> CHexaInterfaceElementSet::Nodes(const geo::IElement &element) const
{
  assert(false);
  return std::vector<int>();
}


//////////////////////////////////////////////////////////////////////
// Implementation of the hexainterface element
//////////////////////////////////////////////////////////////////////

CHexaInterfaceElement::CHexaInterfaceElement(CHexaInterfaceElementSet& element_set,
                       const std::vector<geo::IPoint*>& points,
                       const geo::CHexahedron &upperBody, 
                       const geo::CHexahedron &lowerBody)
: m_element_set(element_set), m_pUpperBody(&upperBody), m_pLowerBody(&lowerBody)
{
  assert(points.size() == 8);
  assert(&upperBody != 0);
  assert(&lowerBody != 0);

  for(int i = 0; i < points.size(); i++)
    m_points[i] = m_element_set.RegisterPoint(*points[i]);

  m_nIndex = m_element_set.RegisterElement(*this);
}

CHexaInterfaceElement::CHexaInterfaceElement(CHexaInterfaceElementSet& element_set,
                       geo::IPoint &p0, 
                       geo::IPoint &p1, 
                       geo::IPoint &p2, 
                       geo::IPoint &p3,
                       geo::IPoint &p4, 
                       geo::IPoint &p5, 
                       geo::IPoint &p6, 
                       geo::IPoint &p7,
                       const geo::CHexahedron &upperBody,
                       const geo::CHexahedron &lowerBody)
: m_element_set(element_set), m_pUpperBody(&upperBody), m_pLowerBody(&lowerBody)
{
  assert(&upperBody != 0);
  assert(&lowerBody != 0);


  m_points[0] = m_element_set.RegisterPoint(p0);
  m_points[1] = m_element_set.RegisterPoint(p1);
  m_points[2] = m_element_set.RegisterPoint(p2);
  m_points[3] = m_element_set.RegisterPoint(p3);
  m_points[4] = m_element_set.RegisterPoint(p4);
  m_points[5] = m_element_set.RegisterPoint(p5);
  m_points[6] = m_element_set.RegisterPoint(p6);
  m_points[7] = m_element_set.RegisterPoint(p7);

  m_nIndex = m_element_set.RegisterElement(*this);

}

CHexaInterfaceElement::~CHexaInterfaceElement()
{
  ClearFaces();
}

  // Indexing system
const geo::IElementSet* CHexaInterfaceElement::IndexingElementSet() const
{
  return &m_element_set;
}

int CHexaInterfaceElement::Index() const
{
  return m_nIndex;
}

int CHexaInterfaceElement::PointIndex(int nIndex) const
{
  return m_points[nIndex];
}

//=============================================================================
// Override virtual functions from geo::IBody

const geo::IPoint &CHexaInterfaceElement::Point(const int nIndex) const
{
  return m_element_set.Point(m_points[nIndex]);
}
  
void CHexaInterfaceElement::Point(const int nIndex, const geo::IPoint &pt)
{
  assert(false);	// Not implemented yet!
}
  
int CHexaInterfaceElement::NrOfPoints() const
{
  return 8;
}

const geo::IBodyFace &CHexaInterfaceElement::Face(const int nIndex) const
{
  if(m_vcFaces.size() == 0)
    CreateFaces();

  return *m_vcFaces[nIndex];
}

int CHexaInterfaceElement::NrOfFaces() const
{
  if(m_vcFaces.size() == 0)
    CreateFaces();

  return m_vcFaces.size();
}

void CHexaInterfaceElement::CreateFaces() const
{
  assert(m_vcFaces.size() == 0);	// Must be empty.

  // Create 2 faces here from the point vector ( Take Point(int) )
  m_vcFaces.push_back(new geo::CQuadrilateral(*this, 0, 3, 2, 1, 0));
  m_vcFaces.push_back(new geo::CQuadrilateral(*this, 7, 4, 5, 6, 0));
}

void CHexaInterfaceElement::ClearFaces()
{
  for(int i = 0; i < m_vcFaces.size(); i++)
    delete m_vcFaces[i];
  m_vcFaces.clear();
}



// An interfaceelement doesn't have a volume because it's completely flat!
double CHexaInterfaceElement::Volume() const
{
  return 0;
}

bool CHexaInterfaceElement::Contains(const geo::IPoint &point, bool bIncludeEdge) const
{
  // NOTE : bIncludeEdge IS NOT IMPLEMENTED YET!!!
  assert(!bIncludeEdge);

  bool ret = false;

  double xi, eta;
  double dZtop, dZbot;

  // Calculate local coordinates of requested point (xi-eta coordinate system).
  xi = ((point.X() - Point(0).X()) / (Point(1).X() - Point(0).X())) - 0.5;
  eta = ((point.Y() - Point(0).Y()) / (Point(3).Y() - Point(0).Y())) - 0.5;

  // Check is point is within the horizontal boundaries of the element.
  if ((xi+EPS >= -0.5) && (xi-EPS <= 0.5) && (eta+EPS >= -0.5) && (eta-EPS <= 0.5))
  {
    // Apply shape functions to get global z-top and z-bottom coordinates.
    dZtop = Point(0).Z() * (0.5 - xi) * (0.5 - eta) + Point(1).Z() * (0.5 + xi) * (0.5 - eta) +
        Point(2).Z() * (0.5 + xi) * (0.5 + eta) + Point(3).Z() * (0.5 - xi) * (0.5 + eta);
    dZbot = Point(4).Z() * (0.5 - xi) * (0.5 - eta) + Point(5).Z() * (0.5 + xi) * (0.5 - eta) +
        Point(6).Z() * (0.5 + xi) * (0.5 + eta) + Point(7).Z() * (0.5 - xi) * (0.5 + eta);

    // Point lies inside element if ztop <= z <= zbot.
    if ((dZtop - EPS <= point.Z()) && (dZbot + EPS >= point.Z()))
      ret = true;
  }

  return ret;
}

geo::CValue CHexaInterfaceElement::InterpolateValue(const geo::IPoint &point, const std::vector<geo::CValue> &values) const
{
  if(ContainsInvalidValue(values)) return geo::CValue();

  assert(false);
  return geo::CValue();
}

// Compare if 2 CInterfaceElement elements are the same and return true if they do.
bool CHexaInterfaceElement::operator==(const CHexaInterfaceElement& rhs)
{
  if ((m_pUpperBody != rhs.m_pUpperBody) || (m_pLowerBody != rhs.m_pLowerBody))
    return false;

  // Compare the points.
  for(int i = 0; i < 8; i++)
    if (m_points[i] != rhs.m_points[i])
      return false;

  return true;
}

//=============================================================================
void CHexaInterfaceElement::SetUpperBody(const geo::CHexahedron &body)
{
  m_pUpperBody = &body;
}

void CHexaInterfaceElement::SetLowerBody(const geo::CHexahedron &body)
{
  m_pLowerBody = &body;
}

const geo::CHexahedron &CHexaInterfaceElement::GetUpperBody() const
{
  return *m_pUpperBody;
}

const geo::CHexahedron &CHexaInterfaceElement::GetLowerBody() const
{
  return *m_pLowerBody;
}

std::set<geo::CPoint> CHexaInterfaceElement::Intersection(const geo::IPlane &plane) const
{

  assert(false);
  return std::set<geo::CPoint>();
}
//=============================================================================

