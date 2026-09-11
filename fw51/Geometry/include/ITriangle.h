/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ITriangle.h: interface for the ITriangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITRIANGLE_H__DCE5B760_C0D5_4045_AD80_3C57186F4DBB__INCLUDED_)
#define AFX_ITRIANGLE_H__DCE5B760_C0D5_4045_AD80_3C57186F4DBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"
#include "IFace.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT ITriangle : public IFace {

public:
  ITriangle();

  enum eLineName { L1 = 1, L2 = 2, L3 = 0 };

  virtual int NrOfLines() const { return 3; }

  virtual int NrOfPoints() const;
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual IElement::TDoubleVec WorldToIso(const geo::IPoint &point) const;

  // a quick contains function for triangles
  virtual bool IsSelfIntersecting() const { return false; }
  virtual bool IsConvex() const { return true; }
  virtual CVector Normal() const;
  virtual bool IsPlanar() const { return true; }
  virtual IElement::TDoubleVec ShapeFunction(const IElement::TDoubleVec &isocoords) const;

  virtual CMatrix ShapeFunctionDerived(const IElement::TDoubleVec &isocoords) const;
  virtual std::vector<IElement::TDoubleVec> IsoCoordinates() const;

  // integration points
  virtual int IntegrationPointSize() const;
  virtual const TDoubleVec &IntegrationPointCoords(int nIndex) const;
  virtual const double &IntegrationPointWeight(int nIndex) const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleTriangle(*this); }

  static void PrepareMapping();

private:
  static void BuildIntegrationPoints(IElement::TIntPtVec &vec, int numint);
  static const TIntPtVec &IntegrationPoints(int order);
  const TIntPtVec &IntegrationPoints() const;

  void Swap(const double **d1, const double **d2) const;
  IElement::TDoubleVec WorldToIso1stOrder(const IPoint &point) const;
};

} // namespace geo

#endif // !defined(AFX_ITRIANGLE_H__DCE5B760_C0D5_4045_AD80_3C57186F4DBB__INCLUDED_)
