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
#include "MPKernel.h"

#include "ConvexHullImpl.h"


/* Brief description of (parallel) algorithm

The old serial algorithm worked as follows:
1. Sort the points by distance from the center (farthest away earlier in the ordering)
2. Create an initial tetrahedron (ideally this is as big as possible)
3. Now walk over all the other points and
4.     compare them to each face of the hull we already have
5.     if outside, then add new faces to the hull, and remove some
6. Create normals for all the faces

Improvements:
a. In VolumeSign, a temporary vector was created, and it bogged the routine down; replaced with an array
b. We also removed the hull increment code from VolumeSign, as it is not used during hull creation (we calculate normals afterwards)
c. We create the vertex data structures all at once in the beginning, and keep track of vertices processed, on hull, etc, in situ

Parallel algorithm follows the serial one, with these additions:
1. When we have sorted the points, we also create a 3D grid and each cell contains one point max
   As we do this in order, and assign the first point, it's the point furthest from center, exactly as we want it
   We try to have around 400k cells max
3. We first process 16 points as the original did, to accommodate regular shapes (for example, cube, as in the ConvexHull3D unit test)
   Walk over the boxes, and process those points
   Do a parallel yes/no check to determine the points still outside the box
   Process the remaining points

Further optimizations are possible, eg:
a. Remove new/delete for edges and faces too, and remove the std::set for visible edges
b. Some initialization routines can be parallelized
c. If a, then we can also calculate normals in parallel

The heuristics (<500 points, go serial; #boxes) should be tested and if need be, adjusted.

*/




IConvexHullImpl *createConvexHullImpl(const CConvexHull& parent)
{
  return new CConvexHullImpl(parent);
}

// Define flags
#define ONHULL     true
#define REMOVED    true
#define VISIBLE    true
#define PROCESSED  true
#define SAFE    DOUBLE_MAX_    // Range of safe coord values.

/*!
  Construction of tVertexStructure
*/
tVertexStructure::tVertexStructure()
: pDuplicate(0),
  bOnhull(!ONHULL),
  bProcessed(false),
  bDeleted(false),
  nNext(-1),
  nPrev(-1)
{
}

/*!
  Destruction
*/
tVertexStructure::~tVertexStructure()
{
}

bool tVertexStructure::operator<(const tVertexStructure& rhs) const
{
  return dWeight > rhs.dWeight;
}

tEdgeStructure::tEdgeStructure() : pNext(NULL), pPrev(NULL), nIndex(-1)
{
}

void tEdgeStructure::Add(tEdge nEdge)
{
  tEdge nv = this;
  while (nv->pNext != NULL) nv = nv->pNext;
  nv->pNext = nEdge;
  nEdge->pPrev = nv;
};

void tEdgeStructure::Delete()
{
  if (pPrev != NULL) pPrev->pNext = pNext;
  if (pNext != NULL) pNext->pPrev = pPrev;
  delete this;
};

tFaceStructure::tFaceStructure() : pNext(NULL), pPrev(NULL), nIndex(-1)
{
}

void tFaceStructure::Add(tFace nFace)
{
  tFace nv = this;
  while (nv->pNext != NULL) nv = nv->pNext;
  nv->pNext = nFace;
  nFace->pPrev = nv;
};

void tFaceStructure::Delete()
{
  if (pPrev != NULL) pPrev->pNext = pNext;
  if (pNext != NULL) pNext->pPrev = pPrev;
  delete this;
};


tBoxStructure::tBoxStructure() : m_halfLengthX(0), m_halfLengthY(0), m_halfLengthZ(0), m_strideX(0), m_strideY(0)
{
}

void tBoxStructure::Set(int x, int y, int z)
{
  if (x < 1)
    x = 1;
  if (y < 1)
    y = 1;
  if (z < 1)
    z = 1;

  m_halfLengthX = x;
  m_halfLengthY = y;
  m_halfLengthZ = z;

  x = x * 2 + 1;
  y = y * 2 + 1;
  z = z * 2 + 1;

  m_strideX = y * z;
  m_strideY = z;

  m_vcBoxes.resize(x * y * z, -1);
}

void tBoxStructure::Add(int x, int y, int z, int v)
{
  x += m_halfLengthX;
  y += m_halfLengthY;
  z += m_halfLengthZ;

  int k = x * m_strideX + y * m_strideY + z;

  assert(k >= 0 && k < m_vcBoxes.size());

  if (m_vcBoxes[k] < 0)
      m_vcBoxes[k] = v;
}


CConvexHullImpl::CConvexHullImpl(const CConvexHull& parent) : IConvexHullImpl(parent), m_tEdges(0), m_tFaces(0), nVertexRoot(-1), nVertexCount(0), m_parent(parent)
{
}

CConvexHullImpl::~CConvexHullImpl()
{
  ClearAll();
}

void CConvexHullImpl::SetPoints(IProgressBase *progress, const std::vector<const IPoint *>& points)
{
  int size = points.size();

  if(progress)
  {
    int times = size > 500 ? 5 : 3;
    progress->AddSteps(times * size);
  }

  ftn_double_t prev = IObject::setCompareTolerance( DBL_EPSILON );
  // Get extremes
  double xmin = (*points.begin())->X();
  double xmax = (*points.begin())->X();
  double ymin = (*points.begin())->Y();
  double ymax = (*points.begin())->Y();
  double zmin = (*points.begin())->Z();
  double zmax = (*points.begin())->Z();
  std::vector<const IPoint*>::const_iterator it = points.begin() + 1;
  while ( it != points.end() ) {
    if ( (*it)->X() < xmin ) xmin = (*it)->X();
    if ( (*it)->X() > xmax ) xmax = (*it)->X();
    if ( (*it)->Y() < ymin ) ymin = (*it)->Y();
    if ( (*it)->Y() > ymax ) ymax = (*it)->Y();
    if ( (*it)->Z() < zmin ) zmin = (*it)->Z();
    if ( (*it)->Z() > zmax ) zmax = (*it)->Z();
    ++it;
    if (progress) progress->Step(1);
  }
  // Get (squared)distance to centre for each point as weight
  CPoint centre( ( xmin + xmax ) / 2.,
                 ( ymin + ymax ) / 2.,
                 ( zmin + zmax ) / 2. );

  m_vcVertices.resize(points.size());
  for (size_t i = 0; i < points.size(); ++i)
  {
    double dx = RoundNumber(points[i]->X() - centre.X());
    double dy = RoundNumber(points[i]->Y() - centre.Y());
    double dz = RoundNumber(points[i]->Z() - centre.Z());
    m_vcVertices[i].Point.Set(dx, dy, dz);
    m_vcVertices[i].dWeight = dx * dx + dy * dy + dz * dz;
    if (progress) progress->Step(1);
  }

  IObject::setCompareTolerance( prev );

  double absXmin= fabs(xmin - centre.X());
  double absYmin= fabs(ymin - centre.Y());
  double absZmin= fabs(zmin - centre.Z());
  double absXmax= fabs(xmax - centre.X());
  double absYmax= fabs(ymax - centre.Y());
  double absZmax= fabs(zmax - centre.Z());
  m_maxDist= absXmin;
  if ( m_maxDist < absYmin ) m_maxDist= absYmin;
  if ( m_maxDist < absZmin ) m_maxDist= absZmin;
  if ( m_maxDist < absXmax ) m_maxDist= absXmax;
  if ( m_maxDist < absYmax ) m_maxDist= absYmax;
  if ( m_maxDist < absZmax ) m_maxDist= absZmax;

  m_centrePoint= centre;

  if (m_vcVertices.size() <= 500)
  {
    std::sort(m_vcVertices.begin(), m_vcVertices.end());
    for (int i = 1; i < m_vcVertices.size(); ++i)
    {
      if (m_vcVertices[i].Point == m_vcVertices[i - 1].Point) // skip (exact) duplicates; TBD if we can use epsilon
        m_vcVertices[i].bProcessed = true;
    }
  }
  else
  {
    tbb::parallel_sort(m_vcVertices.begin(), m_vcVertices.end());

    double d[3];

    d[0] = absXmax + absXmin;
    d[1] = absYmax + absYmin;
    d[2] = absZmax + absZmin;

    int smallest = 0, largest = 0;

    for (int i = 1; i < 3; ++i)
    {
      if (d[i] <= d[smallest])
        smallest = i;
      if (d[i] > d[largest])
        largest = i;
    }

    if (d[smallest] == 0) // this shouldn't happen
      smallest = 3 - smallest - largest;

    assert(d[smallest] != 0); // shouldn't happen either

    int other = 3 - smallest - largest;

    int factor[3];

    if (d[largest] / d[smallest] > 1.5)
    {
      factor[largest]  = ceil(d[largest] / 100);
      factor[other]    = ceil(d[other] / 100);
      factor[smallest] = ceil(d[smallest] / 40);
    }
    else
    {
      factor[largest]  = ceil(d[largest] / 72);
      factor[other]    = ceil(d[other] / 72);
      factor[smallest] = ceil(d[smallest] / 72);
    }


    m_box.Set(ceil(d[0] / (2 * factor[0])), ceil(d[1] / (2 * factor[1])), ceil(d[2] / (2 * factor[2])));

    if (progress) progress->AddSteps(m_box.m_vcBoxes.size());

    for (int i = 0; i < m_vcVertices.size(); ++i)
    {
      int x = m_vcVertices[i].Point.X() / factor[0];
      int y = m_vcVertices[i].Point.Y() / factor[1];
      int z = m_vcVertices[i].Point.Z() / factor[2];

      m_box.Add(x, y, z, i);

      if (i > 0 && m_vcVertices[i].Point == m_vcVertices[i - 1].Point) // skip (exact) duplicates; TBD if we can use epsilon
        m_vcVertices[i].bProcessed = true;

      if (progress) progress->Step(1);
    }
  }
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
void CConvexHullImpl::SetValidDigits(double digits)
{
  m_validDigits = digits;
}

double CConvexHullImpl::GetValidDigits() const
{
  return m_validDigits;
}

CVector CConvexHullImpl::MidPoint() const
{
  return m_centrePoint;
}

void CConvexHullImpl::GetPointsAndFaces(std::vector<CPoint> &points, std::vector<CConvexHull::CConvexHullFace> &faces) const
{
  assert(nVertexCount == VertexSize());
  points.resize(nVertexCount);
  size_t j = 0;
  for (int i = nVertexRoot; i >= 0; i = m_vcVertices[i].nNext)
    points[j++].Set(m_vcVertices[i].Point.X(), m_vcVertices[i].Point.Y(), m_vcVertices[i].Point.Z());

  if (points.size() <= 500)
    std::sort(points.begin(), points.end());
  else
    tbb::parallel_sort(points.begin(), points.end());

  if (!m_tFaces)
    return;

  size_t i = 0;

  for (tFace f = m_tFaces; f != NULL; f = f->pNext)
    ++i;
  faces.resize(i);

  i = 0;
  for (tFace f = m_tFaces; f != NULL; f = f->pNext)
  {
    faces[i].Set(m_parent,
      FindPointIndex(points, f->vertex[0]->Point), FindPointIndex(points, f->vertex[1]->Point), FindPointIndex(points, f->vertex[2]->Point),
      f->normal.X(), f->normal.Y(), f->normal.Z());
    ++i;
  }
}

 
/*! Determine the smallest value digit that is significant for testValue
 * The function calculates the third power of testValue in two ways and uses
 * its difference to determine the smallest significant increment.
 * The largest value of digits that can be determined is 1.
 * \return digits could be determined.
 */
#ifdef _MSC_VER
#pragma optimize("g",off)
#endif
bool CConvexHullImpl::CheckValidDigits(const double &testValue, double &digits)
{
  bool foundIt= false;
  double step= 1;
  double diff= 0;
  const double eps=
    pow(10.0,(int )log10(testValue/pow(2.0,52)));//IEEE-754 -> 52
  double directPow3, indirectPow3;
  double curValue;

  do
  {
    // sample values around the testValue
    for ( curValue =  0.9 * testValue
        ; curValue <= 1.1 * testValue
        ; curValue += 0.01 * testValue )
    {
      // directly calculate the 3rd power (comes from VolumeSign)
      directPow3= (curValue+step) * (curValue+step) * (curValue+step);
      double aaa, aab, abb, bbb;
      aaa= curValue*curValue*curValue;
      aab= curValue*curValue*step;
      abb= curValue*step*step;
      bbb= step*step*step;
      // Calulate the third power in another way
      indirectPow3= ( aaa + (3*aab + (3*abb + bbb)));
      diff= fabs(directPow3 - indirectPow3);
      // Is the difference in outcome larger than the allowed error 'step'?
      if ( diff >= step ) break;
    }

    // For all samples there was no significant difference
    if ( diff < step )
    {
      foundIt= true;
      digits= step; // significant digits
      step /= 10;
    }
    else
    {
      break;
    }
  } while ( step >= eps); //or break

  return foundIt;
}
#ifdef _MSC_VER
#pragma optimize("g",on)
#endif

/*! Round  double d to the valid number of digits
 * m_validDigits=1e-2, d=1.236 => return 1.24
 */
double CConvexHullImpl::RoundNumber(const double& d) const
{
  if (m_validDigits > 0)
  {
    int sign= d>=0?1:-1;
    if ( m_validDigits == 1 )
      return (int )(d+sign*0.5); //faster
    else
      return (int )d+((int )((d -(int )d)/m_validDigits)+sign*0.5)*m_validDigits;
  }
  return d;
}

/*! Round IPoint components to the valid number of digits
 * \param  p Point
 * \return p
 *
 * O'Rourke advices in 'Computational Geometry' to use
 * integer coordinates to counteract numerical imprecision.
 * We round off to m_validDigits. 
 *
 * http://msdn.microsoft.com/en-us/library/e02ya398.aspx:
 * The double type contains 64 bits: 1 for sign,
 * 11 for the exponent, and 52 for the mantissa.
 * Its range is +/-1.7E308 with at least 15 digits of precision.
 */
IPoint &CConvexHullImpl::RoundPoint(IPoint& p) const
{
  if ( m_validDigits > 0 )
  {
    p.X( RoundNumber(p.X()));
    p.Y( RoundNumber(p.Y()));
    p.Z( RoundNumber(p.Z()));
  }
  return p;
}

/*!
  Appends a vertex to the system
*/
int CConvexHullImpl::AddVertex(int nVertex)
{
  if (m_vcVertices[nVertex].bProcessed)
    return -1;

  m_vcVertices[nVertex].bProcessed = true;

  if (nVertexRoot >= 0)
  {
    m_vcVertices[nVertex].nNext = nVertexRoot;
    m_vcVertices[nVertexRoot].nPrev = nVertex;
  }
  nVertexRoot = nVertex;
  ++nVertexCount;

  return nVertex;
}


void CConvexHullImpl::DeleteVertex(int nVertex)
{
  m_vcVertices[nVertex].bDeleted = true;

  int nNext = m_vcVertices[nVertex].nNext;
  int nPrev = m_vcVertices[nVertex].nPrev;

  if (nNext >= 0)
    m_vcVertices[nNext].nPrev = nPrev;
  if (nPrev >= 0)
    m_vcVertices[nPrev].nNext = nNext;

  if (nVertexRoot == nVertex)
    nVertexRoot = nNext;

  --nVertexCount;
}

int CConvexHullImpl::VertexSize() const
{
  int size = 0;
  for (int i = nVertexRoot; i >= 0; i = m_vcVertices[i].nNext)
    ++size;
  return size;
}



int CConvexHullImpl::FindPointIndex(const std::vector<CPoint> &points, const IPoint &pnt) const
{
  std::vector<CPoint>::const_iterator it = std::lower_bound(points.begin(), points.end(), pnt);
  if (it == points.end())
    return -1;
  else
    return it - points.begin();
}


//=============================================================================
// Calculate the convexhull.
// Returns true if all the checks are positive.
bool CConvexHullImpl::Calculate(IProgressBase *progress)
{
  double digits;
  if (  m_validDigits < 0 )
  {
    if ( CheckValidDigits( m_maxDist, digits) ) 
      m_validDigits= digits;
    else
      m_validDigits= 0.0; //undetermined.
  }

  if (DoubleTriangle())
  {
    ConstructHull(progress);
    CalcNormals();

    return Checks();
  }
  else
    return false;
}



//---------------------------------------------------------------------
// DoubleTriangle builds the initial double triangle.  It first finds 3
// noncollinear points and makes two faces out of them, in opposite order.
// It then finds a fourth point that is not coplanar with that face.  The
// vertices are stored in the face structure in counterclockwise order so
// that the volume between the face and the point is negative. Lastly, the
// 3 newfaces to the fourth point are constructed and the data structures
// are cleaned up.
//---------------------------------------------------------------------
bool CConvexHullImpl::DoubleTriangle()
{
  tFace f0, f1 = NULL;

  // Find 3 noncollinear points.
  int nFound1 = -1, nFound2 = -1, nFound3 = -1;
  const int size = m_vcVertices.size();

  for(int i = 0; i < size - 3;)
  {
    if (m_vcVertices[i].bProcessed)
    {
      ++i;
      continue;
    }

    int j = i + 1;
    while (j < size - 2 && m_vcVertices[j].bProcessed) // skip duplicates of m_vcVertices[i]
      ++j;

    if (j == size - 2)
      return false;

    int k = j + 1;
    while (k < size - 1 && m_vcVertices[k].bProcessed)  // skip duplicates of m_vcVertices[j]
      ++k;

    if (k == size - 1)
      return false;

    if ( !Collinear(m_vcVertices[i].Point, m_vcVertices[j].Point, m_vcVertices[k].Point) ) {
      nFound1 = i;
      nFound2 = j;
      nFound3 = k;
      break;
    }

    i = k + 1;
  }

  if( nFound1 == -1 ) return false;    

  // Add the found vertices to the system and remove them from the row...
  tVertex v0 = &m_vcVertices[AddVertex(nFound1)];
  tVertex v1 = &m_vcVertices[AddVertex(nFound2)];
  tVertex v2 = &m_vcVertices[AddVertex(nFound3++)];

  // Create the two "twin" faces.
  f0 = MakeFace( v0, v1, v2, f1 );
  f1 = MakeFace( v2, v1, v0, f0 );

  // Find a fourth, noncoplanar point to form tetrahedron.
  while( nFound3 < m_vcVertices.size() ) {
    if( !m_vcVertices[nFound3].bProcessed && VolumeSign(f0, &m_vcVertices[nFound3]) != 0 ) {
      // Found add to the system and make thetra
      tVertex v3 = &m_vcVertices[AddVertex(nFound3)];
      AddOne( v3 );
      CleanUp();     
      return true;
    }
    ++nFound3;
  }

  return false;
}




class CTaskFilterVertices
{
  const CConvexHullImpl& m_parent;
  std::vector<tsVertex>& m_data;
  const tFace m_tFaces;

  mp::CKernelDispatcher *m_dispatcher;

public:
  CTaskFilterVertices(const CConvexHullImpl& parent, std::vector<tsVertex>& data, const tFace faces) : m_parent(parent), m_data(data), m_tFaces(faces), m_dispatcher(0) {}

  std::vector<tsVertex>& data() const { return m_data; }
  size_t size() const { return m_data.size(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    size_t i;
    for (i = r.begin(); i != r.end(); ++i)
    {
      if (!m_data[i].bProcessed)
      {
        tFace f = m_tFaces;
        bool bOutside = false;
        while (f != NULL && !bOutside)
        {
          if (m_parent.VolumeSign( f, &m_data[i] ) < 0)
          {
            bOutside = true;
          }
          f = f->pNext;
        }
        if (!bOutside)
          m_data[i].bProcessed = true;
      }
    }
    if (m_dispatcher)
      m_dispatcher->Step(r.size());
  }
};



//---------------------------------------------------------------------
// ConstructHull adds the vertices to the hull one at a time. The hull
// vertices are those in the list marked as onhull.
//---------------------------------------------------------------------
void CConvexHullImpl::ConstructHull(IProgressBase *progress)
{
  int size = (int)m_vcVertices.size();

  if (size > 500)
  {

    for(int i = 0; i < 16; i++)
    {
      int nVertex = AddVertex(i);
      if (nVertex >= 0)
      {
        tVertex v = &m_vcVertices[nVertex];
        AddOne( v );
        CleanUp();
      }
    }

    for (int i = 0; i < m_box.m_vcBoxes.size(); ++i)
    {
      int nVertex = m_box.m_vcBoxes[i];
      if (nVertex >= 0)
      {
        nVertex = AddVertex(nVertex);
        if (nVertex >= 0)
        {
          tVertex v = &m_vcVertices[nVertex];
          AddOne( v );
          CleanUp();
        }
      }
      if(progress) progress->Step();
    }

    mp::CKernelParallel<CTaskFilterVertices> parKernel;
    CTaskFilterVertices taskFilterVertices(*this, m_vcVertices, m_tFaces);

    if (progress)
    {
      mp::CKernelDispatcher kernelDispatcher;
      mp::IDispatchedTask *disTaskFilterVertices = NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskFilterVertices)(parKernel, taskFilterVertices);

      kernelDispatcher.launch(*progress, disTaskFilterVertices);
    }
    else
    {
      parKernel.execute(taskFilterVertices);
    }
  }

  for(int i = 0; i < m_vcVertices.size(); i++)
  {
    int nVertex = AddVertex(i);
    if (nVertex >= 0)
    {
      tVertex v = &m_vcVertices[nVertex];
      AddOne( v );
      CleanUp();
    }
    if(progress) progress->Step();
  }
}

//---------------------------------------------------------------------
// AddOne is passed a vertex. It first determines all faces visible from
// that point. If none are visible then the point is marked as not
// onhull. Next is a loop over edges. If both faces adjacent to an edge
// are visible, then the edge is marked for deletion. If just one of the
// adjacent faces is visible then a new face is constructed.
//---------------------------------------------------------------------
bool CConvexHullImpl::AddOne(tVertex p)
{
  // Mark faces visible from p.
  std::set<tEdge> visibleEdges;

  tFace f = m_tFaces;
  while (f != NULL)
  {
    if (VolumeSign( f, p ) < 0)
    {
      f->visible = VISIBLE;
          visibleEdges.insert( f->edge[0] );
          visibleEdges.insert( f->edge[1] );
          visibleEdges.insert( f->edge[2] );
    }
    f = f->pNext;
  }

  // If no faces are visible from p, then p is inside the hull.
  if (visibleEdges.size() == 0)
  {
    p->bOnhull = !ONHULL;
    return false;
  }

  // Mark edges in interior of visible region for deletion. Erect a newface based on each border edge.
  std::set<tEdge>::const_iterator it;
  for ( it = visibleEdges.begin(); it != visibleEdges.end(); ++it )
  {
    if ( (*it)->adjface[0]->visible && (*it)->adjface[1]->visible )
      (*it)->bDelete = REMOVED;  // e interior: mark for deletion.
    else
    {
      if ( (*it)->adjface[0]->visible || (*it)->adjface[1]->visible )
        (*it)->newface = MakeConeFace( *it, p );  // e border: make a new face.
    }
  }
  visibleEdges.clear();
  return true;
}




/*---------------------------------------------------------------------
VolumeSign returns the sign of the volume of the tetrahedron determined by f
and p.  VolumeSign is +1 iff p is on the negative side of f,
where the positive side is determined by the rh-rule.  So the volume 
is positive if the ccw normal to f points outside the tetrahedron.
The final fewer-multiplications form is due to Bob Williamson.
---------------------------------------------------------------------*/
int  CConvexHullImpl::VolumeSign( tFace f, tVertex p ) const //mantis 2401 wjrx
{
  double  vol;
  double  ax, ay, az, bx, by, bz, cx, cy, cz;
  double  input[6];

  ax = f->vertex[0]->Point.X() - p->Point.X();
  ay = f->vertex[0]->Point.Y() - p->Point.Y();
  az = f->vertex[0]->Point.Z() - p->Point.Z();
  bx = f->vertex[1]->Point.X() - p->Point.X();
  by = f->vertex[1]->Point.Y() - p->Point.Y();
  bz = f->vertex[1]->Point.Z() - p->Point.Z();
  cx = f->vertex[2]->Point.X() - p->Point.X();
  cy = f->vertex[2]->Point.Y() - p->Point.Y();
  cz = f->vertex[2]->Point.Z() - p->Point.Z();

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
  vol = CConvexHull::KahanSummation(input, 6);

  /* The volume should be an integer. */
  if      ( vol >  0.0 )  return  1;
  else if ( vol < -0.0 )  return -1;
  else                    return  0;
}


//---------------------------------------------------------------------
// MakeConeFace makes a new face and two new edges between the
// edge and the point that are passed to it. It returns a pointer to
// the new face.
//---------------------------------------------------------------------
tFace CConvexHullImpl::MakeConeFace( tEdge e, tVertex p ) throw (const char *)
{
  tEdge new_edge[2];
  tFace new_face;
  int   i;

  // Make two new edges (if they don't already exist).
  for ( i=0; i < 2; ++i ) {
    // If the edge exists, copy it into new_edge.
    if ( !( new_edge[i] = e->endpts[i]->pDuplicate) ) {
      // Otherwise (duplicate is NULL), MakeNullEdge.
      new_edge[i]              = MakeNullEdge();
      new_edge[i]->endpts[0]   = e->endpts[i];
      new_edge[i]->endpts[1]   = p;
      e->endpts[i]->pDuplicate = new_edge[i];
    }
  }

  // Make the new face.
  new_face          = MakeNullFace();
  new_face->edge[0] = e;
  new_face->edge[1] = new_edge[0];
  new_face->edge[2] = new_edge[1];
  MakeCcw( new_face, e, p );

  // Set the adjacent face pointers.
  for ( i=0; i < 2; ++i ) {
    // Only one NULL link should be set to new_face.
    if (      !new_edge[i]->adjface[0] ) new_edge[i]->adjface[0] = new_face;
    else if ( !new_edge[i]->adjface[1] ) new_edge[i]->adjface[1] = new_face;
    else throw( (const char *)("Convex Hull Failed (MakeConeFace)"));
  }

  return new_face;
}

//---------------------------------------------------------------------
// MakeCcw puts the vertices in the face structure in counterclock wise
// order.  We want to store the vertices in the same
// order as in the visible face.  The third vertex is always p.
//---------------------------------------------------------------------
void CConvexHullImpl::MakeCcw( tFace f, tEdge e, tVertex p )
{
  tFace fv;    // The visible face adjacent to e.
  int   i;     // Index of e->endpoint[0] in fv.
  tEdge s;     // Temporary, for swapping.

  if  ( e->adjface[0]->visible ) fv = e->adjface[0];
  else                           fv = e->adjface[1];

  // Set vertex[0] & [1] of f to have the same orientation as
  // do the corresponding vertices of fv.
  for ( i=0; fv->vertex[i] != e->endpts[0]; ++i )
    ;

  // Orient f the same as fv.
  if ( fv->vertex[ (i+1) % 3 ] != e->endpts[1] ) {
    f->vertex[0] = e->endpts[1];
    f->vertex[1] = e->endpts[0];
  } else {
    f->vertex[0] = e->endpts[0];
    f->vertex[1] = e->endpts[1];

    // Swap.
    s = f->edge[1];
    f->edge[1] = f->edge[2];
    f->edge[2] = s;
  }
  // This swap is tricky. e is edge[0]. edge[1] is based on endpt[0],
  // edge[2] on endpt[1].  So if e is oriented "forwards," we
  // need to move edge[1] to follow [0], because it precedes.

  f->vertex[2] = p;
}


//---------------------------------------------------------------------
// MakeNullEdge creates a new cell and initializes all pointers to NULL
// and sets all flags to off.  It returns a pointer to the empty cell.
//---------------------------------------------------------------------
tEdge CConvexHullImpl::MakeNullEdge()
{
  tEdge  e;

  e = new tsEdge;
  e->adjface[0] = e->adjface[1] = e->newface = NULL;
  e->endpts[0] = e->endpts[1] = NULL;
  e->bDelete = !REMOVED;

  if (m_tEdges == NULL)
    m_tEdges = e;
  else
    m_tEdges->Add(e);
  return e;
}

//---------------------------------------------------------------------
// MakeNullFace creates a new face structure and initializes all of its
// flags to NULL and sets all the flags to off.  It returns a pointer
// to the empty cell.
//---------------------------------------------------------------------
tFace CConvexHullImpl::MakeNullFace()
{
  tFace f;
  int i;

  f = new tsFace;
  for ( i=0; i < 3; ++i )
  {
    f->edge[i] = NULL;
    f->vertex[i] = NULL;
  }
  f->visible = !VISIBLE;

  if (m_tFaces == NULL)
    m_tFaces = f;
  else
    m_tFaces->Add(f);
  return f;
}

//---------------------------------------------------------------------
// MakeFace creates a new face structure from three vertices (in ccw
// order).  It returns a pointer to the face.
//---------------------------------------------------------------------
tFace CConvexHullImpl::MakeFace( tVertex v0, tVertex v1, tVertex v2, tFace fold )
{
  tFace  f;
  tEdge  e0, e1, e2;

  // Create edges of the initial triangle.
  if( !fold )
  {
    e0 = MakeNullEdge();
    e1 = MakeNullEdge();
    e2 = MakeNullEdge();

        // Initialize end points here
    e0->endpts[0] = v0; e0->endpts[1] = v1;
    e1->endpts[0] = v1; e1->endpts[1] = v2;
    e2->endpts[0] = v2; e2->endpts[1] = v0;
  }
  else
  {
    // Copy from fold, in reverse order.
    e0 = fold->edge[2];
    e1 = fold->edge[1];
    e2 = fold->edge[0];
  }

  // Create face for triangle.
  f = MakeNullFace();
  f->edge[0]   = e0;  f->edge[1]   = e1; f->edge[2]   = e2;
  f->vertex[0] = v0;  f->vertex[1] = v1; f->vertex[2] = v2;

  // Link edges to face.
  if ( !fold )
    e0->adjface[0] = e1->adjface[0] = e2->adjface[0] = f;
  else
    e0->adjface[1] = e1->adjface[1] = e2->adjface[1] = f;

  return f;
}

//---------------------------------------------------------------------
// CleanUp goes through each data structure list and clears all
// flags and NULLs out some pointers.  The order of processing
// (edges, faces, vertices) is important.
//---------------------------------------------------------------------
void CConvexHullImpl::CleanUp()
{
   CleanEdges();
   CleanFaces();
   CleanVertices();
}


//---------------------------------------------------------------------
// CleanEdges runs through the edge list and cleans up the structure.
// If there is a newface then it will put that face in place of the
// visible face and NULL out newface. It also deletes so marked edges.
//---------------------------------------------------------------------
void CConvexHullImpl::CleanEdges()
{
  tEdge e;  // Primary index into edge list.

  // Integrate the newface's into the data structure. Check every edge.
  // And delete any edges marked for deletion.
  e = m_tEdges;
  while (e != NULL)
  {
    tEdge next = e->pNext;
    if ( e->newface )
    {
      if ( e->adjface[0]->visible )
        e->adjface[0] = e->newface;
      else
        e->adjface[1] = e->newface;
      e->newface = NULL;
    }
    else if ( e->bDelete )
    {
      if ( m_tEdges == e ) m_tEdges = next;
      e->Delete();
    }
    e = next;
  }
}

//---------------------------------------------------------------------
// CalcNormals runs through the face list and calculates unit normals
//---------------------------------------------------------------------
void CConvexHullImpl::CalcNormals()
{
  tFace f= m_tFaces;
  while (f != NULL)
  {
    CVector dir1(f->vertex[0]->Point,f->vertex[1]->Point);
    CVector dir2(f->vertex[0]->Point,f->vertex[2]->Point);
    f->normal = dir1.CrossProduct( dir2 ).UnitVector();

    f = f->pNext;
  }
}

//---------------------------------------------------------------------
// CleanFaces runs through the face list and deletes any face marked visible.
//---------------------------------------------------------------------
void CConvexHullImpl::CleanFaces()
{
  tFace f;  // Primary pointer into face list.
  tFace t;

  f = m_tFaces;
  while (f != NULL)
  {
    if ( f->visible )
    {
      t = f;
      f = f->pNext;
      if (m_tFaces == t) m_tFaces = f;
      t->Delete();
    }
    else
      f = f->pNext;
  }
}

//---------------------------------------------------------------------
// CleanVertices runs through the vertex list and deletes the
// vertices that are marked as processed but are not incident to any
// undeleted edges.
//---------------------------------------------------------------------
void CConvexHullImpl::CleanVertices()
{
  tEdge    e;

  // Mark all vertices incident to some undeleted edge as on the hull.
  e = m_tEdges;
  while (e != NULL)
  {
    e->endpts[0]->bOnhull = e->endpts[1]->bOnhull = ONHULL;
    e = e->pNext;
  }

  int nVertex = nVertexRoot;
  while (nVertex >= 0)
  {
    int nNext = m_vcVertices[nVertex].nNext;
    if (m_vcVertices[nVertex].bOnhull)
    {
      m_vcVertices[nVertex].pDuplicate = NULL;
      m_vcVertices[nVertex].bOnhull = !ONHULL;
    }
    else
    {
      DeleteVertex(nVertex);
    }
    nVertex = nNext;
  }

}

//---------------------------------------------------------------------
// Collinear checks to see if the three points given are collinear,
// by checking to see if each element of the cross product is zero.
//---------------------------------------------------------------------
bool CConvexHullImpl::Collinear(const IPoint& a,
                            const IPoint& b,
                            const IPoint& c)
{
   return
         ( c.Z() - a.Z() ) * ( b.Y() - a.Y() ) -
         ( b.Z() - a.Z() ) * ( c.Y() - a.Y() ) == 0
      && ( b.Z() - a.Z() ) * ( c.X() - a.X() ) -
         ( b.X() - a.X() ) * ( c.Z() - a.Z() ) == 0
      && ( b.X() - a.X() ) * ( c.Y() - a.Y() ) -
         ( b.Y() - a.Y() ) * ( c.X() - a.X() ) == 0  ;
}

//---------------------------------------------------------------------
// Consistency runs through the edge list and checks that all
// adjacent faces have their endpoints in opposite order.  This verifies
// that the vertices are in counterclockwise order.
// Return true if edges are consistence.
//---------------------------------------------------------------------
bool CConvexHullImpl::Consistency()
{
  register tEdge e;
  register int i, j;

  e = m_tEdges;
  while (e != NULL)
  {
    // find index of endpoint[0] in adjacent face[0].
    for ( i = 0; e->adjface[0]->vertex[i] != e->endpts[0]; ++i ) ;

    // find index of endpoint[0] in adjacent face[1].
    for ( j = 0; e->adjface[1]->vertex[j] != e->endpts[0]; ++j ) ;

    // check if the endpoints occur in opposite order.
    if ( !( e->adjface[0]->vertex[ (i+1) % 3 ] == e->adjface[1]->vertex[ (j+2) % 3 ] ||
        e->adjface[0]->vertex[ (i+2) % 3 ] == e->adjface[1]->vertex[ (j+1) % 3 ] )  )
      break;
    e = e->pNext;
  }

  return (e == NULL);
}

//---------------------------------------------------------------------
// Convexity checks that the volume between every face and every
// point is negative.  This shows that each point is inside every face
// and therefore the hull is convex.
// Return true if hull is convex.
//---------------------------------------------------------------------
bool CConvexHullImpl::Convexity()
{
  register tFace f;

  f = m_tFaces;
  while (f != NULL)
  {
    for (int i = nVertexRoot; i >= 0; i = m_vcVertices[i].nNext)
    {
      if (VolumeSign(f, &m_vcVertices[i]) < 0)
        return false;
    }
    f = f->pNext;
  }

  return (f == NULL);
}

//---------------------------------------------------------------------
// CheckEuler checks Euler's relation, as well as its implications when
// all faces are known to be triangles.
// Return true if Euler is right.
//---------------------------------------------------------------------
bool CConvexHullImpl::CheckEuler(int V, int E, int F)
{
  if ((V - E + F) != 2)
    return false;

  if (F != (2 * V - 4))
    return false;

  if ((2 * E) != (3 * F))
    return false;

  return true;
}

//---------------------------------------------------------------------
// 
//
//
//---------------------------------------------------------------------
bool CConvexHullImpl::NonDegenerate()
{
  tFace f = m_tFaces;

  while (f)
  {
    CPoint p[3];
    
    for (size_t i = 0; i < 3; ++i)
    {
      p[i] = f->vertex[i]->Point;
      p[i] = RoundPoint(p[i]);
    }

    if (p[0] == p[1] || p[0] == p[2] || p[1] == p[2])
      return false;

    f = f->pNext;
  }

  return true;
}

//---------------------------------------------------------------------
// Does some checks on the whole hull.
// Return true is the hull is consistent, convex, the Euler rules
// are correct, and the faces are not degenerate.
//---------------------------------------------------------------------
bool CConvexHullImpl::Checks()
{
  tEdge e;
  tFace f;
  int V = 0, E = 0, F = 0;
  bool ret = false;

  ret = Consistency();
  if (ret)
  {
    ret = Convexity();
    if (ret)
    {
      V = nVertexCount;

      e = m_tEdges;
      while (e != NULL)
      {
        E++;
        e = e->pNext;
      }


      f = m_tFaces;
      while (f != NULL)
      {
        F++;
        f = f ->pNext;
      }

      ret = CheckEuler( V, E, F );

      if (ret)
      {
        ret = NonDegenerate();
      }
    }
  }
  return ret;
}

//---------------------------------------------------------------------
// Clear the datastructure.
void CConvexHullImpl::ClearAll()
{
  ClearVertices();
  ClearEdges();
  ClearFaces();
  //ClearBuckets();
}

void CConvexHullImpl::ClearVertices()
{
  /*
  TVertexSet::iterator it = m_tVertices.begin();
  while ( it != m_tVertices.end() ) {
    delete *it;
    ++it;
  }
  m_tVertices.clear();
  */
  m_vcVertices.clear();
}

void CConvexHullImpl::ClearEdges()
{
  tEdge t, v = m_tEdges;
  while (v != NULL)
  {
    t = v->pNext;
    v->Delete();
    v = t;
  }
  m_tEdges = NULL;
}

void CConvexHullImpl::ClearFaces()
{
  tFace t, v = m_tFaces;
  while (v != NULL)
  {
    t = v->pNext;
    v->Delete();
    v = t;
  }
  m_tFaces = NULL;
}


}//end namespace geo


#ifdef _MSC_VER
#pragma warning ( pop )
#endif