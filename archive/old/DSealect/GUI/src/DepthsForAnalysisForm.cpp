// DepthsForAnalysisForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "treeobject.h"
#include "AnalysisPointList.h"
#include "DepthsForAnalysisForm.h"
#include "DSealectGlobal.h"
#include "AddAnalysisPointDlg.h"
#include "CDataModel.h"
#include "CAnalysis.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepthsForAnalysisForm

IMPLEMENT_DYNCREATE(CDepthsForAnalysisForm, CBaseFormView)


static BOOL AnalysisPointOk(CAnalysisPoint* point)
{
	return FALSE;
}


//##ModelId=3C0DC15E0000
CDepthsForAnalysisForm::CDepthsForAnalysisForm()
	: CBaseFormView(CDepthsForAnalysisForm::IDD)
{
	

	//{{AFX_DATA_INIT(CDepthsForAnalysisForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15E0012
CDepthsForAnalysisForm::~CDepthsForAnalysisForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC15E000F
void CDepthsForAnalysisForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDepthsForAnalysisForm)
	DDX_Control(pDX, IDC_lstAnalysisPoint, m_lstAnalysisPointList);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;

	if (!pDX->m_bSaveAndValidate)
	{
		//SetUnitlabel(IDC_lbUAnalysisPointTMD, GetModel()->Analysis()->AnalysisPoint(0)->TMD());
	}

	m_lstAnalysisPointList.Update();

	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
	UpdateButtons();
}


BEGIN_MESSAGE_MAP(CDepthsForAnalysisForm, CBaseFormView)
	//{{AFX_MSG_MAP(CDepthsForAnalysisForm)
	ON_BN_CLICKED(IDC_btnAddAnalysisPoint, OnbtnAddAnalysisPoint)
	ON_BN_CLICKED(IDC_btnRemoveAnalysisPoint, OnbtnRemoveAnalysisPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepthsForAnalysisForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15E001F
void CDepthsForAnalysisForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC15E0021
void CDepthsForAnalysisForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDepthsForAnalysisForm message handlers

//##ModelId=3C20440400AC
void CDepthsForAnalysisForm::OnbtnAddAnalysisPoint() 
{
	if(GetDoc()->ContinueAndLoseResultsWarning())
	{
		CAddAnalysisPointDlg dlg(GetModel()->Analysis());
		dlg.DoModal();
		UpdateData();
	}
	
}

//##ModelId=3C20440400CC
void CDepthsForAnalysisForm::OnbtnRemoveAnalysisPoint() 
{
	if(GetDoc()->ContinueAndLoseResultsWarning())
	{
		if(m_lstAnalysisPointList.GetSelectedAnalysisPoint())
		{
			GetModel()->Analysis()->RemoveAnalysisPoint(m_lstAnalysisPointList.GetSelectedAnalysisPoint());
			GetDocument()->SetModifiedFlag(TRUE);
			UpdateData();
		}
	}

}

//##ModelId=3C0DC15E0001
BOOL CDepthsForAnalysisForm::CheckComplete() const
{

	if(!TreeEntry()->Previous()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;

	if(GetModel()->Analysis()->NrOfAnalysisPoints()<1)
	{
		return FALSE;
	}

  double l = GetModel()->PolyLine().GetLength();
  for(int i = 0; i < GetModel()->Analysis()->NrOfAnalysisPoints(); ++i)
  {
    CAnalysisPoint* pPoint = GetModel()->Analysis()->GetAnalysisPointAt(i);
    if(pPoint->TMD()->Value() > l)
      return FALSE;

    if(pPoint->TMD()->Value() >=
       GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD()->Value())
      return FALSE;
  }
		
	return TRUE;
}

//##ModelId=3C60D57A008D
void CDepthsForAnalysisForm::OnInitialUpdate() 
{
	CBaseFormView::OnInitialUpdate();
	
	GetDlgItem(IDC_fraAnalysisPointTMD)->SetFont(GetFont(TRUE));

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_MORE_AnalysisPoint);
		m_toolTip.AddTool( GetDlgItem(IDC_fraAnalysisPointTMD), sTmp);
		GetDlgItem(IDC_fraAnalysisPointTMD)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		
		//IDC_fraAnalysisPointTMD
		//IDTT_DFA_fraAnalysisPointTMD

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
	
}

