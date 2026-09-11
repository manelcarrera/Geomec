 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IFace.cpp: implementation of the IFace class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <cmath>
#include "IFace.h"
#include "Plane.h"
#include "Matrix.h"
#include "Line.h"
#include <tbb/spin_rw_mutex.h>

namespace {

// Lock for IFace's line cache. We lock on (this >> 6 & 0xf) to spread the mutexes a bit, without giving a mutex to each IFace.
// The latter is not only to reduce space, but also because mutexes can't be copied, and we have many derived classes, which
// would need to call the currently non-existent IFace copy constructor... more trouble than it's worth.

tbb::spin_rw_mutex myGlobalIFaceMutex[16];

}


namespace geo {

//////////////////////////////////////////////////////////////////////
// Implementation of CFaceLine
//////////////////////////////////////////////////////////////////////

IFace::CFaceLine::CFaceLine(const IFace& parent, int nIndex)
: m_nIndex(nIndex), m_parent(parent)
{
}

size_t IFace::CFaceLine::NrOfNodes() const
{
	return m_parent.NrOfLineNodes();
}

const INode& IFace::CFaceLine::Node(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfNodes());
	return m_parent.Node(FaceNode(nIndex));
}

int IFace::CFaceLine::NrOfPoints() const
{
	return m_parent.NrOfLinePoints();
}

const IPoint& IFace::CFaceLine::Point(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfPoints());
	return m_parent.Point(FacePoint(nIndex));
}

void IFace::CFaceLine::Node(int /*nIndex*/, const IPoint& /*point*/)
{
	assert(false);
//	assert(nIndex >= 0 && nIndex < NrOfNodes());
//	m_parent.Node(FaceNode(nIndex), point);
}

size_t IFace::CFaceLine::Order() const
{
	return m_parent.Order();
}

int IFace::CFaceLine::PointIndex(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfPoints());
	return m_parent.PointIndex(FacePoint(nIndex));
}

const IElementSet* IFace::CFaceLine::IndexingElementSet() const
{
	return 0;
}

int IFace::CFaceLine::FacePoint(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfPoints());
	return m_parent.LinePointIndex(m_nIndex, nIndex);
}

int IFace::CFaceLine::FaceNode(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfNodes());
	return m_parent.LineNodeIndex(m_nIndex, nIndex);
}

//////////////////////////////////////////////////////////////////////
// Implementation of CPlanePointLess
//////////////////////////////////////////////////////////////////////

bool IFace::CPlanePointLess::operator()(const geo::CPoint& p1, const geo::CPoint& p2)
{
	if(m_vector.Empty())
		m_vector = geo::CVector(p1, p2);

	if(p1 == p2)
		return false;

	geo::CPlane plane(p1, m_vector);
	return plane.SignedDistance(p2, false) < 0;
}

//////////////////////////////////////////////////////////////////////
// Implementation of IFace
//////////////////////////////////////////////////////////////////////


IFace::IFace()
{

}

IFace::~IFace()
{
	DestroyLineCache();
}

bool IFace::IsSelfIntersecting() const
{
	AssertValid();
	//assert(IsPlanar());

	for(int i = 0; i < NrOfPoints(); i++)
	{
		CPoint p1 = Point(i);
		CPoint p2 = Point((i + 1) % NrOfPoints());
		CLine l1  = CLine(p1, p2);

		for (int j = i + 2; j < NrOfPoints() ; j++)
		{
			CPoint p3 = Point(j);
			CPoint p4 = Point((j + 1) % NrOfPoints());

			CLine l2 = CLine(p3, p4);

			CPoint pi = l1.Intersection(l2);

			if (!pi.Empty() && (!(pi == p1) && !(pi == p2) && !(pi == p3) && !(pi == p4)))
			{
				if(l1.Contains(pi) && l2.Contains(pi))
				{
					return true;
				}
			}
		}
	}

	return false;

}

IFace::TLineVec IFace::Intersection(const IPlane &plane) const
{
	AssertValid();
	TLineVec vcRet;

	typedef std::set<geo::CPoint> TPointSet;
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

	TPointSet::iterator it = stPoint.begin();
	while(it != stPoint.end())
	{
		// Take first point
		geo::CPoint first = *it;
		geo::CPoint second;
		bool bStop = false;
		do {
			geo::CPoint previous = *it;
			it++;
			if(it != stPoint.end())
			{
				if(Contains((*it + previous)/2.0, true))
					second = *it;
				else
					bStop = true;
			}
		} while(!bStop && (it != stPoint.end()));

		if(!second.Empty())
			vcRet.push_back(geo::CLine(first, second));
	}


	//No intersection found, return..
	return vcRet;
}

bool IFace::Intersects(const IPlane& plane) const
{
	double dprev = 0.0;
	int i;
	for(i = 0; i < NrOfPoints(); i++)
	{
		double d = plane.SignedDistance(Point(i));
		// EPS is not used, because we want real zeros
		if(d == 0.0) return true; // exactly on it

		if(i)
		{
			// it's either intersecting (d < 0 && dprev > 0 or v.v.)
			// or it's hitting (d = 0 || dprev = 0)
			// EPS is not used, because we want real zeros
			if(d * dprev <= 0) return true;
		}
		dprev = d;
	}

	return false;
}

CPoint IFace::Intersection(const ILine &line) const
{
	AssertValid();
	//assert(IsPlanar());

	//get a plane parallel to me
	CPlane plane(Point(0), Point(1), Point(2));
	CPoint point = plane.Intersection(line);

	//if the line is paralell to the plane
	if(point.Empty())
		return point;

	//if the intersection is inside or on the face
	if(Contains(point,true))
		return point;

	//the intersection is outside the face
	return CPoint();
}

double IFace::SignedArea() const
{
  AssertValid();
  //assert(IsPlanar());

  // Rotate the points towards the XY plane. Retrieve the rotation angle to
  // correct if necessary the sign of the area.
  CArray<CPoint> arr;
  double angle = GetPointArrInPlane(CPlane::PlaneXY, arr );

  // Signed-area equation from:
  // 'Computational Geometry' 2nd ed. - J. O'Rourke, eq. 1.13, page 21
  const size_t nr = arr.Size();
  double area=0.0;
  for(size_t i = 0; i < nr; i++)
  {
    const CPoint& pt1( arr.Object( i ) );
    const CPoint& pt2( arr.Object( (i+1) % nr ) );

    double x1 = pt1.X();
    double x2 = pt2.X();

    double y1 = pt1.Y();
    double y2 = pt2.Y();
    area += ( x1 + x2 ) * ( y2 - y1 );
  }
  area *= 0.5;

  // The signed area of a polygon face is positive if the points of the face are
  // ordered in counter-clockwise direction, when looking at the face from the
  // 'tip' of the normal to the face. If the rotation angle in
  // GetPointArrInPlane() was 180 degrees, the points were not modified, so
  // the loop above has  walked the points in the opposite direction.
  if ( fabs( angle -180.0 ) < EPS )
    area *= -1.0;

  return area;
}

int IFace::NrOfLines() const
{
	return NrOfPoints();
}

void IFace::CreateLineCache() const
{
	assert( m_vcLine.size() == 0 );
	int i;
	for(i = 0; i < NrOfLines(); i++)
		m_vcLine.push_back(new CFaceLine(*this, i));
}

void IFace::DestroyLineCache() const
{
	for(int i = 0; i < m_vcLine.size(); i++)
		delete m_vcLine[i];
	m_vcLine.clear();
}

const ILine& IFace::Line(int nIndex) const
{
  AssertValid();

  {
    int index = (intptr_t)this >> 6 & 0xf;

    tbb::spin_rw_mutex::scoped_lock lock(myGlobalIFaceMutex[index], false);

    if (m_vcLine.empty())
    {
      lock.upgrade_to_writer();

      if (m_vcLine.empty()) CreateLineCache();
    }
  }

	assert(nIndex >= 0 && nIndex < m_vcLine.size());
	return *m_vcLine[nIndex];
}

size_t IFace::NrOfLinePoints() const
{
	return Order()+1;
}

int IFace::LinePointIndex(int nLineIndex, int nPointIndex) const
{
	assert(nLineIndex >= 0 && nLineIndex < NrOfLines());
  assert( nPointIndex >=0 && nPointIndex < NrOfLinePoints() );
  return ( Order() * nLineIndex + nPointIndex ) % NrOfPoints();
}

int IFace::LinePointIndex(const ILine &line, int nPointIndex) const
{
	int i;
	for(i = 0; i < NrOfLines(); i++)
	{
		if(&Line(i) == &line)
			return LinePointIndex(i, nPointIndex);
	}

	assert(false);
	return -1;
}

size_t IFace::NrOfLineNodes() const
{
	if(NrOfNodes() == 0)
		return 0;
	return Order() + 1;
}

int IFace::LineNodeIndex(int nLineIndex, int nNodeIndex) const
{
    assert(nLineIndex >= 0 && nLineIndex < NrOfLines());
    assert(nNodeIndex >= 0 && nNodeIndex < NrOfLineNodes());

    return ( Order() * nLineIndex + nNodeIndex) % NrOfNodes();
}

int IFace::LineNodeIndex(const ILine &line, int nNodeIndex) const
{
	int i;
	for(i = 0; i < NrOfLines(); i++)
	{
		if(&Line(i) == &line)
			return LineNodeIndex(i, nNodeIndex);
	}

	assert(false);
	return -1;
}

bool IFace::IsPlanar() const
{
	AssertValid(); //NrOfPoints() > 2

	// 3 points are always planer
	if(NrOfPoints() == 3)
		return true;

	//NrOfPoints() > 3

  CPlane plane = Plane();
	assert(!plane.Empty());

	for(int j = 2; j < NrOfPoints(); j++)
	{
		if(plane.Distance(Point(j)) > EPS)
			return false;

	}
	return true;

}

bool IFace::IsConvex() const
{
	AssertValid();
	//assert(IsPlanar());

	CArray<CPoint> arr;
  GetPointArrInPlane( CPlane::PlaneXY, arr );

  size_t nr = arr.Size();
	int flag = 0;

	// a convex-test for poly 2d (x,y)
  for(size_t i = 0; i < nr; i++)
	{
    size_t j = (i + 1) % nr;
    size_t k = (i + 2) % nr;
		CPoint pi = arr.Object(i);
		CPoint pj = arr.Object(j);
		CPoint pk = arr.Object(k);

		double z;
		z = (pj.X() - pi.X()) * (pk.Y() - pj.Y());
		z -= (pj.Y() - pi.Y()) * (pk.X() - pj.X());

		if (z < 0.0)
		{
			flag |= 1;
		}
		else if (z > 0.0)
		{
			flag |= 2;
		}

		if (flag == 3)
		{
			return false;
		}
	}

	assert(flag != 0);
	return true;

}

CVector IFace::Normal() const
{
  AssertValid();
  //assert(IsPlanar());

  CVector norm = CVector::NullVector;

  // Use only the corner points
  const int step = Order();
  CPoint p0 = Point( 0 );
  CPoint p1 = Point( 1*step );
  for ( int i = 0; i < NrOfPoints(); i += step )
  {
    CPoint p2 = Point( ( (i+2)*step ) % NrOfPoints() );

    CVector v1( p0 - p1 );
    CVector v2( p1 - p2 );
    CVector n = v1.CrossProduct( v2 );
    if ( ! n.isNullVector() )
      norm = norm + n;
    p0 = p1;
    p1 = p2;
  }

  if( norm.isNullVector() )
    return CVector::NullVector;

  // Signed area is negative if nodes are oriented in clockwise direction
  if( SignedArea() < 0. )
    norm.Flip();

  return norm.UnitVector();
}

CVector IFace::NormalInPoint(const geo::IPoint &/*pt*/) const
{
	return Normal();
}

void IFace::AssertValid() const
{
	assert(NrOfPoints() > 2);
}


//copy the points of Iface in an array and rotate it around the origin
//by an angle so that after rotation the copy of IFace is parallel to the plane
// [BXA] The transformation matrices are never passed as arguments
double IFace::GetPointArrInPlane(const IPlane&   plane,
                               CArray<CPoint>& arr,
                               IMatrix*        pTM /*=NULL */,
                               IMatrix*        pTMI/*=NULL */) const
{
	AssertValid();
	//assert(IsPlanar());

	arr.Clear();
	for(int i = 0; i < NrOfPoints(); i++)
	{
    CPoint p(Point(i));
    arr.PushBack(p);
  }
	IMatrix *TM,*TMI;

	if(pTM)
		TM=pTM;
	else
		TM=new CMatrix();

	if(pTMI)
		TMI=pTMI;
	else
		TMI=new CMatrix();

  double angle = GetTransMatInPlane(plane,*TM,*TMI);

	arr.Transform(*TM);

	if(!pTM)
		delete TM;

	if(!pTMI)
		delete TMI;

  return angle;
}

//ignore Normal direction
CPlane IFace::Plane() const
{
	AssertValid();
	//assert(IsPlanar());

  // Only use the corner points.
  CPlane plane;
  const int stride = Order();
  CPoint p0( Point( 0 ) );
  CPoint p1( Point( 1*stride ) );
  for( int i = 2*stride; i < NrOfPoints(); i+= stride )
  {
    CPoint p2( Point( i ) );
    if( ! CPoint::CoLinear( p0, p1, p2 ) )
    {
      plane = CPlane( p0, p1, p2 );
      break;
    }
  }
	return plane;
}

double IFace::GetTransMatInPlane(const IPlane& plane, IMatrix& TM, IMatrix& TMI) const
{
  // [BXA] Consider passing IMatrix ptrs to those matrices are are needed.
  // For the SignedArea the inverse transformation is calculated here, but
  // the matrix is not used

  AssertValid();
  //assert(IsPlanar());

  CVector n1       = Plane().Normal();    //get the normal vector
  CVector n2       = plane.Normal();      //get the normal vector
  CVector RotVec   = n1.CrossProduct(n2); //get the rotation vector
  double RotAngle  = n1.AngleDeg(n2);     //get the rotation angle

  //get rotation matrix
  if( ! RotVec.isNullVector() )
  {
    RotVec.GetRotationMatrix(       RotAngle, TM );
    RotVec.GetRotationMatrix(-1.0 * RotAngle, TMI);
  }
  else // angle == 0 || angle == 180.0
  {
    TM.CreateIdentityMatrix( 4 );
    TMI = TM;
  }

  CPoint p = Point(0);
  p.Transform(TM);

  //get translation matrix if needed
  if(plane.Distance(p) > EPS)
  {
    CMatrix tmp;
    tmp.CreateIdentityMatrix(4);
    CPoint pp = plane.Project(p);
    CVector trans = CVector(pp-p);

    tmp.Value(0, 3, trans.X());
    tmp.Value(1, 3, trans.Y());
    tmp.Value(2, 3, trans.Z());
    TM = tmp*TM; //mult. trans*rot

    trans.Flip(); //inverse
    tmp.Value(0, 3, trans.X());
    tmp.Value(1, 3, trans.Y());
    tmp.Value(2, 3, trans.Z());
    TMI = TMI*tmp; //mult. rot*trans
  }
  return RotAngle;
}

// sign of return value depends on direction (implmentation) of Normal().
double IFace::SignedDistance(const IPoint &point, bool bExact) const
{
	CPlane faceplane(Line(0).First(), Normal());
	return faceplane.SignedDistance(point, bExact);
}


bool IFace::dblCmpLt(const double &par1, const double &par2) const
{
	return ((par1 - par2) > EPS);
}

bool IFace::dblCmpEq(const double &par1,const double &par2) const
{
	bool b1;
	bool b2;
	b1 = dblCmpLt(par1, par2);
	b2 = dblCmpLt(par2, par1);
	return !(b1 || b2);
}

bool IFace::dblCmpGt(const double &par1, const double &par2) const
{
	return ((par2 - par1) > EPS);
}

bool IFace::dblCmpLE(const double &par1, const double &par2) const
{
	return (dblCmpLt(par1, par2) || dblCmpEq(par1, par2));
}

bool IFace::dblCmpGE(const double &par1, const double &par2) const
{
	return (dblCmpGt(par1, par2) || dblCmpEq(par1, par2));
}

bool IFace::PointInPoly(const IPoint& point, std::vector<IPoint*>& points) const
{
	// see if the point lies in the polygon defined by the points
	// compute the total angle of the triangles spanned by the
	// given point and each line segment
	// if this angle is around 360 degrees (or 2*PI), the point is
	// inside the polygon

	// points must all be in the Z = 0 plane
	assert(fabs(point.Z()) < compareTolerance());

	double totalangle = 0;

	int i, j;
	for(j = 0, i = (int)points.size() - 1; j < points.size(); i = j++) {
		assert(fabs(points[j]->Z()) < compareTolerance());
		geo::CVector vi = *points[i] - point;
		geo::CVector vj = *points[j] - point;
		geo::CVector vc = vi.CrossProduct(vj);

		// only if the resulting vector is not a null-vector
		// this adds to the resulting angle
		if(fabs(vc.Z()) > EPS)
		{
			double cosa = vi.UnitVector().DotProduct(vj.UnitVector());
			assert(cosa >= -1 && cosa <= 1);
			double sign = (vc.Z() > 0 ? 1 : -1);

			// angle in radians
			double a = sign * acos(cosa);

			// add it to the total angle
			totalangle += a;
		}
	}

	return (fabs(totalangle - 2 * PI) < EPS);
}

bool IFace::ContainsInXYPlane(const IPoint &point, bool bIncludeEdge) const
{
	CPoint *pPoint = new CPoint[NrOfPoints()];

	int i;
	for(i = 0; i < NrOfPoints(); i++)
		pPoint[i] = CPoint(Point(i).X(), Point(i).Y(), 0);

	geo::CPoint xy_point(point.X(), point.Y(), 0);

	for(i = 0; i < NrOfPoints(); i++)
	{
		if(pPoint[i] == xy_point)
		{
			delete [] pPoint;
			return bIncludeEdge;
		}
	}

   for(i = 0; i < NrOfPoints(); i++)
   {
		CLine l(pPoint[i], pPoint[(i + 1)%NrOfPoints()]);
		if(l.Contains(xy_point))
		{
			delete [] pPoint;
			return bIncludeEdge;
		}
	}


   double anglesum = 0;
   for(i = 0; i < NrOfPoints(); i++)
   {
     CVector v1( xy_point, pPoint[i] );
     CVector v2( xy_point, pPoint[(i+1)%NrOfPoints()] );
     // 2D angle in Z=0 plane, having a sign; so not using AngleRad()
     double theta1 = atan2( v1.Y(), v1.X() );
     double theta2 = atan2( v2.Y(), v2.X() );
     double dtheta = theta2 - theta1;
     while ( dtheta >  PI ) dtheta -= TWOPI;
     while ( dtheta < -PI ) dtheta += TWOPI;
     anglesum += dtheta;
   }

   delete [] pPoint;
   return (fabs(fabs(anglesum) - TWOPI) < EPS);
}

bool IFace::Contains(const IPoint &point, bool bIncludeEdge) const
{
  if ( fabs( SignedDistance( point, true ) ) > EPS ) return false;

  // For most of the algorithms above there is a pathological case if the point
  // being queries lies exactly on a vertex. The easiest way to cope with this
  // is to test that as a separate process and make your own decision as to
  // whether you want to consider them inside or outside.
	// Is the point on the edge of the face?
	int i;
	for(i = 0; i < NrOfPoints(); i++)
	{
		CLine l( Point(i), Point( (i+1)%NrOfPoints() ) );
		if ( l.Contains( point ) ) return bIncludeEdge;
	}

	// Do the contains ...
  // This solution was motivated by solution 2 and correspondence with Reinier
  // van Vliet and Remco Lam. To determine whether a point is on the interior
  // of a convex polygon in 3D one might be tempted to first determine whether
  // the point is on the plane, then determine it's interior status.
  // Both of these can be accomplished at once by computing the sum of the
  // angles between the test point (q below) and every pair of edge points
  // p[i]->p[i+1]. This sum will only be twopi if both the point is on the plane
  // of the polygon AND on the interior.
  // The angle sum will tend to 0 the further away from the polygon point q
  // becomes.
  // The following code snippet returns the angle sum between the test point q
  // and all the vertex pairs. Note that the angle sum is returned in radians.

  // Note that the above original comment mentions convexity, but the code below
  // is extended to nonconvex polygons by comparing the vector planes. If out of
  // plane, false is returned, if in plane, the sign is adjusted to the
  // orientation, thus getting nonconvex coverage too.

   double anglesum = 0;
   CVector faceNormal;
   for(i = 0; i < NrOfPoints(); i++)
   {
     CVector v1( point, Point(i) );
     CVector v2( point, Point((i+1)%NrOfPoints()) );

     CVector normal = v1.CrossProduct(v2);
     if ( normal.isNullVector() ) continue; // No contribution to angle

     if ( faceNormal.Empty() ) faceNormal = normal; // To compare if one plane

     if ( faceNormal.IsParallel( normal ) ) {
       // Same plane, get angle contribution
       if ( faceNormal.IsOppositeDirection( normal ) )
         anglesum -= v1.AngleRad(v2);
       else
         anglesum += v1.AngleRad(v2);
     } else {
       // Not in same plane
       return false;
     }
   }
   return ( fabs( fabs( anglesum ) - TWOPI) < EPS );
}

// return a representative length for this element
// length of a line
// square root of area for a face
// 3rd square root of volume for a body
double IFace::RepresentativeLength() const
{
	double vol = Area();
	assert(vol >= 0);

	return sqrt(vol);
}

void IFace::InvalidateCache()
{
	DestroyLineCache();
}

}
