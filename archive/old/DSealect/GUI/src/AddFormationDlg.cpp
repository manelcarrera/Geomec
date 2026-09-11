// AddFormationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DoubleEdit.h"
#include "DSealect.h"
#include "DSealectDoc.h"
#include "CFormationTopography.h"
#include "AddFormationDlg.h"
#include "DSealectGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFormationDlg dialog

/*
CAddFormationDlg::CAddFormationDlg(CWnd* pParent )
	: CDialog(CAddFormationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddFormationDlg)
	m_strFormationName = _T("");
	//}}AFX_DATA_INIT
}
*/


//##ModelId=3C0DC1610187
CAddFormationDlg::CAddFormationDlg(CFormationTopography* pFT,CWnd* pParent)
: CDialog(CAddFormationDlg::IDD, pParent)
{
	
	m_bFormationAdded=false;
	ASSERT(pFT);
	m_pFT = pFT;
	
	// KAN NIET
	//SetUnitlabel(IDC_lbUBottomTVD,	pFT->GetFormationAt(0)->BottomTVD());
	//ValidateQuantity(pDX,&m_debBottomTVD,pFT->GetFormationAt(0)->BottomTVD(),	CString("BottomTVD"));

	m_strFormationName = _T("");
}

//##ModelId=3C0DC16101A6
void CAddFormationDlg::DoDataExchange(CDataExchange* pDX)
{
	
	

	//{{AFX_DATA_MAP(CAddFormationDlg)
	DDX_Control(pDX, IDC_ebBottomTVD, m_debBottomTVD);
	DDX_Text(pDX, IDC_ebFormationName, m_strFormationName);
	DDV_MaxChars(pDX, m_strFormationName, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddFormationDlg, CDialog)
	//{{AFX_MSG_MAP(CAddFormationDlg)
	ON_EN_KILLFOCUS(IDC_ebBottomTVD, OnKillfocusBottomTVD)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddFormationDlg message handlers

//##ModelId=3C0DC16101B5
void CAddFormationDlg::OnKillfocusBottomTVD() 
{
	UpdateData();
	
}

//##ModelId=3C0DC16101B7
void CAddFormationDlg::OnOK() 
{
	UpdateData();
	if(m_debBottomTVD.IsEmpty())
		return;
	
	CString strError;
	CLengthQuantity temp;
	temp.Value(m_debBottomTVD.GetValue(),GetDoc()->UnitSystem());
	m_bFormationAdded=m_pFT->AddFormation(m_strFormationName,temp.Value(), strError);

	if(!m_bFormationAdded)
	{
		MessageBox(strError);
		if (m_strFormationName.IsEmpty())
		{
			((CDoubleEdit*)GetDlgItem(IDC_ebFormationName))->SetFocus();
		}
		else
		{
			((CDoubleEdit*)GetDlgItem(IDC_ebBottomTVD))->SetFocus();
		}
		return;
	}
	CDialog::OnOK();
}

//##ModelId=3C0DC1610186
BOOL CAddFormationDlg::IsFormationAdded()
{
	return m_bFormationAdded;
}

//##ModelId=3C2044080158
BOOL CAddFormationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CLengthQuantity temp;
	CStatic *lb=(CStatic*)GetDlgItem(IDC_lbUBottomTVD);
	SetUnitlabel(lb,&temp);
	
	((CDoubleEdit*)GetDlgItem(IDC_ebBottomTVD))->SetFocus();

	return FALSE; //TRUE;	// return TRUE unless you set the focus to a control
							// EXCEPTION: OCX Property Pages should return FALSE
}
