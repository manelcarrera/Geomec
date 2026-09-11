// FormationMatListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "DSealectGlobal.h"
#include "DoubleEdit.h"
#include "FormationMaterial.h"
#include "FormationMatListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormationMatListCtrl

//##ModelId=3C60D57502D0
CFormationMatListCtrl::CFormationMatListCtrl()
{
}

//##ModelId=3C60D57502DF
CFormationMatListCtrl::~CFormationMatListCtrl()
{
}


BEGIN_MESSAGE_MAP(CFormationMatListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CFormationMatListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormationMatListCtrl message handlers

//##ModelId=3C60D57502CF
void CFormationMatListCtrl::CreateColums()
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
		IDS_POISSON_RATIO
		IDS_VOLUMETRIC_SPEC_HEAT
		IDS_THERMAL_CONDUCTIVITY
		IDS_THERMAL_EXPANSION
		IDS_PLASTICITY
		IDS_COHESION
		IDS_FRICTION_ANGLE
		IDS_FRICTION_ANGLE
		IDS_HARDENING_TYPE
		IDS_HARDENING_GRADIENT
		IDS_FRACTURE_ENERGY
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

	for(int i=IDS_YOUNGS_MODULUS ; i<= IDS_FRACTURE_ENERGY;i++)
	{
		strMatName.LoadString(i);
		lvItem.iItem=pos;
		lvItem.pszText= strMatName.GetBufferSetLength(strMatName.GetLength());
		InsertItem(&lvItem); 
		SetItemText(pos,0,strMatName);
		pos++;
	}
	

}

//##ModelId=3C60D57502D1
void CFormationMatListCtrl::ShowMaterial(CFormationMaterial& mat)
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

	for(int i=IDS_YOUNGS_MODULUS ; i<= IDS_PLASTICITY;i++) //show these params always
	{
		strMatName.LoadString(i);
		lvItem.iItem=pos;
		lvItem.pszText= strMatName.GetBufferSetLength(strMatName.GetLength());
		InsertItem(&lvItem); 
		SetItemText(pos,0,strMatName);
		pos++;
	}
	
	if(mat.Plasticity()) //don't show lines for plasticparameters if there is no plasticity
	{
		for(int i=IDS_COHESION ; i<= IDS_FRACTURE_ENERGY;i++)
		{
			if(i==IDS_HARDENING_GRADIENT)
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
				if(i==IDS_FRACTURE_ENERGY) 
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

	if(!mat.ThermalConductivity()->Undefined())
		strVal=CDoubleEdit::Format(mat.ThermalExpansion()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";	
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.ThermalExpansion()->UnitName(GetDoc()->UnitSystem()).c_str());

  if(!mat.Density()->Undefined())
    strVal = CDoubleEdit::Format(mat.Density()->Value(GetDoc()->UnitSystem()));
  else
    strVal="";
  SetItemText(pos, 1, strVal);
  SetItemText(pos++, 2, mat.Density()->UnitName(GetDoc()->UnitSystem()).c_str());

	if(mat.Plasticity())
		SetItemText(pos,1,"Yes");
	else
		SetItemText(pos,1,"No");
	SetItemText(pos++,2,dum.UnitName(GetDoc()->UnitSystem()).c_str());

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
		if(!mat.HardeningGradient()->Undefined())
			strVal=CDoubleEdit::Format(mat.HardeningGradient()->Value(GetDoc()->UnitSystem()));
		else
			strVal="";
		SetItemText(pos,1,strVal);
		SetItemText(pos++,2,mat.HardeningGradient()->UnitName(GetDoc()->UnitSystem()).c_str());
	}

	if(!mat.FractureEnergy()->Undefined())
		strVal=CDoubleEdit::Format(mat.FractureEnergy()->Value(GetDoc()->UnitSystem()));
	else
		strVal="";
	SetItemText(pos,1,strVal);
	SetItemText(pos++,2,mat.FractureEnergy()->UnitName(GetDoc()->UnitSystem()).c_str());
	
}

//##ModelId=3C60D57502D3
void CFormationMatListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CreateColums();
}
