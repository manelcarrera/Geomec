// IPolyLine.h: interface
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPOLYLINE_H__FBA98FCE_E342_416F_B9FA_746A24E6B587__INCLUDED_)
#define AFX_IPOLYLINE_H__FBA98FCE_E342_416F_B9FA_746A24E6B587__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"
#include "IElementSet.h"
#include "VectorTempl.h"

#include "GeometryExports.h"

namespace geo {
class ILine;
class IPlane;

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : Interface for a 3D polyline
// Status      : Finished (80%)
// Remarks     :
// Last review : 27-03-2002
class GEOMETRY_EXPORT IPolyLine : public IElementSet {
protected:
  IPolyLine();

public:
  virtual int PointSize() const;
  virtual const IPoint &Point(int nIndex) const;

  virtual int ElementSize() const;
  virtual const IElement &Element(int nIndex) const;

  virtual int LineSize() const = 0;
  virtual const ILine &Line(int Index) const = 0;

  // implemented in this inteface:
  virtual CArray<CPoint> Intersection(const IPlane &plane) const;

  // return the point for the given length
  virtual CPoint GetPointAtLength(const double &l) const;

  // return the length above the point at given index
  virtual double GetLengthAtPoint(int Index) const;

  // return the interpolated length above the given point
  // returns -1 if the point is not on the polyline
  virtual double GetLengthAtPoint(const IPoint &p) const;

  // returns the total length of the polyline
  virtual double GetLength() const;

  virtual void Swap(int nIndex1, int nIndex2) = 0;

  // Project the given point on the polyline.
  // If pLineIndex is not NULL, it is given the
  // index of the line that contains the returned point.
  // The returned point can be empty, if no valid
  // projection can be found. In that case pLineIndex is
  // not initialized.
  CPoint Project(const IPoint &point, int *pLineIndex = 0) const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandlePolyLine(*this); }
};

} // namespace geo

#endif // !defined(AFX_IPOLYLINE_H__FBA98FCE_E342_416F_B9FA_746A24E6B587__INCLUDED_)
