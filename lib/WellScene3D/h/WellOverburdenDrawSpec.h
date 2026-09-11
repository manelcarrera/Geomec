// WellOverburdenDrawSpec.h: interface for the CWellOverburdenDrawSpec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLOVERBURDENDRAWSPEC_H__63CAF831_A13E_4260_A32F_EFAE1B3EBCBE__INCLUDED_)
#define AFX_WELLOVERBURDENDRAWSPEC_H__63CAF831_A13E_4260_A32F_EFAE1B3EBCBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellDrawSpecBase.h"

class CWellOverburdenDrawSpec;
class CWellSceneInterMed;

class COverburdenSymbol: public ISymbol
{
  friend class CWellOverburdenDrawSpec;
public:

  //ISymbol interface
  virtual const int NrOfPositions() const;
  virtual const geo::IPoint& Position(int Index) const;
  virtual const geo::IArray& Geometry() const;
  virtual const geo::IPoint& PivotPoint() const;

  //IObject interface
  virtual void Rotate(const geo::IVector &vec, const double &dAngleDeg);
  virtual void Move(const geo::IVector &vec);
  virtual void Transform(const geo::IMatrix &matrix);
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual geo::CPoint Min() const;
  virtual geo::CPoint Max() const;

  virtual ~COverburdenSymbol();
private:
  COverburdenSymbol(CWellOverburdenDrawSpec&);

  geo::CPoint m_PivotPoint;
  geo::CPoint m_Position;
  geo::CPtrArray<geo::IObject> m_Array;
};






class CWellOverburdenDrawSpec : public CWellDrawSpecBase  
{
public:
  friend class CWellSceneInterMed;

  virtual ~CWellOverburdenDrawSpec();


private:
  virtual void CreateScene();
  //only CWellSceneInterMed can construct a CWellOverburdenDrawSpec
  CWellOverburdenDrawSpec(CWellSceneInterMed& WellSceneInterMed);
  
};

#endif // !defined(AFX_WELLOVERBURDENDRAWSPEC_H__63CAF831_A13E_4260_A32F_EFAE1B3EBCBE__INCLUDED_)
