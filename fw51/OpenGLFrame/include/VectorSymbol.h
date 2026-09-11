 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// VectorSymbol.h: interface for the CVectorSymbol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTORSYMBOL_H__DFF021DF_AB3D_4FC4_ACC9_D20A38B91973__INCLUDED_)
#define AFX_VECTORSYMBOL_H__DFF021DF_AB3D_4FC4_ACC9_D20A38B91973__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISymbol.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CVectorSymbolBase : public ISymbol  
{

protected:
  geo::CPtrArray<geo::IObject> m_geometry;
  geo::CPoint m_location;
  geo::CPoint m_pivot;
  virtual void CreateGeometry(const geo::IVector &/*vector*/){}

public:
  CVectorSymbolBase(const geo::IPoint &location, const geo::IVector &dir, const double &length,bool TailIsPivot=true);
  virtual ~CVectorSymbolBase();

  //ISymbol interface
  virtual int NrOfPositions() const;
  virtual const geo::IPoint& Position(int Index) const;
  virtual const geo::IArray& Geometry() const;
  virtual const geo::IPoint& PivotPoint() const;

  virtual void Rotate(const geo::IVector &vec, const double &dAngleDeg);
  virtual void Move(const geo::IVector &vec);
  virtual void Transform(const geo::IMatrix &matrix);
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual geo::CPoint Min() const;
  virtual geo::CPoint Max() const;

};



class OPENGLFRAME_EXPORT CVectorSymbol : public CVectorSymbolBase  
{
  virtual void CreateGeometry(const geo::IVector &vector);
public:
  CVectorSymbol(const geo::IPoint &location, const geo::IVector &dir, const double &length,bool TailIsPivot=true);
};



class OPENGLFRAME_EXPORT CArrowSymbol : public CVectorSymbolBase  
{
  virtual void CreateGeometry(const geo::IVector &vector);
public:
  CArrowSymbol(const geo::IPoint &location, const geo::IVector &dir, const double &length,bool TailIsPivot=true);
};




#endif // !defined(AFX_VECTORSYMBOL_H__DFF021DF_AB3D_4FC4_ACC9_D20A38B91973__INCLUDED_)
