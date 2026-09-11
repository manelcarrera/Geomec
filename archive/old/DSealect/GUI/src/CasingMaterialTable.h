// CasingMaterialTable.h: interface for the CCasingMaterialTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASINGMATERIALTABLE_H__96F4593D_312C_4F2C_B41C_ADB9B9494915__INCLUDED_)
#define AFX_CASINGMATERIALTABLE_H__96F4593D_312C_4F2C_B41C_ADB9B9494915__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define CASING_MATERIAL_TABLE					"CasingMaterial"
#define CASING_MATERIAL_NAME					"MaterialName"
#define CASING_MATERIAL_OUTERDIAMETER			"OuterDiameter"
#define CASING_MATERIAL_INNERDIAMETER			"InnerDiameter"
#define CASING_MATERIAL_YOUNG					"YoungsModulus"
#define CASING_MATERIAL_POISSON					"PoissonRatio"
#define CASING_MATERIAL_VOLUMETRICSPECIFICHEAT	"VolumetricSpecificHeat"
#define CASING_MATERIAL_THERMALCOND				"ThermalConductivity"
#define CASING_MATERIAL_THERMALEXPANS			"ThermalExpansion"
#define CASING_MATERIAL_WEIGHTPERUNITLENGTH		"WeightPerUnitLength"
#define CASING_MATERIAL_PLASTICITY				"Plasticity"
#define CASING_MATERIAL_YIELDSTRENGTH			"YieldStrength"



class CCasingMaterialTable : public CDSTable  
{
public:
	CCasingMaterialTable(CDaoDatabase& Database);
	virtual ~CCasingMaterialTable();

	virtual void Open();
	virtual void Create();
	virtual CString TableName(){return CASING_MATERIAL_TABLE;}
};

#endif // !defined(AFX_CASINGMATERIALTABLE_H__96F4593D_312C_4F2C_B41C_ADB9B9494915__INCLUDED_)
