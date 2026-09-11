// WellPointPickDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewWellPathBase.h"
#include "DDXQuantity.h"
#include "WellPointPickDlg.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellPointPickDlg dialog


// wjrx mantis 3564
//
CWellPointPickDlg::CWellPointPickDlg
( std::list<CNewWellPoint> *pList
, CNewWellPathBase& wellpath
, std::list<std::list<INewWellSection *> > Sections
, CDoubleQuantity::UNIT unit /*= CDoubleQuantity::SI_UNIT*/
, CWnd* pParent /*=NULL*/
)
: CDialog(CWellPointPickDlg::IDD, pParent)
, m_pNewWellPath(&wellpath)
, m_pNewOrgList(pList)
, m_WellPointListCtrl(wellpath)
{
	
	double max = wellpath.TMD(wellpath.WellPointSize()-1,unit);
	double min = wellpath.TMD(0,unit);

	m_pSingleValue  = new CLengthQuantity((max+min)/2.0,min,max,false,false,unit);
	m_pFrom         = new CLengthQuantity(min+0.1,min,max,false,false,unit);
	m_pTo           = new CLengthQuantity(max-0.1,min,max,false,false,unit);
	m_NrOfPoints    = 50; 

	//copy original list (for cancel)
	m_NewList = *m_pNewOrgList;

	m_NewSections   = Sections;
	m_pDepManager = new CDependenciesManager();
	m_pDepManager->SetUnit(unit);

	//{{AFX_DATA_INIT(CWellPointPickDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWellPointPickDlg::~CWellPointPickDlg()
{

	delete m_pDepManager;
	delete m_pSingleValue;
	delete m_pFrom;
	delete m_pTo;


}

void CWellPointPickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWellPointPickDlg)
	DDX_Control(pDX, UT_IDC_EDIT_RANGE_NROPTS, m_EditNrOfPoints);
	DDX_Control(pDX, UT_IDC_EDIT_RANGE_TO, m_EditRangeTo);
	DDX_Control(pDX, UT_IDC_EDIT_RANGE_FROM, m_EditRangeFrom);
	DDX_Control(pDX, UT_IDC_EDIT_SINGLE, m_EditSingle);
	//}}AFX_DATA_MAP

	DDX_Control(pDX, UT_IDC_LIST_DSBATCH_POINTS, m_WellPointListCtrl);

	DDV_Quantity(pDX, UT_IDC_EDIT_SINGLE, m_pSingleValue , m_pDepManager, "Single Value");
	DDV_Quantity(pDX, UT_IDC_EDIT_RANGE_FROM, m_pFrom,     m_pDepManager, "From");
	DDV_Quantity(pDX, UT_IDC_EDIT_RANGE_TO, m_pTo,         m_pDepManager, "To");

	//{{AFX_DATA_MAP(CMudWeightPtsDlg)
	DDX_Text(pDX, UT_IDC_EDIT_RANGE_NROPTS, m_NrOfPoints);
	//}}AFX_DATA_MAP

	CString name;

	CString strUnit = CString("[") + CString(m_pSingleValue->UnitName(Unit()).c_str()) + "]";
	DDX_Text(pDX, UT_IDC_STATIC_WP_NAME, name);
	DDX_Text(pDX, UT_IDC_STATIC_SINGLE_UNIT, strUnit);
	DDX_Text(pDX, UT_IDC_ST_RNGE_FR_UNIT, strUnit);
	DDX_Text(pDX, UT_IDC_ST_RNGE_TO_UNIT, strUnit);

	SetButtonState();

}


BEGIN_MESSAGE_MAP(CWellPointPickDlg, CDialog)
	//{{AFX_MSG_MAP(CWellPointPickDlg)
	ON_BN_CLICKED(UT_IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(UT_IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(UT_IDC_BUTTON_RECALCDEF, OnButtonAddDefault)
	ON_BN_CLICKED(UT_IDC_RADIO_RANGE, OnRadioRange)
	ON_BN_CLICKED(UT_IDC_RADIO_SINGLE_POINT, OnRadioSinglePoint)
	ON_NOTIFY(NM_CLICK, UT_IDC_LIST_DSBATCH_POINTS, OnClickList)
	ON_NOTIFY(LVN_ITEMCHANGING, UT_IDC_LIST_DSBATCH_POINTS, OnItemchangingList)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VALIDATE_DOUBLE_EDIT, OnValidateDoubleEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellPointPickDlg message handlers
void CWellPointPickDlg::OnButtonAdd() 
{
  CWaitCursor wait;

  UpdateData(TRUE);

  bool bOutOfBoundary=false;
  bool bDefined= true;

  if(m_pRadioSingle->GetCheck())
  {
    if(m_pSingleValue->Undefined())
    return;

    CNewWellPoint p = OnCreateWellPoint(*m_pNewWellPath,m_pSingleValue->Value(), bDefined);
    if ( bDefined )
    {
      m_NewList.push_back(p);
      m_NewList.sort(CNewWellPoint::less);
      m_NewList.unique(CNewWellPoint::equal);
    }
    else
      bOutOfBoundary= true;
  }
  else
  {
    double to   = m_pFrom->Value();
    double from = m_pTo->Value();

    double dInterval = (to - from) / (double)(m_NrOfPoints - 1);
    CNewWellPoint From = OnCreateWellPoint(*m_pNewWellPath,from, bDefined);

    if ( bDefined )
    {
      m_NewList.push_back(From);
    }
    else
      bOutOfBoundary= true;

    CNewWellPoint To = OnCreateWellPoint(*m_pNewWellPath,to, bDefined);
    if ( bDefined )
    {
      m_NewList.push_back(To);
    }
    else 
      bOutOfBoundary= true;

    for(int i = 1; i < m_NrOfPoints - 1; i++)
    {
      CNewWellPoint p = OnCreateWellPoint(*m_pNewWellPath,from+dInterval * double(i), bDefined);
      if ( bDefined ) 
      {
        m_NewList.push_back(p);
      }
      else
        bOutOfBoundary= true;
    }

    m_NewList.sort(CNewWellPoint::less);
    m_NewList.unique(CNewWellPoint::equal);
  }

  if(bOutOfBoundary)
    _m()->msg(OnOutOfBoundaryMessage(), MB_OK|MB_ICONINFORMATION, 0);

  m_WellPointListCtrl.UpdateList();
}

/*virtual*/ CString CWellPointPickDlg::OnOutOfBoundaryMessage()
{
	return "Some points are outside the boundary and are not added";
}

void CWellPointPickDlg::OnButtonRemove() 
{
	m_WellPointListCtrl.RemoveSelectedPoints(m_pNewWellPath!=0);
	SetButtonState();
}

void CWellPointPickDlg::OnButtonAddDefault() 
{
	OnAddDefault(m_NewList);
}

BOOL CWellPointPickDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rect;

	m_pRadioSingle = (CButton*)GetDlgItem(UT_IDC_RADIO_SINGLE_POINT);
	m_pRadioRange = (CButton*)GetDlgItem(UT_IDC_RADIO_RANGE);
	m_pReset = (CButton*)GetDlgItem(UT_IDC_BUTTON_RECALCDEF);

	m_pRadioSingle->SetCheck(1);
	m_EditRangeFrom.EnableWindow(FALSE);
	m_EditRangeTo.EnableWindow(FALSE);
	m_EditNrOfPoints.EnableWindow(FALSE);

	{ 
          // geen m_NewList.description
          // SetWindowText(m_NewList-?>GetDescription().toAscii().constData());
          m_WellPointListCtrl.OnInitList(m_NewList,m_NewSections,Unit());
        }

	m_WellPointListCtrl.UpdateList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


CDoubleQuantity::UNIT CWellPointPickDlg::Unit()
{
	return m_pDepManager->GetUnit(); 
}




void CWellPointPickDlg::SetButtonState()
{
	CButton* pRemoveButton = (CButton*)GetDlgItem(UT_IDC_BUTTON_REMOVE);
	POSITION pos = m_WellPointListCtrl.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		//nothing is selected
		pRemoveButton->EnableWindow(FALSE);
		return;
	}
	else
	{
		pRemoveButton->EnableWindow(TRUE);
	}


}

void CWellPointPickDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetButtonState();
	*pResult = 0;
}

void CWellPointPickDlg::OnItemchangingList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetButtonState();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
}

void CWellPointPickDlg::OnRadioRange() 
{
	m_EditRangeFrom.EnableWindow(TRUE);
	m_EditRangeTo.EnableWindow(TRUE);
	m_EditNrOfPoints.EnableWindow(TRUE);
	m_EditSingle.EnableWindow(FALSE);

}

void CWellPointPickDlg::OnRadioSinglePoint() 
{
	m_EditRangeFrom.EnableWindow(FALSE);
	m_EditRangeTo.EnableWindow(FALSE);
	m_EditNrOfPoints.EnableWindow(FALSE);
	m_EditSingle.EnableWindow(TRUE);
}


LRESULT CWellPointPickDlg::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{
   UpdateData();
   return TRUE;
}

CNewWellPoint CWellPointPickDlg::OnCreateWellPoint(CNewWellPathBase& wellpath, double tmd, bool &bDefined)
{
  bDefined= true;
  return CNewWellPoint(wellpath,tmd);
}

void CWellPointPickDlg::OnOK() 
{
  {
    *m_pNewOrgList= m_NewList;
    //m_pNewOrgList->sort(CNewWellPoint::less);
    //m_pNewOrgList->unique(CNewWellPoint::equal);
  }
  CDialog::OnOK();
}

void CWellPointPickDlg::OnCancel() 
{
	CDialog::OnCancel();
}
