/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ITetrahedron.cpp: implementation of the ITetrahedron class.
//
//////////////////////////////////////////////////////////////////////

#include "ITetrahedron.h"
#include "BodyLine.h"
#include "BodyTriangle.h"
#include "Matrix.h"
#include "Plane.h"
#include "Vector.h"
#include "dimple.h"
#include <cmath>

#include "lbel.h"

#include <tbb/spin_rw_mutex.h>

namespace {

// See comment in IFace

tbb::spin_rw_mutex myGlobalITetrahedronMutex[16];

} // namespace

static const int quad_point_indices[] = {0, 2, 4, 9};

static const int cubic_point_indices[] = {0, 3, 6, 15};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

void ITetrahedron::BuildIntegrationPoints(IElement::TIntPtVec &vec, int numint) {
  int i;
  for (i = 0; i < numint; i++) {
    // the volume hammer coordinates returned include the fourth
    // (dependent) coordinate 1 - xi - eta - zeta
    double coord[4];
    double weight;
    HammerVolumeIntegration(i + 1, numint, coord, &weight);
    std::pair<IElement::TDoubleVec, double> prHamvol;
    prHamvol.first.push_back(coord[0]);
    prHamvol.first.push_back(coord[1]);
    prHamvol.first.push_back(coord[2]);
    prHamvol.second = weight;
    vec.push_back(prHamvol);
  }
}

const IElement::TIntPtVec &ITetrahedron::IntegrationPoints(int order) {
  // integration point location and weights
  static TIntPtVec s_LinearIntegrationPoints;
  static TIntPtVec s_QuadIntegrationPoints;
  static TIntPtVec s_CubicIntegrationPoints;

  switch (order) {
  case 1:
    if (s_LinearIntegrationPoints.empty())
      BuildIntegrationPoints(s_LinearIntegrationPoints, 1);
    return s_LinearIntegrationPoints;
    break;
  case 2:
    if (s_QuadIntegrationPoints.empty())
      BuildIntegrationPoints(s_QuadIntegrationPoints, 4);
    return s_QuadIntegrationPoints;
    break;
  case 3:
    if (s_CubicIntegrationPoints.empty())
      BuildIntegrationPoints(s_CubicIntegrationPoints, 5);
    return s_CubicIntegrationPoints;
    break;
  }

  assert(false);
  TIntPtVec *pBogus = 0;
  return *pBogus;
}

const IElement::TIntPtVec &ITetrahedron::IntegrationPoints() const { return IntegrationPoints(Order()); }

void ITetrahedron::PrepareMapping() {
  for (int i = 1; i < 4; ++i)
    IntegrationPoints(i);
}

ITetrahedron::ITetrahedron() {}

ITetrahedron::~ITetrahedron() {
  int i;
  for (i = 0; i < m_vcFaces.size(); i++)
    if (m_vcFaces[i])
      delete m_vcFaces[i];
}

double ITetrahedron::SignedVolume() const {
  assert(Order() == 1); // won't be exact otherwise
  geo::CVector v1(Point(1) - Point(0));
  geo::CVector v2(Point(2) - Point(1));
  geo::CVector v3(Point(3) - Point(1));
  return (v1.DotProduct(v2.CrossProduct(v3))) / 6;
}

int ITetrahedron::NrOfPoints() const {
  return NrOfNodes();
  //	return 4;
}

const IPoint &ITetrahedron::Point(int nIndex) const {
  return Node(nIndex);
#if 0
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  switch(Order())
  {
  case 1:
    return Node(nIndex);
  case 2:
    return Node(quad_point_indices[nIndex]);
  case 3:
    return Node(cubic_point_indices[nIndex]);
  }

  assert(false);
  const IPoint* pBogus = 0;
  return *pBogus;
#endif
}

void ITetrahedron::Point(int nIndex, const IPoint &pt) {
  Node(nIndex, pt);
#if 0
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  switch(Order())
  {
  case 1:
    Node(nIndex, pt);
    break;
  case 2:
    Node(quad_point_indices[nIndex], pt);
    break;
  case 3:
    Node(cubic_point_indices[nIndex], pt);
    break;
  default:
    assert(false);
  }
#endif
}

int ITetrahedron::NrOfFaces() const { return 4; }

const IFace &ITetrahedron::Face(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfFaces());

  {
    int index = (intptr_t)this >> 6 & 0xf;

    tbb::spin_rw_mutex::scoped_lock lock(myGlobalITetrahedronMutex[index], false);

    if (m_vcFaces.empty()) {
      lock.upgrade_to_writer();

      if (m_vcFaces.empty())
        m_vcFaces.resize(4, 0);
      if (!m_vcFaces[nIndex])
        m_vcFaces[nIndex] = new CBodyTriangle(*const_cast<ITetrahedron *>(this), nIndex);
    } else if (!m_vcFaces[nIndex]) {
      lock.upgrade_to_writer();

      if (!m_vcFaces[nIndex])
        m_vcFaces[nIndex] = new CBodyTriangle(*const_cast<ITetrahedron *>(this), nIndex);
    }
  }

  return *m_vcFaces[nIndex];
}

void ITetrahedron::InitFaceNodeIndices(std::vector<TFaceNodeVec> &FaceNodeIdxs) {
  assert(FaceNodeIdxs.size() == 0); // Init once ...

  FaceNodeIdxs.resize(3, TFaceNodeVec(4));

  // Initialise for First Order elements
  // Face 0
  FaceNodeIdxs[0][0].resize(3);
  FaceNodeIdxs[0][0][0] = 1;
  FaceNodeIdxs[0][0][1] = 2;
  FaceNodeIdxs[0][0][2] = 3;
  // Face 1
  FaceNodeIdxs[0][1].resize(3);
  FaceNodeIdxs[0][1][0] = 0;
  FaceNodeIdxs[0][1][1] = 3;
  FaceNodeIdxs[0][1][2] = 2;
  // Face 2
  FaceNodeIdxs[0][2].resize(3);
  FaceNodeIdxs[0][2][0] = 0;
  FaceNodeIdxs[0][2][1] = 1;
  FaceNodeIdxs[0][2][2] = 3;
  // Face 3
  FaceNodeIdxs[0][3].resize(3);
  FaceNodeIdxs[0][3][0] = 0;
  FaceNodeIdxs[0][3][1] = 2;
  FaceNodeIdxs[0][3][2] = 1;

  // Initialise for Second Order elements
  // Face 0
  FaceNodeIdxs[1][0].resize(6);
  FaceNodeIdxs[1][0][0] = 2;
  FaceNodeIdxs[1][0][1] = 3;
  FaceNodeIdxs[1][0][2] = 4;
  FaceNodeIdxs[1][0][3] = 8;
  FaceNodeIdxs[1][0][4] = 9;
  FaceNodeIdxs[1][0][5] = 7;
  // Face 1
  FaceNodeIdxs[1][1].resize(6);
  FaceNodeIdxs[1][1][0] = 0;
  FaceNodeIdxs[1][1][1] = 6;
  FaceNodeIdxs[1][1][2] = 9;
  FaceNodeIdxs[1][1][3] = 8;
  FaceNodeIdxs[1][1][4] = 4;
  FaceNodeIdxs[1][1][5] = 5;
  // Face 2
  FaceNodeIdxs[1][2].resize(6);
  FaceNodeIdxs[1][2][0] = 0;
  FaceNodeIdxs[1][2][1] = 1;
  FaceNodeIdxs[1][2][2] = 2;
  FaceNodeIdxs[1][2][3] = 7;
  FaceNodeIdxs[1][2][4] = 9;
  FaceNodeIdxs[1][2][5] = 6;
  // Face 3
  FaceNodeIdxs[1][3].resize(6);
  FaceNodeIdxs[1][3][0] = 0;
  FaceNodeIdxs[1][3][1] = 5;
  FaceNodeIdxs[1][3][2] = 4;
  FaceNodeIdxs[1][3][3] = 3;
  FaceNodeIdxs[1][3][4] = 2;
  FaceNodeIdxs[1][3][5] = 1;

  // Intialise for Third Order elements
  // Face 0
  FaceNodeIdxs[2][0].resize(9);
  FaceNodeIdxs[2][0][0] = 3;
  FaceNodeIdxs[2][0][1] = 4;
  FaceNodeIdxs[2][0][2] = 5;
  FaceNodeIdxs[2][0][3] = 6;
  FaceNodeIdxs[2][0][4] = 11;
  FaceNodeIdxs[2][0][5] = 14;
  FaceNodeIdxs[2][0][6] = 15;
  FaceNodeIdxs[2][0][7] = 13;
  FaceNodeIdxs[2][0][8] = 10;
  // Face 1
  FaceNodeIdxs[2][1].resize(9);
  FaceNodeIdxs[2][1][0] = 0;
  FaceNodeIdxs[2][1][1] = 9;
  FaceNodeIdxs[2][1][2] = 12;
  FaceNodeIdxs[2][1][3] = 15;
  FaceNodeIdxs[2][1][4] = 14;
  FaceNodeIdxs[2][1][5] = 11;
  FaceNodeIdxs[2][1][6] = 6;
  FaceNodeIdxs[2][1][7] = 7;
  FaceNodeIdxs[2][1][8] = 8;
  // Face 2
  FaceNodeIdxs[2][2].resize(9);
  FaceNodeIdxs[2][2][0] = 0;
  FaceNodeIdxs[2][2][1] = 1;
  FaceNodeIdxs[2][2][2] = 2;
  FaceNodeIdxs[2][2][3] = 3;
  FaceNodeIdxs[2][2][4] = 10;
  FaceNodeIdxs[2][2][5] = 13;
  FaceNodeIdxs[2][2][6] = 15;
  FaceNodeIdxs[2][2][7] = 12;
  FaceNodeIdxs[2][2][8] = 9;
  // Face 3
  FaceNodeIdxs[2][3].resize(9);
  FaceNodeIdxs[2][3][0] = 0;
  FaceNodeIdxs[2][3][1] = 8;
  FaceNodeIdxs[2][3][2] = 7;
  FaceNodeIdxs[2][3][3] = 6;
  FaceNodeIdxs[2][3][4] = 5;
  FaceNodeIdxs[2][3][5] = 4;
  FaceNodeIdxs[2][3][6] = 3;
  FaceNodeIdxs[2][3][7] = 2;
  FaceNodeIdxs[2][3][8] = 1;
}

const ITetrahedron::TIndexVec &ITetrahedron::FacePointIndices(int nIndex) const { return FaceNodeIndices(1, nIndex); }

int ITetrahedron::NrOfLines() const { return 6; }

const ILine &ITetrahedron::Line(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfLines());

  {
    int index = (intptr_t)this >> 6 & 0xf;

    tbb::spin_rw_mutex::scoped_lock lock(myGlobalITetrahedronMutex[index], false);

    if (m_vcLines.empty()) {
      lock.upgrade_to_writer();

      if (m_vcLines.empty())
        m_vcLines.resize(NrOfLines(), 0);

      if (!m_vcLines[nIndex])
        m_vcLines[nIndex] = new CBodyLine(*const_cast<ITetrahedron *>(this), nIndex);
    } else if (!m_vcLines[nIndex]) {
      lock.upgrade_to_writer();

      if (!m_vcLines[nIndex])
        m_vcLines[nIndex] = new CBodyLine(*const_cast<ITetrahedron *>(this), nIndex);
    }
  }

  return *m_vcLines[nIndex];
}

const ITetrahedron::TIndexVec &ITetrahedron::LinePointIndices(int nIndex) const { return LineNodeIndices(1, nIndex); }

void ITetrahedron::InitLineNodeIndices(std::vector<TLineNodeVec> &LineNodeIdxs) {
  assert(LineNodeIdxs.size() == 0); // Init once ...

  LineNodeIdxs.resize(3, TLineNodeVec(6));

  // Initialise for First Order elements
  // Line 0
  LineNodeIdxs[0][0].resize(2);
  LineNodeIdxs[0][0][0] = 0;
  LineNodeIdxs[0][0][1] = 1;
  // Line 1
  LineNodeIdxs[0][1].resize(2);
  LineNodeIdxs[0][1][0] = 1;
  LineNodeIdxs[0][1][1] = 2;
  // Line 2
  LineNodeIdxs[0][2].resize(2);
  LineNodeIdxs[0][2][0] = 2;
  LineNodeIdxs[0][2][1] = 0;
  // Line 3
  LineNodeIdxs[0][3].resize(2);
  LineNodeIdxs[0][3][0] = 0;
  LineNodeIdxs[0][3][1] = 3;
  // Line 4
  LineNodeIdxs[0][4].resize(2);
  LineNodeIdxs[0][4][0] = 1;
  LineNodeIdxs[0][4][1] = 3;
  // Line 5
  LineNodeIdxs[0][5].resize(2);
  LineNodeIdxs[0][5][0] = 2;
  LineNodeIdxs[0][5][1] = 3;

  // Initialise for Second Order Elements
  // Line 0
  LineNodeIdxs[1][0].resize(3);
  LineNodeIdxs[1][0][0] = 0;
  LineNodeIdxs[1][0][1] = 1;
  LineNodeIdxs[1][0][2] = 2;
  // Line 1
  LineNodeIdxs[1][1].resize(3);
  LineNodeIdxs[1][1][0] = 2;
  LineNodeIdxs[1][1][1] = 3;
  LineNodeIdxs[1][1][2] = 4;
  // Line 2
  LineNodeIdxs[1][2].resize(3);
  LineNodeIdxs[1][2][0] = 4;
  LineNodeIdxs[1][2][1] = 5;
  LineNodeIdxs[1][2][2] = 0;
  // Line 3
  LineNodeIdxs[1][3].resize(3);
  LineNodeIdxs[1][3][0] = 0;
  LineNodeIdxs[1][3][1] = 6;
  LineNodeIdxs[1][3][2] = 9;
  // Line 4
  LineNodeIdxs[1][4].resize(3);
  LineNodeIdxs[1][4][0] = 2;
  LineNodeIdxs[1][4][1] = 7;
  LineNodeIdxs[1][4][2] = 9;
  // Line 5
  LineNodeIdxs[1][5].resize(3);
  LineNodeIdxs[1][5][0] = 4;
  LineNodeIdxs[1][5][1] = 8;
  LineNodeIdxs[1][5][2] = 9;

  // Initialise for Third Order elements
  // Line 0
  LineNodeIdxs[2][0].resize(4);
  LineNodeIdxs[2][0][0] = 0;
  LineNodeIdxs[2][0][1] = 1;
  LineNodeIdxs[2][0][2] = 2;
  LineNodeIdxs[2][0][3] = 3;
  // Line 1
  LineNodeIdxs[2][1].resize(4);
  LineNodeIdxs[2][1][0] = 3;
  LineNodeIdxs[2][1][1] = 4;
  LineNodeIdxs[2][1][2] = 5;
  LineNodeIdxs[2][1][3] = 6;
  // Line 2
  LineNodeIdxs[2][2].resize(4);
  LineNodeIdxs[2][2][0] = 6;
  LineNodeIdxs[2][2][1] = 7;
  LineNodeIdxs[2][2][2] = 8;
  LineNodeIdxs[2][2][3] = 0;
  // Line 3
  LineNodeIdxs[2][3].resize(4);
  LineNodeIdxs[2][3][0] = 0;
  LineNodeIdxs[2][3][1] = 9;
  LineNodeIdxs[2][3][2] = 12;
  LineNodeIdxs[2][3][3] = 15;
  // Line 4
  LineNodeIdxs[2][4].resize(4);
  LineNodeIdxs[2][4][0] = 3;
  LineNodeIdxs[2][4][1] = 10;
  LineNodeIdxs[2][4][2] = 13;
  LineNodeIdxs[2][4][3] = 15;
  // Line 5
  LineNodeIdxs[2][5].resize(4);
  LineNodeIdxs[2][5][0] = 6;
  LineNodeIdxs[2][5][1] = 11;
  LineNodeIdxs[2][5][2] = 14;
  LineNodeIdxs[2][5][3] = 15;
}

const ITetrahedron::TIndexVec &ITetrahedron::LineNodeIndices(int order, int nIndex) {
  static std::vector<TLineNodeVec> s_LineNodeIndices;

  assert(nIndex >= 0 && nIndex < 6);
  if (s_LineNodeIndices.size() == 0)
    InitLineNodeIndices(s_LineNodeIndices);

  return s_LineNodeIndices[order - 1][nIndex];
}

const ITetrahedron::TIndexVec &ITetrahedron::LineNodeIndices(int nIndex) const {
  return LineNodeIndices(Order(), nIndex);
}

const ITetrahedron::TIndexVec &ITetrahedron::FaceNodeIndices(int order, int nIndex) {
  static std::vector<TFaceNodeVec> s_FaceNodeIndices;

  assert(nIndex >= 0 && nIndex < 4);
  if (s_FaceNodeIndices.size() == 0)
    InitFaceNodeIndices(s_FaceNodeIndices);

  return s_FaceNodeIndices[order - 1][nIndex];
}

const ITetrahedron::TIndexVec &ITetrahedron::FaceNodeIndices(int nIndex) const {
  return FaceNodeIndices(Order(), nIndex);
}

IElement::TDoubleVec ITetrahedron::ShapeFunction(const IElement::TDoubleVec &isocoords) const {
  assert(isocoords.size() == 3); // xi, eta and zeta coordinate

  double *values = new double[NrOfNodes()];
  double *coords = new double[4];

  coords[0] = isocoords[0];
  coords[1] = isocoords[1];
  coords[2] = isocoords[2];
  coords[3] = 1 - isocoords[0] - isocoords[1] - isocoords[2];

  switch (Order()) {
  case 1:
    TetraHedronShape(coords, values);
    break;
  case 2:
    QuadTetrahedronShape(coords, values);
    break;
  case 3:
    CubicTetrahedronShape(coords, values);
    break;
  default:
    assert(false);
  }

  IElement::TDoubleVec vcRet(NrOfNodes());

  int i;
  for (i = 0; i < NrOfNodes(); i++)
    vcRet[i] = values[i];

  delete[] values;
  delete[] coords;

  return vcRet;
}

bool ITetrahedron::CheckOrientation(const IPoint &p0, const IPoint &p1, const IPoint &p2, const IPoint &p3) {
  CPlane plane(p1, p2, p3);
  double dist = plane.SignedDistance(p0);

  assert(fabs(dist) > EPS);

  if (plane.SignedDistance(p0) > 0)
    return false; // points are supplied in a wrong order, swap p2 and p3 around
  else
    return true; // order of points is correct
}

double ITetrahedron::InfluenceVolume(int /*nNode*/) const {
  // This function returns the influencing volume for the node
  // at the moment 1/4 of the volume is returned, but this is too simple
  return (1.0 / 4.0) * Volume();
}

CMatrix ITetrahedron::ShapeFunctionDerived(const IElement::TDoubleVec &isocoords) const {
  assert(isocoords.size() == 3);

  int i;
  for (i = 0; i < isocoords.size(); i++)
    assert(isocoords[i] >= 0.0 && isocoords[i] <= 1.0);

  // 3 rows, NrOfNodes() columns
  CMatrix ret(3, NrOfNodes());

  double *p = new double[3 * NrOfNodes()];

  // tetrahedron assumes 4D volumetric coordinates:
  // 1 = xi
  // 2 = eta
  // 3 = zeta
  // 4 = 1 - xi - eta - zeta

  double volcoord[4];
  volcoord[0] = isocoords[0];
  volcoord[1] = isocoords[1];
  volcoord[2] = isocoords[2];
  volcoord[3] = 1 - isocoords[0] - isocoords[1] - isocoords[2];

  assert(volcoord[3] >= 0.0);

  switch (Order()) {
  case 1:
    DerivedTetraHedronShape(p);
    break;
  case 2:
    DerivedQuadTetrahedronShape(volcoord, p);
    break;
  case 3:
    DerivedCubicTetrahedronShape(volcoord, p);
    break;
  default:
    assert(false);
  }

  double *v = p;

  int j;
  for (j = 0; j < NrOfNodes(); j++) {
    for (i = 0; i < 3; i++) {
      ret.Value(i, j, *(v++));
    }
  }

  delete[] p;
  return ret;
}

std::vector<IElement::TDoubleVec> ITetrahedron::IsoCoordinates() const {
  std::vector<IElement::TDoubleVec> ret;

  switch (Order()) {
  case 1:
    // node 1
    ret.push_back(MakeVec(1, 0, 0));
    // node 2
    ret.push_back(MakeVec(0, 1, 0));
    // node 3
    ret.push_back(MakeVec(0, 0, 1));
    // node 4
    ret.push_back(MakeVec(0, 0, 0));
    break;
  case 2:
    // node 1
    ret.push_back(MakeVec(1, 0, 0));
    // node 2
    ret.push_back(MakeVec(0.5, 0.5, 0));
    // node 3
    ret.push_back(MakeVec(0, 1, 0));
    // node 4
    ret.push_back(MakeVec(0, 0.5, 0.5));
    // node 5
    ret.push_back(MakeVec(0, 0, 1));
    // node 6
    ret.push_back(MakeVec(0.5, 0, 0.5));
    // node 7
    ret.push_back(MakeVec(0.5, 0, 0));
    // node 8
    ret.push_back(MakeVec(0, 0.5, 0));
    // node 9
    ret.push_back(MakeVec(0, 0, 0.5));
    // node 10
    ret.push_back(MakeVec(0, 0, 0));
    break;
  case 3:
    // node 1
    ret.push_back(MakeVec(1, 0, 0));
    // node 2
    ret.push_back(MakeVec(2. / 3, 1. / 3, 0));
    // node 3
    ret.push_back(MakeVec(1. / 3, 2. / 3, 0));
    // node 4
    ret.push_back(MakeVec(0, 1, 0));
    // node 5
    ret.push_back(MakeVec(0, 2. / 3, 1. / 3));
    // node 6
    ret.push_back(MakeVec(0, 1. / 3, 2. / 3));
    // node 7
    ret.push_back(MakeVec(0, 0, 1));
    // node 8
    ret.push_back(MakeVec(1. / 3, 0, 2. / 3));
    // node 9
    ret.push_back(MakeVec(2. / 3, 0, 1. / 3));
    // node 10
    ret.push_back(MakeVec(2. / 3, 0, 0));
    // node 11
    ret.push_back(MakeVec(0, 2. / 3, 0));
    // node 12
    ret.push_back(MakeVec(0, 0, 2. / 3));
    // node 13
    ret.push_back(MakeVec(1. / 3, 0, 0));
    // node 14
    ret.push_back(MakeVec(0, 1. / 3, 0));
    // node 15
    ret.push_back(MakeVec(0, 0, 1. / 3));
    // node 16
    ret.push_back(MakeVec(0, 0, 0));
    break;
  default:
    assert(false);
  }

  return ret;
}

double ITetrahedron::Size() const { return -IBody::Size(); }

IElement::TDoubleVec ITetrahedron::WorldToIso1stOrder(const IPoint &point) const {
  // fast implementation for 1st order tetrahedrons
  assert(Order() == 1);
  assert(NrOfPoints() == 4);

  CMatrix A(3, 3);
  A.Value(0, 0, Point(0).X() - Point(3).X());
  A.Value(0, 1, Point(1).X() - Point(3).X());
  A.Value(0, 2, Point(2).X() - Point(3).X());

  A.Value(1, 0, Point(0).Y() - Point(3).Y());
  A.Value(1, 1, Point(1).Y() - Point(3).Y());
  A.Value(1, 2, Point(2).Y() - Point(3).Y());

  A.Value(2, 0, Point(0).Z() - Point(3).Z());
  A.Value(2, 1, Point(1).Z() - Point(3).Z());
  A.Value(2, 2, Point(2).Z() - Point(3).Z());

  CMatrix rhs(3, 1);
  rhs.Value(0, 0, point.X() - Point(3).X());
  rhs.Value(1, 0, point.Y() - Point(3).Y());
  rhs.Value(2, 0, point.Z() - Point(3).Z());

  CMatrix ret = A.GetInverse() * rhs;
  assert(ret.RowSize() == 3 && ret.ColumnSize() == 1);

  // ret contains the xi, eta and zeta for the given point
  IElement::TDoubleVec vcRet(3);
  vcRet[0] = ret.Value(0, 0);
  vcRet[1] = ret.Value(1, 0);
  vcRet[2] = ret.Value(2, 0);

  return vcRet;
}

IElement::TDoubleVec ITetrahedron::WorldToIso(const IPoint &point) const {
  if (Order() == 1)
    return WorldToIso1stOrder(point);

  return IBody::WorldToIso(point);
}

int ITetrahedron::IntegrationPointSize() const { return (int)IntegrationPoints().size(); }

const IElement::TDoubleVec &ITetrahedron::IntegrationPointCoords(int nIndex) const {
  assert(nIndex >= 0 && nIndex < IntegrationPointSize());
  return IntegrationPoints()[nIndex].first;
}

const double &ITetrahedron::IntegrationPointWeight(int nIndex) const {
  assert(nIndex >= 0 && nIndex < IntegrationPointSize());
  return IntegrationPoints()[nIndex].second;
}

} // end namespace geo
