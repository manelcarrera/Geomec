// ControlTable.h: interface for the CControlTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLTABLE_H__D38BFC88_90A6_4775_A97A_5D0B40CC1EA1__INCLUDED_)
#define AFX_CONTROLTABLE_H__D38BFC88_90A6_4775_A97A_5D0B40CC1EA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define CONTROL_TABLE							"Control"
#define CONTROL_PROJECT_NAME					"Project Name"
#define CONTROL_WELL_TYPE						"Well Type"
#define CONTROL_BALANCED_COMPLETION				"Balanced Completion"
#define CONTROL_UNDERBALANCED_COMPLETION		"Underbalanced Completion"
#define CONTROL_OVERBALANCED_COMPLETION			"Overbalanced Completion"
#define CONTROL_CONVERSION						"Conversion"
#define CONTROL_FRACTURING						"Fracturing"
#define CONTROL_EVACUATION						"Evacuation"
#define CONTROL_DEFAULT_MESH_REFINEMENT			"Default Mesh Refinement"
#define CONTROL_ROCK_REFINEMENT					"Rock Refinement"
#define CONTROL_CASING_REFINEMENT				"Casing Refinement"
#define CONTROL_CEMENT_REFINEMENT				"Cement Refinement"
#define CONTROL_OUTPUT_LEVEL					"Output Level"
#define CONTROL_TEST_PRESSURE "Surface Test Pressure"
#define CONTROL_FLUID_PRESSURE_GRADIENT "Fluid Pressure Gradient"
#define CONTROL_SHUT_IN							"Shut In"


class CDSModelDatabase;

class CControlTable  : public CDSTable
{
public:
	CControlTable(CDaoDatabase& Database);
	virtual ~CControlTable();
	virtual void Open();
	virtual void Create();

};



#endif // !defined(AFX_CONTROLTABLE_H__D38BFC88_90A6_4775_A97A_5D0B40CC1EA1__INCLUDED_)
