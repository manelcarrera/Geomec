// AnalysisPointList.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "CAnalysis.h"
#include "DoubleEdit.h"
#include "DSealectGlobal.h"
#include "CAnalysisPoint.h"
#include "AnalysisPointList.h"
#include "CDataModel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnalysisPointList

//##ModelId=3C20440703AA
CAnalysisPointList::CAnalysisPointList(AnalysisPointOkFuncPtr pAnalysisPointOk, const IUserColumnInfo* pUserColumnInfo)
: m_pUserColumnInfo(pUserColumnInfo),
  m_iUserColumn(-1)
{
	m_pCurrentAnalysisPoint=0;
	m_pAnalysisPointOkFunc=pAnalysisPointOk;
}


//##ModelId=3C20440703AC
CAnalysisPointList::CAnalysisPointList(const IUserColumnInfo* pUserColumnInfo)
: m_pUserColumnInfo(pUserColumnInfo),
  m_iUserColumn(-1)
{
	m_pCurrentAnalysisPoint=0;
	m_pAnalysisPointOkFunc=0;
}

//##ModelId=3C20440703DD
CAnalysisPointList::~CAnalysisPointList()
{
  if(m_pUserColumnInfo)
    delete m_pUserColumnInfo;
}


BEGIN_MESSAGE_MAP(CAnalysisPointList, CListCtrl)
	//{{AFX_MSG_MAP(CAnalysisPointList)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalysisPointList message handlers


//##ModelId=3C20440703DC
void CAnalysisPointList::Update()
{
	if(!GetDoc()->DSealectModel()->IsPolyLineCreated())
		return;

	CAnalysis *pAnalysis = GetDoc()->DSealectModel()->Analysis();

	LV_COLUMN col;
	col.mask=LVCF_TEXT;
	CLengthQuantity temp; // temporary quantity only needed for getting the unit-string 
	CString strUnit;

	strUnit="TMD ";
	strUnit+= GetUnitString(&temp);
	col.pszText = (LPSTR)(LPCSTR)strUnit;
	SetColumn(1, &col);

	strUnit="TVD ";
	strUnit+= GetUnitString(&temp);
	col.pszText = (LPSTR)(LPCSTR)strUnit;
	SetColumn(2, &col);

  if(m_pUserColumnInfo)
  {
    assert(m_iUserColumn >= 0);
    strUnit = m_pUserColumnInfo->Title();
    col.pszText = (LPSTR)(LPCSTR)strUnit;
    SetColumn(m_iUserColumn, &col);
  }

	LV_ITEM lvItem;

	lvItem.mask= LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;

	
	DeleteAllItems();

	for(int i=0 ;i<pAnalysis->NrOfAnalysisPoints();i++)
	{
		CString sTmp;
		lvItem.iItem=i;

		lvItem.iSubItem=0;
		
		sTmp.Format("%i",i+1);

		lvItem.pszText= sTmp.GetBufferSetLength(sTmp.GetLength());
		InsertItem(&lvItem);

	  int iColumn = 1;

		sTmp=CDoubleEdit::Format(pAnalysis->GetAnalysisPointAt(i)->TMD()->Value(GetDoc()->UnitSystem()));
		SetItemText(i,iColumn++,sTmp.GetBufferSetLength(sTmp.GetLength()));

    std::pair<double, bool> prTVD = GetDoc()->DSealectModel()->TMDtoTVD(pAnalysis->GetAnalysisPointAt(i)->TMD()->Value());
    if(prTVD.second)
    {
		  CLengthQuantity temp;
		  temp.Value(prTVD.first, QU::SI_UNIT);
		  sTmp = CDoubleEdit::Format(temp.Value(GetDoc()->UnitSystem()));
    }
    else
    {
      sTmp = "<outside range>";
    }

		SetItemText(i,iColumn++,sTmp.GetBufferSetLength(sTmp.GetLength()));

		if(m_pAnalysisPointOkFunc)
		{
			BOOL bIsOk=m_pAnalysisPointOkFunc(pAnalysis->GetAnalysisPointAt(i));
			if(bIsOk)
			{
				sTmp="OK";
			}
			else
			{
				sTmp="Not OK";
			}
			SetItemText(i,iColumn++,sTmp.GetBufferSetLength(sTmp.GetLength()));
		}

    if(m_pUserColumnInfo)
    {
      sTmp = m_pUserColumnInfo->Text(*pAnalysis->GetAnalysisPointAt(i));
      SetItemText(i, iColumn++, sTmp.GetBufferSetLength(sTmp.GetLength()));
    }
	}
	
	if(pAnalysis->NrOfAnalysisPoints()>0)
	{
		int nItem = pAnalysis->GetAnalysisPointIndex(m_pCurrentAnalysisPoint);
		if(nItem<0 ||nItem > pAnalysis->NrOfAnalysisPoints()-1)
		{
			m_pCurrentAnalysisPoint=pAnalysis->GetAnalysisPointAt(0);
			nItem=0;
		}
		SetItem(nItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
	}

	Invalidate();
}


//##ModelId=3C20440703DF
void CAnalysisPointList::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	TRACE( "CAnalysisPointList::OnClick\n");
	TRACE( "item:      %i \n" ,pNMListView->iItem);
	TRACE( "old state: %i \n" ,pNMListView->uOldState);
	TRACE( "new state: %i \n" ,pNMListView->uNewState);
	TRACE( "changed  : %i \n" ,pNMListView->uChanged);
	TRACE( "LVIS_SELECTED: %i \n" ,LVIS_SELECTED);
	TRACE("\n\n");
	
	int nItem = pNMListView->iItem;


	if (nItem<0)
	{
		m_pCurrentAnalysisPoint	=0;
		GetParent()->UpdateData(FALSE);
	    return ;
	}

	m_pCurrentAnalysisPoint= GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointAt(nItem);

	GetParent()->UpdateData(FALSE);

	*pResult = 0;
}

//##ModelId=3C2044080003
int CAnalysisPointList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}

//##ModelId=3C20440703CC
BOOL CAnalysisPointList::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CListCtrl::PreCreateWindow(cs);

}

//##ModelId=3C20440703BA
BOOL CAnalysisPointList::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

//##ModelId=3C20440703D9
void CAnalysisPointList::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CreateColums();

}


//##ModelId=3C2044080010
void CAnalysisPointList::CreateColums()
{
	int nColumnCount = GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
	   DeleteColumn(0);
	}

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvColumn.fmt= LVCFMT_LEFT;
	CLengthQuantity temp; // temporary quantity only needed for getting the unit-string 
	CString strUnit;

  int iSubItem = 0;
	
	lvColumn.iSubItem=iSubItem;
	lvColumn.cx=40;
	lvColumn.pszText="Point";
	InsertColumn(iSubItem++, &lvColumn);
	
	lvColumn.iSubItem=iSubItem;
	lvColumn.cx=70;
	strUnit="TMD ";
	strUnit+= GetUnitString(&temp);
	lvColumn.pszText=(LPSTR)(LPCSTR)strUnit;
	InsertColumn(iSubItem++, &lvColumn);

	lvColumn.iSubItem=iSubItem;
	lvColumn.cx=70;
	strUnit="TVD ";
	strUnit+= GetUnitString(&temp);
	lvColumn.pszText=(LPSTR)(LPCSTR)strUnit;
	InsertColumn(iSubItem++, &lvColumn);
	
	if(m_pAnalysisPointOkFunc)
	{
		lvColumn.iSubItem=iSubItem;
		lvColumn.pszText="Valid";
    lvColumn.cx = 40;
		InsertColumn(iSubItem++, &lvColumn);
	}

  if(m_pUserColumnInfo)
  {
    lvColumn.iSubItem = iSubItem;
    CString str = m_pUserColumnInfo->Title();
    lvColumn.pszText = (LPSTR)(LPCSTR)str;
    lvColumn.cx = 70;
    m_iUserColumn = iSubItem++;
    InsertColumn(m_iUserColumn, &lvColumn);
  }
}

//##ModelId=3C20440703DB
CAnalysisPoint* CAnalysisPointList::GetSelectedAnalysisPoint()
{

	
	int Index=GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointIndex(m_pCurrentAnalysisPoint);

	if(Index <0)
	{
		if(GetDoc()->DSealectModel()->Analysis()->NrOfAnalysisPoints()>0)
		{
			m_pCurrentAnalysisPoint= GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointAt(0);
			Index=0;
		}
		else
		{
			m_pCurrentAnalysisPoint=0;
		}
	}

	if(m_pCurrentAnalysisPoint)
	{
		SetItem(Index, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
	}

	return m_pCurrentAnalysisPoint;
}
