// AddAnalysisPointDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "CAnalysis.h"
#include "DoubleEdit.h"
#include "AddAnalysisPointDlg.h"
#include "CDataModel.h"
#include "DSealectGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddAnalysisPointDlg dialog


//##ModelId=3C20440802CF
CAddAnalysisPointDlg::CAddAnalysisPointDlg(CAnalysis* pAnalysis,CWnd* pParent /*=NULL*/)
	: CDialog(CAddAnalysisPointDlg::IDD, pParent)
	
{
	m_pAnalysis=pAnalysis;
	m_bAnalysisPointAdded=false;
	//{{AFX_DATA_INIT(CAddAnalysisPointDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


//##ModelId=3C20440802FE
void CAddAnalysisPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddAnalysisPointDlg)
	DDX_Control(pDX, IDC_debAddAnalysisPointTVD, m_debTVD);
	DDX_Control(pDX, IDC_debAddAnalysisPointTMD, m_debTMD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddAnalysisPointDlg, CDialog)
	//{{AFX_MSG_MAP(CAddAnalysisPointDlg)
	ON_EN_CHANGE(IDC_debAddAnalysisPointTMD, OnChangedebAddAnalysisPointTMD)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddAnalysisPointDlg message handlers

//##ModelId=3C2044080301
void CAddAnalysisPointDlg::OnOK() 
{
	UpdateData();

	if(m_debTMD.IsEmpty())
		return;
	
	CString strError;
	CLengthQuantity tempLQ;
	tempLQ.Value(m_debTMD.GetValue(),GetDoc()->UnitSystem());

	//TODO check boundarys
	m_bAnalysisPointAdded= m_pAnalysis->AddAnalysisPoint(tempLQ.Value() ,strError);

	if(!m_bAnalysisPointAdded)
	{
		MessageBox(strError);
		((CDoubleEdit*)GetDlgItem(IDC_debAddAnalysisPointTMD))->SetFocus();
		return;
	}
	else
	{
		GetDoc()->SetModifiedFlag(TRUE);
		
	}

	CDialog::OnOK();
}

//##ModelId=3C204408031D
void CAddAnalysisPointDlg::OnCancel() 
{
	CDialog::OnCancel();
}

 
//##ModelId=3C204408031F
BOOL CAddAnalysisPointDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CLengthQuantity tempLQ;
	CStatic *lbUTMD=(CStatic*)GetDlgItem(IDC_lbUAddAnalysisPointTMD);
	SetUnitlabel(lbUTMD,&tempLQ);
	CStatic *lbUTVD=(CStatic*)GetDlgItem(IDC_lbUAddAnalysisPointTVD);
	SetUnitlabel(lbUTVD,&tempLQ);

	GetDlgItem(IDC_debAddAnalysisPointTVD)->EnableWindow(FALSE);
	CStatic *lbP= (CStatic*)GetDlgItem(IDC_lbAttention);
	CString strTop;
	strTop.Format("%#4.4f",GetDoc()->DSealectModel()->GetModelTopTMD().Value(GetDoc()->UnitSystem()));
	CString strBottom;

	//strBottom.Format("%#4.4f",GetDoc()->DSealectModel()->GetModelBottomTMD().Value(GetDoc()->UnitSystem()));
	strBottom.Format("%#4.4f",GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD()->Value(GetDoc()->UnitSystem()));

	lbP->SetWindowText(CString("Enter value between\n") + strTop + " and " + strBottom); 

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	((CDoubleEdit*)GetDlgItem(IDC_debAddAnalysisPointTMD))->SetFocus();

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_AAP_debAddAnalysisPointTMD);
		m_toolTip.AddTool( GetDlgItem(IDC_debAddAnalysisPointTMD), sTmp);
		GetDlgItem(IDC_debAddAnalysisPointTMD)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_AAP_debAddAnalysisPointTVD);
		m_toolTip.AddTool( GetDlgItem(IDC_debAddAnalysisPointTVD), sTmp);
		GetDlgItem(IDC_debAddAnalysisPointTVD)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_AAP_debAddAnalysisPointTMD
		//IDTT_AAP_debAddAnalysisPointTVD
		

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}

	return FALSE; //TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//##ModelId=3C2044080321
void CAddAnalysisPointDlg::OnChangedebAddAnalysisPointTMD() 
{
	//TODO check boundarys
		
	if(CDoubleEdit::IsNumericValue(m_debTMD.GetString()))
	{

		CLengthQuantity temp;
		temp.Value(m_debTMD.GetValue(),GetDoc()->UnitSystem());//temp = si
		
		if(temp.Value() >= GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD()->Value())
		{
			m_debTVD.MakeEmpty();
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			return;
		}

		if(temp.Value() <= GetDoc()->DSealectModel()->GetModelTopTMD().Value())
		{
			m_debTVD.MakeEmpty();
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			return;
		}

    std::pair<double, bool> prTVD = GetDoc()->DSealectModel()->TMDtoTVD(temp.Value());
    if(!prTVD.second)
    {
      m_debTVD.MakeEmpty();
      GetDlgItem(IDOK)->EnableWindow(FALSE);
      return;
    }

		temp.Value(prTVD.first, QU::SI_UNIT);
		m_debTVD.ShowValue(temp.Value(GetDoc()->UnitSystem()));
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		m_debTVD.MakeEmpty();
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}



//##ModelId=3DDA0C600150
BOOL CAddAnalysisPointDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(GetDoc()->m_bShowToolTip)
		m_toolTip.RelayEvent(pMsg);
  	   
	return CDialog::PreTranslateMessage(pMsg);
}
