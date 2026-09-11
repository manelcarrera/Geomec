// CementMaterialTable.h: interface for the CCementMaterialTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTMATERIALTABLE_H__3A4BB248_F20D_4DF7_AF2E_066DEFE3E31E__INCLUDED_)
#define AFX_CEMENTMATERIALTABLE_H__3A4BB248_F20D_4DF7_AF2E_066DEFE3E31E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define CEMENT_MATERIAL_TABLE					"CementMaterial"
#define CEMENT_MATERIAL_NAME					"MaterialName"
#define CEMENT_MATERIAL_YOUNG					"YoungsModulus"
#define CEMENT_MATERIAL_POISSON					"PoissonRatio"
#define CEMENT_MATERIAL_VOLUMETRICHEAT			"VolumetricSpecificHeat"
#define CEMENT_MATERIAL_THERMALCOND				"ThermalConductivity"
#define CEMENT_MATERIAL_THERMALEXPANS			"ThermalExpansion"
#define CEMENT_MATERIAL_TENSILESTRENGTH			"TensileStrength"
#define CEMENT_MATERIAL_TENSFRACTUREENERGY		"TensFractureEnergy"
#define CEMENT_MATERIAL_COHESION				"Cohesion"
#define CEMENT_MATERIAL_FRICTION				"FrictionAngle"
#define CEMENT_MATERIAL_HARDENINGTYPE			"HardeningType"
#define CEMENT_MATERIAL_HARDENINGGRAD			"HardeningGradient"
#define CEMENT_MATERIAL_COMPFRACTUREENERGY		"CompFractureEnergy"
#define CEMENT_MATERIAL_VOLUMETRICSHRINKAGE		"VolumetricShrinkage"
#define CEMENT_MATERIAL_HYDRATIONHEAT			"HydrationHeat"
	

class CCementMaterialTable : public CDSTable  
{
public:
	CCementMaterialTable(CDaoDatabase& Database);
	virtual ~CCementMaterialTable();
	virtual void Open();
	virtual void Create();
	virtual CString TableName() {return CEMENT_MATERIAL_TABLE;}
};

#endif // !defined(AFX_CEMENTMATERIALTABLE_H__3A4BB248_F20D_4DF7_AF2E_066DEFE3E31E__INCLUDED_)
