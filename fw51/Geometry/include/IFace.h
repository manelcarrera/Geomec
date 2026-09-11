// IFace.h: interface for the IFace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IFACE_H__5B834355_1C88_42A0_868B_B2FA7E258155__INCLUDED_)
#define AFX_IFACE_H__5B834355_1C88_42A0_868B_B2FA7E258155__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"
#include "IElement.h"
#include "ILine.h"
#include "Vector.h"
#include <set>

#include "GeometryExports.h"

namespace geo {
class CVector;
class CPoint;
class IPlane;
class CPlane;
class IBody;

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description	: The IFace object represents a flat face in 3D space.
//				: it is the base element of a surface and must be convex,and not selfintersecting
//				: If the face is part of a body, make sure that the normal points outward.
// Status		: Finished (80%)
// Remarks		:
//				:
// Last review	: 27-03-2002
class GEOMETRY_EXPORT IFace : public IElement {
  class GEOMETRY_EXPORT CPlanePointLess {
    geo::CVector m_vector;

  public:
    bool operator()(const geo::CPoint &p1, const geo::CPoint &p2);
  };

  class GEOMETRY_EXPORT CFaceLine : public ILine {
    int m_nIndex;
    const IFace &m_parent;

  public:
    CFaceLine(const IFace &parent, int nIndex);
    virtual int NrOfPoints() const;
    virtual const IPoint &Point(int nIndex) const;
    using ILine::Point;
    virtual size_t NrOfNodes() const;
    virtual const INode &Node(int nIndex) const;
    virtual void Node(int nIndex, const IPoint &point);
    virtual int PointIndex(int nIndex) const;
    virtual const IElementSet *IndexingElementSet() const;

    virtual size_t Order() const;

    int FacePoint(int nIndex) const;
    int FaceNode(int nIndex) const;
  };

  mutable std::vector<CFaceLine *> m_vcLine; // cache for Line() function

  void CreateLineCache() const;
  void DestroyLineCache() const;
  bool PointInPoly(const IPoint &point, std::vector<IPoint *> &points) const;

protected:
  bool dblCmpLt(const double &par1, const double &par2) const;
  bool dblCmpEq(const double &par1, const double &par2) const;
  bool dblCmpGt(const double &par1, const double &par2) const;
  bool dblCmpLE(const double &par1, const double &par2) const;
  bool dblCmpGE(const double &par1, const double &par2) const;
  IFace();
  virtual void InvalidateCache();

public:
  class GEOMETRY_EXPORT FaceLess {
  public:
    bool operator()(const IFace *f1, const IFace *f2) const { return *f1 < *f2; }
  };

  virtual ~IFace();
  // returns the point of intersection between a face and a line.
  virtual CPoint Intersection(const ILine &line) const;
  typedef std::vector<CLine> TLineVec;
  virtual TLineVec Intersection(const IPlane &plane) const;
  virtual bool Intersects(const IPlane &plane) const;
  virtual bool IsSelfIntersecting() const;
  double Area() const { return Size(); }
  virtual double SignedArea() const;

  virtual int NrOfLines() const;
  virtual const ILine &Line(int nIndex) const;
  typedef std::vector<int> TIndexVec;
  // returns a vector of this face's local point indices that the line with the
  // given index is connected to (this is without higher order intermediate nodes)
  size_t NrOfLinePoints() const;
  int LinePointIndex(int nLineIndex, int nPointIndex) const;
  // same as above, given the line instead of the index
  int LinePointIndex(const ILine &line, int nPointIndex) const;

  // returns a vector of this face's local node indices that the line with the
  // given index is connected to (this includes higher order intermediate nodes)
  size_t NrOfLineNodes() const;
  int LineNodeIndex(int nLineIndex, int nNodeIndex) const;
  // same as above, given the line instead of the index
  int LineNodeIndex(const ILine &line, int nNodeIndex) const;

  virtual bool IsConvex() const;
  virtual void AssertValid() const;
  virtual CVector Normal() const;
  virtual CVector NormalInPoint(const geo::IPoint &pt) const;
  virtual bool IsPlanar() const;
  virtual bool Contains(const IPoint &point, bool bIncludeEdge) const;
  virtual bool ContainsInXYPlane(const IPoint &point, bool bIncludeEdge) const;

  virtual const IBody *Parent() const { return 0; }

  // returns the (smallest) signed distance of the point ot the face.
  // IMPORTANT: if you want the true value "bExact" should be "true" (see IPlane).
  // sign of return value depends on direction (implmentation) of Normal().
  virtual double SignedDistance(const IPoint &point, bool bExact) const;

  // get the transformation matrix (and inverse)
  // for transforming this face in a plane
  //  returns the rotation angle in degrees
  double GetTransMatInPlane(const IPlane &plane, IMatrix &TM, IMatrix &TMI) const;

  // copy the points of Iface in an array and rotate it around the origin
  // by an angle so that after rotation the copy of IFace is on the plane
  //  returns the rotation angle in degrees
  double GetPointArrInPlane(const IPlane &plane, CArray<CPoint> &arr, IMatrix *pTM = 0, IMatrix *pTMI = 0) const;

  // returns a plane; The direction of the normal of this plane is not dependent on the ordering of the points!!!!
  CPlane Plane() const;

  // return a representative length for this element
  // length of a line
  // square root of area for a face
  // 3rd square root of volume for a body
  virtual double RepresentativeLength() const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleFace(*this); }
};

} // namespace geo

#endif // !defined(AFX_IFACE_H__5B834355_1C88_42A0_868B_B2FA7E258155__INCLUDED_)
