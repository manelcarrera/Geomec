// FormationTable.cpp: implementation of the CFormationTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSModelDatabase.h"
#include "FormationTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFormationTable::CFormationTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CFormationTable::~CFormationTable()
{

}

void CFormationTable::Open()
{
	if(!RS().IsOpen()){
		// Open the table
		RS().Open(dbOpenTable,FORMATION_TABLE);
	}
}

void CFormationTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(FORMATION_TABLE);
	TableDef.CreateField(FORMATION_ID,dbLong,0,dbAutoIncrField);

	TableDef.CreateField(FORMATION_NAME,dbText,255);

	CDaoFieldInfo fieldinfo;
	fieldinfo.m_strName = FORMATION_MATERIAL_NAME;
	fieldinfo.m_nType = dbText;
	fieldinfo.m_lSize = 255;
	fieldinfo.m_lAttributes = 0;
	fieldinfo.m_bAllowZeroLength = TRUE;
	TableDef.CreateField(fieldinfo);



	TableDef.CreateField(FORMATION_BOTTOM_TVD,dbDouble,0);	
	TableDef.CreateField(FORMATION_GEOTHERMAL_GRAD,dbDouble,0);		
	TableDef.CreateField(FORMATION_MAX_HOR_STRESS_RATIO,dbDouble,0);
	TableDef.CreateField(FORMATION_MIN_HOR_STRESS_RATIO,dbDouble,0);
	TableDef.CreateField(FORMATION_AZIMUTH_SH_WITH_NORTH,dbDouble,0);
	TableDef.CreateField(FORMATION_POROUS,dbBoolean,0);			
	TableDef.CreateField(FORMATION_TOP_PORE_PRESSURE,dbDouble,0);	
	TableDef.CreateField(FORMATION_BOTTOM_PORE_PRESSURE,dbDouble,0);

	TableDef.Append();	
	TableDef.Close();
}
