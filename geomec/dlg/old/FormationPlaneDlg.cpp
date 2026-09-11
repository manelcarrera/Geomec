// FormationPlaneDlg.cpp : implementation file
//

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "FormationPlaneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormationPlaneDlg dialog


CFormationPlaneDlg::CFormationPlaneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFormationPlaneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFormationPlaneDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFormationPlaneDlg::DoDataExchange(CDataExchange* pDX)
{
//	if(!pDX->m_bSaveAndValidate)
//		m_nTop = 0;

	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CFormationPlaneDlg)
	DDX_Check(pDX, IDC_CHK_TOP, m_nTop);
	DDX_Check(pDX, IDC_CHK_MIDDLE, m_nMiddle);
	DDX_Check(pDX, IDC_CHK_BOTTOM, m_nBottom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormationPlaneDlg, CDialog)
	//{{AFX_MSG_MAP(CFormationPlaneDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormationPlaneDlg message handlers

int CFormationPlaneDlg::GetPlaneLevel(int level)
{
	switch(level)
	{
	case 0: return m_nTop;
	case 1: return m_nMiddle;
	case 2: return m_nBottom;
	}
	return -1;
}

void CFormationPlaneDlg::SetPlane(int level, int state)
{
	switch(level)
	{
	case 0: m_nTop = state; break;
	case 1: m_nMiddle = state; break;
	case 2: m_nBottom = state; break;
	}
}
