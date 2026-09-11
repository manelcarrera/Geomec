// CementMaterialTable.cpp: implementation of the CCementMaterialTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CementMaterialTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCementMaterialTable::CCementMaterialTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CCementMaterialTable::~CCementMaterialTable()
{

}


void CCementMaterialTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, CEMENT_MATERIAL_TABLE);
	}
}

void CCementMaterialTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(CEMENT_MATERIAL_TABLE);
	TableDef.CreateField(CEMENT_MATERIAL_NAME, dbText,0);
	TableDef.CreateField(CEMENT_MATERIAL_YOUNG,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_POISSON,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_VOLUMETRICHEAT,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_THERMALCOND,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_THERMALEXPANS,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_TENSILESTRENGTH,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_TENSFRACTUREENERGY,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_COHESION,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_FRICTION,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_HARDENINGTYPE,dbLong,0);
	TableDef.CreateField(CEMENT_MATERIAL_HARDENINGGRAD,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_COMPFRACTUREENERGY,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_VOLUMETRICSHRINKAGE,dbDouble,0);
	TableDef.CreateField(CEMENT_MATERIAL_HYDRATIONHEAT,dbDouble,0);
	TableDef.Append();
	TableDef.Close();

}
