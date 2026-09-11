// BatchTableTable.h: interface for the CBatchTableTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATCHTABLETABLE_H__B0A32886_E7D9_474D_8750_6A986AD8F656__INCLUDED_)
#define AFX_BATCHTABLETABLE_H__B0A32886_E7D9_474D_8750_6A986AD8F656__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define BATCHTABLE_TABLE						"BatchTable"
#define BATCHTABLE_ID							"BatchTableID"
#define BATCHTABLE_USE_FOR_CALCULATION			"UseForCalculation"
#define BATCHTABLE_IS_CALCULATED     			"IsCalculated"
#define BATCHTABLE_CALCULATION_SUCCEEDED		"CalculationSucceeded"
#define BATCHTABLE_HASRESULTS					"HasResults"
#define BATCHTABLE_DSO_FILE						"DSOFile"

#define BATCHTABLE_DEFAULT_STEPPING_SCHEME		"Default Stepping Scheme"
#define BATCHTABLE_TIME_STEP_NUMBER				"Time Step Number"
#define BATCHTABLE_ITERATION_SCHEME				"Iteration Scheme"
#define BATCHTABLE_MAX_NUMBER_OF_ITERATIONS		"Max Number Of Iterations"
#define BATCHTABLE_CONVERGENCE_NORM				"Convergence Norm"
#define BATCHTABLE_STOP_BATCH_RUN				"Stop Batch Run"
#define BATCHTABLE_FURTHER_POST_PROCESSING		"Further Post Processing"
//#define BATCHTABLE_NO_MONITORING				"No Monitoring"
#define BATCHTABLE_SAVE_DIAGNOSTIC				"Save Diagnostic"

#define BATCHTABLE_PLOT_FOREQPLAST				"Formation Plast Plot"
#define BATCHTABLE_PLOT_CASEQPLAST				"Casing Plast Plot"
#define BATCHTABLE_PLOT_FORDEB					"Formation Debonding Plot"
#define BATCHTABLE_PLOT_CASDEB					"Casing Debonding Plot"
#define BATCHTABLE_PLOT_CEMEQPLAST				"Cement Plast Plot"
#define BATCHTABLE_PLOT_RADCRACK				"Cement Rad Cracking Plot"

#define BATCHTABLE_DIAGNOSTIC_FILE				"DiagnosticFile"

class CDSModelDatabase;

class CBatchTableTable : public CDSTable  
{
public:
	CBatchTableTable(CDaoDatabase& Database);
	virtual ~CBatchTableTable();
	virtual void Open();
	virtual void Create();

};

#endif // !defined(AFX_BATCHTABLETABLE_H__B0A32886_E7D9_474D_8750_6A986AD8F656__INCLUDED_)
