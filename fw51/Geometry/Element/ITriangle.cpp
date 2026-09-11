/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ITriangle.cpp: implementation of the ITriangle class.
//
//////////////////////////////////////////////////////////////////////

#include <QDebug>

#include "dimple.h"
#include <vector>

#include "ICoordinate.h"
#include "IElement.h"
#include "IElementSet.h"
#include "IFace.h"
#include "ILine.h"
#include "IObject.h"
#include "IPlane.h"
#include "IPoint.h"
#include "IProgressBase.h"
#include "ITriangle.h"
#include "IValue.h"
#include "IValueSet.h"
#include "IVector.h"
#include "Line.h"
#include "Matrix.h"
#include "Plane.h"
#include "Point.h"
#include "Value.h"
#include "Vector.h"
#include "VectorTempl.h"

#include "lbel.h"

// edge names
static const char L1[] = "L1";
static const char L2[] = "L2";
static const char L3[] = "L3";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

void ITriangle::BuildIntegrationPoints(IElement::TIntPtVec &vec, int numint) {
  int i;
  for (i = 0; i < numint; i++) {
    // the hammer coordinates returned include the third
    // (dependent) coordinate 1 - xi - eta
    double coord[3];
    double weight;
    HammerAreaIntegration(i + 1, numint, coord, &weight);
    std::pair<IElement::TDoubleVec, double> prHammer;
    prHammer.first.push_back(coord[0]);
    prHammer.first.push_back(coord[1]);
    prHammer.second = weight;
    vec.push_back(prHammer);
  }
}

const IElement::TIntPtVec &ITriangle::IntegrationPoints(int order) {

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
      BuildIntegrationPoints(s_QuadIntegrationPoints, 3);
    return s_QuadIntegrationPoints;
    break;
  case 3:
    if (s_CubicIntegrationPoints.empty())
      BuildIntegrationPoints(s_CubicIntegrationPoints, 4);
    return s_CubicIntegrationPoints;
    break;
  }

  assert(false);
  TIntPtVec *pBogus = 0;
  return *pBogus;
}

const IElement::TIntPtVec &ITriangle::IntegrationPoints() const { return IntegrationPoints(Order()); }

void ITriangle::PrepareMapping() {
  for (int i = 1; i < 4; ++i)
    IntegrationPoints(i);
}

ITriangle::ITriangle() {}

int ITriangle::NrOfPoints() const {
  if (NrOfNodes() > 0)
    return NrOfNodes();
  return 3;
}

const IPoint &ITriangle::Point(int nIndex) const {
  return Node(nIndex);
#if 0
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  return Node(Order() * nIndex);
#endif
}

void ITriangle::Point(int nIndex, const IPoint &pt) {
  Node(nIndex, pt);
#if 0
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  Node(Order() * nIndex, pt);
#endif
}

void ITriangle::Swap(const double **d1, const double **d2) const {
  const double *tmp = *d1;
  *d1 = *d2;
  *d2 = tmp;
}

CVector ITriangle::Normal() const {
  const CPoint &p0 = Point(0);
  const CPoint &p1 = Point(Order() * 1);
  const CPoint &p2 = Point(Order() * 2);
  const CVector &dir1 = CVector(p2 - p0);
  const CVector &dir2 = CVector(p2 - p1);
  CVector pointBasedNormal = dir1.CrossProduct(dir2);
  if (!pointBasedNormal.isNullVector())
    pointBasedNormal = pointBasedNormal.UnitVector();
  return pointBasedNormal;
}

IElement::TDoubleVec ITriangle::ShapeFunction(const IElement::TDoubleVec &isocoords) const {
  int nNodeSize = NrOfNodes();
  if (nNodeSize == 0)
    nNodeSize = NrOfPoints();

  assert(isocoords.size() == 2); // xi and eta coordinate

  double *values = new double[nNodeSize];
  double *coords = new double[3];

  coords[0] = isocoords[0];
  coords[1] = isocoords[1];
  coords[2] = 1 - isocoords[0] - isocoords[1];

  switch (Order()) {
  case 1:
    TriangleShape(coords, values);
    break;
  case 2:
    TriangleShapeQuadIntPol(coords, values);
    break;
  case 3:
    TriangleShapeCubicIntPol(coords, values);
    break;
  default:
    assert(false);
  }

  IElement::TDoubleVec vcRet(nNodeSize);

  int i;
  for (i = 0; i < nNodeSize; i++)
    vcRet[i] = values[i];

  delete[] coords;
  delete[] values;

  return vcRet;
}

CMatrix ITriangle::ShapeFunctionDerived(const IElement::TDoubleVec &isocoords) const {
  int nNodeSize = NrOfNodes();
  if (nNodeSize == 0)
    nNodeSize = NrOfPoints();

  assert(isocoords.size() == 2);
  assert(isocoords[0] >= 0.0 && isocoords[0] <= 1.0);
  assert(isocoords[1] >= 0.0 && isocoords[1] <= 1.0);

  // 2 rows, NrOfNodes() columns
  CMatrix ret(2, nNodeSize);

  double *p = new double[2 * nNodeSize];

  double coord[3];
  coord[0] = isocoords[0];
  coord[1] = isocoords[1];
  coord[2] = 1 - isocoords[0] - isocoords[1];

  assert(coord[2] >= 0.0);

  switch (Order()) {
  case 1:
    DerivedTriangleShape(coord, 0, p);
    break;
  case 2:
    DerivedTriangleShapeQuadIntPol(coord, p);
    break;
  case 3:
    DerivedTriangleShapeCubicIntPol(coord, p);
    break;
  default:
    assert(false);
  }

  double *v = p;

  int i;
  int j;
  for (j = 0; j < nNodeSize; j++) {
    for (i = 0; i < 2; i++) {
      ret.Value(i, j, *(v++));
    }
  }

  delete[] p;
  return ret;
}

std::vector<IElement::TDoubleVec> ITriangle::IsoCoordinates() const {
  std::vector<IElement::TDoubleVec> ret;

  switch (Order()) {
  case 1:
    // node 1
    ret.push_back(MakeVec(1, 0));
    // node 2
    ret.push_back(MakeVec(0, 1));
    // node 3
    ret.push_back(MakeVec(0, 0));
    break;
  case 2:
    // node 1
    ret.push_back(MakeVec(1, 0));
    // node 2
    ret.push_back(MakeVec(0.5, 0.5));
    // node 3
    ret.push_back(MakeVec(0, 1));
    // node 4
    ret.push_back(MakeVec(0, 0.5));
    // node 5
    ret.push_back(MakeVec(0, 0));
    // node 6
    ret.push_back(MakeVec(0.5, 0));
    break;
  case 3:
    // node 1
    ret.push_back(MakeVec(1, 0));
    // node 2
    ret.push_back(MakeVec(2. / 3, 1. / 3));
    // node 3
    ret.push_back(MakeVec(1. / 3, 2. / 3));
    // node 4
    ret.push_back(MakeVec(0, 1));
    // node 5
    ret.push_back(MakeVec(0, 2. / 3));
    // node 6
    ret.push_back(MakeVec(0, 1. / 3));
    // node 7
    ret.push_back(MakeVec(0, 0));
    // node 8
    ret.push_back(MakeVec(1. / 3, 0));
    // node 9
    ret.push_back(MakeVec(2. / 3, 0));
    break;
  default:
    assert(false);
  }

  return ret;
}

IElement::TDoubleVec ITriangle::WorldToIso1stOrder(const IPoint &point) const {
  // a fast implementation for 1st order triangles
  assert(Order() == 1);
  assert(NrOfPoints() == 3);

  double x0 = Point(0).X();
  double x1 = Point(1).X();
  double x2 = Point(2).X();

  double y0 = Point(0).Y();
  double y1 = Point(1).Y();
  double y2 = Point(2).Y();

  double xp = point.X();
  double yp = point.Y();

  // in case of a vertically aligned triangle (normal with 0 Z component)
  // we need to use the z coordinates to make sure we get an invertible matrix
  // (independent system)
  CVector n = Normal();
  if (fabs(n.Z()) < EPS) {
    if (n == CVector::Xaxis) {
      // use y and z coords
      x0 = Point(0).Y();
      x1 = Point(1).Y();
      x2 = Point(2).Y();

      y0 = Point(0).Z();
      y1 = Point(1).Z();
      y2 = Point(2).Z();

      xp = point.Y();
      yp = point.Z();
    } else {
      // use x and z coords
      y0 = Point(0).Z();
      y1 = Point(1).Z();
      y2 = Point(2).Z();

      yp = point.Z();
    }
  }

  CMatrix A(2, 2);
  A.Value(0, 0, x0 - x2);
  A.Value(0, 1, x1 - x2);

  A.Value(1, 0, y0 - y2);
  A.Value(1, 1, y1 - y2);

  CMatrix rhs(2, 1);
  rhs.Value(0, 0, xp - x2);
  rhs.Value(1, 0, yp - y2);

  CMatrix ret = A.GetInverse() * rhs;
  assert(ret.RowSize() == 2 && ret.ColumnSize() == 1);

  // ret contains the xi and eta for the given point
  IElement::TDoubleVec vcRet(2);
  vcRet[0] = ret.Value(0, 0);
  vcRet[1] = ret.Value(1, 0);

  return vcRet;
}

IElement::TDoubleVec ITriangle::WorldToIso(const IPoint &point) const {
  if (Order() == 1)
    return WorldToIso1stOrder(point);

  return IFace::WorldToIso(point);
}

int ITriangle::IntegrationPointSize() const { return (int)IntegrationPoints().size(); }

const IElement::TDoubleVec &ITriangle::IntegrationPointCoords(int nIndex) const {
  assert(nIndex >= 0 && nIndex < IntegrationPointSize());
  return IntegrationPoints()[nIndex].first;
}

const double &ITriangle::IntegrationPointWeight(int nIndex) const {
  assert(nIndex >= 0 && nIndex < IntegrationPointSize());
  return IntegrationPoints()[nIndex].second;
}

} // namespace geo
