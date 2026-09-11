// FormationTable.h: interface for the CFormationTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATIONTABLE_H__349B4CE5_195D_438E_9F36_775A610333CC__INCLUDED_)
#define AFX_FORMATIONTABLE_H__349B4CE5_195D_438E_9F36_775A610333CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define FORMATION_TABLE					"Formation"
#define FORMATION_ID					"FormationID"
#define FORMATION_NAME					"FormationName"
#define FORMATION_MATERIAL_NAME     	"MaterialName"
#define FORMATION_BOTTOM_TVD			"BottomTVD"
#define FORMATION_GEOTHERMAL_GRAD		"GeothermalGradient"
#define FORMATION_VERT_STRESS_GRAD		"VerticalStressGradient"
#define FORMATION_MAX_HOR_STRESS_RATIO	"MaxHorizontalStressRatio"
#define FORMATION_MIN_HOR_STRESS_RATIO	"MinHorizontalStressRatio"
#define FORMATION_AZIMUTH_SH_WITH_NORTH	"AzimuthOfSHwithNorth"
#define FORMATION_POROUS				"Porous"
#define FORMATION_TOP_PORE_PRESSURE		"TopPorePressure"
#define FORMATION_BOTTOM_PORE_PRESSURE	"BottomPorePressure"




class CDSModelDatabase;


class CFormationTable : public CDSTable  
{
public:
	CFormationTable(CDaoDatabase& Database);
	virtual ~CFormationTable();
	virtual void Open();
	virtual void Create();
};

#endif // !defined(AFX_FORMATIONTABLE_H__349B4CE5_195D_438E_9F36_775A610333CC__INCLUDED_)
