// attritetrasubhorizon.cpp : implementation file
//

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include <cmath>
#include "geomec.h"
#include "attritetrasubhorizon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriTetraSubHorizon dialog


CAttriTetraSubHorizon::CAttriTetraSubHorizon(CTetraSubHorizon& horizon, UINT uTemplateID, CWnd* pParent)
: CAttributesTemplate<CTetraSubHorizon>(uTemplateID, horizon, pParent)
{
	ASSERT( (horizon.Slip() || (uTemplateID == IDD_ATTRI_SUB_FAULT)) ||
			(!horizon.Slip() || (uTemplateID == IDD_ATTRI_SUB_HORIZON)));

	//{{AFX_DATA_INIT(CAttriTetraSubHorizon)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CAttriTetraSubHorizon::EnableValues(BOOL val)
{
	GetDlgItem(IDC_ED_FRICTION_ANGLE)->EnableWindow(val);
	GetDlgItem(IDC_ED_COHESION)->EnableWindow(val);

}

void CAttriTetraSubHorizon::EnableStickValues(BOOL val)
{
	GetDlgItem(IDC_ED_FRICTION_ANGLE_STICK)->EnableWindow(val);
	GetDlgItem(IDC_ED_COHESION_STICK)->EnableWindow(val);
}
void CAttriTetraSubHorizon::DoDataExchange(CDataExchange* pDX)
{
	// Variables ....
	CString strName, strCohesionUnit, strAngleUnit;
	double dCohesion, dFrictionAngle;
	double dStickCohesion, dStickFricAngle;
	int nSlipType;
	BOOL bFlip;

	if(!pDX->m_bSaveAndValidate)
	{
		strName = Copy().Name();

		if(Copy().Slip())
		{
			bFlip = Copy().Flip() == true;
			dStickCohesion = Copy().StickCohesion().Value(UnitNode().Unit());
			dStickFricAngle = Copy().StickFricAngle().Value(UnitNode().Unit());
			dCohesion = Copy().Cohesion().Value(UnitNode().Unit());
			dFrictionAngle = Copy().FrictionAngle().Value(UnitNode().Unit());
			strCohesionUnit = CString(Copy().Cohesion().UnitName(UnitNode().Unit()).c_str());
			strAngleUnit = CString(Copy().FrictionAngle().UnitName(UnitNode().Unit()).c_str());
			GetDlgItem(IDC_CK_FLIP)->EnableWindow(Copy().SurfaceSize() > 1);

			switch(Copy().SlipType())
			{
			case CTetraSubHorizon::SLIP:
				nSlipType = 0;
				EnableValues(FALSE);
				EnableStickValues(FALSE);
				break;
			case CTetraSubHorizon::STICK:
				nSlipType = 1;
				EnableValues(FALSE);
				EnableStickValues(TRUE);
				break;
			case CTetraSubHorizon::USER:
				nSlipType = 2;
				EnableValues(TRUE);
				EnableStickValues(FALSE);
				break;
			default:
				assert(false);
			}
		}

		
	}


	CAttributesTemplate<CTetraSubHorizon>::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttriTetraSubHorizon)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	
	DDX_Text(pDX, IDC_ED_NAME, strName);	

 	if(Copy().Slip())
	{
		DDX_Radio(pDX, IDC_RADIO_SLIP, nSlipType);
		// Do the units
		DDX_Text(pDX, IDC_ST_FRICTION_ANGLE_UNIT, strAngleUnit);
		DDX_Text(pDX, IDC_ST_COHESION_UNIT, strCohesionUnit);
		DDX_Text(pDX, IDC_ST_FRICTION_ANGLE_UNIT2, strAngleUnit);
		DDX_Text(pDX, IDC_ST_COHESION_UNIT2, strCohesionUnit);
		DDX_Text(pDX, IDC_ED_FRICTION_ANGLE_STICK, dStickFricAngle); 
		DDX_Text(pDX, IDC_ED_COHESION_STICK, dStickCohesion); 
		DDX_Text(pDX, IDC_ED_COHESION, dCohesion);
		DDX_Text(pDX, IDC_ED_FRICTION_ANGLE, dFrictionAngle);
		DDX_Check(pDX, IDC_CK_FLIP, bFlip);
	//	if(nSlipType == 0 || nSlipType == 1)
//			EnableValues(FALSE);
//		else
//			EnableValues(TRUE);
	}

	if(pDX->m_bSaveAndValidate)
	{
		Copy().Name(strName);
		if(Copy().Slip())
		{
			DDV_MinMaxDouble(pDX, 
							 dCohesion, 
							 Copy().Cohesion().MinValue( UnitNode().Unit()),
							 Copy().Cohesion().MaxValue( UnitNode().Unit()));

			DDV_MinMaxDouble(pDX, 
							 dFrictionAngle, 
							 Copy().FrictionAngle().MinValue( UnitNode().Unit()),
							 Copy().FrictionAngle().MaxValue( UnitNode().Unit()));
			// Prevent the cohesion and friction angle from by zero
			if(fabs(dCohesion) < EPS)
			{
				CString sMessage;
				sMessage.Format("Enter a number between %f and %f", Copy().Cohesion().MinValue( UnitNode().Unit()), Copy().Cohesion().MaxValue( UnitNode().Unit()));
				AfxMessageBox(sMessage);
				pDX->Fail();
			}
			if(fabs(dFrictionAngle) < EPS)
			{
				CString sMessage;
				sMessage.Format("Enter a number between %f and %f", Copy().FrictionAngle().MinValue( UnitNode().Unit()), Copy().FrictionAngle().MaxValue( UnitNode().Unit()));
				AfxMessageBox(sMessage);
				pDX->Fail();
			}

			if(nSlipType == 0)
			{
				Copy().SlipType(CTetraSubHorizon::SLIP);
			}
			else if(nSlipType == 1)
			{
				Copy().SlipType(CTetraSubHorizon::STICK);
				Copy().StickQuantities(dStickCohesion, dStickFricAngle, UnitNode().Unit());
			}
			else
			{
				assert(nSlipType == 2);
				Copy().SlipType(CTetraSubHorizon::USER);
				Copy().Slip(dCohesion, dFrictionAngle, UnitNode().Unit());
			}
			Copy().Flip(bFlip != FALSE);
		}
	}

}


BEGIN_MESSAGE_MAP(CAttriTetraSubHorizon, CDialog)
	//{{AFX_MSG_MAP(CAttriTetraSubHorizon)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ST_COLOR, OnStColor)
	ON_BN_CLICKED(IDC_RADIO_STICK, OnStick)
	ON_BN_CLICKED(IDC_RADIO_SLIP, OnSlip)
	ON_BN_CLICKED(IDC_RADIO_USER, OnUserDefined)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriTetraSubHorizon message handlers

void CAttriTetraSubHorizon::OnPaint() 
{
	CPaintDC dc(this); 
	
	FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

void CAttriTetraSubHorizon::OnStColor() 
{
	Copy().Color(SelectColor(Copy().Color()));
	Invalidate();
	UpdateData(FALSE);	
}

void CAttriTetraSubHorizon::OnStick()
{
	Copy().SlipType(CTetraSubHorizon::STICK);
	UpdateData(FALSE);
}

void CAttriTetraSubHorizon::OnSlip()
{
	Copy().SlipType(CTetraSubHorizon::SLIP);
	UpdateData(FALSE);
}

void CAttriTetraSubHorizon::OnUserDefined()
{
	Copy().SlipType(CTetraSubHorizon::USER);
	UpdateData(FALSE);
}