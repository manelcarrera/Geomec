// exportdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "exportdlg.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog


CExportDlg::CExportDlg(IExportFormat::TAxisSystem axis_system, CWnd* pParent /*=NULL*/)
	: CDialog(CExportDlg::IDD, pParent), m_axis_system(axis_system)
{
	//{{AFX_DATA_INIT(CExportDlg)
	//}}AFX_DATA_INIT
}


void CExportDlg::DoDataExchange(CDataExchange* pDX)
{
	int nXYZ = 0;

	CDialog::DoDataExchange(pDX);

	if(!pDX->m_bSaveAndValidate)
	{
		if(m_axis_system == IExportFormat::AXIS2D)
			nXYZ = 1;
	}
	//{{AFX_DATA_MAP(CExportDlg)
	DDX_Radio(pDX, IDC_RB_XYZ, nXYZ);
	//}}AFX_DATA_MAP

	if(pDX->m_bSaveAndValidate)
	{
		if(nXYZ == 1)
			m_axis_system = IExportFormat::AXIS2D;
		else
			m_axis_system = IExportFormat::AXIS3D;

	}
}


BEGIN_MESSAGE_MAP(CExportDlg, CDialog)
	//{{AFX_MSG_MAP(CExportDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportDlg message handlers
IExportFormat::TAxisSystem CExportDlg::AxisSystem() const
{
	return m_axis_system;
}
