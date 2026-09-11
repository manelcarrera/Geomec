// Box.h: interface for the CBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOX_H__B1A0C8A3_E9FD_4B6E_AB4C_D3C06934AB5F__INCLUDED_)
#define AFX_BOX_H__B1A0C8A3_E9FD_4B6E_AB4C_D3C06934AB5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IBox.h"
#include "Node.h"

#include "GeometryExports.h"

namespace geo {

class CPoint;
class CVector;
class IBox;
class CLine;

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The CBox bject represents an rectangular box in 3D space 
// Status      : Finished (100%)
// Remarks     : See IBox
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  CBox : public IBox
{
  CArray<CNode> m_Points;
public:
  CBox();
  CBox(const IPoint &Pos, const IVector &Dir1, const IVector &Dir2, const IVector &Dir3);
  CBox(const IPoint &MinPoint, const IPoint &MaxPoint);
  CBox(const CBox &rhs);
  CBox& operator=(const CBox& rhs);

  virtual ~CBox();
  CArray<CNode> GeneratePoints() const;
  CArray<CLine>  GenerateLines() const;

  void AssertValid() const;

  //IBox Interface
  virtual double Width() const;
  virtual double Depth() const;
  virtual double Height() const;

  //IElement Interface
  virtual const IPoint &Point( int nIndex ) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int NrOfPoints() const;

  virtual double Size() const;

  virtual CPoint Max() const;
  virtual CPoint Min() const;

  virtual bool Contains(const IPoint &point, bool bIncludeEdge) const;

  virtual size_t Order() const;
  virtual const char* FaceName(int /*nIndex*/) const { return 0; }

  virtual size_t NrOfNodes() const;
  virtual const INode& Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint& point);
};

}
#endif // !defined(AFX_BOX_H__B1A0C8A3_E9FD_4B6E_AB4C_D3C06934AB5F__INCLUDED_)
