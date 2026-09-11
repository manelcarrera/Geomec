// gm3WellPath.h: interface for the CWellPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GM3WELLPATH_H__2F744A64_EB60_44CC_AB97_064FA04EB3DF__INCLUDED_)
#define AFX_GM3WELLPATH_H__2F744A64_EB60_44CC_AB97_064FA04EB3DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

#include "IProgressBase.h"
#include "polyline.h"
#include "gm3wellpoint.h"

namespace gm 
{
class C3DDatabase;

class C3DWellPath 
{
	double m_dElevation;
	QString m_strName;
	geo::CPoint m_RefPoint;
	bool m_bIsVertical;
	geo::CPtrArray<C3DWellPoint> m_PointArr;
	geo::CPolyLine m_PolyLine;

public:
	C3DWellPath();
	virtual ~C3DWellPath();

	void ReadWellPath(C3DDatabase &db, const long lWellPathKey, IProgressBase &callback);
	virtual bool PushBack(const geo::IPoint &point);
	double DerrickElevation() const {return m_dElevation;}
	double Northing() const {return m_RefPoint.X();}
	double Easting() const {return m_RefPoint.Y();}
	bool IsVertical() const {return m_bIsVertical;}
	
	const geo::CPtrArray<C3DWellPoint>& Points() const {return m_PointArr;}
	const geo::CPolyLine& PolyLine() const {return m_PolyLine;}

	const QString& Name() const {return m_strName;}

};

}

#endif // !defined(AFX_GM3WELLPATH_H__2F744A64_EB60_44CC_AB97_064FA04EB3DF__INCLUDED_)
