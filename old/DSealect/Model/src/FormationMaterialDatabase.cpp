// FormationMaterialDatabase.cpp: implementation of the CFormationMaterialDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FormationMaterialDatabase.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

#define FORM_MAT_DATABASE_CURRENT_VERSION CVersionInfo(1,0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C60D576035C
CFormationMaterialDatabase::CFormationMaterialDatabase()
{
	CVersionTable* tp=dynamic_cast<CVersionTable*>(m_pVersionTable.get());
	tp->VersionName("FormationMaterial");
	m_VersionInfo=FORM_MAT_DATABASE_CURRENT_VERSION;
}

//##ModelId=3C60D576035D
CFormationMaterialDatabase::~CFormationMaterialDatabase()
{

}

//##ModelId=3C60D576035F
BOOL CFormationMaterialDatabase::AddMaterial(CDSMaterial &mat)
{
	CFormationMaterial* fm=dynamic_cast<CFormationMaterial*>(&mat);

	if(!fm)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	MaterialTable().Open();
	MaterialTable().RS().AddNew();
	MaterialTable().SetField(FORMATION_MATERIAL_NAME,			     fm->Name());
	MaterialTable().SetField(FORMATION_MATERIAL_YOUNG,				*fm->YoungsModulus());
	MaterialTable().SetField(FORMATION_MATERIAL_POISSON,			*fm->PoissonRatio());
	MaterialTable().SetField(FORMATION_MATERIAL_VOLUMETRICHEAT,		*fm->VolumetricSpecificHeat());
	MaterialTable().SetField(FORMATION_MATERIAL_THERMALCOND,		*fm->ThermalConductivity());
	MaterialTable().SetField(FORMATION_MATERIAL_THERMALEXPANS,		*fm->ThermalExpansion());
	MaterialTable().SetField(FORMATION_MATERIAL_COHESION,   		*fm->Cohesion());
	MaterialTable().SetField(FORMATION_MATERIAL_FRICTION,   		*fm->FrictionAngle());
	MaterialTable().SetField(FORMATION_MATERIAL_HARDENINGGRAD, 		*fm->HardeningGradient());
	MaterialTable().SetField(FORMATION_MATERIAL_FRACTUREENERGY,		*fm->FractureEnergy());
  MaterialTable().SetField(FORMATION_MATERIAL_DENSITY, *fm->Density());

	long lHardType;
	lHardType=(long)fm->HardeningType();
	MaterialTable().SetField(FORMATION_MATERIAL_HARDENINGTYPE,		lHardType);

	MaterialTable().SetField(FORMATION_MATERIAL_PLASTICITY,	fm->Plasticity());


	MaterialTable().RS().Update();
	MaterialTable().Close();

	return TRUE;
}


//##ModelId=3C60D5760362
BOOL CFormationMaterialDatabase::GetMaterial(CString name,CDSMaterial& mat)
{
	
	CFormationMaterialTable fmt(Database());

	CString sqlStatement;

	sqlStatement = "SELECT * FROM ";
	sqlStatement += fmt.TableName();
	sqlStatement += " WHERE ";
	sqlStatement += MATERIAL_NAME;
	sqlStatement += " = '";
	sqlStatement += name;
	sqlStatement += "'";

	CDaoQueryDef qd(&Database());
	qd.Create(NULL, sqlStatement);

	fmt.Close();
	fmt.RS().Open(&qd,dbOpenDynaset,dbReadOnly);
	if(fmt.RS().GetRecordCount() !=1)
	{
		qd.Close();
		fmt.RS().Close();
		fmt.Close();
		return FALSE; 
	}

	CFormationMaterial fm;
	fm.Name(name);

	fmt.GetField(FORMATION_MATERIAL_YOUNG,			*fm.YoungsModulus());
	fmt.GetField(FORMATION_MATERIAL_POISSON,		*fm.PoissonRatio());
	fmt.GetField(FORMATION_MATERIAL_VOLUMETRICHEAT,	*fm.VolumetricSpecificHeat());
	fmt.GetField(FORMATION_MATERIAL_THERMALCOND,	*fm.ThermalConductivity());
	fmt.GetField(FORMATION_MATERIAL_THERMALEXPANS,	*fm.ThermalExpansion());
	fmt.GetField(FORMATION_MATERIAL_COHESION,		*fm.Cohesion());
	fmt.GetField(FORMATION_MATERIAL_FRICTION,		*fm.FrictionAngle());
	fmt.GetField(FORMATION_MATERIAL_HARDENINGGRAD,	*fm.HardeningGradient());
	fmt.GetField(FORMATION_MATERIAL_FRACTUREENERGY,	*fm.FractureEnergy());

  try
  {
    fmt.GetField(FORMATION_MATERIAL_DENSITY, *fm.Density());
  }
  catch(CDaoException* e)
  {
    e->Delete();
  }



	long lHardeningType;
	fmt.GetField(FORMATION_MATERIAL_HARDENINGTYPE,lHardeningType);
	fm.HardeningType(eHardeningType(lHardeningType));

	BOOL bPlasticity;
	fmt.GetField(FORMATION_MATERIAL_PLASTICITY,	bPlasticity);
	fm.Plasticity(bPlasticity);

	qd.Close();
	fmt.RS().Close();
	fmt.Close();

	//CFormationMaterial& type = dynamic_cast<CFormationMaterial&>(mat);
	//type = fm;
	dynamic_cast<CFormationMaterial&>(mat) = fm;

	return TRUE;
}
