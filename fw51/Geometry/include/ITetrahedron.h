/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ITetrahedron.h: interface for the ITetrahedron class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITETRAHEDRON_H__C86E3F89_D729_4A1D_9543_E7507DE111EF__INCLUDED_)
#define AFX_ITETRAHEDRON_H__C86E3F89_D729_4A1D_9543_E7507DE111EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"
#include "IBody.h"

#include "GeometryExports.h"

namespace geo {

class CBodyTriangle;
class CBodyLine;

class GEOMETRY_EXPORT ITetrahedron : public IBody {

public:
  // Construction / Destruction
  ITetrahedron();
  virtual ~ITetrahedron();

  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int NrOfPoints() const;

  // the number of faces surrounding this body
  virtual int NrOfFaces() const;
  enum eFaceName { L1 = 0, L2 = 1, L3 = 2, L4 = 3 };
  // face retrieval
  virtual const IFace &Face(int nIndex) const;

  using IBody::FaceNodeIndices;
  using IBody::FacePointIndices;
  using IBody::LineNodeIndices;
  using IBody::LinePointIndices;
  virtual const TIndexVec &FacePointIndices(int nIndex) const;

  virtual int NrOfLines() const;
  virtual const ILine &Line(int nIndex) const;
  virtual const TIndexVec &LinePointIndices(int nIndex) const;
  virtual const TIndexVec &LineNodeIndices(int nIndex) const;

  // returns a vector of this body's local node indices that the face with the
  // given index is connected to
  virtual const TIndexVec &FaceNodeIndices(int nIndex) const;

  virtual CMatrix ShapeFunctionDerived(const IElement::TDoubleVec &isocoords) const;
  virtual std::vector<IElement::TDoubleVec> IsoCoordinates() const;

  virtual IElement::TDoubleVec WorldToIso(const geo::IPoint &point) const;

  virtual IElement::TDoubleVec ShapeFunction(const IElement::TDoubleVec &isocoords) const;

  // integration points
  virtual int IntegrationPointSize() const;
  virtual const TDoubleVec &IntegrationPointCoords(int nIndex) const;
  virtual const double &IntegrationPointWeight(int nIndex) const;

  virtual double Size() const;

  // implemented here
  virtual double SignedVolume() const;

  double InfluenceVolume(int nNode) const;
  virtual double InfluenceSize(int nNode) const { return InfluenceVolume(nNode); }

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleTetrahedron(*this); }

  // This static function can be used before an ITetrahedron object has been constructed to check if the order in which
  // the points are presented to the constructor wil result in a tetrahedron with a negative volume (false) or a
  // tetrahedron with a positive volume (true). If the function returns false swap p2 and p3 around.
  static bool CheckOrientation(const IPoint &p0, const IPoint &p1, const IPoint &p2, const IPoint &p3);
  static void PrepareMapping();

private:
  typedef std::vector<TIndexVec> TFaceNodeVec;
  static void InitFaceNodeIndices(std::vector<TFaceNodeVec> &FaceNodeIdxs);
  static const TIndexVec &FaceNodeIndices(int order, int nIndex);

  typedef std::vector<TIndexVec> TLineNodeVec;
  static void InitLineNodeIndices(std::vector<TLineNodeVec> &LineNodeIdxs);
  static const TIndexVec &LineNodeIndices(int order, int nIndex);

  static void BuildIntegrationPoints(IElement::TIntPtVec &vec, int numint);
  static const TIntPtVec &IntegrationPoints(int order);
  const TIntPtVec &IntegrationPoints() const;

  mutable std::vector<CBodyTriangle *> m_vcFaces;
  mutable std::vector<CBodyLine *> m_vcLines;

  IElement::TDoubleVec WorldToIso1stOrder(const IPoint &point) const;
};

} // end namespace geo
#endif // !defined(AFX_ITETRAHEDRON_H__C86E3F89_D729_4A1D_9543_E7507DE111EF__INCLUDED_)
