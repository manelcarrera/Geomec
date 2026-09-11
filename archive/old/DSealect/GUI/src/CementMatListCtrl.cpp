// CementMatListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "DSealectGlobal.h"
#include "DoubleEdit.h"
#include "CementMaterial.h"
#include "CementMatListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CCementMatListCtrl

//##ModelId=3C60D57B02B2
CCementMatListCtrl::CCementMatListCtrl()
{
}

//##ModelId=3C60D57B02C2
CCementMatListCtrl::~CCementMatListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCementMatListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCementMatListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCementMatListCtrl message handlers

//##ModelId=3C60D57B02B1
void CCementMatListCtrl::CreateColums()
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
		
 #define IDS_CEM_YOUNGS_MODULUS          7000
#define IDS_CEM_POISSON_RATIO           7001
#define IDS_CEM_VOLUMETRIC_SPEC_HEAT    7002
#define IDS_CEM_THERMAL_CONDUCTIVITY    7003
#define IDS_CEM_TERMAL_EXPANSION        7004
#define IDS_CEM_TENSILE_STRENGTH        7005
#define IDS_CEM_TENS_FRACTURE_ENERGY    7006
#define IDS_CEM_COHESION                7007
#define IDS_CEM_FRICTION_ANGLE          7008
#define IDS_CEM_HARDENING_TYPE          7009
#define IDS_CEM_HARDENING_GRADIENT      7010
#define IDS_CEM_COMP_FRACTURE_ENERGY    7011
#define IDS_CEM_VOLUMETRIC_SHRINKAGE    7012
#define IDS_CEM_HYDRATION_HEAT          7013
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

	for(int i=IDS_CEM_YOUNGS_MODULUS ; i<= IDS_CEM_HYDRATION_HEAT;i++)
	{
		strMatName.LoadString(i);
		lvItem.iItem=pos;
		lvItem.pszText= strMatName.GetBufferSetLength(strMatName.GetLength());
		InsertItem(&lvItem); 
		SetItemText(pos,0,strMatName);
		pos++;
	}

}

//##ModelId=3C60D57B02B3
void CCementMatListCtrl::ShowMaterial(CCementMaterial& mat)
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

	for(int i=IDS_CEM_YOUNGS_MODULUS ; i<= IDS_CEM_HYDRATION_HEAT;i++)
	{	
		if(i==IDS_CEM_HARDENING_GRADIENT)
		{
			if(mat.HardeningType()==LinearHardening) //don't show HardGrad if type is LinHard
			{
				strMatName.LoadString(i);
				lvItem.iItem=pos;
				lvItem.pszText= strMatName.GetBufferSetLength(strMatName.GetLength());
				InsertItem(&lvItem); 
				SetItemText(pos,0,strMatName);
				pos++;	
			}
		}
		else
		{
			if(i==IDS_CEM_COMP_FRACTURE_ENERGY) 
			{
				if(mat.HardeningType()==LinearSoftening||mat.HardeningType()==ParabolicSoftening)
					//don't show FracEnergy if type is LinSoft or ParabSoft
				{
					strMatName.LoadString(i);
					lvItem.iItem=pos;
					lvItem.pszText= strMatName.GetBufferSetLength(strMatName.GetLength());
					InsertItem(&lvItem); 
					SetItemText(pos,0,strMatName);
					pos++;
				}
			}
			else
			{
				strMatName.LoadString(i);
				lvItem.iItem=pos;
				lvItem.pszText= strMatName.GetBufferSetLength(strMatName.GetLength());
				InsertItem(&lvItem); 
				SetItemText(pos,0,strMatName);
				pos++;
			}
		}
		
	}


	CString strVal;
	pos=0;
	CNoneQuantity dum;

	
	SetItemText(pos,1,mat.Name());
	SetItemText(pos++,2,dum.UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.YoungsModulus()->Undefined())
		strVal=CDoubleEdit::Format(mat.YoungsModulus()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.YoungsModulus()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.PoissonRatio()->Undefined())
		strVal=CDoubleEdit::Format(mat.PoissonRatio()->Value());
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

	if(!mat.TensileStrength()->Undefined())
		strVal=CDoubleEdit::Format(mat.TensileStrength()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.TensileStrength()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.TensFractureEnergy()->Undefined())
		strVal=CDoubleEdit::Format(mat.TensFractureEnergy()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.TensFractureEnergy()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.Cohesion()->Undefined())
		strVal=CDoubleEdit::Format(mat.Cohesion()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.Cohesion()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.FrictionAngle()->Undefined())
		strVal=CDoubleEdit::Format(mat.FrictionAngle()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.FrictionAngle()->UnitName(GetDoc()->UnitSystem()).c_str());

	SetItemText(pos,1,CDSMaterial::GetHardeningName(const_cast<eHardeningType>(mat.HardeningType())));
	SetItemText(pos++,2,dum.UnitName(GetDoc()->UnitSystem()).c_str());

	if(mat.HardeningType()==LinearHardening)
	{
		if(!mat.CompHardeningGradient()->Undefined())
			strVal=CDoubleEdit::Format(mat.CompHardeningGradient()->Value(GetDoc()->UnitSystem()));
		else
			strVal="";
		SetItemText(pos,1,strVal);
		SetItemText(pos++,2,mat.CompHardeningGradient()->UnitName(GetDoc()->UnitSystem()).c_str());
	}

	if(mat.HardeningType()==LinearSoftening||mat.HardeningType()==ParabolicSoftening)
	{
		if(!mat.CompFractureEnergy()->Undefined())
			strVal=CDoubleEdit::Format(mat.CompFractureEnergy()->Value(GetDoc()->UnitSystem()));
		else
			strVal="";
		SetItemText(pos,1,strVal);
		SetItemText(pos++,2,mat.CompFractureEnergy()->UnitName(GetDoc()->UnitSystem()).c_str());
	}

	if(!mat.VolumetricShrinkage()->Undefined())
		strVal=CDoubleEdit::Format(-mat.VolumetricShrinkage()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.VolumetricShrinkage()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(!mat.HydrationHeat()->Undefined())
		strVal=CDoubleEdit::Format(mat.HydrationHeat()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.HydrationHeat()->UnitName(GetDoc()->UnitSystem()).c_str());
	
}

//##ModelId=3C60D57B02C0
void CCementMatListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CreateColums();
}

