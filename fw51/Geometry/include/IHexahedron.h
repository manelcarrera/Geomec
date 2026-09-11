/* Copyright (c) 2011 TNO DIANA BV                              Confidential */

#ifndef IHEXAHEDRON_H
#define IHEXAHEDRON_H

#include "DispatchVisitorBase.h"
#include "IBody.h"

#include "GeometryExports.h"

namespace geo {

class CBodyQuadrilateral;
class CBodyLine;

class GEOMETRY_EXPORT IHexahedron : public IBody {

public:
  IHexahedron();
  ~IHexahedron();

  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int NrOfPoints() const;

  // the number of faces surrounding this body
  virtual int NrOfFaces() const;
  // face retrieval
  enum eFaceName { KSI1 = 4, KSI2 = 2, ETA1 = 1, ETA2 = 3, ZETA1 = 0, ZETA2 = 5 };
  virtual const IFace &Face(int nIndex) const;

  // returns a vector of this body's local node indices that the face with the
  // given index is connected to
  using IBody::FaceNodeIndices;
  using IBody::FacePointIndices;
  virtual const TIndexVec &FaceNodeIndices(int nIndex) const;
  virtual const TIndexVec &FacePointIndices(int nIndex) const;

  virtual int NrOfLines() const;
  virtual const ILine &Line(int nIndex) const;
  using IBody::LineNodeIndices;
  using IBody::LinePointIndices;
  virtual const TIndexVec &LinePointIndices(int nIndex) const;
  virtual const TIndexVec &LineNodeIndices(int nIndex) const;

  virtual IElement::TDoubleVec ShapeFunction(const IElement::TDoubleVec &isocoords) const;

  virtual CMatrix ShapeFunctionDerived(const IElement::TDoubleVec &isocoords) const;
  virtual std::vector<IElement::TDoubleVec> IsoCoordinates() const;

  // integration points
  virtual int IntegrationPointSize() const;
  virtual const TDoubleVec &IntegrationPointCoords(int nIndex) const;
  virtual const double &IntegrationPointWeight(int nIndex) const;

  static void PrepareMapping();

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleHexahedron(*this); }

private:
  typedef std::vector<TIndexVec> TFaceNodeVec;
  static void InitFaceNodeIndices(std::vector<TFaceNodeVec> &FaceNodeIdxs);
  static const TIndexVec &FaceNodeIndices(int order, int nIndex);

  typedef std::vector<TIndexVec> TLineNodeVec;
  static void InitLineNodeIndices(std::vector<TLineNodeVec> &LineNodeIdxs);
  static const TIndexVec &LineNodeIndices(int order, int nIndex);

  const TIntPtVec &IntegrationPoints() const;
  static const TIntPtVec &IntegrationPoints(int order);
  static void BuildIntegrationPoints(IElement::TIntPtVec &vec, int numint);

  mutable std::vector<CBodyQuadrilateral *> m_vcFaces;
  mutable std::vector<CBodyLine *> m_vcLines;
};

} // namespace geo

#endif
