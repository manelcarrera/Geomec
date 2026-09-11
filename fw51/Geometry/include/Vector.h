// Vector.h: interface for the CVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CVECTOR_H__E4785A60_5194_4D15_85CE_0B479207B842__INCLUDED_)
#define AFX_CVECTOR_H__E4785A60_5194_4D15_85CE_0B479207B842__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "IVector.h"

#include "GeometryExports.h"

namespace geo {

class ILine;
class IPoint;

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : Interface for a 3D geometrical vector
// Status      : Finished (100%)
// Remarks     : See IVector for interface
// Last review : 27-03-2002

class GEOMETRY_EXPORT  CVector : public IVector
{
public:
  CVector();
  CVector(const double &dX, const double &dY, const double &dZ = 0.0);
  CVector(const IVector &rhs);
  CVector(const CVector &rhs);
  CVector(const IPoint &p);
  CVector(const IPoint &first, const IPoint &second);
  CVector(const ILine &line);

  static CVector CreateFromAngles(const double& azimuth_deg, const double& inclination_deg);

  // Interface to IVector
  virtual const double &X() const;
  virtual const double &Y() const;
  virtual const double &Z() const;
  virtual void X(const double &dX);
  virtual void Y(const double &dY);
  virtual void Z(const double &dZ);


  //IObject interface
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual CPoint Min() const;
  virtual CPoint Max() const;

  static const CVector NullVector;
  static const CVector Xaxis;
  static const CVector Yaxis;
  static const CVector Zaxis;

  CVector& operator=(const CVector& rhs);

  virtual double operator[]( int i ) const;

private:
  double m_coor[3];
  bool   m_empty;
};

  double Determinant(const CVector &v1, const CVector &v2, const CVector &v3);
  CVector GetSolution(const CVector &v1, const CVector &v2, const CVector &v3, const CVector &v4);
}

#endif // !defined(AFX_CVECTOR_H__E4785A60_5194_4D15_85CE_0B479207B842__INCLUDED_)
