// DrillingOperationTable.cpp: implementation of the CDrillingOperationTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSModelDatabase.h"
#include "DrillingOperationTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDrillingOperationTable::CDrillingOperationTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CDrillingOperationTable::~CDrillingOperationTable()
{

}

void CDrillingOperationTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, DRILLING_OPERATION_TABLE);
	}
}

void CDrillingOperationTable::Create()
{
	CDaoTableDef TableDef(&Database());

	TableDef.Create(DRILLING_OPERATION_TABLE);

	TableDef.CreateField(DRILLING_OPERATION_ID, dbLong,0,dbAutoIncrField);
	TableDef.CreateField(DRILLING_OPERATION_BOTTOM_TMD,dbDouble,0);			
	TableDef.CreateField(DRILLING_OPERATION_BOREHOLE_DIAMETER,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_MUD_DENSITY_AT_SURFACE,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_MUD_COMPRESSIBILITY,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_CASING_AXIAL_STRESSES,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_HOOK_LOAD,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_WORK_STRING_USED,dbBoolean,0);	
	TableDef.CreateField(DRILLING_OPERATION_DISP_FLUID_STRESS_GRAD,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_DISP_FLUID_OVERPRESSURE,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_DISP_FLUID_COMPRESSIBILITY,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_WORK_STRING_PACKERS_TMD,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_NEW_FLUID_IN_ANNULUS,dbBoolean,0);	
	TableDef.CreateField(DRILLING_OPERATION_NEW_FLUID_STRESS_GRAD,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_NEW_FLUID_OVERPRESSURE,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_NEW_FLUID_COMPRESSIBILITY,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_PERCENTAGE_ECCENTRICITY,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_GUIDE_SHOE_DEPTH_ALONG_HOLE,dbDouble,0);	
	
	TableDef.CreateField(DRILLING_OPERATION_TOP_OF_CEMENT_TMD,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_PRESSURE_FROM_FLUIDS_ABOVE,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_LEAD_AND_TAIL_CEMENTS,dbBoolean,0);	
	TableDef.CreateField(DRILLING_OPERATION_TOP_OF_TAIL_CEMENT_TMD,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_LEAD_CEMENT_STRESS_GRAD,dbDouble,0);	
	TableDef.CreateField(DRILLING_OPERATION_TAIL_CEMENT_STRESS_GRAD,dbDouble,0);	
	

	CDaoFieldInfo fieldinfo;
	fieldinfo.m_strName = DRILLING_OPERATION_CASING_MATERIAL_NAME;
	fieldinfo.m_nType = dbText;
	fieldinfo.m_lSize = 255;
	fieldinfo.m_lAttributes = 0;
	fieldinfo.m_bAllowZeroLength = TRUE;
	TableDef.CreateField(fieldinfo);

	TableDef.Append();
	TableDef.Close();
}

