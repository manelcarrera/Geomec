// WellPoint.h: interface for the CWellPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLPOINT_H__3636A584_627C_4614_9741_7E3EB9960421__INCLUDED_)
#define AFX_WELLPOINT_H__3636A584_627C_4614_9741_7E3EB9960421__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "point.h"
#include <vector>

namespace gm {

class C3DWellPoint : public geo::CPoint
{
	double m_dAzimuth;
	double m_dInclination;
	double m_dAHD; // along hole depth
	std::vector<double> m_vcDirect; // directional vector in the point
	bool m_bDirectCalc; // if the directional vector has been calculated or not
public:
	C3DWellPoint();
	C3DWellPoint(const double &dAHD, const double &dInclination, const double &dAzimuth, const double &dX, const double &dY, const double &dZ);
	C3DWellPoint(const C3DWellPoint &WellPoint);
	C3DWellPoint(const geo::IPoint& point);
	virtual ~C3DWellPoint();

	void Azimuth(const double &dValue); 
	double Azimuth() const {return m_dAzimuth;}

	void Inclination(const double &dValue); 
	double Inclination() const {return m_dInclination;}

	void AlongHoleDepth(const double &dValue) {m_dAHD = dValue;}
	double AlongHoleDepth() const {return m_dAHD;}

	void CalcDirectionalVec();
	bool DirectVectCalculated() const {return m_bDirectCalc;}
	const std::vector<double> DirectionVect();
	bool operator <(const C3DWellPoint &rhs) const;
};

}

#endif // !defined(AFX_WELLPOINT_H__3636A584_627C_4614_9741_7E3EB9960421__INCLUDED_)
