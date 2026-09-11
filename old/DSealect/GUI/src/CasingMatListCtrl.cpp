// CasingMatListCtrl.cpp: implementation of the CCasingMatListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "DSealectGlobal.h"
#include "DoubleEdit.h"
#include "CasingMaterial.h"
#include "CasingMatListCtrl.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


// CCasingMatListCtrl.cpp : implementation file
//

//##ModelId=3C60D57E0189
CCasingMatListCtrl::CCasingMatListCtrl()
{

}

//##ModelId=3C60D57E019B
CCasingMatListCtrl::~CCasingMatListCtrl()
{

}


BEGIN_MESSAGE_MAP(CCasingMatListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCasingMatListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCasingMatListCtrl message handlers

//##ModelId=3C60D57E0188
void CCasingMatListCtrl::CreateColums()
{
	int nColumnCount = GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
	   DeleteColumn(0);
	}

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvColumn.fmt= LVCFMT_LEFT;

	lvColumn.cx=140;
	lvColumn.iSubItem=0;
	lvColumn.pszText="Parameter";
	InsertColumn(0, &lvColumn);
	
	lvColumn.cx=65;
	lvColumn.iSubItem=1;
	lvColumn.pszText="Value";
	InsertColumn(1, &lvColumn);

	lvColumn.cx=70;
	lvColumn.iSubItem=2;
	lvColumn.pszText="Unit";
	InsertColumn(2, &lvColumn);

	/*
		
  #define IDS_CAS_OUTER_DIAMETER          9000
#define IDS_CAS_INNER_DIAMETER          9001
#define IDS_CAS_YOUNGS_MODULUS          9002
#define IDS_CAS-POISSON_RATIO           9003
#define IDS_CAS_VOLUMETRIC_SPEC_HEAT    9004
#define IDS_CAS_THERMAL_CONDUCTIVITY    9005
#define IDS_CAS_THERMAL_EXPANSION       9006
#define IDS_CAS_WEIGHT_PER_UNIT_LENGTH  9007
#define IDS_CAS_PLASTICITY              9008
#define IDS_CAS_YIELD_STRENGTH          9009
	*/


	CString strMatName;
	LV_ITEM lvItem;
	int pos=0;

	lvItem.state=0;
	lvItem.mask=0;
	lvItem.iItem=pos;
	lvItem.iSubItem=0;
	lvItem.pszText= "Material Name";
	InsertItem(&lvItem); 
	SetItemText(pos,0,"Material Name");
	pos++;

	for(int i=IDS_CAS_OUTER_DIAMETER ; i<= IDS_CAS_YIELD_STRENGTH;i++)
	{
		strMatName.LoadString(i);
		lvItem.iItem=pos;
		lvItem.pszText= strMatName.GetBufferSetLength(strMatName.GetLength());
		InsertItem(&lvItem); 
		SetItemText(pos,0,strMatName);
		pos++;
	}

}

//##ModelId=3C60D57E0197
void CCasingMatListCtrl::ShowMaterial(CCasingMaterial& mat)
{
	LV_ITEM lvItem;
	int pos=0;
	CString strMatName;

	DeleteAllItems();

	lvItem.state=0;
	lvItem.mask=0;
	lvItem.iItem=pos;
	lvItem.iSubItem=0;
	lvItem.pszText= "Material Name";
	InsertItem(&lvItem); 
	SetItemText(pos,0,"Material Name");
	pos++;

	for(int i=IDS_CAS_OUTER_DIAMETER ; i<= IDS_CAS_YIELD_STRENGTH;i++)
	{
		strMatName.LoadString(i);
		lvItem.iItem=pos;
		lvItem.pszText= strMatName.GetBufferSetLength(strMatName.GetLength());
		InsertItem(&lvItem); 
		SetItemText(pos,0,strMatName);
		pos++;
	}


	CString strVal;
	pos=0;
	CNoneQuantity dum;

	
	SetItemText(pos,1,mat.Name());
	SetItemText(pos++,2,dum.UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.CasingOuterDiameter()->Undefined())
		strVal=CDoubleEdit::Format(mat.CasingOuterDiameter()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.CasingOuterDiameter()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.CasingInnerDiameter()->Undefined())
		strVal=CDoubleEdit::Format(mat.CasingInnerDiameter()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.CasingInnerDiameter()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.YoungsModulus()->Undefined())
		strVal=CDoubleEdit::Format(mat.YoungsModulus()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.YoungsModulus()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.PoissonRatio()->Undefined())
		strVal=CDoubleEdit::Format(mat.PoissonRatio()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";	
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.PoissonRatio()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.VolumetricSpecificHeat()->Undefined())
		strVal=CDoubleEdit::Format(mat.VolumetricSpecificHeat()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";	
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.VolumetricSpecificHeat()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.ThermalConductivity()->Undefined())
		strVal=CDoubleEdit::Format(mat.ThermalConductivity()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.ThermalConductivity()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.ThermalExpansion()->Undefined())
		strVal=CDoubleEdit::Format(mat.ThermalExpansion()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.ThermalExpansion()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.WeightPerUnitLength()->Undefined())
		strVal=CDoubleEdit::Format(mat.WeightPerUnitLength()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.WeightPerUnitLength()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(mat.Plasticity())
		SetItemText(pos,1,"Yes");
	else
		SetItemText(pos,1,"No");
	SetItemText(pos++,2,dum.UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.YieldStrength()->Undefined())
		strVal=CDoubleEdit::Format(mat.YieldStrength()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.YieldStrength()->UnitName(GetDoc()->UnitSystem()).c_str());
	
}

//##ModelId=3C60D57E0199
void CCasingMatListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CreateColums();
}
