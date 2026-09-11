/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ConvexHull.cpp: implementation of the CConvexElement class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "Vector.h"
#include "Point.h"
#include "Matrix.h"
#include "ConvexHull.h"
#include "IProgressBase.h"
#include "BucketKernel.h"
#include <algorithm>
#include <cmath>

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 4290 ) // see comment in ConvexHull.h
#endif


namespace geo {



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvexHull::CConvexHull(const std::vector<const IPoint*> &points)
: m_dHullIncrement(0.0),
  m_centrePoint(0.0,0.0,0.0),
  m_validDigits(0.0), //undetermined
  m_invalid(false),
  m_vcInputPoints(points)
{
}

CConvexHull::CConvexHull(CPtrArray<IPoint> &points)
: m_dHullIncrement(0.0),
  m_centrePoint(0.0,0.0,0.0),
  m_validDigits(0.0), //undetermined
  m_invalid(false),
  m_vcInputPoints(points.Size())
{
  for(size_t i = 0; i < m_vcInputPoints.size(); i++)
    m_vcInputPoints[i] = &points.Object(i);
}

CConvexHull::~CConvexHull()
{
}

/*! Set the number of valid digits
 * \param digits 
 * digits< 0: Calculate the number of valid digits internally.
 * digits= 0: Unset valid digits, use floats 'as is'.
 * digits= 1: Digit validity is integer only: 1.
 * digits= 1e-1: One valid digit after decimal point: 1.1
 * digits= 1e-2: 1.11
 * digits= 1e-3: 1.111 etc.
 */
void CConvexHull::SetValidDigits(const double &digits /*=-1*/)
{
  m_validDigits= digits;
}
  

CPoint CConvexHull::GlobalPoint(int nIndex) const
{
  return m_vcHullPoints[nIndex] + m_centrePoint; //undo translation
}

CElementPoint CConvexHull::MidPoint() const
{
  return m_centrePoint;
}

long CConvexHull::SaveProgressSize() const
{
  return m_vcHullPoints.size() + m_vcHullFaces.size();
}

const IPoint &CConvexHull::Point(int nIndex) const throw (const char *)
{
  if ( !(nIndex >= 0 && nIndex < m_vcHullPoints.size()))
    throw( (const char *)("Convex Hull Failed (CConvexHull::Point)"));
  return m_vcHullPoints[nIndex];
}

void CConvexHull::Point(int /*nIndex*/, const IPoint &/*pt*/)
{
  assert(false);  // Points cannot be changed from outside the convexhull.
}

int CConvexHull::NrOfPoints() const
{
  return (int)m_vcHullPoints.size();
}

const IFace &CConvexHull::Face(int nIndex) const throw (const char *)
{
  if ( !(nIndex >= 0 && nIndex < m_vcHullFaces.size()))
    throw( (const char *)("Convex Hull Failed (Face)"));
  return m_vcHullFaces[nIndex];
}

int CConvexHull::NrOfFaces() const
{
  return (int)m_vcHullFaces.size();
}

const CConvexHull::TIndexVec&
CConvexHull::FacePointIndices(int nIndex) const throw (const char *)
{
  const CConvexHullFace *pFace = dynamic_cast<const CConvexHullFace *> (&Face(nIndex));
  if (! pFace) throw( (const char *)("Convex Hull Failed (FacePointIndices)"));
  return pFace->PointIndices();
}

const CConvexHull::TIndexVec&
CConvexHull::FaceNodeIndices(int nIndex) const throw (const char *)
{
  if ( !(Order() == 1)) throw( (const char *)("Convex Hull Failed (FaceNodeIndices)"));
  return FacePointIndices(nIndex);
}

int CConvexHull::NrOfLines() const
{
  assert(false); // no implementation of line interface
  return 0;
}

const ILine &CConvexHull::Line(int /*nIndex*/) const
{
  assert(false);
  const ILine *pBogus = 0;
  return *pBogus;
}

const CConvexHull::TIndexVec& CConvexHull::LinePointIndices(int /*nIndex*/) const
{
  assert(false);
  TIndexVec *pBogus = 0;
  return *pBogus;
}

const CConvexHull::TIndexVec& CConvexHull::LineNodeIndices(int /*nIndex*/) const
{
  assert(false);
  TIndexVec *pBogus = 0;
  return *pBogus;
}

// Calculate the volume accoording to "Graphics Gems V", chap. II.1.
double CConvexHull::Size() const
{
  double v= 0.0;

  for (size_t i = 0; i < m_vcHullFaces.size(); ++i)
  {
    CVector v1,v2,v3,v4;
    CPoint  p[3];

    for (size_t j = 0; j < 3; ++j)
      p[j] = m_vcHullFaces[i].Point(j);

    v1 = CVector(p[0]).CrossProduct(CVector(p[1]));
    v2 = CVector(p[2]).CrossProduct(CVector(p[0]));
    v3 = CVector(p[1]).CrossProduct(CVector(p[2]));
    v4 = CVector(p[2]).CrossProduct(CVector(p[0]));

    v = v + CVector(p[0]).DotProduct((v1 + v2) + (v3 + v4));
  }
  return v/6;
}

size_t CConvexHull::Order() const
{
  return 1;
}


//=============================================================================
// Calculate the convexhull.
// Returns true on success
bool CConvexHull::Calculate(IProgressBase *progress)
{
  if (m_vcInputPoints.size() == 0)
    return true;

  bool retval = false;

  IConvexHullImpl *hullImpl = createConvexHullImpl(*this);

  if (hullImpl)
  {
    hullImpl->SetValidDigits(m_validDigits);
    hullImpl->SetPoints(progress, m_vcInputPoints);

    if (hullImpl->Calculate(progress))
    {
      m_validDigits = hullImpl->GetValidDigits();
      m_centrePoint = hullImpl->MidPoint();

      hullImpl->GetPointsAndFaces(m_vcHullPoints, m_vcHullFaces);

      retval = true;
    }

    delete hullImpl;
  }

  return retval;

}

/* Do a summation of the vector 'input'
 * http://en.wikipedia.org/wiki/Kahan_summation_algorithm
 *
 * Note: replaced std::vector with local array, as the vector ctor/dtor caused a severe performance hit
 *
 * Switch off code optimization
 * http://support.microsoft.com/kb/254226
 * http://msdn.microsoft.com/en-us/library/chh3fb0k(VS.80).aspx
 */
#ifdef _MSC_VER
#pragma optimize("g",off)
#endif
double CConvexHull::KahanSummation(const double *input, int size)
{
 double sum = input[0];
 double c = 0.0;          //A running compensation for lost low-order bits.
 for (int i= 1; i< size; ++i)
 {
  double y = input[i] - c; // So far, so good: c is zero.
  double t = sum + y;      // Alas, sum is big, y small,
                           // so low-order digits of y are lost.
  c = (t - sum) - y;       // (t - sum) recovers the high-order part of y;
                           // subtracting y recovers -(low part of y)
  sum = t;                 // Algebraically, c should always be zero.
                           // Beware eagerly optimising compilers!
 }                         // Next time around, the lost low part
                           // will be added to y in a fresh attempt.
return sum;
  
}
#ifdef _MSC_VER
#pragma optimize("g",on)
#endif

double CConvexHull::RoundNumber(double d) const
{
  int sign = d >=0 ? 1 : -1;

  if ( m_validDigits == 1 )
    return (int )(d+sign*0.5); //faster
  else
    return (int )d+((int )((d -(int )d)/m_validDigits)+sign*0.5)*m_validDigits;
}

int CConvexHull::VolumeSign(const CConvexHullFace& face, const IPoint &point) const
{
  double  vol;
  double  ax, ay, az, bx, by, bz, cx, cy, cz;
  double  input[6];

  ax = bx = cx = RoundNumber(m_centrePoint.X() - point.X());
  ay = by = cy = RoundNumber(m_centrePoint.Y() - point.Y());
  az = bz = cz = RoundNumber(m_centrePoint.Z() - point.Z());

  const CPoint& p0 = face.Point(0);
  const CPoint& p1 = face.Point(1);
  const CPoint& p2 = face.Point(2);

  ax += p0.X();
  ay += p0.Y();
  az += p0.Z();
  bx += p1.X();
  by += p1.Y();
  bz += p1.Z();
  cx += p2.X();
  cy += p2.Y();
  cz += p2.Z();


  if (m_dHullIncrement != 0.0)
  {
    const CVector& normal = face.Normal();

    if (!normal.Empty())
    {
      double delta = normal.X() * m_dHullIncrement;

      ax += delta;
      bx += delta;
      cx += delta;

      delta = normal.Y() * m_dHullIncrement;

      ay += delta;
      by += delta;
      cy += delta;

      delta = normal.Z() * m_dHullIncrement;

      az += delta;
      bz += delta;
      cz += delta;
    }
  }

   /* This is what the KahanSummation does:
   vol =   ax * (by*cz - bz*cy)
         + ay * (bz*cx - bx*cz)
         + az * (bx*cy - by*cx);
   */
   input[0] =  ax*by*cz;
   input[1] = -ax*bz*cy;
   input[2] =  ay*bz*cx;
   input[3] = -ay*bx*cz;
   input[4] =  az*bx*cy;
   input[5] = -az*by*cx;
   vol = KahanSummation(input, 6);

   /* The volume should be an integer. */
   if      ( vol >  0.0 )  return  1;
   else if ( vol < -0.0 )  return -1;
   else                    return  0;
}



//=============================================================================
bool CConvexHull::Contains(const IPoint &point, bool bIncludeEdge) const
{
  bool ret = true;

  // Check to see if point is inside convexhull.
  size_t NrOfZeroVolume = 0;

  for (size_t i = 0; i < m_vcHullFaces.size(); ++i)
  {
    int sign = VolumeSign(m_vcHullFaces[i], point);

    if (sign < 0)
    {
      ret = false;
      break;
    }
    else if (sign == 0)
    {
      ++NrOfZeroVolume;
    }
  }


  // Check to see if point is at border of the convex element.
  if (ret && !bIncludeEdge)
  {
    CPoint p = point - m_centrePoint;
    // should we round here? I think it's not very useful
    for(size_t i = 0; i < m_vcHullFaces.size(); ++i)
      if (m_vcHullFaces[i].Contains(p, true))
      {
        ret = false;
        break;
      }
  }

  // keeping this around, for there are invalid hulls stored in models
  if (ret && NrOfZeroVolume > 0 && NrOfZeroVolume == m_vcHullFaces.size()) // See Bug 124671; the pointsets there have #0vol == #faces, but we may fail to catch all errors
  {
    if (m_min.Empty())
    {
      m_min = m_vcHullPoints[0];
      m_max = m_vcHullPoints[0];

      for (size_t i = 1; i < m_vcHullPoints.size(); ++i)
      {
        m_min = m_min.Min(m_vcHullPoints[i]);
        m_max = m_max.Max(m_vcHullPoints[i]);
      }
  
      if (m_dHullIncrement > 0)
      {
        m_min.Set(m_min.X() - m_dHullIncrement, m_min.Y() - m_dHullIncrement, m_min.Z() - m_dHullIncrement);
        m_max.Set(m_max.X() + m_dHullIncrement, m_max.Y() + m_dHullIncrement, m_max.Z() + m_dHullIncrement);
      }
      else
      {
        // let's not quibble about precision errors and extend the CH with 1% in each direction (due to the nature of the bug, we expect that this doesn't really matter)

        double deltaX = std::abs(m_max.X() - m_min.X()) / 100;
        double deltaY = std::abs(m_max.Y() - m_min.Y()) / 100;
        double deltaZ = std::abs(m_max.Z() - m_min.Z()) / 100;

        m_min.Set(m_min.X() - deltaX, m_min.Y() - deltaY, m_min.Z() - deltaZ);
        m_max.Set(m_max.X() + deltaX, m_max.Y() + deltaY, m_max.Z() + deltaZ);
      }
    }

    CPoint p = point - m_centrePoint;

    if (p.X() < m_min.X() || p.Y() < m_min.Y() || p.Z() < m_min.Z()
      || p.X() > m_max.X() || p.Y() > m_max.Y() || p.Z() > m_max.Z())
    {
      if (!m_invalid)
      {
        m_invalid = true;
        throw((const char *)("Convex Hull is not consistent and unsuitable to determine containment of points. Using bounding box for now."));
      }
      else
      {
        ret = false;
      }
    }

  }

  return ret;
}

//#############################################################################
CConvexHull::CConvexHullFace::CConvexHullFace()
: m_vcPoint(3), m_parent(0)
{
}

CConvexHull::CConvexHullFace& CConvexHull::CConvexHullFace::operator=(const CConvexHull::CConvexHullFace& rhs)
{
  m_parent  = rhs.m_parent;
  m_vcPoint = rhs.m_vcPoint;
  m_normal  = rhs.m_normal;

  return *this;
}

void CConvexHull::CConvexHullFace::Set(const CConvexHull &Parent, int n1, int n2, int n3, double x, double y, double z)
{
  m_parent = &Parent;
  m_vcPoint[0] = n1;
  m_vcPoint[1] = n2;
  m_vcPoint[2] = n3;
  m_normal.Set(x, y, z);
}

CVector CConvexHull::CConvexHullFace::Normal() const
{
  return m_normal;
}

const CConvexHull::TIndexVec& CConvexHull::CConvexHullFace::PointIndices() const
{
  return m_vcPoint;
}

int CConvexHull::CConvexHullFace::PointIndex(int nLocalIndex) const throw (const char *)
{
  if ( ! (nLocalIndex >= 0 && nLocalIndex < 3) )
    throw( (const char *)("Convex Hull Failed (PointIndex)"));
  return m_vcPoint[nLocalIndex];
}

const IPoint &CConvexHull::CConvexHullFace::Point(int nIndex) const throw (const char *)
{
  if ( ! (nIndex >= 0 && nIndex < NrOfPoints()) )
    throw( (const char *)("Convex Hull Failed (CConvexHullFace::Point)"));

  return m_parent->Point(PointIndex(nIndex));
}

void CConvexHull::CConvexHullFace::Point(int /*nIndex*/, const IPoint &/*pt*/)
{
  assert(false);
}

int CConvexHull::CConvexHullFace::NrOfPoints() const
{
  return 3;
}

size_t CConvexHull::CConvexHullFace::Order() const
{
  return 1;
}



}//end namespace geo


#ifdef _MSC_VER
#pragma warning ( pop )
#endif