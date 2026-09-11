// UnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "UnitDlg.h"
#include "DSealectGlobal.h"
#include "DSealectDoc.h"

#include <afxwin.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitDlg dialog


const char* strUnitNames[] =
{
	"SI",
	"US",
	"Field"
};


//##ModelId=3C0DC1560119
CUnitDlg::CUnitDlg(CWnd* pParent /*=NULL*/)
//: CDialog(CUnitDlg::IDD, pParent)
{

	//{{AFX_DATA_INIT(CUnitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C2043F901A6
BOOL CUnitDlg::Create(CWnd* pParentWnd, UINT nStyle)
{
// Associate the dialog template with the class:
	BOOL ret= CDialogBar::Create (pParentWnd,
                             IDD_DIALOGBAR_UNITS,
                              nStyle,
                              IDD_DIALOGBAR_UNITS);

	CComboBox *cb = (CComboBox*)GetDlgItem(IDC_COMBO_UNITS);

	m_UnitVec.push_back(QU::SI_USER_UNIT);
	m_UnitVec.push_back(QU::US_UNIT);
	m_UnitVec.push_back(QU::FIELD_UNIT);

	cb->Clear();
	cb->AddString(strUnitNames[0]);
	cb->AddString(strUnitNames[1]);
	cb->AddString(strUnitNames[2]);
	
	cb->SetCurSel(GetDoc()->GetIndexUnitCombo());
	return ret;
} 


BEGIN_MESSAGE_MAP(CUnitDlg, CDialogBar)
	//{{AFX_MSG_MAP(CUnitDlg)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_UNITS, OnSelchangeComboUnits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitDlg message handlers



//##ModelId=3C2043F901D5
int CUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


//##ModelId=3C2043F90223
void CUnitDlg::OnSelchangeComboUnits() 
{
	POSITION pos= AfxGetApp()->GetFirstDocTemplatePosition();

	if(pos)
	{
		CDocTemplate* dt = AfxGetApp()->GetNextDocTemplate(pos);
		POSITION pos2 = dt->GetFirstDocPosition();
		if(pos2)
		{
			
			CDSealectDoc *pDoc= (CDSealectDoc*)dt->GetNextDoc(pos2);
			CComboBox *cb = (CComboBox*)GetDlgItem(IDC_COMBO_UNITS);
			pDoc->SetUnitSystem((QU::UNIT)m_UnitVec[cb->GetCurSel()]);
		}
	}
	//MessageBox("OnSelchangeComboUnits");
}
