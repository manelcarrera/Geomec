// BatchTableTable.cpp: implementation of the CBatchTableTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSModelDatabase.h"
#include "dsealect.h"
#include "BatchTableTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBatchTableTable::CBatchTableTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CBatchTableTable::~CBatchTableTable()
{

}

void CBatchTableTable::Open()
{
	if(!RS().IsOpen()){
		// Open the table
		RS().Open(dbOpenTable,BATCHTABLE_TABLE);
	}
}

void CBatchTableTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(BATCHTABLE_TABLE);
	
	TableDef.CreateField(BATCHTABLE_USE_FOR_CALCULATION,dbBoolean,0);			
	TableDef.CreateField(BATCHTABLE_IS_CALCULATED,dbBoolean,0);			
	TableDef.CreateField(BATCHTABLE_CALCULATION_SUCCEEDED,dbBoolean,0);			
	TableDef.CreateField(BATCHTABLE_HASRESULTS,dbBoolean,0);			
	TableDef.CreateField(BATCHTABLE_DSO_FILE,dbLongBinary,0);			
	TableDef.CreateField(BATCHTABLE_ID,dbLong,0,dbAutoIncrField);

	TableDef.CreateField(BATCHTABLE_DEFAULT_STEPPING_SCHEME,dbBoolean,0);
	TableDef.CreateField(BATCHTABLE_TIME_STEP_NUMBER,dbLong,0);
	TableDef.CreateField(BATCHTABLE_ITERATION_SCHEME,dbLong,0);//enum
	TableDef.CreateField(BATCHTABLE_MAX_NUMBER_OF_ITERATIONS,dbLong,0);
	TableDef.CreateField(BATCHTABLE_CONVERGENCE_NORM,dbLong,0);//enum
	TableDef.CreateField(BATCHTABLE_STOP_BATCH_RUN,dbBoolean,0);
	TableDef.CreateField(BATCHTABLE_FURTHER_POST_PROCESSING,dbBoolean,0);
	//TableDef.CreateField(BATCHTABLE_NO_MONITORING,dbBoolean,0);
	TableDef.CreateField(BATCHTABLE_SAVE_DIAGNOSTIC,dbBoolean,0);

	TableDef.CreateField(BATCHTABLE_PLOT_FOREQPLAST,dbLongBinary,0);
	TableDef.CreateField(BATCHTABLE_PLOT_CASEQPLAST,dbLongBinary,0);
	TableDef.CreateField(BATCHTABLE_PLOT_FORDEB,dbLongBinary,0);
	TableDef.CreateField(BATCHTABLE_PLOT_CASDEB,dbLongBinary,0);
	TableDef.CreateField(BATCHTABLE_PLOT_CEMEQPLAST,dbLongBinary,0);
	TableDef.CreateField(BATCHTABLE_PLOT_RADCRACK,dbLongBinary,0);
	
	TableDef.CreateField(BATCHTABLE_DIAGNOSTIC_FILE,dbLongBinary,0);
	
	TableDef.Append();	
	TableDef.Close();
}

  