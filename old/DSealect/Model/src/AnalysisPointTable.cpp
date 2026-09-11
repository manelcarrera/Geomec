// AnalysisPointTable.cpp: implementation of the CAnalysisPointTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSModelDatabase.h"
#include "AnalysisPointTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAnalysisPointTable::CAnalysisPointTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CAnalysisPointTable::~CAnalysisPointTable()
{

}

void CAnalysisPointTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, ANALYSIS_POINT_TABLE);
	}
}

void CAnalysisPointTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(ANALYSIS_POINT_TABLE);
	TableDef.CreateField(ANALYSIS_POINT_TMD,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_DRILLING_FLUID_TEMP,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_TIME_OF_EXPOS_TO_DRIL_FLUID,dbDouble,0);
//	TableDef.CreateField(ANALYSIS_POINT_TEST_PRESSURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_COMPLETION_PRESSURE_DIFF,dbDouble,0);
//	TableDef.CreateField(ANALYSIS_POINT_PRODUCTION_PORE_PRESSURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_BOTTOM_HOLE_FLOWING_PRESSURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_BOTTOM_HOLE_FLOWING_TEMP,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_TIME_OF_CONTINUOUS_PROD,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_MAXIMUM_INJECTION_PRESSURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_INJECTION_TEMPERATURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_TIME_OF_CONTINUOUS_INJECTION,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_MAX_INJECTING_FRAC_PRESSURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_INJECTION_FRAC_TREATING_FLUID_TEMP,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_INJECTION_FRAC_TIME_OF_EXPOSURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_BOTTOM_HOLE_EVAC_TREATING_PRESSURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_EVAC_TREATING_FLUID_TEMP,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_EVAC_TIME_OF_EXPOSURE,dbDouble,0);
	TableDef.CreateField(ANALYSIS_POINT_ID, dbLong,0,dbAutoIncrField);
		
	TableDef.Append();
	TableDef.Close();
}

