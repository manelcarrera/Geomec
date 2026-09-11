// ControlTable.cpp: implementation of the CControlTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSModelDatabase.h"
#include "ControlTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CControlTable::CControlTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CControlTable::~CControlTable()
{

}

void CControlTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, CONTROL_TABLE);
	}
}

void CControlTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(CONTROL_TABLE);
	TableDef.CreateField(CONTROL_PROJECT_NAME,dbText,20);
	TableDef.CreateField(CONTROL_WELL_TYPE,dbLong,0);//enum
	TableDef.CreateField(CONTROL_BALANCED_COMPLETION,dbBoolean,0);
	TableDef.CreateField(CONTROL_UNDERBALANCED_COMPLETION,dbBoolean,0);
	TableDef.CreateField(CONTROL_OVERBALANCED_COMPLETION,dbBoolean,0);
	TableDef.CreateField(CONTROL_CONVERSION,dbBoolean,0);
	TableDef.CreateField(CONTROL_FRACTURING,dbBoolean,0);
	TableDef.CreateField(CONTROL_EVACUATION,dbBoolean,0);
	TableDef.CreateField(CONTROL_DEFAULT_MESH_REFINEMENT,dbBoolean,0);
	TableDef.CreateField(CONTROL_ROCK_REFINEMENT,dbLong,0);
	TableDef.CreateField(CONTROL_CASING_REFINEMENT,dbLong,0);
	TableDef.CreateField(CONTROL_CEMENT_REFINEMENT,dbLong,0);
	TableDef.CreateField(CONTROL_OUTPUT_LEVEL,dbLong,0);
	TableDef.CreateField(CONTROL_TEST_PRESSURE,dbDouble,0);
  TableDef.CreateField(CONTROL_FLUID_PRESSURE_GRADIENT,dbDouble,0);
	TableDef.CreateField(CONTROL_SHUT_IN,dbBoolean,0);
	TableDef.Append();
	TableDef.Close();
}
