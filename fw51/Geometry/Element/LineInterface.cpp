/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// LineInterface.cpp: implementation of the CLineInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"

#include "IFace.h"
#include "IMesh.h"
#include "INode.h"
#include "LineInterface.h"
#include "Vector.h"

namespace geo {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*!
   IMPORTANT:
   - the order of the nodes is NOT cyclic
   - DON'T swap the sides, the face connected to 1-2 should be at the top, the other at the bottom
     (the z-axis must point outward)

   1  F  2
   :=====:
   3  B  4

   n1 through n4 are the mesh node indices in mesh this interface is connected to
   f1 is the face connected at the 1-2 side
   F is the front face and B is the back face
*/
CLineInterface::CLineInterface(IMesh &mesh, int n1, int n2, const geo::IFace &front, int n3, int n4,
                               const geo::IFace &back)
    : m_front(front), m_back(back), m_mesh(mesh) {
  m_nodes[0] = n1;
  m_nodes[1] = n2;
  m_nodes[2] = n3;
  m_nodes[3] = n4;

  int i;
  for (i = 0; i < 4; i++) {
    INode &node = const_cast<INode &>(Node(i));
    node.RegisterElement(*this);
  }

  m_nIndex = mesh.RegisterElement(*this);
}

CLineInterface::~CLineInterface() {}

int CLineInterface::PointIndex(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_nodes[nIndex];
}

size_t CLineInterface::NrOfNodes() const { return 4; }

const INode &CLineInterface::Node(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_mesh.Node(PointIndex(nIndex));
}

void CLineInterface::Node(int nIndex, const IPoint &point) {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  m_mesh.Node(PointIndex(nIndex), point);
}

const IElementSet *CLineInterface::IndexingElementSet() const { return &m_mesh; }

int CLineInterface::Index() const { return m_nIndex; }

const ILine &CLineInterface::getLine(int n0, int n1, const geo::IFace &face) const {
  for (int i = 0; i < face.NrOfLines(); i++) {
    assert(face.Line(i).NrOfNodes() == 2);
    if ((face.Line(i).Node(0).Index() == n0 && face.Line(i).Node(1).Index() == n1) ||
        (face.Line(i).Node(0).Index() == n1 && face.Line(i).Node(1).Index() == n0))
      return face.Line(i);
  }
  assert(false);
  ILine *pBogus = 0;
  return *pBogus;
}

/*!
  Returns the local node index on the front or back face
  \param nIndex 0 or 1 local node (0..2) of front face 2 or 3 local node (0..2) of back face
*/
int CLineInterface::FaceNode(int nIndex) const {
  assert(nIndex >= 0);
  assert(nIndex < 4);

  if (nIndex == 0 || nIndex == 1) {
    for (int i = 0; i < m_front.NrOfNodes(); i++) {
      if (m_nodes[nIndex] == m_front.Node(i).Index())
        return i;
    }
  } else {
    for (int i = 0; i < m_back.NrOfNodes(); i++) {
      if (m_nodes[nIndex] == m_back.Node(i).Index())
        return i;
    }
  }
  assert(false);
  return -1;
}

const IElement &CLineInterface::Front() const { return FrontLine(); }

const ILine &CLineInterface::FrontLine() const { return getLine(m_nodes[0], m_nodes[1], m_front); }

const IFace &CLineInterface::FrontFace() const { return m_front; }

const IElement &CLineInterface::Back() const { return BackLine(); }

const ILine &CLineInterface::BackLine() const { return getLine(m_nodes[2], m_nodes[3], m_back); }

const IFace &CLineInterface::BackFace() const { return m_back; }

std::string CLineInterface::Type() const { return std::string("L8IF"); }

void CLineInterface::AssertValid() const {
  // assert proper connectivity (no crossing, etc)
  assert(&Node(0) == &FrontFace().Node(FaceNode(0)));
  assert(&Node(1) == &FrontFace().Node(FaceNode(1)));
  assert(&Node(2) == &BackFace().Node(FaceNode(2)));
  assert(&Node(3) == &BackFace().Node(FaceNode(3)));

  // assert that face1 is connected to the 'left' side and face2 to the 'right' side
  // (when walking from node 1 to node 2)

  // the element is 2D, so ignore z-coordinate
  CPoint p1(Point(0));
  p1.Z(0);
  CPoint p2(Point(1));
  p2.Z(0);
  CPoint m1 = m_front.MidPoint();
  m1.Z(0);
  CPoint m2 = m_back.MidPoint();

  CVector v = p2 - p1;  // the vector along the line
  CVector v1 = m1 - p1; // the vector to the midpoint of face1
  CVector v2 = m2 - p1; // the vector to the midpoint of face2

  // cross products should point outward

  // if this fails face1 is not at the 'left' side of the line
  CVector c1 = v1.CrossProduct(v).UnitVector();
  assert(c1 == CVector::Zaxis);
  // if this fails face2 is not at the 'right' side of the line
  CVector c2 = v.CrossProduct(v2).UnitVector();
  assert(c2 == CVector::Zaxis);
}

/*!
   Use the front or back for interface polation
*/
CValue CLineInterface::InterpolateValue(const IPoint &point, const std::vector<CValue> &values) const {
  std::vector<CValue> vcValue(values);
  vcValue.resize(FrontLine().NrOfPoints());
  return FrontLine().InterpolateValue(point, vcValue);
}

/*!
   Use the front or back for contains
*/
bool CLineInterface::Contains(const IPoint &point, bool bIncludeEdge) const {
  return FrontLine().Contains(point, bIncludeEdge);
}

} // namespace geo
