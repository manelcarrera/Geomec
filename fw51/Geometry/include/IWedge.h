 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */

#ifndef IWEDGE_H
#define IWEDGE_H

#include "IBody.h"

#include "GeometryExports.h"

namespace geo {

class CBodyLine;

class GEOMETRY_EXPORT  IWedge : public IBody
{

public: 
  IWedge();
  virtual ~IWedge();

  virtual int NrOfPoints() const;
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);

  virtual int NrOfFaces() const;
  enum eFaceName { L1 = 1, L2 = 2, L3 = 3, ZETA1 = 0, ZETA2 = 4 };
  virtual const IFace &Face(int nIndex) const;
  using IBody::FaceNodeIndices;
  using IBody::FacePointIndices;
  virtual const TIndexVec& FaceNodeIndices(int nIndex) const;
  virtual const TIndexVec& FacePointIndices(int nIndex) const;

  virtual int NrOfLines() const;
  virtual const ILine &Line(int nIndex) const;
  using IBody::LinePointIndices;
  using IBody::LineNodeIndices;
  virtual const TIndexVec& LinePointIndices(int nIndex) const;
  virtual const TIndexVec& LineNodeIndices(int nIndex) const;

  virtual IElement::TDoubleVec ShapeFunction(const IElement::TDoubleVec& isocoords) const;

  virtual CMatrix ShapeFunctionDerived(const IElement::TDoubleVec& isocoords) const;
  virtual std::vector<IElement::TDoubleVec> IsoCoordinates() const;

  // integration points
  virtual int IntegrationPointSize() const;
  virtual const TDoubleVec& IntegrationPointCoords(int nIndex) const;
  virtual const double& IntegrationPointWeight(int nIndex) const;

  static void PrepareMapping();

private:
  typedef std::vector<TIndexVec> TFaceNodeVec;
  static const TIndexVec& FaceNodeIndices( int order, int nIndex );
  static void InitFaceNodeIndices( std::vector<TFaceNodeVec>& FaceNodeIdxs );

  typedef std::vector<TIndexVec> TLineNodeVec;
  static const TIndexVec& LineNodeIndices( int order, int nIndex );
  static void InitLineNodeIndices( std::vector<TLineNodeVec>& lineNodeIdxs );

  static void BuildIntegrationPoints(IElement::TIntPtVec& vec, int nintxy, int nintz);
  static const TIntPtVec& IntegrationPoints( int order );
  const TIntPtVec& IntegrationPoints() const;

  mutable std::vector<IFace*> m_vcFaces;
  mutable std::vector<CBodyLine*> m_vcLines;
};

} // namespace geo

#endif
