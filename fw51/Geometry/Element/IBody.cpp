 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IBody.cpp: implementation of the IBody class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include<cmath>
#include "Point.h"
#include "Vector.h"
#include "ConvexHull.h"
#include "Plane.h"
#include "Polygon.h"
#include "LinkedLine.h"

namespace polylib {
#include "PhPolyLib.h"
#include "PhTypes.h"
#include "PhPolyhedron.h"
#include "PhMatrix.h"
#include "PhPolyParam.h"
}

#define MAX_RAYS 200 // the maximum number of rays allowed in polylib

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

IBody::IBody()
{
}

IFace& IBody::Face(int nIndex)
{
  const IBody *pConstThis = this;
  const IFace& face = pConstThis->Face(nIndex);
  return const_cast<IFace&> (face);
}

// This function checks for the body if it intersects with the plane.
// It does this by assessing for each point in the body on which side of the plane the point is situated.
// If in the body two points are situated on separate sides of the plane the plane has to intersect with the 
// body. If a point is on the plane, this is seen as intersection. Return "true"if Intersects, false if not.
bool IBody::Intersects(const IPlane &plane, bool bIncludeEdge /* = true */) const
{
  int    i;
  int    j;
  double distance = 0;
  bool   side = false;

  for(i = 0; i < NrOfFaces(); i++)
  {
    for(j = 0; j < Face(i).NrOfPoints(); j++)
    {
      distance = plane.SignedDistance(Face(i).Point(j));
      
      if(fabs(distance) < EPS)
      { // point is on plane, plane intersects..
        return bIncludeEdge;
      }

      if(i == 0 && j == 0)
      { // first point, set the side...
        side = (distance > 0);
      }
      else
      { // set the bool with which side will be compared...
        bool comp = (distance > 0);

        if(comp != side)
        { // point is on other side of plane, plane intersects..
          return true;
        }
      }
    }
  }
  return false;
}

CPolygon IBody::IntersectionConv(const IPlane &plane) const
{
  assert(Order() == 1); // the linked line is only implemented for linear elements

  CLinkedLine line;
  if(Intersects(plane, true))
  {
    for(int i = 0; i < NrOfFaces(); i++)
    {
      // We make an intersection and store the intersection in the line map
      IFace::TLineVec vcLine = Face(i).Intersection(plane);
      for(int j = 0; j < vcLine.size(); j++)
      {
        line.AddLine(vcLine[j].First(), vcLine[j].Second());
        // assert(Contains(vcLine[j].First(), true));
        // assert(Contains(vcLine[j].Second(), true));
      }
    }

    // We fill up the polygon in the right order
    if(line.Valid())
    {
      CPolygon ret;
      double cosalpha =
        plane.Normal().UnitVector().DotProduct(line.Normal().UnitVector());
      if(cosalpha > 0)
      {
        for(int i = 0; i < line.NrOfPoints(); i++)
          ret.PushBack(line.Point(i), true);

      }
      else
      {
        for(int i = 0; i < line.NrOfPoints(); i++)
          ret.PushBack(line.Point(line.NrOfPoints() - i - 1));
      }
      
      // assert(plane.Normal() == ret.Normal());
      assert(ret.NrOfPoints() > 2);
      if(ret.NrOfPoints() > 2)
        return ret;

    }
  }
  return CPolygon();
}

// When intersecting a body with a plane, you alway get the resulting lines (LineSet, this is required in
// IVolume where this function is called for each of its bodies), and if you supply a non 0 pointer for the
// pointset, this is filled  with the points describing the intersection.
void IBody::Intersection(const IPlane &plane, std::set<CLine, ILine::CLineLess> &LineSet, std::set<CPoint> *PointSet /* = 0 */) const
{
  if(Intersects(plane, true))
  {
    for(int i = 0; i < NrOfFaces(); i++)
    {
      IFace::TLineVec vcLine = Face(i).Intersection(plane);
      for(int j = 0; j < vcLine.size(); j++)
      {
        LineSet.insert(vcLine[j]);
        if(PointSet)
        {
          PointSet->insert(vcLine[j].First());
          PointSet->insert(vcLine[j].Second());
          assert(Contains(vcLine[j].First(), true));
          assert(Contains(vcLine[j].Second(), true));
        }
      }
    }
  }
  return;
}

void IBody::Intersection(const ILine &line, std::set<CPoint> &PointSet) const
{
  bool bFirstIn = Contains(line.First(), false);
  bool bSecondIn = Contains(line.Second(), false);
  int NrIntersections;

  if(bFirstIn && bSecondIn) // line is inside the body
    return;

  if(bFirstIn || bSecondIn)
    NrIntersections = 1;
  else
    NrIntersections = 2;

  int Intersections = 0;
  for(int i = 0; i < NrOfFaces(); i++)
  {
    CPoint pt = Face(i).Intersection(line);
    if(!pt.Empty())
    {
      if(line.Contains(pt, true))
      {
        PointSet.insert(pt);
        Intersections++;
      }
    }

    if(Intersections == NrIntersections)
      break; // bodies have to be convex, so this means that no more than 2 intersections with a line are possible
  }

  //assert(Intersections > 0);

  return;
}

bool IBody::Contains(const IPoint &point, bool bIncludeEdge) const
{
  // skip some obvious values
  const IPoint &min = Min();
  const IPoint &max = Max();
  if( ( point.X() < min.X() - compareTolerance() ||
        point.Y() < min.Y() - compareTolerance() ||
        point.Z() < min.Z() - compareTolerance()    ) ||
      ( point.X() > max.X() + compareTolerance() ||
        point.Y() > max.Y() + compareTolerance() ||
        point.Z() > max.Z() + compareTolerance()    )     )
  {
    return false;
  }

  for(int i = 0; i < NrOfFaces(); i++)
  {
    if(Face(i).Contains(point, true)) return bIncludeEdge;
    if(Face(i).SignedDistance(point, false) > 0) return false;
  }
  return true;
}

int IBody::FaceIndex(const IFace &face) const
{
  for(int i = 0; i < NrOfFaces(); i++)
  {
    if(&Face(i) == &face) return i;
  }

  assert(false); // does not seem to be a bodyface of this body
  return -1;
}

const IBody::TIndexVec& IBody::FacePointIndices(const IFace &face) const
{
  int nIndex = FaceIndex(face);
  assert(nIndex >= 0 && nIndex < NrOfFaces());

  return FacePointIndices(nIndex);
}

const IBody::TIndexVec& IBody::FaceNodeIndices(const IFace &face) const
{
  int nIndex = FaceIndex(face);
  assert(nIndex >= 0 && nIndex < NrOfFaces());

  return FaceNodeIndices(nIndex);
}

static polylib::Polyhedron *GetPolyhedron(const IBody &body, const geo::IPoint& ptOffset)
{
  std::vector<const IPoint*> vcPoints;

  int i;
  for(i = 0; i < body.NrOfPoints(); i++) vcPoints.push_back(&body.Point(i));

  CConvexHull hull(vcPoints);
  bool bCreated = hull.Calculate();
  assert(bCreated);

  // create constraint matrix with num_faces rows and 5 columns
  // matrix setup:
  // one row for each face's plane
  // constraint is plane equation (plane's normal is n):
  // -nx * x + -ny * y + -nz * z + c >= 0
  // c is derived by entering one of the points into the equation
  // columns:
  // 1: inequality/equality (1/0) -> always set to 1
  // 2: -nx
  // 3: -ny
  // 4: -nz
  // 5: c
  // and the above formula (plane equation) is assumed by the polyhedron library
  polylib::Matrix *MatConstraint = polylib::Matrix_Alloc(hull.NrOfFaces(), 5);

  // convex hull is offset around its midpoint, make sure to do the translation!!
  geo::CPoint ptMid(hull.MidPoint());

  for(i = 0; i < hull.NrOfFaces(); i++)
  {
    const IFace &Face = hull.Face(i);
    assert(Face.NrOfPoints() > 2);

    // use first 3 points for plane definition, assume proper winding order (normal points outward)
    CPlane pl(Face.Point(0) + ptMid - ptOffset, Face.Point(1) + ptMid - ptOffset, Face.Point(2) + ptMid - ptOffset);

    // use first point to derive constant
    double C = pl.Normal().X() * (Face.Point(0).X() + ptMid.X() - ptOffset.X()) +
               pl.Normal().Y() * (Face.Point(0).Y() + ptMid.Y() - ptOffset.Y()) +
               pl.Normal().Z() * (Face.Point(0).Z() + ptMid.Z() - ptOffset.Z());

    MatConstraint->p[i][0] = 1;
    MatConstraint->p[i][1] = -pl.Normal().X();
    MatConstraint->p[i][2] = -pl.Normal().Y();
    MatConstraint->p[i][3] = -pl.Normal().Z();
    MatConstraint->p[i][4] = C;
  }

  polylib::Polyhedron *pRet = polylib::Constraints2Polyhedron(MatConstraint, MAX_RAYS);
  Matrix_Free(MatConstraint);

  return pRet;
}

IBody::TIntersection IBody::Intersection(const IBody &rhs) const
{
  TIntersection ret;

  // first check if bounding boxes overlap
  if(rhs.Min().X() > Max().X() ||
     rhs.Min().Y() > Max().Y() ||
     rhs.Min().Z() > Max().Z() ||
     rhs.Max().X() < Min().X() ||
     rhs.Max().Y() < Min().Y() ||
     rhs.Max().Z() < Min().Z())
  {
    // not intersecting
    ret.first = 0.0;
    return ret;
  }

  // offset by midpoint of the two bodies
  // the polyhedron library becomes less accurate with very large coordinate values
  geo::CPoint ptCenter((MidPoint() + rhs.MidPoint()) / 2);

  polylib::Polyhedron *pPolyThis = GetPolyhedron(*this, ptCenter);
  polylib::Polyhedron *pPolyRhs  = GetPolyhedron(rhs, ptCenter);

  assert(pPolyThis->NbRays != 0);
  assert(pPolyRhs->NbRays != 0);

  // intersect the two polygons
  polylib::Polyhedron *pPolyInt = polylib::DomainIntersection(pPolyThis, pPolyRhs, MAX_RAYS);

  polylib::Param_Polyhedron *pParam = polylib::GenParamPolyhedron(pPolyInt);

  polylib::Param_Vertices *pVert = pParam ? pParam->V : 0;

  if(!pVert)
  {
    ret.first = 0.0;
    return ret; // not intersecting
  }

  // generate convex hull of new polygon
  std::vector<const geo::IPoint*> vcPoints;
  while(pVert)
  {
    polylib::Matrix *MatVert = pVert->Vertex;
    double dX = MatVert->p[0][0] / MatVert->p[0][1];
    double dY = MatVert->p[1][0] / MatVert->p[1][1];
    double dZ = MatVert->p[2][0] / MatVert->p[2][1];

    // these points are only used for the volume calculation, no need to translate to origin
    vcPoints.push_back(new geo::CPoint(dX, dY, dZ));

    // revert to system origin
    ret.second.push_back(geo::CPoint(dX + ptCenter.X(), dY + ptCenter.Y(), dZ + ptCenter.Z()));

    pVert = pVert->next;
  }

  // free polyhedron objects
  Param_Polyhedron_Free(pParam);
  Domain_Free(pPolyInt);
  Polyhedron_Free(pPolyRhs);
  Polyhedron_Free(pPolyThis);

  if(vcPoints.size() < 4)
  {
    // not intersecting
    ret.first = 0.0;
    return ret;
  }

  geo::CConvexHull hull(vcPoints);

  bool bHullOk;
  try
  {
    bHullOk = hull.Calculate();
  }
  catch(const char*)
  {
    bHullOk = false;
  }

  if(!bHullOk)
  {
    ret.first = 0.0;
    return ret; // not intersecting
  }

  ret.first = hull.Volume();

  for(std::vector<const geo::IPoint*>::iterator it = vcPoints.begin(); it != vcPoints.end(); it++)
  {
    delete *it;
  }

  return ret;
}

// return a representative length for this element
// length of a line
// square root of area for a face
// 3rd square root of volume for a body
double IBody::RepresentativeLength() const
{
  double dRet = Point(1).Distance(Point(0));
  if(NrOfPoints() < 2)
  {
    double vol = Volume();
    assert(vol >= 0);

    dRet = pow(vol, 1.0 / 3.0);
  }

  return dRet;
}

const IBody::TIndexVec& IBody::LinePointIndices(const ILine& line) const
{
  int i;
  for(i = 0; i < NrOfLines(); i++)
  {
    if(&Line(i) == &line)
      return LinePointIndices(i);
  }

  assert(false);
  TIndexVec *pBogus = 0;
  return *pBogus;
}

const IBody::TIndexVec& IBody::LineNodeIndices(const ILine &line) const
{
  int i;
  for(i = 0; i < NrOfLines(); i++)
  {
    if(&Line(i) == &line)
      return LineNodeIndices(i);
  }

  assert(false);
  TIndexVec *pBogus = 0;
  return *pBogus;
}

const char* IBody::SideName(int nIndex) const
{
  return FaceName(nIndex);
}

bool IBody::IsBody() const
{
  return true;
}

}
