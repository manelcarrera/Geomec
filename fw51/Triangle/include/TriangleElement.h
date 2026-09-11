 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriangleMeshElement.h: interface for the CTriangleMeshElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGLEMESHELEMENT_H__3818997C_6E6B_4734_86CE_6EA6D8E40491__INCLUDED_)
#define AFX_TRIANGLEMESHELEMENT_H__3818997C_6E6B_4734_86CE_6EA6D8E40491__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITriangle.h"

class CTriangleSurface;
class CTriangleElement : public geo::ITriangle 
{
  int m_nIndex;
  const CTriangleSurface& m_surface;
  int *m_pPointIndex; // point indices from the surface
public:
  CTriangleElement(const CTriangleSurface& surface, int nIndex, int *pointindex);
  virtual ~CTriangleElement();

  // Nodal interface
  virtual const geo::IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const geo::IPoint &pt);
  virtual int NrOfPoints() const;

  // Distributed interface
  virtual const geo::IElementSet* IndexingElementSet() const;
  virtual int Index() const;
  virtual int PointIndex(int nIndex) const;
  virtual std::string Type() const;
  
  virtual void AssertValid() const;
  virtual bool Empty() const;

  virtual size_t Order() const { return 1; }
};

#endif // !defined(AFX_TRIANGLEMESHELEMENT_H__3818997C_6E6B_4734_86CE_6EA6D8E40491__INCLUDED_)
