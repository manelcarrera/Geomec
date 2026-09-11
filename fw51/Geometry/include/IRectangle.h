// IRectangle.h: interface for the IRectangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IRECTANGLE_H__1DD56B9D_2A6C_4EC4_9A8E_59B013887FCC__INCLUDED_)
#define AFX_IRECTANGLE_H__1DD56B9D_2A6C_4EC4_9A8E_59B013887FCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IQuadrilateral.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {
// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description	: The IRectangle object represents a rectangle in 3D
// Status		: Finished (50%) 
// Remarks		: 
//				:  	
// Last review	: 27-03-2002 
class GEOMETRY_EXPORT  IRectangle : public IQuadrilateral
{
public:
  IRectangle();

  virtual const IPoint& Refpoint() const = 0;
  virtual const IVector& Direction1() const = 0;
  virtual const IVector& Direction2() const = 0;

  //returns -1 if p is not a point on the rectangle
  virtual int GetOpositeIndex(const geo::CPoint& p);

  //From IElement...
  virtual int NrOfPoints() const;
  virtual TDoubleVec WorldToIso(const geo::IPoint& point) const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleRectangle(*this); }

};
}

#endif // !defined(AFX_IRECTANGLE_H__1DD56B9D_2A6C_4EC4_9A8E_59B013887FCC__INCLUDED_)
