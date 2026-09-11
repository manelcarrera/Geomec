// ModelOverburdenTable.h: interface for the CModelOverburdenTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELOVERBURDENTABLE_H__B2E5D1F5_3ECB_4066_BC9D_52EDE9B0E90D__INCLUDED_)
#define AFX_MODELOVERBURDENTABLE_H__B2E5D1F5_3ECB_4066_BC9D_52EDE9B0E90D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define MODEL_OVERBURDEN_TABLE							"Model Overburden"
#define MODEL_OVERBURDEN_ID								"Model Overburden ID"
#define MODEL_OVERBURDEN_OFFSHORE_DEFINITION			"Offshore Definition"
#define MODEL_OVERBURDEN_ELEVATION						"Elevation"
#define MODEL_OVERBURDEN_OVERBURDEN_BOTTOM_TVD			"Overburden Bottom TVD"
#define MODEL_OVERBURDEN_SURFACE_UNDIST_TEMP			"Surface Undisturbed Temperature"
#define MODEL_OVERBURDEN_OB_GEOTHERMAL_GRAD				"OB Geothermal Gradient"
#define MODEL_OVERBURDEN_OB_AVER_VERT_STRESS_GRAD		"OB Average Vertical Stress Gradient"
#define MODEL_OVERBURDEN_AVERAGE_DENSITY "OB Average Density"
#define MODEL_OVERBURDEN_SEABED_TVD						"Seabed TVD"
#define MODEL_OVERBURDEN_WATERDEPTH           "Water Depth"
#define MODEL_OVERBURDEN_WATER_DENSITY					"Water Density"


class CDSModelDatabase;

class CModelOverburdenTable  : public CDSTable
{
public:
	CModelOverburdenTable(CDaoDatabase& Database);
	virtual ~CModelOverburdenTable();
	virtual void Open();
	virtual void Create();

};


#endif // !defined(AFX_MODELOVERBURDENTABLE_H__B2E5D1F5_3ECB_4066_BC9D_52EDE9B0E90D__INCLUDED_)
