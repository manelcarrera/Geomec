// Copyright (c) 2011 TNO DIANA BV                              Confidential
//
// ILine.h: interface for the ILine class.
//
// Description : Interface for a 3D line
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILINE_H__CB6A3A09_48A4_43F3_9471_F371FCB92D66__INCLUDED_)
#define AFX_ILINE_H__CB6A3A09_48A4_43F3_9471_F371FCB92D66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"
#include "IElement.h"

#include "GeometryExports.h"

namespace geo {
class CVector;
class IPoint;
class IPlane;

class GEOMETRY_EXPORT ILine : public IElement {

protected:
  // The default constructor of CLine constructs an
  // undefined line for container purposes. Initializing is
  // done by assignment.
  ILine();

public:
  class GEOMETRY_EXPORT CLineLess {
  public:
    bool operator()(const CLine &Line1, const CLine &Line2) const;
  };

  const IPoint &First() const;
  void First(const IPoint &point);

  const IPoint &Second() const;
  void Second(const IPoint &point);

  virtual int NrOfPoints() const;
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);

  virtual int NrOfLines() const;
  virtual const ILine &Line(int nIndex) const;

  virtual IElement::TDoubleVec WorldToIso(const geo::IPoint &point) const;

  virtual IElement::TDoubleVec ShapeFunction(const IElement::TDoubleVec &isocoords) const;

  // integration points
  virtual int IntegrationPointSize() const;
  virtual const TDoubleVec &IntegrationPointCoords(int nIndex) const;
  virtual const double &IntegrationPointWeight(int nIndex) const;

  virtual double Size() const;
  double Length() const { return Size(); }

  double SquareLength() const;

  // The intersection between two lines return a point.
  // Return an Empty point if lines are paralel or overplapping
  // Intersection are given for infinite lines
  // thus also if intersection point is not on one of the line
  CPoint Intersection(const ILine &line) const;

  // Calculates the closest points of two lines. (infinite lines)
  // if they intersects these points are the same.
  // if they parallel it returns false otherwise true
  // PointAtLine is the point on the given line
  // PointAtThis is the point on "this" line
  bool GetClosestPoints(const ILine &line, CPoint &PointAtLine, CPoint &PointAtThis) const;

  // gives the direction vector of the line
  CVector Dir() const;

  // project a point on the line (perpendicular to the line)
  CPoint Project(const IPoint &point) const;

  // gives the distance between a point and a line.
  // that is the smallest distance (perpendicular to the line)
  double Distance(const IPoint &pt) const;
  double SquareDistance(const IPoint &pt) const;

  // see if a given points is "on" the line
  // bIncludeEdge: include the definition points of the line
  bool Contains(const IPoint &point, bool bIncludeEdge = true) const;

  // see if two lines are parallel(have no intersection)
  bool IsParallel(const ILine &line) const;

  // returns the min point of the boundingbox of the line
  CPoint Min() const;

  // returns the max point of the boundingbox of the line
  CPoint Max() const;

  // assignment
  ILine &operator=(const ILine &rhs);

  // compares lines (internal EPS is used)  First,Second dependent
  bool operator==(const ILine &rhs) const;

  // compares lines First() and Second() indepenedent!!
  bool IsEqual(const ILine &rhs) const;

  // see if two lines are connected
  //(share one common vertex?)
  //  so return the indexes of the connection point
  // ##ModelId=3BC2A45D0076
  bool IsConnected(const ILine &line, int &nIndex1, int &nIndex2) const;

  // the complete transformation-matrix to rotate objects with
  // an ange arround this line
  // ##ModelId=3BC2A45D007F
  void GetRotationMatrix(double AngleDeg, IMatrix &matrix) const;

  // Interface of IObject
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual void Rotate(const IVector &vec, const double &dAngleDeg);
  virtual void Move(const IVector &vec);
  virtual void Transform(const IMatrix &matrix);
  virtual void Mirror(const geo::IPlane &plane);

  virtual CMatrix ShapeFunctionDerived(const IElement::TDoubleVec &isocoords) const;
  virtual std::vector<IElement::TDoubleVec> IsoCoordinates() const;

  // See if the point pt lies in the direction of vecDir from the reference point ptRef
  // ptRef and pt must be on the line and vecDir must be a vector parallel to the line
  bool InDirectionOf(const IPoint &ptRef, const IVector &vecDir, const IPoint &pt) const;

  // See if the two points are on the same side of the line
  // if bIncludeEdge is true, then true is also returned if both
  // points are near enough to the line
  bool SameSide(const IPoint &pt1, const IPoint &pt2, bool bIncludeLine);

  // return a representative length for this element
  // length of a line
  // square root of area for a face
  // 3rd square root of volume for a body
  virtual double RepresentativeLength() const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleLine(*this); }

  static void PrepareMapping();

private:
  const IElement::TIntPtVec &IntegrationPoints() const;
  static const IElement::TIntPtVec &IntegrationPoints(int order);
  static void BuildIntegrationPoints(IElement::TIntPtVec &vec, int numint);
};

} // namespace geo
#endif // !defined(AFX_ILINE_H__CB6A3A09_48A4_43F3_9471_F371FCB92D66__INCLUDED_)
