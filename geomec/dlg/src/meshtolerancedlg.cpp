// meshtolerancedlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "meshtolerancedlg.h"
#include "tetramesh.h"
//#include "tetmeshbase.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeshToleranceDlg dialog


CMeshToleranceDlg::CMeshToleranceDlg(geo::CTetMeshBase &mesh, CWnd* pParent /*=NULL*/)
	: CDialog(CMeshToleranceDlg::IDD, pParent), m_mesh(mesh)
{
	//{{AFX_DATA_INIT(CMeshToleranceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_dTolerance = m_mesh.Tolerance();
}


void CMeshToleranceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeshToleranceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	/*if(!pDX->m_bSaveAndValidate)
	{
		m_dTolerance
	}*/

	DDX_Text(pDX, IDC_EDIT_TOLERANCE, m_dTolerance);
	
}


BEGIN_MESSAGE_MAP(CMeshToleranceDlg, CDialog)
	//{{AFX_MSG_MAP(CMeshToleranceDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshToleranceDlg message handlers

void CMeshToleranceDlg::OnOK() 
{
	UpdateData(TRUE);
	if(m_dTolerance < 0.0)
	{
		_m()->msg("Please enter a positive number.");
		return;
	}

	m_mesh.Tolerance(m_dTolerance);
	
	CDialog::OnOK();

}
