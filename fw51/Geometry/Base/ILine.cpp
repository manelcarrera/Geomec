 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ILine.cpp: implementation of the ILine class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"

#include "Vector.h"
#include "Point.h"
#include "Matrix.h"
#include "VectorTempl.h"
#include "IElementSet.h"
#include "IValue.h"
#include "Value.h"
#include "IProgressBase.h"
#include "IValueSet.h"
#include "ILine.h"

#include "Line.h"
#include "lbel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

void ILine::BuildIntegrationPoints(IElement::TIntPtVec& vec, int numint)
{
	int ixi;
	double xxi;
	double wxi;

	for(ixi = 0; ixi < numint; ixi++)
	{
		GaussCoeff(ixi + 1, numint, &xxi, &wxi);
		std::pair<IElement::TDoubleVec, double> prGaussp;
		prGaussp.first.push_back(xxi);
		prGaussp.second = wxi;
		vec.push_back(prGaussp);
	}
}

const IElement::TIntPtVec& ILine::IntegrationPoints( int order )
{
    // integration point location and weights
    static IElement::TIntPtVec s_LinearIntegrationPoints;
    static IElement::TIntPtVec s_QuadIntegrationPoints;
    static IElement::TIntPtVec s_CubicIntegrationPoints;
    static IElement::TIntPtVec s_4thOrderIntegrationPoints;

    switch( order )	{
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
	case 4:
		if(s_4thOrderIntegrationPoints.empty())
			BuildIntegrationPoints(s_4thOrderIntegrationPoints, 4);
		return s_4thOrderIntegrationPoints;
		break;
	}

	assert(false);
	TIntPtVec* pBogus = 0;
	return *pBogus;
}

const IElement::TIntPtVec& ILine::IntegrationPoints() const
{
    return IntegrationPoints( Order() );
}

void ILine::PrepareMapping()
{
  for (int i = 1; i < 5; ++i)
    IntegrationPoints(i);
}

ILine::ILine()
{
}


ILine& ILine::operator=(const ILine &rhs)
{
	if(rhs.Empty())
	{
		First(CPoint());
		Second(CPoint());
		assert(Empty());
	}
	else
	{
		First(rhs.First());
		Second(rhs.Second());
		assert(!Empty());
	}

	return *this;	
}

double ILine::SquareLength() const
{
	return First().SquareDistance(Second());
}

double ILine::Size() const
{
	if(NrOfNodes()) return IElement::Size(); // a real mesh element

	// just a simple line
	assert(Order() == 1);

	return First().Distance(Second());
}

//##ModelId=3B83AE72000E
bool ILine::IsParallel(const ILine &line) const
{
	CVector v1 = CVector(Second() - First());
	CVector v2 = CVector(line.Second() - line.First());
	return v1.CrossProduct(v2).isNullVector();
}

bool ILine::IsConnected(const ILine &line, int &nIndex1, int &nIndex2) const
{
	//if lines are equal,no connection
	if(*this == line)
		return false;

	if(First() == line.First())
	{
		nIndex1 = 0;
		nIndex2 = 0;
		return true;
	}

	if(First() == line.Second())
	{
		nIndex1 = 0;
		nIndex2 = 1;
		return true;
	}

	if(Second() == line.First())
	{
		nIndex1 = 1;
		nIndex2 = 0;
		return true;
	}

	if(Second() == line.Second())
	{
		nIndex1 = 1;
		nIndex2 = 1;
		return true;
	}

	//the lines share no common vertex
	return false;
}


//##ModelId=3B4C71CF018A
bool ILine::Contains(const IPoint &pt, bool bIncludeEdge) const
{
	if(Distance(pt) > EPS)
	{
		//vertex not on the line
		return false;
	}

	double l1 = First().Distance(pt);
	double l2 = Second().Distance(pt);

	if( fabs(l1) < EPS || fabs(l2) < EPS )
	{
		//vertex is on first or second point
		return bIncludeEdge;
	}

	double l = Length();

	if(fabs(l1+l2-l)<EPS)
		return true;

	return false;
	//return( !((l1 + l2) > l) );

}


//##ModelId=3B4C71CF017A
double ILine::Distance(const IPoint &pt) const
{
	CPoint pr = Project(pt);
	return pr.Distance(pt);
}

double ILine::SquareDistance(const IPoint& pt) const
{
	CPoint pr = Project(pt);
	return pr.SquareDistance(pt);
}

//##ModelId=3B4C71CF015C
CPoint ILine::Project(const IPoint &pt) const
{

	CVector v = CVector(Second() - First());
	CVector w = CVector(pt - First());

	double c1 = w.DotProduct(v);
	double c2 = v.DotProduct(v);

	if (fabs(c2) < EPS)
		return CPoint(First().X(), First().Y(), pt.Z());

	double b = c1 / c2;
	return First() + CPoint(v * b);
}


//##ModelId=3A90DFD401DE
CPoint ILine::Max() const
{
	return First().Max(Second());
}



//##ModelId=3A90DFD401D0
CPoint ILine::Min() const
{
	return First().Min(Second());
}

//##ModelId=3B4C71CF014E
CPoint ILine::Intersection(const ILine &line) const
{

	if (IsParallel(line))
		return CPoint();

    CVector   u = Second() - First();
    CVector   v = line.Second() - line.First();
    CVector   w = First() - line.First();

    double    a = u.DotProduct(u);        // always >= 0
    double    b = u.DotProduct(v);
    double    c = v.DotProduct(v);        // always >= 0
    double    d = u.DotProduct(w);
    double    e = v.DotProduct(w);
    double    D = a * c - b * b;              // always > 0

    double sc = (b * e - c * d) / D;
    double tc = (a * e - b * d) / D;

	CPoint pa = First()      + u * sc;
	CPoint pb = line.First() + v * tc;

	if (pa == pb)
		return pa;

	return CPoint();
}


bool ILine::GetClosestPoints(const ILine &line, CPoint& PointAtLine , CPoint& PointAtThis) const
{

	if (IsParallel(line))
	{
		return false;
	}

    CVector   u = Second() - First();
    CVector   v = line.Second() - line.First();
    CVector   w = First() - line.First();

    double    a = u.DotProduct(u);        // always >= 0
    double    b = u.DotProduct(v);
    double    c = v.DotProduct(v);        // always >= 0
    double    d = u.DotProduct(w);
    double    e = v.DotProduct(w);
    double    D = a * c - b * b;              // always > 0

    double sc = (b * e - c * d) / D;
    double tc = (a * e - b * d) / D;

	PointAtThis = First()      + u * sc;
	PointAtLine = line.First() + v * tc;

	return true;

}


CVector ILine::Dir() const
{
	return CVector(Second() - First());
}

bool ILine::operator==(const ILine &rhs) const
{
	//First() and Second() must match
	//this means lines are not equal if First() and Second are reversed

	if(!(*this < rhs) && !(rhs < *this))
		return true;

	return false;	
}

bool ILine::IsEqual(const ILine &rhs) const
{
	//order independace compare
	if(rhs == *this)
		return true;

	return (rhs.Second() == First() && rhs.First() == Second()) ;

}

void ILine::GetRotationMatrix(double AngleDeg, IMatrix& matrix) const
{

	CMatrix T(4, 4);
	CMatrix Ti(4, 4);
	
	T.Value(3, 0,  -First().X());
	T.Value(3, 1,  -First().Y());
	T.Value(3, 2,  -First().Z());

	Ti.Value(3, 0,  First().X());
	Ti.Value(3, 1,  First().Y());
	Ti.Value(3, 2,  First().Z());

	Dir().GetRotationMatrix(AngleDeg,matrix);
	matrix = T * matrix;
	matrix = matrix * Ti;
}

void ILine::Move(const IVector &vec)
{
	First(First() + vec);
	Second(Second() + vec);
}

void ILine::Transform(const IMatrix &matrix)
{
	CPoint first = First();
	CPoint second = Second();
	first.Transform(matrix);
	second.Transform(matrix);
	First(first);
	Second(second);
}

void ILine::Rotate(const IVector &vec, const double &dAngleDeg)
{
	CPoint first = First();
	CPoint second = Second();
	first.Rotate(vec , dAngleDeg);
	second.Rotate(vec , dAngleDeg);
	First(first);
	Second(second);

}

void ILine::AssertValid() const
{
	assert(!First().Empty() && !Second().Empty());
	// No assert on difference between first and second point,
	// Diana 2-node spring elements also have line shape, but zero length.
}

bool ILine::Empty() const
{
	return First().Empty() || Second().Empty();
}

const IPoint &ILine::First() const
{
	return Point(0);
}

void ILine::First(const IPoint& point)
{
	Point(0, point);
}

const IPoint &ILine::Second() const
{
	return Point(NrOfPoints()-1);
}

void ILine::Second(const IPoint& point)
{
	Point(NrOfPoints()-1, point);
}

int ILine::NrOfPoints() const
{
	if ( NrOfNodes() > 0 ) return NrOfNodes();
	return 2;
}

const IPoint& ILine::Point(int nIndex) const
{
	return Node(nIndex);
}

void ILine::Point(int nIndex, const IPoint &pt)
{
	Node(nIndex, pt);
}

IElement::TDoubleVec ILine::ShapeFunction(const IElement::TDoubleVec& isocoords) const
{
	IElement::TDoubleVec vcRet(NrOfNodes());
	assert(isocoords.size() == 1); // only xi coordinate

	double *values = new double[NrOfNodes()];

	LineShape(NrOfNodes(), isocoords[0], values);

	for(size_t i = 0; i < NrOfNodes(); i++) vcRet[i] = values[i];

	delete[] values;

	return vcRet;
}

// See if the point pt lies in the direction of vecDir from the reference point ptRef
// ptRef and pt must be on the line and vecDir must be a vector parallel to the line
bool ILine::InDirectionOf(const IPoint &ptRef, const IVector &vecDir, const IPoint &pt) const
{
#ifdef _DEBUG
	assert(Distance(ptRef) < EPS);
	assert(Distance(pt) < EPS);
	assert(!vecDir.isNullVector());
	assert(!ptRef.Empty());
	assert(!pt.Empty());
	assert(!vecDir.Empty());
	assert(!Empty());

	{
		CPoint ptDebug = ptRef + vecDir;
		assert(Distance(ptDebug) < EPS);
	}
#endif

	if(fabs(vecDir.X()) > compareTolerance() )
	{
		return (vecDir.X() * (pt.X() - ptRef.X()) > 0);
	}
	if(fabs(vecDir.Y()) > compareTolerance() )
	{
		return (vecDir.Y() * (pt.Y() - ptRef.Y()) > 0);
	}
	if(fabs(vecDir.Z()) > compareTolerance() )
	{
		return (vecDir.Z() * (pt.Z() - ptRef.Z()) > 0);
	}

	// almost NullVector
	return false;
}

// See if the two points are on the same side of the line
// if bIncludeEdge is true, then true is also returned if both
// points are near enough to the line
bool ILine::SameSide(const IPoint &pt1, const IPoint &pt2, bool bIncludeLine)
{
	CVector cp1 = Dir().CrossProduct(CVector(pt1 - First()));
	CVector cp2 = Dir().CrossProduct(CVector(pt2 - Second()));

	double dot = cp1.DotProduct(cp2);
	
	if(fabs(dot) < EPS) return bIncludeLine;

	if(dot > 0) return true;

	return false;
}

// return a representative length for this element
// length of a line
// square root of area for a face
// 3rd square root of volume for a body
double ILine::RepresentativeLength() const
{
	return Length();
}

int ILine::NrOfLines() const
{
	return 1;
}

const ILine& ILine::Line(int /*nIndex*/) const
{
	return *this;
}

bool ILine::CLineLess::operator()(const CLine &Line1, const CLine &Line2) const
{
	if(Line1.IsEqual(Line2))
		return false;
	return Line1 < Line2;
}

CMatrix ILine::ShapeFunctionDerived(const IElement::TDoubleVec& isocoords) const
{
	assert(isocoords.size() == 1);
	const double& xi = isocoords[0];
	assert(xi >= -1.0 && xi <= 1.0);

	// 1 row, NrOfNodes() columns
	CMatrix ret(1, NrOfNodes());

	double *p = new double[NrOfNodes()];

	DerivedLineShape(NrOfNodes(), xi, p);

	double *v = p;

	int j;
	for(j = 0; j < NrOfNodes(); j++)
	{
		ret.Value(0, j, *(v++));
	}

	delete[] p;
	return ret;
}

std::vector<IElement::TDoubleVec> ILine::IsoCoordinates() const
{
	assert(NrOfNodes() > 1);
	std::vector<IElement::TDoubleVec> ret;
	for(size_t i = 0; i < NrOfNodes(); i++)
	{
		IElement::TDoubleVec vec(1);
		vec[0] = (double)i / (NrOfNodes() - 1);
		ret.push_back(vec);
	}

	return ret;
}

IElement::TDoubleVec ILine::WorldToIso(const geo::IPoint& point) const
{
	assert(Order() == 1);
	assert(Contains(point));
	double lp = Point(0).Distance(point);

	IElement::TDoubleVec vcRet(1);
	vcRet[0] = 2 * lp / Length() - 1;

	return vcRet;
}

int ILine::IntegrationPointSize() const
{
	return (int)IntegrationPoints().size();
}

const IElement::TDoubleVec& ILine::IntegrationPointCoords(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < IntegrationPointSize());
	return IntegrationPoints()[nIndex].first;
}

const double& ILine::IntegrationPointWeight(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < IntegrationPointSize());
	return IntegrationPoints()[nIndex].second;
}

void ILine::Mirror(const geo::IPlane& plane)
{
	CPoint first = First();
	CPoint second = Second();
	first.Mirror(plane);
	second.Mirror(plane);
	First(first);
	Second(second);

}

}
