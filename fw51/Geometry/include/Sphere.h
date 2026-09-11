// Sphere.h: interface for the CSphere class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPHERE_H__15ABAC35_8DA2_411B_A1B3_65D1DEED3A52__INCLUDED_)
#define AFX_SPHERE_H__15ABAC35_8DA2_411B_A1B3_65D1DEED3A52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISphere.h"

#include "GeometryExports.h"

namespace geo {
// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The CSphere object represents a sphere 3D space 
// Status      : Finished (50%) 
// Remarks     : We need a CCircle as a result from the intersections with a plane
//	           :  	
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  CSphere  : public ISphere
{
  CPoint m_mid_point;
  double m_radius;
public:
  // Construction 
  CSphere();	
  CSphere(const IPoint& mid_point, const double Radius);
  CSphere(const ISphere& rhs);

  // Interface of ISphere support
  virtual const double& Radius() const;
  virtual void Radius(const double &radius);
  virtual const IPoint& MidPoint() const;
  virtual void MidPoint(const IPoint& mid_point);

};
}

#endif // !defined(AFX_SPHERE_H__15ABAC35_8DA2_411B_A1B3_65D1DEED3A52__INCLUDED_)
