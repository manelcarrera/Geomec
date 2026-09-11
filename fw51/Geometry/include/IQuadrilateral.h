 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */

#ifndef IQUADRILATERAL_H
#define IQUADRILATERAL_H

#include "IFace.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT  IQuadrilateral : public IFace
{

public: 
  IQuadrilateral();
  ~IQuadrilateral();

  virtual int NrOfPoints() const;
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);

  enum eLineName { ETA1 = 0, KSI2 = 1, ETA2 = 2, KSI1 = 3 };

  virtual int NrOfLines() const { return 4; }
  
  virtual IElement::TDoubleVec ShapeFunction(const IElement::TDoubleVec& isocoords) const;

  virtual CMatrix ShapeFunctionDerived(const IElement::TDoubleVec& isocoords) const;
  virtual std::vector<IElement::TDoubleVec> IsoCoordinates() const;

  double GetDepthAt(const double& x, const double& y) const;

  // integration points
  virtual int IntegrationPointSize() const;
  virtual const TDoubleVec& IntegrationPointCoords(int nIndex) const;
  virtual const double& IntegrationPointWeight(int nIndex) const;

  static void PrepareMapping();

  // We override this function, because the base implementation assumes a planar
  // face. The quadrilateral is not planar at all, but is convex.
  virtual TLineVec Intersection(const IPlane &plane) const;
  virtual CPoint Intersection(const ILine &line) const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleQuadrilateral(*this); }

private:
  static void BuildIntegrationPoints( IElement::TIntPtVec& vec, int numint );
  static const TIntPtVec& IntegrationPoints( int order );
  const TIntPtVec& IntegrationPoints() const;

};

} // namespace geo

#endif
