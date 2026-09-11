// AnalysisPointTable.h: interface for the CAnalysisPointTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYSISPOINTTABLE_H__F5A75200_A5FB_4590_95C3_A560E8B59E81__INCLUDED_)
#define AFX_ANALYSISPOINTTABLE_H__F5A75200_A5FB_4590_95C3_A560E8B59E81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define ANALYSIS_POINT_TABLE								"Analysis Point"
#define ANALYSIS_POINT_TMD									"TMD"
#define ANALYSIS_POINT_DRILLING_FLUID_TEMP					"Drilling Fluid Temperature"
#define ANALYSIS_POINT_TIME_OF_EXPOS_TO_DRIL_FLUID			"Time of Exposure to Drilling Fluid"
//#define ANALYSIS_POINT_TEST_PRESSURE						"Test Pressure"
#define ANALYSIS_POINT_COMPLETION_PRESSURE_DIFF				"Completion Pressure Difference"
//#define ANALYSIS_POINT_PRODUCTION_PORE_PRESSURE				"Production Pore Pressure"
#define ANALYSIS_POINT_BOTTOM_HOLE_FLOWING_PRESSURE			"Bottom Hole Flowing Pressure"
#define ANALYSIS_POINT_BOTTOM_HOLE_FLOWING_TEMP				"Bottom Hole Flowing Temperature"
#define ANALYSIS_POINT_TIME_OF_CONTINUOUS_PROD				"Time of Continuous Production"
#define ANALYSIS_POINT_MAXIMUM_INJECTION_PRESSURE			"Maximum Injection Pressure"
#define ANALYSIS_POINT_INJECTION_TEMPERATURE				"Injection Temperature"
#define ANALYSIS_POINT_TIME_OF_CONTINUOUS_INJECTION			"Time of Continuous Injection"
#define ANALYSIS_POINT_MAX_INJECTING_FRAC_PRESSURE			"Max Injecting Frac Pressure"
#define ANALYSIS_POINT_INJECTION_FRAC_TREATING_FLUID_TEMP	"Injection Frac Treating Fluid Temperature"
#define ANALYSIS_POINT_INJECTION_FRAC_TIME_OF_EXPOSURE		"Injection Frac Time of Exposure"
#define ANALYSIS_POINT_BOTTOM_HOLE_EVAC_TREATING_PRESSURE	"Bottom Hole Evac Treating Pressure"
#define ANALYSIS_POINT_EVAC_TREATING_FLUID_TEMP				"Evac Treating Fluid Temp"
#define ANALYSIS_POINT_EVAC_TIME_OF_EXPOSURE				"Evac Time of Exposure"
#define ANALYSIS_POINT_ID									"Analysis Point ID"

class CDSModelDatabase;

class CAnalysisPointTable  : public CDSTable
{
public:
	CAnalysisPointTable(CDaoDatabase& Database);
	virtual ~CAnalysisPointTable();
	virtual void Open();
	virtual void Create();

};



#endif // !defined(AFX_ANALYSISPOINTTABLE_H__F5A75200_A5FB_4590_95C3_A560E8B59E81__INCLUDED_)
