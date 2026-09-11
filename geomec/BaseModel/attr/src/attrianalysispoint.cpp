// attrianalysispoint.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "attrianalysispoint.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriAnalysisPoint dialog


CAttriAnalysisPoint::CAttriAnalysisPoint(CAnalysisPoint& point, CWnd* pParent /*=NULL*/)
: CGeomecAttriTemp<CAnalysisPoint>(CAttriAnalysisPoint::IDD, point, pParent)
{
	//{{AFX_DATA_INIT(CAttriAnalysisPoint)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAttriAnalysisPoint::DoDataExchange(CDataExchange* pDX)
{
	CString sName;
	geo::CPoint point(0, 0, 0);

	CGeomecAttriTemp<CAnalysisPoint>::DoDataExchange(pDX);

	if(!pDX->m_bSaveAndValidate)
	{
		sName = Copy().Name().toStdString().c_str();
		point = Copy().Point();
	}

	//{{AFX_DATA_MAP(CAttriAnalysisPoint)
	DDX_Point(pDX, IDC_ED_NORTH, IDC_ED_EAST, IDC_ED_DEPTH, point, IDC_ST_UNIT1, IDC_ST_UNIT2, IDC_ST_UNIT3);
	DDX_Text(pDX, IDC_EB_NAME, sName);
	//}}AFX_DATA_MAP

	if(pDX->m_bSaveAndValidate)
	{
		Copy().Name((LPCSTR) sName);
		Copy().Point(point);
	}
}


void CAttriAnalysisPoint::OnColor() 
{
	Copy().Color(SelectColor(Copy().Color()));
	Invalidate();
}

void CAttriAnalysisPoint::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

BEGIN_MESSAGE_MAP(CAttriAnalysisPoint, CDialog)
	//{{AFX_MSG_MAP(CAttriAnalysisPoint)
	ON_BN_CLICKED(IDC_ST_COLOR, OnColor)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriAnalysisPoint message handlers
