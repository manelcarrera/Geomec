/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IPolyLine.cpp: implementation of the IPolyLine class.
//
//////////////////////////////////////////////////////////////////////
#include "IPolyLine.h"
#include "ILine.h"
#include "IPlane.h"
#include "Vector.h"
#include "dimple.h"
#include <cmath>

namespace geo {

IPolyLine::IPolyLine() {}

// return the length above the point at given index
double IPolyLine::GetLengthAtPoint(int Index) const {
  double l = 0.0;

  for (int i = 0; i < Index; i++) {
    l = l + Line(i).Length();
  }
  return l;
}

// return the length above the given point
// returns -1 if the point is not on the polyline
double IPolyLine::GetLengthAtPoint(const IPoint &p) const {
  double l = 0.0;

  for (int i = 0; i < LineSize(); i++) {
    if (Line(i).Contains(p, true)) {
      CPoint dp(Line(i).Second() - Line(i).First());

      double fac = 0.;
      if (fabs(dp.X()) > compareTolerance())
        fac = (p.X() - Line(i).First().X()) / dp.X();
      else if (fabs(dp.Y()) > compareTolerance())
        fac = (p.Y() - Line(i).First().Y()) / dp.Y();
      else if (fabs(dp.Z()) > compareTolerance())
        fac = (p.Z() - Line(i).First().Z()) / dp.Z();
      else
        assert(false);

      l = l + fac * Line(i).Length();
      return l;
    }
    l = l + Line(i).Length();
  }
  return -1;
}

CPoint IPolyLine::GetPointAtLength(const double &l) const {
  AssertValid();

  if (l <= 0.0) // extrapolate before first point
  {
    double fac = l / Line(0).Length();
    return Line(0).First() + Line(0).Dir() * fac;
  } else if (l >= GetLength()) // extrapolate after last point
  {
    double fac = l / Line(LineSize() - 1).Length();
    return Line(LineSize() - 1).First() + Line(LineSize() - 1).Dir() * fac;
  }

  double Length = 0;

  // interpolate between polyline points
  for (int i = 0; i < LineSize(); i++) {
    if (l >= Length && l <= Length + Line(i).Length()) {
      double fac = (l - Length) / Line(i).Length();
      return Line(i).First() + Line(i).Dir() * fac;
    }
    Length = Length + Line(i).Length();
  }

  // not supp. to come here
  assert(false);
  return CPoint();
}

double IPolyLine::GetLength() const {
  AssertValid();
  double Length = 0;

  for (int i = 0; i < LineSize(); i++) {
    Length = Length + Line(i).Length();
  }

  return Length;
}

CArray<CPoint> IPolyLine::Intersection(const IPlane &plane) const {
  CArray<CPoint> ret;
  for (int i = 0; i < LineSize(); i++) {
    CPoint pt = plane.Intersection(Line(i));
    if (!pt.Empty()) {
      ret.PushBack(pt);
    }
  }
  return ret;
}

int IPolyLine::PointSize() const { return LineSize() + 1; }

const IPoint &IPolyLine::Point(int nIndex) const {
  if (nIndex == LineSize())
    return Line(nIndex - 1).Second();

  return Line(nIndex).First();
}

int IPolyLine::ElementSize() const { return LineSize(); }

const IElement &IPolyLine::Element(int nIndex) const { return Line(nIndex); }

/*!
  Project the given point on the polyline.
  If pLineIndex is not NULL, it is given the
  index of the line that contains the returned point.
  The returned point can be empty, if no valid
  projection can be found. In that case pLineIndex is set to -1.
*/
CPoint IPolyLine::Project(const IPoint &point, int *pLineIndex) const {
  CPoint ptRet;
  double dMinSquareDist = DBL_MAX;

  if (pLineIndex)
    (*pLineIndex) = -1;

  int i;
  for (i = 0; i < LineSize(); i++) {
    const ILine &line = Line(i);
    CPoint pt = line.Project(point);
    if (line.Contains(pt, true)) {
      double dSquareDist = point.SquareDistance(pt);
      // is it nearer?
      if (ptRet.Empty() || dSquareDist < dMinSquareDist) {
        ptRet = pt;
        dMinSquareDist = dSquareDist;
        if (pLineIndex)
          *pLineIndex = i;
      }
    }
  }

  return ptRet;
}

} // namespace geo
