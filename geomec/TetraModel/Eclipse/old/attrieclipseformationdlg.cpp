// attrieclipseformationdlg.cpp : implementation file
//

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "attrieclipseformationdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriEclipseFormationDlg dialog


CAttriEclipseFormationDlg::CAttriEclipseFormationDlg(CEclipseFormation& formation, CWnd* pParent /*=NULL*/)
	: CAttributesTemplate<CEclipseFormation>(CAttriEclipseFormationDlg::IDD, formation, pParent)
{
	//{{AFX_DATA_INIT(CAttriEclipseFormationDlg)
	//}}AFX_DATA_INIT
}


void CAttriEclipseFormationDlg::DoDataExchange(CDataExchange* pDX)
{
	CAttributesTemplate<CEclipseFormation>::DoDataExchange(pDX);

	int nCellSize;
	CString strName;
	if(!pDX->m_bSaveAndValidate)
	{
		nCellSize = Copy().Reservoir().SizeX() * Copy().Reservoir().SizeY() * (Copy().Reservoir().UpperBoundary() - Copy().Reservoir().LowerBoundary() + 1);
		strName = Copy().Name();
	}

	//{{AFX_DATA_MAP(CAttriEclipseFormationDlg)

	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_ST_CELL_SIZE, nCellSize);
	DDX_Text(pDX, IDC_ED_NAME, strName);
	
	if(pDX->m_bSaveAndValidate)
	{
		Copy().Name(strName);
	}

}


BEGIN_MESSAGE_MAP(CAttriEclipseFormationDlg, CDialog)
	//{{AFX_MSG_MAP(CAttriEclipseFormationDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ST_COLOR, OnColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriEclipseFormationDlg message handlers

void CAttriEclipseFormationDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

void CAttriEclipseFormationDlg::OnColor() 
{
	Copy().Color(SelectColor(Copy().Color()));
	Invalidate();
}
