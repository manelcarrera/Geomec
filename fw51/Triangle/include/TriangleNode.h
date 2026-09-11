 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriangleMeshNode.h: interface for the CTriangleMeshNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGLEMESHNODE_H__9DEAD1CF_367F_47A3_9D8B_95AED39E9427__INCLUDED_)
#define AFX_TRIANGLEMESHNODE_H__9DEAD1CF_367F_47A3_9D8B_95AED39E9427__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPoint.h"

class CTriangleSurface;
class CTriangleNode : public geo::IPoint  
{
  int m_nIndex;
  CTriangleSurface& m_surface;
  double *m_pCoord; // coordinate position in surface

public:
  CTriangleNode(CTriangleSurface& surface, int nIndex, double *coord);
  virtual ~CTriangleNode();

  // Coordinate access
  virtual const double &X() const;
  virtual const double &Y() const;
  virtual const double &Z() const;

  // Coordinate modification
  virtual void X(const double &dX);
  virtual void Y(const double &dY);
  virtual void Z(const double &dZ);	

  virtual void AssertValid() const;
  virtual bool Empty() const;
};

#endif // !defined(AFX_TRIANGLEMESHNODE_H__9DEAD1CF_367F_47A3_9D8B_95AED39E9427__INCLUDED_)
