// CasingMaterialDatabase.cpp: implementation of the CCasingMaterialDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CasingMaterialDatabase.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

#define CASING_MAT_DATABASE_CURRENT_VERSION CVersionInfo(1,0)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCasingMaterialDatabase::CCasingMaterialDatabase()
{
	CVersionTable* tp=dynamic_cast<CVersionTable*>(m_pVersionTable.get());
	tp->VersionName("CasingMaterial");
	m_VersionInfo=CASING_MAT_DATABASE_CURRENT_VERSION;
}

CCasingMaterialDatabase::~CCasingMaterialDatabase()
{

}

BOOL CCasingMaterialDatabase::AddMaterial(CDSMaterial &mat)
{
	CCasingMaterial* cam=dynamic_cast<CCasingMaterial*>(&mat);

	if(!cam)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	MaterialTable().Open();
	MaterialTable().RS().AddNew();

	MaterialTable().SetField(CASING_MATERIAL_NAME,						 cam->Name());
	MaterialTable().SetField(CASING_MATERIAL_OUTERDIAMETER,				*cam->CasingOuterDiameter());
	MaterialTable().SetField(CASING_MATERIAL_INNERDIAMETER,				*cam->CasingInnerDiameter());
	MaterialTable().SetField(CASING_MATERIAL_YOUNG,						*cam->YoungsModulus());
	MaterialTable().SetField(CASING_MATERIAL_POISSON,					*cam->PoissonRatio());
	MaterialTable().SetField(CASING_MATERIAL_VOLUMETRICSPECIFICHEAT,	*cam->VolumetricSpecificHeat());
	MaterialTable().SetField(CASING_MATERIAL_THERMALCOND,   			*cam->ThermalConductivity());
	MaterialTable().SetField(CASING_MATERIAL_THERMALEXPANS,   			*cam->ThermalExpansion());
	MaterialTable().SetField(CASING_MATERIAL_WEIGHTPERUNITLENGTH, 		*cam->WeightPerUnitLength());
	MaterialTable().SetField(CASING_MATERIAL_PLASTICITY,				cam->Plasticity());
	MaterialTable().SetField(CASING_MATERIAL_YIELDSTRENGTH,				*cam->YieldStrength());

	MaterialTable().RS().Update();
	MaterialTable().Close();

	return TRUE;
}


BOOL CCasingMaterialDatabase::GetMaterial(CString name,CDSMaterial& mat)
{
	CCasingMaterialTable cmt(Database());

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

	CCasingMaterial cam;
	cam.Name(name);
	
	cmt.GetField(CASING_MATERIAL_OUTERDIAMETER,				*cam.CasingOuterDiameter());
	cmt.GetField(CASING_MATERIAL_INNERDIAMETER,				*cam.CasingInnerDiameter());
	cmt.GetField(CASING_MATERIAL_YOUNG,						*cam.YoungsModulus());
	cmt.GetField(CASING_MATERIAL_POISSON,					*cam.PoissonRatio());
	cmt.GetField(CASING_MATERIAL_VOLUMETRICSPECIFICHEAT,	*cam.VolumetricSpecificHeat());
	cmt.GetField(CASING_MATERIAL_THERMALCOND,   			*cam.ThermalConductivity());
	cmt.GetField(CASING_MATERIAL_THERMALEXPANS,   			*cam.ThermalExpansion());
	cmt.GetField(CASING_MATERIAL_WEIGHTPERUNITLENGTH, 		*cam.WeightPerUnitLength());

	BOOL bPlasticiy;
	cmt.GetField(CASING_MATERIAL_PLASTICITY,				bPlasticiy);
	cam.Plasticity(bPlasticiy==TRUE);

	cmt.GetField(CASING_MATERIAL_YIELDSTRENGTH,				*cam.YieldStrength());
	qd.Close();
	cmt.RS().Close();
	cmt.Close();

	//CCasingMaterial& type = dynamic_cast<CCasingMaterial&>(mat);
	//type = cam;
	dynamic_cast<CCasingMaterial&>(mat) = cam;

	return TRUE;
}