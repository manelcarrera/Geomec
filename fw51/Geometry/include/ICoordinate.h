// Coordinate.h: interface for the CCoordinate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COORDINATE_H__F5BF114A_4268_4A0C_AFA6_9AE5AC3F94B8__INCLUDED_)
#define AFX_COORDINATE_H__F5BF114A_4268_4A0C_AFA6_9AE5AC3F94B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IObject.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {
//##ModelId=3BC2A45E02A2

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The ICoordinate interface represent a cartesian coordinate sysytem.
// Status      : Finished (100%)
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  ICoordinate : public IObject
{
protected:
	//##ModelId=3BC2A45E02A4
	ICoordinate(){};
public:
	class GEOMETRY_EXPORT CCoordinateLess
	{
		double m_dEps;
	public:
		CCoordinateLess();
		CCoordinateLess(const double& dEps);
		bool operator()(const geo::ICoordinate *p1, const geo::ICoordinate *p2) const;
	};

	class GEOMETRY_EXPORT CCoordinateXYLess
	{
		double m_dEps;
	public:
		CCoordinateXYLess();
		CCoordinateXYLess(const double& dEps);
		bool operator()(const geo::ICoordinate *p1, const geo::ICoordinate *p2) const;
	};

	//##ModelId=3BC2A45E02B2
	virtual const double &X() const = 0;
	//##ModelId=3BC2A45E02B4
	virtual const double &Y() const = 0;
	//##ModelId=3BC2A45E02B6
	virtual const double &Z() const = 0;

	//##ModelId=3BC2A45E02B8
	virtual void X(const double &dX) = 0;
	//##ModelId=3BC2A45E02BB
	virtual void Y(const double &dY) = 0;
	//##ModelId=3BC2A45E02C3
	virtual void Z(const double &dZ) = 0;

	virtual void Set(const double &dX, const double &dY, const double &dZ);

  // Aliases to accommodate divergence in our and Diana's code (CM2-related)
  double getX() const { return X(); }
  double getY() const { return Y(); }
  double getZ() const { return Z(); }

  bool CloseTo(const ICoordinate& coord, double eps) const;

  //##ModelId=3BC2A45E02C6


	//##ModelId=3BC2A45E02C9
	void Rotate(const IVector &vec, const double &dSinAngle, const double &dCosAngle);
	virtual void Rotate(const IVector &vec, const double &dAngleDeg);
	//##ModelId=3BC2A45E02D1
	virtual void Move(const IVector &vec);
	//##ModelId=3BC2A45E02D4
	virtual void Transform(const IMatrix &matrix);
	//##ModelId=3BC2A45E02D7
	virtual CPoint Min() const;
	//##ModelId=3BC2A45E02D9
	virtual CPoint Max() const;

	ICoordinate& operator=(const ICoordinate& rhs);

	bool operator==(const ICoordinate &rhs) const;
	bool operator!=(const ICoordinate &rhs) const;

	//a lexicographic ordening for coordinates
	bool operator <(const ICoordinate &rhs) const;
	bool operator >(const ICoordinate &rhs) const;

	//handy manipulations on coords
	void Multiply(const ICoordinate &coor);
	void Divide(const ICoordinate &coor);

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleCoordinate(*this); }
};

}

#endif // !defined(AFX_COORDINATE_H__F5BF114A_4268_4A0C_AFA6_9AE5AC3F94B8__INCLUDED_)
