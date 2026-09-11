// CementMaterialDatabase.cpp: implementation of the CCementMaterialDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CementMaterialDatabase.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

#define CEMENT_MAT_DATABASE_CURRENT_VERSION CVersionInfo(1,0)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCementMaterialDatabase::CCementMaterialDatabase()
{

	CVersionTable* tp=dynamic_cast<CVersionTable*>(m_pVersionTable.get());
	tp->VersionName("CementMaterial");
	m_VersionInfo=CEMENT_MAT_DATABASE_CURRENT_VERSION;
}

CCementMaterialDatabase::~CCementMaterialDatabase()
{

}

BOOL CCementMaterialDatabase::AddMaterial(CDSMaterial &mat)
{
	CCementMaterial* cem=dynamic_cast<CCementMaterial*>(&mat);

	if(!cem)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	MaterialTable().Open();
	MaterialTable().RS().AddNew();
	MaterialTable().SetField(CEMENT_MATERIAL_NAME,					 cem->Name());
	MaterialTable().SetField(CEMENT_MATERIAL_YOUNG,					*cem->YoungsModulus());
	MaterialTable().SetField(CEMENT_MATERIAL_POISSON,				*cem->PoissonRatio());
	MaterialTable().SetField(CEMENT_MATERIAL_VOLUMETRICHEAT,		*cem->VolumetricSpecificHeat());
	MaterialTable().SetField(CEMENT_MATERIAL_THERMALCOND,			*cem->ThermalConductivity());
	MaterialTable().SetField(CEMENT_MATERIAL_THERMALEXPANS,			*cem->ThermalExpansion());
	MaterialTable().SetField(CEMENT_MATERIAL_TENSILESTRENGTH,		*cem->TensileStrength());
	MaterialTable().SetField(CEMENT_MATERIAL_TENSFRACTUREENERGY,	*cem->TensFractureEnergy());
	MaterialTable().SetField(CEMENT_MATERIAL_COHESION,   			*cem->Cohesion());
	MaterialTable().SetField(CEMENT_MATERIAL_FRICTION,   			*cem->FrictionAngle());
	MaterialTable().SetField(CEMENT_MATERIAL_HARDENINGGRAD, 		*cem->CompHardeningGradient());
	MaterialTable().SetField(CEMENT_MATERIAL_COMPFRACTUREENERGY,		*cem->CompFractureEnergy());
	MaterialTable().SetField(CEMENT_MATERIAL_VOLUMETRICSHRINKAGE,	*cem->VolumetricShrinkage());
	MaterialTable().SetField(CEMENT_MATERIAL_HYDRATIONHEAT,			*cem->HydrationHeat());

	long lHardType;
	lHardType=(long)cem->HardeningType();
	MaterialTable().SetField(CEMENT_MATERIAL_HARDENINGTYPE,		lHardType);
	MaterialTable().RS().Update();
	MaterialTable().Close();

	return TRUE;
	
}

BOOL CCementMaterialDatabase::GetMaterial(CString name,CDSMaterial &mat)
{
	CCementMaterialTable cmt(Database());

	CString sqlStatement;

	sqlStatement = "SELECT * FROM ";
	sqlStatement += cmt.TableName();
	sqlStatement += " WHERE ";
	sqlStatement += MATERIAL_NAME;
	sqlStatement += " = '";
	sqlStatement += name;
	sqlStatement += "'";

	CDaoQueryDef qd(&Database());
	qd.Create(NULL, sqlStatement);

	cmt.Close();
	cmt.RS().Open(&qd,dbOpenDynaset,dbReadOnly);
	if(cmt.RS().GetRecordCount() !=1)
	{
		qd.Close();
		cmt.RS().Close();
		cmt.Close();
		return FALSE; 
	}

	CCementMaterial cem;
	cem.Name(name);

	cmt.GetField(CEMENT_MATERIAL_YOUNG,					*cem.YoungsModulus());
	cmt.GetField(CEMENT_MATERIAL_POISSON,				*cem.PoissonRatio());
	cmt.GetField(CEMENT_MATERIAL_VOLUMETRICHEAT,		*cem.VolumetricSpecificHeat());
	cmt.GetField(CEMENT_MATERIAL_THERMALCOND,			*cem.ThermalConductivity());
	cmt.GetField(CEMENT_MATERIAL_THERMALEXPANS,			*cem.ThermalExpansion());
	cmt.GetField(CEMENT_MATERIAL_TENSILESTRENGTH,		*cem.TensileStrength());
	cmt.GetField(CEMENT_MATERIAL_TENSFRACTUREENERGY,	*cem.TensFractureEnergy());
	cmt.GetField(CEMENT_MATERIAL_COHESION,   			*cem.Cohesion());
	cmt.GetField(CEMENT_MATERIAL_FRICTION,   			*cem.FrictionAngle());
	cmt.GetField(CEMENT_MATERIAL_HARDENINGGRAD, 		*cem.CompHardeningGradient());
	cmt.GetField(CEMENT_MATERIAL_COMPFRACTUREENERGY,	*cem.CompFractureEnergy());
	cmt.GetField(CEMENT_MATERIAL_VOLUMETRICSHRINKAGE,	*cem.VolumetricShrinkage());
	cmt.GetField(CEMENT_MATERIAL_HYDRATIONHEAT,			*cem.HydrationHeat());

	long lHardeningType;
	cmt.GetField(CEMENT_MATERIAL_HARDENINGTYPE,lHardeningType);
	cem.HardeningType(eHardeningType(lHardeningType));
	
	qd.Close();
	cmt.RS().Close();
	cmt.Close();

	//CCementMaterial& type = dynamic_cast<CFormationMaterial&>(mat);
	//type = cem;
	dynamic_cast<CCementMaterial&>(mat) = cem;

	return TRUE;
	
}
