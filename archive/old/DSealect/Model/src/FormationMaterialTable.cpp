// FormationMaterialTable.cpp: implementation of the CFormationMaterialTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "FormationMaterialTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C60D57502FF
CFormationMaterialTable::CFormationMaterialTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

//##ModelId=3C60D5750301
CFormationMaterialTable::~CFormationMaterialTable()
{

}


//##ModelId=3C60D575030E
void CFormationMaterialTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, FORMATION_MATERIAL_TABLE);
	}
}

//##ModelId=3C60D5750310
void CFormationMaterialTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(FORMATION_MATERIAL_TABLE);
	TableDef.CreateField(FORMATION_MATERIAL_NAME, dbText,255);
	TableDef.CreateField(FORMATION_MATERIAL_YOUNG,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_POISSON,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_VOLUMETRICHEAT,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_THERMALCOND,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_THERMALEXPANS,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_COHESION,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_FRICTION,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_HARDENINGTYPE,dbLong,0);
	TableDef.CreateField(FORMATION_MATERIAL_HARDENINGGRAD,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_FRACTUREENERGY,dbDouble,0);
	TableDef.CreateField(FORMATION_MATERIAL_PLASTICITY,dbBoolean,0);
  TableDef.CreateField(FORMATION_MATERIAL_DENSITY, dbDouble, 0);
	TableDef.Append();
	TableDef.Close();
}

