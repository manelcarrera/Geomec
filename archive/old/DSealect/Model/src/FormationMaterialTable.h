// FormationMaterialTable.h: interface for the CFormationMaterialTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATIONMATERIALTABLE_H__C0D99A61_0ECC_4171_BC6C_1B6B14A8704F__INCLUDED_)
#define AFX_FORMATIONMATERIALTABLE_H__C0D99A61_0ECC_4171_BC6C_1B6B14A8704F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define FORMATION_MATERIAL_TABLE			"FormationMaterial"
#define FORMATION_NAME						"FormationName"	
#define FORMATION_MATERIAL_NAME				"MaterialName"
#define FORMATION_MATERIAL_YOUNG			"YoungsModulus"
#define FORMATION_MATERIAL_POISSON			"PoissonRatio"
#define FORMATION_MATERIAL_VOLUMETRICHEAT	"VolumetricSpecificHeat"
#define FORMATION_MATERIAL_THERMALCOND		"ThermalConductivity"
#define FORMATION_MATERIAL_THERMALEXPANS	"ThermalExpansion"
#define FORMATION_MATERIAL_COHESION			"Cohesion"
#define FORMATION_MATERIAL_FRICTION			"FrictionAngle"
#define FORMATION_MATERIAL_HARDENINGTYPE	"HardeningType"
#define FORMATION_MATERIAL_HARDENINGGRAD	"HardeningGradient"
#define FORMATION_MATERIAL_FRACTUREENERGY	"FractureEnergy"
#define FORMATION_MATERIAL_PLASTICITY   	"Plasticity"
#define FORMATION_MATERIAL_DENSITY        "Density"

	

//##ModelId=3C60D57502EE
class CFormationMaterialTable : public CDSTable  
{
public:
	//##ModelId=3C60D57502FF
	CFormationMaterialTable(CDaoDatabase& Database);
	//##ModelId=3C60D5750301
	virtual ~CFormationMaterialTable();

	//##ModelId=3C60D575030E
	virtual void Open();
	//##ModelId=3C60D5750310
	virtual void Create();
	//##ModelId=3C60D5750312
	virtual CString TableName(){return FORMATION_MATERIAL_TABLE;}
};

#endif // !defined(AFX_FORMATIONMATERIALTABLE_H__C0D99A61_0ECC_4171_BC6C_1B6B14A8704F__INCLUDED_)
