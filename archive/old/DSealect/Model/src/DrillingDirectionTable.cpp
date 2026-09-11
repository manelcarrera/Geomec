// DrillingDirectionTable.cpp: implementation of the CDrillingDirectionTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSModelDatabase.h"
#include "DrillingDirectionTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrillingDirectionTable::CDrillingDirectionTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CDrillingDirectionTable::~CDrillingDirectionTable()
{

}

void CDrillingDirectionTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, DRILLING_DIR_TABLE);
	}
}

void CDrillingDirectionTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(DRILLING_DIR_TABLE);
	TableDef.CreateField(DRILLING_DIR_ID, dbLong,0,dbAutoIncrField);
	TableDef.CreateField(DRILLING_DIR_BOTTOM_TMD,dbDouble,0);			
	TableDef.CreateField(DRILLING_DIR_WELL_INCLINATION,dbDouble,0);	
	TableDef.CreateField(DRILLING_DIR_AZIMUTH_WITH_NORTH,dbDouble,0);
	
	TableDef.Append();
	TableDef.Close();
}
