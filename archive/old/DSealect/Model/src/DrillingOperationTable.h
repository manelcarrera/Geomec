// DrillingOperationTable.h: interface for the CDrillingOperationTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRILLINGOPERATIONTABLE_H__02B40A31_5DFE_425E_BA07_B5592FDC08C4__INCLUDED_)
#define AFX_DRILLINGOPERATIONTABLE_H__02B40A31_5DFE_425E_BA07_B5592FDC08C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define DRILLING_OPERATION_TABLE						"Drilling Operation"
#define DRILLING_OPERATION_ID							"Drilling Operation ID"
#define DRILLING_OPERATION_BOTTOM_TMD					"Bottom TMD"
#define DRILLING_OPERATION_BOREHOLE_DIAMETER			"Borehole Diameter"
#define DRILLING_OPERATION_MUD_DENSITY_AT_SURFACE		"Mud Density at Surface"
#define DRILLING_OPERATION_MUD_COMPRESSIBILITY			"Mud Compressibility"
#define DRILLING_OPERATION_CASING_AXIAL_STRESSES		"Casing Axial Stresses"
#define DRILLING_OPERATION_HOOK_LOAD					"Hook Load"
#define DRILLING_OPERATION_WORK_STRING_USED				"Work String Used"
#define DRILLING_OPERATION_DISP_FLUID_STRESS_GRAD		"Disp Fluid Stress Grad"
#define DRILLING_OPERATION_DISP_FLUID_OVERPRESSURE		"Disp Fluid Overpressure"
#define DRILLING_OPERATION_DISP_FLUID_COMPRESSIBILITY	"Disp Fluid Compressibility"
#define DRILLING_OPERATION_WORK_STRING_PACKERS_TMD		"Work String Packers TMD"
#define DRILLING_OPERATION_NEW_FLUID_IN_ANNULUS			"New Fluid In Annulus"
#define DRILLING_OPERATION_NEW_FLUID_STRESS_GRAD		"New Fluid Stress Grad"
#define DRILLING_OPERATION_NEW_FLUID_OVERPRESSURE		"New Fluid Overpressure"
#define DRILLING_OPERATION_NEW_FLUID_COMPRESSIBILITY	"New Fluid Compressibility"
#define DRILLING_OPERATION_PERCENTAGE_ECCENTRICITY		"Percentage Eccentricity"
#define DRILLING_OPERATION_GUIDE_SHOE_DEPTH_ALONG_HOLE	"Guide Shoe Depth along Hole"
#define DRILLING_OPERATION_CASING_MATERIAL_NAME				"Casing Material"
#define DRILLING_OPERATION_TOP_OF_CEMENT_TMD			"Top of Cement TMD"
#define DRILLING_OPERATION_PRESSURE_FROM_FLUIDS_ABOVE	"Pressure from Fluids Above"
#define DRILLING_OPERATION_LEAD_AND_TAIL_CEMENTS		"Lead and Tail Cements"
#define DRILLING_OPERATION_TOP_OF_TAIL_CEMENT_TMD		"Top of Tail Cement TMD"
#define DRILLING_OPERATION_LEAD_CEMENT_STRESS_GRAD		"Lead Cement Stress Grad"
#define DRILLING_OPERATION_TAIL_CEMENT_STRESS_GRAD		"Tail Cement Stress Grad"


class CDSModelDatabase;

class CDrillingOperationTable  : public CDSTable
{
public:
	CDrillingOperationTable(CDaoDatabase& Database);
	virtual ~CDrillingOperationTable();
	virtual void Open();
	virtual void Create();

};


#endif // !defined(AFX_DRILLINGOPERATIONTABLE_H__02B40A31_5DFE_425E_BA07_B5592FDC08C4__INCLUDED_)
