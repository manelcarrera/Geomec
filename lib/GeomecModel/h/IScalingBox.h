// IScalingBox.h: interface for the IScalingBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISCALINGBOX_H__95834643_1717_4EAA_9E2D_465FEE84AB0E__INCLUDED_)
#define AFX_ISCALINGBOX_H__95834643_1717_4EAA_9E2D_465FEE84AB0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Box.h"
#include "colornode.h"
#include "GeomecModelVisitor.h"

//  This is a simple interface class for a box, that is a cube
class IScalingBox : public CColorNode
{
	geo::CBox m_box;
	double m_dSideX;
	double m_dSideY;
	double m_dSideZ;
	double m_dAzimuth;
	geo::CPoint m_ptCenter;
//	geo::CPoint m_Min;
//	geo::CPoint m_Max;
protected:
	void UpdateBox();

public:
	IScalingBox(CFemAppModel &model);
	IScalingBox(CFemAppModel &model, const QString &strName);
	IScalingBox(CFemAppModel &model, const geo::IPoint &center, const double side);
	IScalingBox(CFemAppModel& model, const geo::IPoint& center, const double& sideX,
				const double& sideY, const double& sideZ, const double& azimuth);
	IScalingBox(const IScalingBox &rhs);
	virtual ~IScalingBox();

	void Side(const double &dSide, bool bUpdate = false);
	void SideX(const double &dSide, bool bUpdate = false);
	void SideY(const double &dSide, bool bUpdate = false);
	void SideZ(const double &dSide, bool bUpdate = false);
	const double& SideX() const;
	const double& SideY() const;
	const double& SideZ() const;
	void Azimuth(const double& azimuth, bool bUpdate = false);
	const double& Azimuth() const;
	void Center(const geo::IPoint &center, bool bUpdate = false);
	const geo::IPoint &Center() const;

	virtual int DisplayListSize() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;

	virtual IScalingBox& operator=(const IScalingBox &rhs);

	const geo::CBox& Box() const { return m_box; }

  ACCEPT_GEOMECMODELVISITORS(VisitIScalingBox);

//	virtual geo::CPoint Min() const { return m_Min; }
//	virtual geo::CPoint Max() const { return m_Max; } 
};




#endif // !defined(AFX_ISCALINGBOX_H__95834643_1717_4EAA_9E2D_465FEE84AB0E__INCLUDED_)
