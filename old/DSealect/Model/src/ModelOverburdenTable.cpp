// ModelOverburdenTable.cpp: implementation of the CModelOverburdenTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSModelDatabase.h"
#include "ModelOverburdenTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CModelOverburdenTable::CModelOverburdenTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CModelOverburdenTable::~CModelOverburdenTable()
{

}

void CModelOverburdenTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, MODEL_OVERBURDEN_TABLE);
	}
}

void CModelOverburdenTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(MODEL_OVERBURDEN_TABLE);
	TableDef.CreateField(MODEL_OVERBURDEN_ID, dbLong,0,dbAutoIncrField);
	TableDef.CreateField(MODEL_OVERBURDEN_OFFSHORE_DEFINITION,dbBoolean,0);
	TableDef.CreateField(MODEL_OVERBURDEN_ELEVATION,dbDouble,0);
	TableDef.CreateField(MODEL_OVERBURDEN_OVERBURDEN_BOTTOM_TVD,dbDouble,0);
	TableDef.CreateField(MODEL_OVERBURDEN_SURFACE_UNDIST_TEMP,dbDouble,0);
	TableDef.CreateField(MODEL_OVERBURDEN_OB_GEOTHERMAL_GRAD,dbDouble,0);
	TableDef.CreateField(MODEL_OVERBURDEN_AVERAGE_DENSITY,dbDouble,0);
	TableDef.CreateField(MODEL_OVERBURDEN_WATERDEPTH,dbDouble,0);
	TableDef.CreateField(MODEL_OVERBURDEN_WATER_DENSITY,dbDouble,0);
		
	TableDef.Append();
	TableDef.Close();
}

