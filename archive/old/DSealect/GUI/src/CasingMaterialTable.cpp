// CasingMaterialTable.cpp: implementation of the CCasingMaterialTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CasingMaterialTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCasingMaterialTable::CCasingMaterialTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CCasingMaterialTable::~CCasingMaterialTable()
{

}


void CCasingMaterialTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, CASING_MATERIAL_TABLE);
	}
}

void CCasingMaterialTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(CASING_MATERIAL_TABLE);
	TableDef.CreateField(CASING_MATERIAL_NAME, dbText,0);
	TableDef.CreateField(CASING_MATERIAL_OUTERDIAMETER,dbDouble,0);
	TableDef.CreateField(CASING_MATERIAL_INNERDIAMETER,dbDouble,0);
	TableDef.CreateField(CASING_MATERIAL_YOUNG,dbDouble,0);
	TableDef.CreateField(CASING_MATERIAL_POISSON,dbDouble,0);
	TableDef.CreateField(CASING_MATERIAL_VOLUMETRICSPECIFICHEAT,dbDouble,0);
	TableDef.CreateField(CASING_MATERIAL_THERMALCOND,dbDouble,0);
	TableDef.CreateField(CASING_MATERIAL_THERMALEXPANS,dbDouble,0);
	TableDef.CreateField(CASING_MATERIAL_WEIGHTPERUNITLENGTH,dbDouble,0);
	TableDef.CreateField(CASING_MATERIAL_PLASTICITY,dbBoolean,0);
	TableDef.CreateField(CASING_MATERIAL_YIELDSTRENGTH,dbDouble,0);
	TableDef.Append();
	TableDef.Close();

}

