// WellSymbolBase.h: interface for the CWellSymbolBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLSYMBOLBASE_H__6CEBE635_B80A_4213_8794_B8979821F438__INCLUDED_)
#define AFX_WELLSYMBOLBASE_H__6CEBE635_B80A_4213_8794_B8979821F438__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISymbol.h"

class CWellSymbolBase : public ISymbol  
{
public:
  CWellSymbolBase(const geo::CPoint& pos, const QString& description,bool autoDelete =true );
  virtual ~CWellSymbolBase();

  //ISymbol interface
  virtual int NrOfPositions() const;
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


protected:
  geo::CPoint m_PivotPoint;
  geo::CPoint m_Position;
  geo::CPtrArray<geo::IObject> m_Array;
  QString m_Description;
  bool m_autoDelete;
};

#endif // !defined(AFX_WELLSYMBOLBASE_H__6CEBE635_B80A_4213_8794_B8979821F438__INCLUDED_)
