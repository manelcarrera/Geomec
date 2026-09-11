/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ConvexHull_2D.cpp: implementation of the CConvexHull_2D class.
//
//////////////////////////////////////////////////////////////////////
#include "ConvexHull_2D.h"
#include "dimple.h"
#include <stack>

namespace geo {

/*----------Point(s) Structure-------------*/
typedef class tPointStructure tsPoint;
typedef tsPoint *tPoint;

/*!
  Default construction
*/
tPointStructure::tPointStructure() : vnum(-1), bDelete(false), m_coor_disabled(0) {}

tPointStructure::tPointStructure(const IPoint &point, int nIndex, int coor_disabled)
    : vnum(nIndex), p(point), bDelete(false), m_coor_disabled(coor_disabled) {}

tPointStructure::tPointStructure(const tPointStructure &rhs)
    : vnum(rhs.vnum), p(rhs.p), bDelete(rhs.bDelete), m_coor_disabled(rhs.m_coor_disabled) {}

tPointStructure &tPointStructure::operator=(const tPointStructure &rhs) {
  vnum = rhs.vnum;
  p = rhs.p;
  bDelete = rhs.bDelete;
  m_coor_disabled = rhs.m_coor_disabled;
  return *this;
}

const double &tPointStructure::X() const {
  if (m_coor_disabled == 0)
    return p.Z();
  return p.X();
}

const double &tPointStructure::Y() const {
  if (m_coor_disabled == 1)
    return p.Z();
  return p.Y();
}

const double &tPointStructure::Z() const {
  assert(false);
  return p.Z();
}

void tPointStructure::print() const {
  if (bDelete)
    printf("%d (%f, %f) x\n", vnum, X(), Y());
  else
    printf("%d (%f, %f)\n", vnum, X(), Y());
}

long tPointStructure::SaveProgressSize() const { return 0; }

void print(tPointArray s, int size) {
  printf("Structure\n");
  for (int i = 0; i < size; i++) {
    s[i].print();
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvexHull_2D::CConvexHull_2D() : m_aPoint(0), m_size(0) {}

CConvexHull_2D::CConvexHull_2D(const std::vector<const geo::IPoint *> &vcPoint, int coor_disabled)
    : m_size((int)vcPoint.size()) {
  m_aPoint = new tPointStructure[m_size];
  for (int i = 0; i < m_size; i++) {
    m_aPoint[i] = tPointStructure(*vcPoint[i], i, coor_disabled);
  }
  BuildConvexHull();
}

CConvexHull_2D::CConvexHull_2D(const CPtrArray<IPoint> &points, int coor_disabled) : m_size(points.Size()) {
  m_aPoint = new tPointStructure[m_size];
  for (int i = 0; i < m_size; i++) {
    m_aPoint[i] = tPointStructure(points.Object(i), i, coor_disabled);
  }
  BuildConvexHull();
}

CConvexHull_2D::~CConvexHull_2D() { delete[] m_aPoint; }

void CConvexHull_2D::LoadHull() {
  assert(m_vcPoint.size() == m_size);
  for (size_t i = 0; i < m_size; ++i)
    m_aPoint[i].point(m_vcPoint[i]);
}

void CConvexHull_2D::StoreHull() {
  m_vcPoint.resize(m_size);

  for (size_t i = 0; i < m_size; ++i)
    m_vcPoint[i] = m_aPoint[i].originalPoint();
}

void CConvexHull_2D::Point(int /*nIndex*/, const IPoint & /*pt*/) { assert(false); }

const IPoint &CConvexHull_2D::Point(int nIndex) const { return m_aPoint[nIndex].originalPoint(); }

int CConvexHull_2D::NrOfPoints() const { return m_size; }

size_t CConvexHull_2D::Order() const { return 1; }

double CConvexHull_2D::Size() const { return fabs(SignedArea()); }

long CConvexHull_2D::SaveProgressSize() const { return 1; }

/*
This code is described in "Computational Geometry in C" (Second Edition),
Chapter 3.  It is not written to be comprehensible without the
explanation in that book.

Input: 2n integer coordinates of points in the plane.
Output: the convex hull, cw, in PostScript; other output precedes the PS.

NB: The original array storing the points is overwritten.

Compile: gcc -o graham graham.c macros.h

Written by Joseph O'Rourke.
Last modified: October 1997
Questions to orourke@cs.smith.edu.
--------------------------------------------------------------------
This code is Copyright 1998 by Joseph O'Rourke.  It may be freely
redistributed in its entirety provided that this copyright notice is
not removed.
--------------------------------------------------------------------
*/

/* Global variables */
int ndelcount = 0;          /* Number bDeleted */
tPointStructure base_point; /* Lowest point */

/*----------Stack Structure-------------*/
typedef struct tStackCell tsStack; /* Used on in NEW() */
typedef tsStack *tStack;
struct tStackCell {
  tPoint p;
  tStack next;
};

/*----------Function Prototypes-------------*/
tStack Pop(tStack s);
tStack Push(tPoint p, tStack top);
int AreaSign(const geo::IPoint &a, const geo::IPoint &b, const geo::IPoint &c);
bool Left(const geo::IPoint &a, const geo::IPoint &b, const geo::IPoint &c);

/*---------------------------------------------------------------------
Compare: returns -1,0,+1 if p1 < p2, =, or > respectively;
here "<" means smaller angle.  Follows the conventions of qsort.
---------------------------------------------------------------------*/
extern "C" int CH2D_Compare(const void *tpi, const void *tpj) {
  int a;       /* area */
  double x, y; /* projections of ri & rj in 1st quadrant */

  tPoint pi, pj;
  pi = (tPoint)tpi;
  pj = (tPoint)tpj;

  a = AreaSign(base_point, *pi, *pj);
  if (a > 0)
    return -1;
  if (a < 0)
    return 1;

  /* Collinear with base_point */
  x = fabs(pi->X() - base_point.X()) - fabs(pj->X() - base_point.X());
  y = fabs(pi->Y() - base_point.Y()) - fabs(pj->Y() - base_point.Y());

  ndelcount++;
  if ((x < 0) || (y < 0)) {
    pi->Delete();
    return -1;
  }

  if ((x > 0) || (y > 0)) {
    pj->Delete();
    return 1;
  }

  /* points are coincident */
  if (*pj < *pi)
    pj->Delete();
  else
    pi->Delete();
  return 0;
}

void CConvexHull_2D::BuildConvexHull() {
  // Do the algorithm
  FindLowest();
  base_point = m_aPoint[0]; /* Assign to base point for Compare Function */
  //   print( m_aPoint, m_size );
  qsort(&m_aPoint[1],    /* pointer to 1st elem */
        m_size - 1,      /* number of elems */
        sizeof(tsPoint), /* size of each elem */
        CH2D_Compare     /* -1,0,+1 compare function */
  );

  // wjrx mantis 3191
  // CH2D_Compare does not catch multiple coincident points on the hull
  //
  int lower = 0;
  while (lower < m_size) {
    // Find the range [lower,upper) of identical points
    int upper = lower + 1;
    while (upper < m_size && m_aPoint[lower].X() == m_aPoint[upper].X() && m_aPoint[lower].Y() == m_aPoint[upper].Y()) {
      ++upper;
    }

    // In the range of identical points, keep one and Delete() the others.
    bool deleteAllOthers = false;
    for (int i = lower; i < upper; ++i) {
      if (deleteAllOthers == true) {
        m_aPoint[i].Delete();
      } else if (m_aPoint[i].isDeleted() == false) {
        deleteAllOthers = true;
      }
    }

    lower = upper;
  }

  //  print( m_aPoint, m_size );
  if (ndelcount > 0) {
    Squash();
    // printf("After squashing:\n");
  }
  //   print( m_aPoint, m_size );
  Graham();

  StoreHull();
}

/*---------------------------------------------------------------------
FindLowest finds the rightmost lowest point and swaps with 0-th.
The lowest point has the min y-coord, and amongst those, the
max x-coord: so it is rightmost among the lowest.
---------------------------------------------------------------------*/
void CConvexHull_2D::FindLowest() {
  int i;
  int m = 0; /* Index of lowest so far. */

  for (i = 1; i < m_size; i++)
    if ((m_aPoint[i].Y() < m_aPoint[m].Y()) ||
        ((m_aPoint[i].Y() == m_aPoint[m].Y()) && (m_aPoint[i].X() > m_aPoint[m].X())))
      m = i;
  //   printf("Swapping %d with 0\n", m);
  Swap(0, m); /* Swap P[0] and P[m] */
}

void CConvexHull_2D::Swap(int i, int j) {
  tPointStructure tmp(m_aPoint[i]);
  m_aPoint[i] = m_aPoint[j];
  m_aPoint[j] = tmp;
}

/*---------------------------------------------------------------------
Pops off top elment of stack s, frees up the cell, and returns new top.
---------------------------------------------------------------------*/
tStack Pop(tStack s) {
  tStack top;

  top = s->next;
  delete s;
  return top;
}

/*---------------------------------------------------------------------
Get a new cell, fill it with p, and push it onto the stack.
Return pointer to new stack top.
---------------------------------------------------------------------*/
tStack Push(tPoint p, tStack top) {
  /* Get new cell and fill it with point. */
  tStack s = new tsStack;

  s->p = p;
  s->next = top;
  return s;
}

/*---------------------------------------------------------------------
Performs the Graham scan on an array of angularly sorted points P.
---------------------------------------------------------------------*/
void CConvexHull_2D::Graham() {
  tStack top;

  int i;
  tPoint p1, p2; /* Top two points on stack. */

  /* Initialize stack. */
  top = NULL;
  top = Push(&m_aPoint[0], top);
  top = Push(&m_aPoint[1], top);

  /* Bottom two elements will never be removed. */
  i = 2;

  while (i < m_size) {
    assert(top->next);
    p1 = top->next->p;
    p2 = top->p;
    if (Left(*p1, *p2, m_aPoint[i])) {
      top = Push(&m_aPoint[i], top);
      i++;
    } else
      top = Pop(top);
  }

  // Write Data back from stack
  tPointArray tmp = m_aPoint;

  // Allocate stack size
  m_size = 1;
  tStack s = top;
  while (s->next) {
    s = s->next;
    m_size++;
  }
  m_aPoint = new tPointStructure[m_size];

  // Populate stack
  i = 0;
  do {
    m_aPoint[i] = *top->p;
    top = Pop(top);
    i++;
  } while (top);

  delete[] tmp;
}

/*---------------------------------------------------------------------
Squash removes all elements from P marked bDelete.
---------------------------------------------------------------------*/
void CConvexHull_2D::Squash() {
  int j = 0;
  for (int i = 0; i < m_size; i++) {
    if (!m_aPoint[i].isDeleted()) { /* if not marked for deletion */
      m_aPoint[j] = m_aPoint[i];    /* Copy P[i] to P[j]. */
      j++;
    }
    /* else do nothing: bDelete by skipping. */
  }
  m_size = j;
}

/*---------------------------------------------------------------------
Returns twice the signed area of the triangle determined by a,b,c.
The area is positive if a,b,c are oriented ccw, negative if cw,
and zero if the points are collinear.
---------------------------------------------------------------------*/
/*

wedx, mantis #3021:
The size of the area between two vectors depends on the length of the vectors. In
the case of this convex hull calculation we're interested in relative values, otherwise
different criteria are used for the same pointsets with different scale factors. This
results in errors in the convex hull for very large-scale pointsets (like in the model
in mantis #3021).

 */
double Area2(const geo::IPoint &a, const geo::IPoint &b, const geo::IPoint &c) {
  double abx = b.X() - a.X();
  double aby = b.Y() - a.Y();
  double acx = c.X() - a.X();
  double acy = c.Y() - a.Y();
  double lab = sqrt(abx * abx + aby * aby);
  double lac = sqrt(acx * acx + acy * acy);

  return abx / lab * acy / lac - acx / lac * aby / lab;
}

/*---------------------------------------------------------------------
Returns true iff c is strictly to the left of the directed
line through a to b.
---------------------------------------------------------------------*/
bool Left(const geo::IPoint &a, const geo::IPoint &b, const geo::IPoint &c) { return Area2(a, b, c) > 0; }

int AreaSign(const geo::IPoint &a, const geo::IPoint &b, const geo::IPoint &c) {
  double area2;

  area2 = Area2(a, b, c);

  if (area2 > 1e-10)
    return 1;
  else if (area2 < -1e-10)
    return -1;
  return 0;
}

/*! Resize the convex hull by increment in all directions.
 * Repeated calls do repeated resizes.
 */
void CConvexHull_2D::ResizeHull(const double &increment) {
  LoadHull();

  // temporary storage for resize hull
  tPointArray aPoint = new tPointStructure[NrOfPoints()];

  for (int ii = 0; ii < NrOfPoints(); ++ii) {
    /*
     * Outside of hull
     *
     *     ^vn_ab      ^ vn_bc   aa,bb,cc: points on hull
     *     |          /          v_ab: vector from aa to bb
     *     |aa    bb /           vn_ab: vector normal to v_ab, points outside
     *  ...*------->*
     *           v_ab\
     *                \
     *                 \
     *                  v v_bc
     *                   * cc
     *                    .
     *                     .
     *
     * Inside of hull
     *
     *
     * We want to move hull point bb outward. We do this by translating lines
     * aa to bb and bb to cc outward and calculating the intersection point
     * of the translated lines.
     */

    int aa = ii - 1;
    if (aa < 0)
      aa = NrOfPoints() - 1;
    int bb = ii;
    int cc = ii + 1;
    if (cc >= NrOfPoints())
      cc = 0;
    CVector v_ab(Point(aa), Point(bb));
    CVector v_bc(Point(bb), Point(cc));
    CVector vn_ab(-v_ab.Y(), v_ab.X());
    CVector vn_bc(-v_bc.Y(), v_bc.X());
    vn_ab = vn_ab.UnitVector() * increment;
    vn_bc = vn_bc.UnitVector() * increment;

    /*
     *      p1      p2
     *  ---*--------*           Move aa 'outward' by argument increment
     *     ^vn_ab
     *     |
     *     |aa
     *  ...*------->*
     *
     */
    CPoint p1 = Point(aa) + vn_ab;
    CPoint p2 = Point(bb) + vn_ab;
    CPoint p3 = Point(bb) + vn_bc;
    CPoint p4 = Point(cc) + vn_bc;

    // Determine new position of point bb by intersecting
    // line from p1 to p2 with line from p3 to p4.
    //
    // http://en.wikipedia.org/wiki/Line-line_intersection
    double x1 = p1.X();
    double y1 = p1.Y();
    double x2 = p2.X();
    double y2 = p2.Y();
    double x3 = p3.X();
    double y3 = p3.Y();
    double x4 = p4.X();
    double y4 = p4.Y();
    double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (fabs(denom) > EPS) {
      aPoint[ii].point(CPoint(((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denom,
                              ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denom));
    } else {
      aPoint[ii].point(p2);
    }
  }

  for (int ii = 0; ii < NrOfPoints(); ++ii) {
    m_aPoint[ii].point(aPoint[ii].originalPoint());
  }

  delete[] aPoint;
}

bool CConvexHull_2D::ContainsXY(const IPoint &p, bool bIncludeEdge) const {
  assert(m_size > 0);
  tPointStructure point(p, -1, m_aPoint[0].disabledCoordinate());
  bool c1 = false;
  bool c2 = false;

  // trace line to the right
  int i, j;
  for (i = 0, j = NrOfPoints() - 1; i < NrOfPoints(); j = i++) {
    if (dblCmpEq(m_aPoint[i].Y(), m_aPoint[j].Y()) && dblCmpEq(point.Y(), m_aPoint[i].Y()) &&
        ((dblCmpLE(point.X(), m_aPoint[i].X()) && dblCmpGE(point.X(), m_aPoint[j].X())) ||
         (dblCmpGE(point.X(), m_aPoint[i].X()) && dblCmpLE(point.X(), m_aPoint[j].X())))) {
      // p is on horizontal line
      return bIncludeEdge;
    }
    if ((((dblCmpLE(m_aPoint[i].Y(), point.Y())) && (dblCmpLt(point.Y(), m_aPoint[j].Y()))) ||
         ((dblCmpLE(m_aPoint[j].Y(), point.Y())) && (dblCmpLt(point.Y(), m_aPoint[i].Y())))) &&
        (dblCmpLt(point.X(), (m_aPoint[j].X() - m_aPoint[i].X()) * (point.Y() - m_aPoint[i].Y()) /
                                     (m_aPoint[j].Y() - m_aPoint[i].Y()) +
                                 m_aPoint[i].X()))) {
      c1 = !c1;
    }
  }

  // trace line to the left
  for (i = 0, j = NrOfPoints() - 1; i < NrOfPoints(); j = i++) {
    if ((((dblCmpLE(m_aPoint[i].Y(), point.Y())) && (dblCmpLt(point.Y(), m_aPoint[j].Y()))) ||
         ((dblCmpLE(m_aPoint[j].Y(), point.Y())) && (dblCmpLt(point.Y(), m_aPoint[i].Y())))) &&
        (dblCmpGt(point.X(), (m_aPoint[j].X() - m_aPoint[i].X()) * (point.Y() - m_aPoint[i].Y()) /
                                     (m_aPoint[j].Y() - m_aPoint[i].Y()) +
                                 m_aPoint[i].X())))

      c2 = !c2;
  }

  if (c1 != c2)
    return bIncludeEdge; // point is on a line

  return c1;
}

} // namespace geo
