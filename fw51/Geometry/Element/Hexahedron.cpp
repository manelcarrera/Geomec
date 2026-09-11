 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include <cmath>
#include "PolyLine.h"
#include "Polygon.h"
#include "Hexahedron.h"
#include "IMesh.h"

// bodyface names
static const char sKSI1[]  = "KSI1";
static const char sKSI2[]  = "KSI2";
static const char sETA1[]  = "ETA1";
static const char sETA2[]  = "ETA2";
static const char sZETA1[] = "ZETA1";
static const char sZETA2[] = "ZETA2";

namespace geo {

// The array m_vcPoints stores the lower 4 points (with the lowest z values) at 
// the first 4 points and the last 4 four are the upper points. 
CHexahedron::CHexahedron( IMesh& mesh,
                          const std::vector<const IPoint*>& points )
 : m_vcNode(points.size()),
   m_mesh(mesh)
{
  assert(points.size() == 8);

  // Register nodes
  for(int i = 0; i < m_vcNode.size(); i++)
    m_vcNode[i] = m_mesh.RegisterNode(*points[i]);

  // Register element
  m_nIndex = m_mesh.RegisterElement(*this);

  AssertValid();
}

CHexahedron::CHexahedron( IMesh& mesh, const std::vector<int>& points )
 : m_vcNode(points), m_mesh(mesh)
{
  assert(points.size() == 8);

  // Nodes are already registered in the mesh. We only have to register the element
  m_nIndex = m_mesh.RegisterElement(*this);

  AssertValid();
}

CHexahedron::~CHexahedron()
{
}

const IElementSet* CHexahedron::IndexingElementSet() const
{
  return &m_mesh;
}

int CHexahedron::Index() const
{
  return m_nIndex;
}

int CHexahedron::PointIndex(int nIndex) const
{
  return m_vcNode[nIndex];
}

const geo::IPoint &CHexahedron::Point(int nIndex) const
{
  return m_mesh.Point(m_vcNode[nIndex]);
}

void CHexahedron::Point(int nIndex, const geo::IPoint &pt)
{
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  assert(NrOfPoints() == NrOfNodes()); // must be first order

  Node(nIndex, pt);
}

size_t CHexahedron::NrOfNodes() const
{
  return 8;
}

const INode& CHexahedron::Node(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return Mesh().Node(m_vcNode[nIndex]);
}

void CHexahedron::Node(int nIndex, const IPoint& point)
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  Mesh().Node(m_vcNode[nIndex], point);
}

size_t CHexahedron::Order() const
{
  return 1;
}

IElement::TDoubleVec CHexahedron::WorldToIso(const geo::IPoint& point) const
{
  // currently only implemented for hexahedrons with vertical boundaries
  // and rectangular top views
  geo::CVector v1(Point(1) - Point(0));
  geo::CVector v2(Point(2) - Point(1));
  geo::CVector v3(Point(3) - Point(2));
  geo::CVector v4(Point(0) - Point(3));
  v1.Z(0);
  v2.Z(0);
  v3.Z(0);
  v4.Z(0);

  geo::CVector vp(point - Point(0));
  vp.Z(0);

  bool bCanUseSimpleImplementation = 
      // make sure top view angles are 90 degrees
      (v1.DotProduct(v2) < EPS) &&
      (v2.DotProduct(v3) < EPS) &&
      (v3.DotProduct(v4) < EPS) &&
      // make sure sides are vertical
      (CVector(Point(4) - Point(0)).UnitVector() == CVector::Zaxis) &&
      (CVector(Point(5) - Point(1)).UnitVector() == CVector::Zaxis) &&
      (CVector(Point(6) - Point(2)).UnitVector() == CVector::Zaxis) &&
      (CVector(Point(7) - Point(3)).UnitVector() == CVector::Zaxis);

  if(!bCanUseSimpleImplementation)
      return IHexahedron::WorldToIso(point);

  IElement::TDoubleVec vcRet(3);

  // xi
  vcRet[0] = (2.0 * vp.DotProduct(v1) / (v1.Length() * v1.Length())) - 1.0;

  // eta
  v4.Flip();
  vcRet[1] = (2.0 * vp.DotProduct(v4) / (v4.Length() * v4.Length())) - 1.0;

  double Ep = vcRet[0];
  double Np = vcRet[1];

  double Zt;
  double Zb;

  // the z top coordinate
  Zt = Point(4).Z() * 0.25 * (1-Ep) * (1-Np) +
         Point(5).Z() * 0.25 * (1+Ep) * (1-Np) +
         Point(6).Z() * 0.25 * (1+Ep) * (1+Np) +
         Point(7).Z() * 0.25 * (1-Ep) * (1+Np);

  // the z bottom coordinate
  Zb = Point(0).Z() * 0.25 * (1-Ep) * (1-Np) +
         Point(1).Z() * 0.25 * (1+Ep) * (1-Np) +
         Point(2).Z() * 0.25 * (1+Ep) * (1+Np) +
         Point(3).Z() * 0.25 * (1-Ep) * (1+Np);

  // zeta
  vcRet[2] = (2 * (point.Z() - Zb) / (Zt - Zb)) - 1.0;

  return vcRet;
}

bool CHexahedron::Contains(const geo::IPoint &point, bool bIncludeEdge) const
{
  // check bounding box
  const IPoint &min = Min();
  const IPoint &max = Max();
  if ((point.X() < min.X() - compareTolerance() ||
       point.Y() < min.Y() - compareTolerance() ||
       point.Z() < min.Z() - compareTolerance())
      ||
      (point.X() > max.X() + compareTolerance() ||
       point.Y() > max.Y() + compareTolerance() ||
       point.Z() > max.Z() + compareTolerance()))
  {
  return false;
  }

  IElement::TDoubleVec vcRet = WorldToIso(point);
  for(int i = 0; i < 3; i++){
    if((vcRet[i] + EPS) < -1.0 || (vcRet[i] - EPS) > 1.0)
      return false;
  }

  // Check to see if point is at the border of the hexahedron element.
  if (!bIncludeEdge) {
    for (int i = 0; i < NrOfFaces(); i++)
      if(Face(i).Contains(point, true))
        return false;
  }

  return true;
}

std::set<CPoint> CHexahedron::Intersection(const IPlane &/*plane*/) const
{
  assert(false);
  std::set<geo::CPoint> retset;
  return retset;
}

CPtrArray<IPoint> CHexahedron::IntersectionPolygon(const CPolygon poly, bool corners /*= true*/)
{
  CPtrArray<IPoint> result;

  for (int i=0; i<NrOfFaces(); i++)
  {
    const IFace *face = &Face(i);

    int j;
    for (j=0; j<poly.NrOfPoints(); j++)
    {
      int n = j+1;
      if (n > poly.NrOfPoints()-1) n=0;
      CLine line(poly.Point(j), poly.Point(n));
      CPoint pt = face->Intersection(line);
      if (!pt.Empty() && face->Contains(pt, true) && line.Contains(pt, true))
        result.PushBack(*(new CPoint(pt)));
    }

    for (j=0; j<face->NrOfPoints(); j++)
    {
      int n = j+1;
      if (n > (face->NrOfPoints()-1)) n = 0;
      CLine line(face->Point(j), face->Point(n));
      CPoint pt = poly.Intersection(line);
      if (!pt.Empty() && poly.Contains(pt, true) && line.Contains(pt, true))
        result.PushBack(*(new CPoint(pt)));
    }
  }

  if (corners) InsertCorners(poly, result);

  return result;
}

static bool Valid(const CHexahedron& h)
{
  geo::CVector vc1(h.Point(0), h.Point(1));
  geo::CVector vc2(h.Point(1), h.Point(2));
  geo::CVector vcx(vc1.CrossProduct(vc2));
  geo::CVector vc3(h.Point(1), h.Point(5));

  return (vcx.AngleRad(vc3) < PI / 2.0);
}

void CHexahedron::AssertValid() const
{
  // make sure code is only executed in debug context
  assert(Valid(*this));

  // Call base-class
  IHexahedron::AssertValid();
}

std::string CHexahedron::Type() const
{
  return std::string("HX24L");
}

double CHexahedron::InfluenceVolume(int /*nNode*/) const
{
  // This function return the influence volume for a node.
  // at the moment simply 1/8 of the volume is returned, but this is too simple.
  return (1.0 / 8.0) * Volume();
}

const IMesh& CHexahedron::Mesh() const
{
  return m_mesh;
}

IMesh& CHexahedron::Mesh()
{
  return m_mesh;
}

const char* CHexahedron::FaceName(int nIndex) const
{
  const char* ret = 0;
  switch(nIndex)
  {
  case 0:
    ret = sZETA1;
    break;
  case 1:
    ret = sETA1;
    break;
  case 2:
    ret = sKSI2;
    break;
  case 3:
    ret = sETA2;
    break;
  case 4:
    ret = sKSI1;
    break;
  case 5:
    ret = sZETA2;
    break;
  default:
    assert(false);
  }

  return ret;
}

}
