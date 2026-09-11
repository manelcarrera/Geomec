/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// SphereSymbol.h: interface for the CSphereSymbol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPHERESYMBOL_H__8144C18B_D757_4CEF_B0B8_04F43BB740A4__INCLUDED_)
#define AFX_SPHERESYMBOL_H__8144C18B_D757_4CEF_B0B8_04F43BB740A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISymbol.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CSphereSymbol : public ISymbol {
  geo::CPoint m_location;
  geo::CPtrArray<geo::IObject> m_geometry;

public:
  CSphereSymbol(const geo::IPoint &location, const double &radius);
  virtual ~CSphereSymbol();

  // ISymbol interface
  virtual int NrOfPositions() const;
  virtual const geo::IPoint &Position(int Index) const;
  virtual const geo::IArray &Geometry() const;
  virtual const geo::IPoint &PivotPoint() const;

  // IObject interface
  virtual void Rotate(const geo::IVector & /*vec*/, const double & /*dAngleDeg*/) { assert(false); }
  virtual void Move(const geo::IVector & /*vec*/) { assert(false); }
  virtual void Transform(const geo::IMatrix & /*matrix*/) { assert(false); }
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual geo::CPoint Min() const;
  virtual geo::CPoint Max() const;
};

#endif // !defined(AFX_SPHERESYMBOL_H__8144C18B_D757_4CEF_B0B8_04F43BB740A4__INCLUDED_)
