 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IQuadrilateral.h"
#include "Triangle.h"
#include "INode.h"
#include "IPlane.h"
#include "Line.h"
#include "lbel.h"
#include "CoordinateSet.h"

namespace geo {


void IQuadrilateral::BuildIntegrationPoints( IElement::TIntPtVec& vec,
                                             int                  numint )
{
  int ixi, ieta;
  double xxi, xeta;
  double wxi, weta;

  for(ixi = 0; ixi < numint; ixi++)
  {
    GaussCoeff(ixi + 1, numint, &xxi, &wxi);
    for(ieta = 0; ieta < numint; ieta++)
    {
      GaussCoeff(ieta + 1, numint, &xeta, &weta);
      std::pair<IElement::TDoubleVec, double> prGaussp;
      prGaussp.first.push_back(xxi);
      prGaussp.first.push_back(xeta);
      prGaussp.second = wxi * weta;
      vec.push_back(prGaussp);
    }
  }
}

const IElement::TIntPtVec& IQuadrilateral::IntegrationPoints( int order )
{
  // integration point location and weights
  static TIntPtVec s_LinearIntegrationPoints;
  static TIntPtVec s_QuadIntegrationPoints;
  static TIntPtVec s_CubicIntegrationPoints;

  switch( order ) {
  case 1:
    if(s_LinearIntegrationPoints.empty())
      BuildIntegrationPoints(s_LinearIntegrationPoints, 1);
    return s_LinearIntegrationPoints;
    break;
  case 2:
    if(s_QuadIntegrationPoints.empty())
      BuildIntegrationPoints(s_QuadIntegrationPoints, 2);
    return s_QuadIntegrationPoints;
    break;
  case 3:
    if(s_CubicIntegrationPoints.empty())
      BuildIntegrationPoints(s_CubicIntegrationPoints, 3);
    return s_CubicIntegrationPoints;
    break;
  }

  assert(false);
  TIntPtVec* pBogus = 0;
  return *pBogus;
}

const IElement::TIntPtVec& IQuadrilateral::IntegrationPoints() const
{
  return IntegrationPoints( Order() );
}

void IQuadrilateral::PrepareMapping()
{
  for (int i = 1; i < 4; ++i)
  IntegrationPoints(i);
}

IQuadrilateral::IQuadrilateral()
{
}

IQuadrilateral::~IQuadrilateral()
{
}

int IQuadrilateral::NrOfPoints() const
{
  return NrOfNodes();
//	return 4;
}

const IPoint &IQuadrilateral::Point(int nIndex) const
{
  return Node( nIndex );
#if 0
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  return Node(Order() * nIndex);
#endif
}

void IQuadrilateral::Point(int nIndex, const IPoint &pt)
{
  Node( nIndex, pt );
#if 0
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  Node(Order() * nIndex, pt);
#endif
}

IElement::TDoubleVec IQuadrilateral::ShapeFunction(const IElement::TDoubleVec& isocoords) const
{
  assert(isocoords.size() == 2); // xi and eta coordinate

  double *values = new double[NrOfNodes()];

  switch(Order())
  {
  case 1:
    QuadriLateralShape(isocoords[0], isocoords[1], 0, values);
    break;
  case 2:
    QuadriLateralShapeQuadIntPol( isocoords[0], isocoords[1], values );
    break;
  case 3:
    QuadriLateralShapeCubicIntPol( isocoords[0], isocoords[1], values );
    break;
  default:
    assert(false);
  }

  IElement::TDoubleVec vcRet(NrOfNodes());
  int i;
  for(i = 0; i < NrOfNodes(); i++) vcRet[i] = values[i];

  delete[] values;

  return vcRet;
}

CMatrix IQuadrilateral::ShapeFunctionDerived(const IElement::TDoubleVec& isocoords) const
{
  assert(isocoords.size() == 2);
  assert(isocoords[0] >= -1.0 && isocoords[0] <= 1.0);
  assert(isocoords[1] >= -1.0 && isocoords[1] <= 1.0);

  // 2 rows, NrOfNodes() columns
  CMatrix ret(2, NrOfNodes());

  double *p = new double[2 * NrOfNodes()];

  switch(Order())
  {
  case 1:
    DerivedQuadriLateralShape(isocoords[0], isocoords[1], 0, p);
    break;
  case 2:
    DerivedQuadriLateralShapeQuadIntPol( isocoords[0], isocoords[1], p );
    break;
  case 3:
    DerivedQuadriLateralShapeCubicIntPol( isocoords[0], isocoords[1], p );
    break;
  default:
    assert(false);
  }

  double *v = p;

  int i;
  int j;
  for(j = 0; j < NrOfNodes(); j++)
  {
    for(i = 0; i < 2; i++)
    {
      ret.Value(i, j, *(v++));
    }
  }

  delete[] p;
  return ret;
}

std::vector<IElement::TDoubleVec> IQuadrilateral::IsoCoordinates() const
{
  std::vector<IElement::TDoubleVec> ret;

  switch(Order())
  {
  case 1:
    // node 1
    ret.push_back(MakeVec(-1, -1));
    // node 2
    ret.push_back(MakeVec( 1, -1));
    // node 3
    ret.push_back(MakeVec( 1,  1));
    // node 4
    ret.push_back(MakeVec(-1,  1));
    break;
  case 2:
    // node 1
    ret.push_back(MakeVec(-1, -1));
    // node 2
    ret.push_back(MakeVec( 0, -1));
    // node 3
    ret.push_back(MakeVec( 1, -1));
    // node 4
    ret.push_back(MakeVec( 1,  0));
    // node 5
    ret.push_back(MakeVec( 1,  1));
    // node 6
    ret.push_back(MakeVec( 0,  1));
    // node 7
    ret.push_back(MakeVec(-1,  1));
    // node 8
    ret.push_back(MakeVec(-1,  0));
    break;
  case 3:
    // node 1
    ret.push_back(MakeVec(-1,    -1   ));
    // node 2
    ret.push_back(MakeVec(-1./3, -1   ));
    // node 3
    ret.push_back(MakeVec( 1./3, -1   ));
    // node 4
    ret.push_back(MakeVec( 1,    -1   ));
    // node 5
    ret.push_back(MakeVec( 1,    -1./3));
    // node 6
    ret.push_back(MakeVec( 1,     1./3));
    // node 7
    ret.push_back(MakeVec( 1,     1   ));
    // node 8
    ret.push_back(MakeVec( 1./3,  1   ));
    // node 9
    ret.push_back(MakeVec(-1./3,  1   ));
    // node 10
    ret.push_back(MakeVec(-1,     1   ));
    // node 11
    ret.push_back(MakeVec(-1,     1./3));
    // node 12
    ret.push_back(MakeVec(-1,    -1./3));
    break;
  default:
    assert(false);
  }

  return ret;
}

double IQuadrilateral::GetDepthAt(const double& x, const double& y) const
{
  // derive xi and eta from x and y
  IElement::TDoubleVec isocoords(2);

  if(
  (Order() == 1) &&
    (Point(0).X() == Point(3).X() || Point(0).X() == Point(1).X()) &&
    (Point(1).X() == Point(2).X() || Point(2).X() == Point(3).X()) &&
    (Point(0).Y() == Point(1).Y() || Point(0).Y() == Point(3).Y()) &&
    (Point(2).Y() == Point(3).Y() || Point(2).Y() == Point(1).Y()))
  {
  	// implementation for 1st order rectangular shaped quads
    if(Point(0).X() == Point(3).X())
    {
      assert(x >= Point(0).X() && x <= Point(1).X());
      isocoords[0] = 2.0 * (x - Point(0).X()) / (Point(1).X() -
              Point(0).X()) - 1.0;
      isocoords[1] = 2.0 * (y - Point(0).Y()) / (Point(3).Y() -
              Point(0).Y()) - 1.0;
    }
    else // Point(0).X() == Point(1).X()
    {
      assert(x >= Point(1).X() && x <= Point(2).X());
      isocoords[0] = 2.0 * (y - Point(0).Y()) / (Point(1).Y() -
              Point(0).Y()) - 1.0;
      isocoords[1] = 2.0 * (x - Point(1).X()) / (Point(2).X() -
              Point(1).X()) - 1.0;
    }
  }
  else
  {
  isocoords = WorldToIso(geo::CPoint(x, y));
  }

  int i;
  std::vector<CValue> values(4);
  for(i = 0; i < 4; i++) values[i] = Point(i).Z();

  CValue val = InterpolateValue(isocoords, values);
  assert(val.Valid());

  return val.Value();
}

int IQuadrilateral::IntegrationPointSize() const
{
  return (int)IntegrationPoints().size();
}

const IElement::TDoubleVec& IQuadrilateral::IntegrationPointCoords(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < IntegrationPointSize());
  return IntegrationPoints()[nIndex].first;
}

const double& IQuadrilateral::IntegrationPointWeight(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < IntegrationPointSize());
  return IntegrationPoints()[nIndex].second;
}

IFace::TLineVec IQuadrilateral::Intersection(const IPlane &plane) const
{
  AssertValid();
  TLineVec vcRet;

  //typedef std::set<geo::CPoint> TPointSet;
  typedef CCoordinateSet<geo::CPoint> TPointSet;
  TPointSet stPoint;
  for(int i = 0; i < NrOfLines(); i++)
  {
    if(plane.IsParallel(Line(i)))
    {
      if(plane.Distance(Line(i).First()) < EPS)
        vcRet.push_back(Line(i));
    }
    else
    {
      CPoint p = plane.Intersection(Line(i));
      if(!p.Empty())
      {
        if(Line(i).Contains(p))
          stPoint.insert(p);
      }
    }
  }

  if( stPoint.size() == 2 )
  {
    TPointSet::iterator it = stPoint.begin();
    geo::CPoint first = *it;
    it++;
    vcRet.push_back(geo::CLine(first, *it));
  }

  return vcRet;
}

CPoint IQuadrilateral::Intersection(const ILine &line) const
{
  CPoint ret;
  CTriangle triangle1(Node(0), Node(1), Node(2));
  ret = triangle1.Intersection(line);
  if(ret.Empty()) {
    CTriangle triangle2(Node(2), Node(3), Node(0));
    ret = triangle2.Intersection(line);
  }

  if(!ret.Empty())
  {
  // get the z coordinate of the quad at the x y position of the found point
  double z = GetDepthAt(ret.X(), ret.Y());

  // perform max 10 iterations
  int maxiter = 10;
  while(maxiter > 0 && fabs(ret.Z() - z) > EPS)
  {
      // project the point on the quad
      ret.Z(z);

      // project on the line
      ret = line.Project(ret);

      // get the z coordinate of the quad at the new xy position
      z = GetDepthAt(ret.X(), ret.Y());
      --maxiter;
  }
  }

  return ret;
}

} // namespace geo
