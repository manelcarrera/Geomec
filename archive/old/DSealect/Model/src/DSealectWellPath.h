// DSealectWellPath.h: interface for the CDSealectWellPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSEALECTWELLPATH_H__B2EAFA86_DD4A_4753_9C2E_3004ABA94C98__INCLUDED_)
#define AFX_DSEALECTWELLPATH_H__B2EAFA86_DD4A_4753_9C2E_3004ABA94C98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Geometry.h"

class CDataModel;

class CDSealectWellPath  
{
private:
	CDataModel *m_pModel;
	geo::CPolyLine m_PolyLine;


public:
	double GetLength();
	BOOL CanCreatePolyLine();
	double TMDtoTVD(const double &length);
	BOOL CreatePolyLine();
	CDSealectWellPath( CDataModel *m_pModel );

	virtual ~CDSealectWellPath();

};

#endif // !defined(AFX_DSEALECTWELLPATH_H__B2EAFA86_DD4A_4753_9C2E_3004ABA94C98__INCLUDED_)
