// Point.h: interface for the CPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT_H__A767A583_D0B5_41A1_89BC_2DFB1AE27A26__INCLUDED_)
#define AFX_POINT_H__A767A583_D0B5_41A1_89BC_2DFB1AE27A26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPoint.h"

#include "GeometryExports.h"

namespace geo {

class IPoint;

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : Default implementation of the IPoint interface
// Status      : Finished (100%)
// Remarks     : See IPoint for definition
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  CPoint : public IPoint
{
public:
  virtual bool Empty() const; 
  virtual void AssertValid() const;

  // Constructors 
  CPoint();
  CPoint(const IVector &vec);
  CPoint(const IPoint &pt);
  CPoint(const CPoint &pt);
  CPoint(const double &dX,
         const double &dY,
         const double &dZ = 0.0);

  CPoint& operator=(const CPoint& rhs);

  // IPoint interface ...
  virtual const double &X() const;
  virtual const double &Y() const;
  virtual const double &Z() const;
  virtual void X(const double &dX);
  virtual void Y(const double &dY);
  virtual void Z(const double &dZ);

  virtual void Set(const double &dX, const double &dY, const double &dZ);

  static const CPoint NullPoint;
  static bool CoLinear(const CPoint& p1,const CPoint& p2,const CPoint& p3);
private:
  double m_coor[3];
  bool   m_empty;
};
	
}

#endif // !defined(AFX_POINT_H__A767A583_D0B5_41A1_89BC_2DFB1AE27A26__INCLUDED_)
