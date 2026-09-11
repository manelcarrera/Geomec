// MatLibListCtrl.cpp: implementation of the CMatLibListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MatLibListCtrl.h"
#include "InPlaceEdit.h"
#include "TabModel.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

/// \brief adjust the column width after filling the fields
/// < a href="http://www.codeproject.com/Articles/11483/Let-Your-ListCtrl-Adjust-its-Column-Widths-Automat">codeproject.com example</a>
void AdjustColumnWidth
( CWnd *wnd //!< Cwnd that contains the CListCtrl
, CListCtrl *pListCtrl //!< CListCtrl on which to adjust columns
)
{
  wnd->SetRedraw(FALSE);
  int nColumnCount = GetColumnCount(pListCtrl);

  for (int i = 0; i < nColumnCount; i++)
  {
  pListCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE);
  int nColumnWidth = pListCtrl->GetColumnWidth(i);
  pListCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
  int nHeaderWidth = pListCtrl->GetColumnWidth(i); 
  int width= nColumnWidth>nHeaderWidth?nColumnWidth:nHeaderWidth;
  pListCtrl->SetColumnWidth(i, width);
  }
  wnd->SetRedraw(TRUE);
} 

/// \brief return the number of columns in the table
/// \return the number of columns in the table
//
int GetColumnCount
( const CListCtrl *pListCtrl //!< CListCtrl to count
)
{
          CHeaderCtrl* pHeaderCtrl = pListCtrl->GetHeaderCtrl();
          return (pHeaderCtrl->GetItemCount());
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatLibListCtrl::CMatLibListCtrl(const CTabModel* tabModel)
: m_tabModel(tabModel)
, m_toolTipA(0)
, m_toolTipB(0)
{
}

CMatLibListCtrl::~CMatLibListCtrl()
{
  if (m_toolTipA != 0)
  {
  delete m_toolTipA;
  }

  if (m_toolTipB != 0)
  {
  delete m_toolTipB;
  }
}

BEGIN_MESSAGE_MAP(CMatLibListCtrl, CListCtrl)
  //{{AFX_MSG_MAP(CTabModel)
  ON_WM_LBUTTONDOWN()
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolNeedText)
  ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolNeedText)
END_MESSAGE_MAP()


// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int CMatLibListCtrl::HitTestEx(CPoint &point, int *col) const
{
  int colnum = 0;
  int row = HitTest( point, NULL );
  
  if( col ) *col = 0;

  // Make sure that the ListView is in LVS_REPORT
  if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
    return row;

  // Get the top and bottom row visible
  row = GetTopIndex();
  int bottom = row + GetCountPerPage();
  if( bottom > GetItemCount() )
    bottom = GetItemCount();
  
  // Get the number of columns
  CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
  int nColumnCount = pHeader->GetItemCount();

  // Loop through the visible rows
  for( ;row <=bottom;row++)
  {
    // Get bounding rect of item and check whether point falls in it.
    CRect rect;
    GetItemRect( row, &rect, LVIR_BOUNDS );
    if( rect.PtInRect(point) )
    {
      // Now find the column
      for( colnum = 0; colnum < nColumnCount; colnum++ )
      {
        int colwidth = GetColumnWidth(colnum);
        if( point.x >= rect.left 
          && point.x <= (rect.left + colwidth ) )
        {
          if( col ) *col = colnum;

          if (colnum == CTabModel::VALUE_COLUMN)
          {
      int nRow, nCol;

      IconHitTest(point, nRow, nCol);

      if ((nRow != -1) && (nCol != -1))
      {
              return -1;
      }
          }

          return row;
        }
        rect.left += colwidth;
      }
    }
  }
  return -1;
}

// EditSubLabel		- Start edit of a sub item label
// Returns		- Temporary pointer to the new edit control
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.
CEdit* CMatLibListCtrl::EditSubLabel( int nItem, int nCol )
{
  // The returned pointer should not be saved

  // Make sure that the item is visible
  if( !EnsureVisible( nItem, TRUE ) ) return NULL;

  // Make sure that nCol is valid
  CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
  int nColumnCount = pHeader->GetItemCount();
  if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
    return NULL;

  // Get the column offset
  int offset = 0;
  for( int i = 0; i < nCol; i++ )
    offset += GetColumnWidth( i );

  CRect rect;
  GetItemRect( nItem, &rect, LVIR_BOUNDS );

  // Now scroll if we need to expose the column
  CRect rcClient;
  GetClientRect( &rcClient );
  if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
  {
    CSize size;
    size.cx = offset + rect.left;
    size.cy = 0;
    Scroll( size );
    rect.left -= size.cx;
  }

  // Get Column alignment
  LV_COLUMN lvcol;
  lvcol.mask = LVCF_FMT;
  GetColumn( nCol, &lvcol );
  DWORD dwStyle ;
  if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
    dwStyle = ES_LEFT;
  else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
    dwStyle = ES_RIGHT;
  else dwStyle = ES_CENTER;

  rect.left += offset+4;
  rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
  if( rect.right > rcClient.right) rect.right = rcClient.right;

  dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
  CEdit *pEdit = new CInPlaceEdit(nItem, nCol, GetItemText( nItem, nCol ));
  pEdit->Create( dwStyle, rect, this, IDC_IPEDIT );


  return pEdit;
}

void CMatLibListCtrl::OnHScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar* pScrollBar)
{
  if( GetFocus() != this ) SetFocus();
  CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMatLibListCtrl::OnVScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar* pScrollBar)
{
  if( GetFocus() != this ) SetFocus();
  CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMatLibListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
  LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
  LV_ITEM	*plvItem = &plvDispInfo->item;

  if (plvItem->pszText != NULL)
  {
    SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
  }
  *pResult = FALSE;
}

void CMatLibListCtrl::OnLButtonDown(unsigned int nFlags, CPoint point)
{
  int index;
  CListCtrl::OnLButtonDown(nFlags, point);

  int colnum;
  if( ( index = HitTestEx( point, &colnum )) != -1 )
  {
    if(colnum == 0 || colnum == 1 || colnum == 3)
      return;

  CString strName = GetItemText(index, 1);
  if(IsLockedParameterName(strName))
      return;

    unsigned int flag = LVIS_FOCUSED;
    if( (GetItemState( index, flag ) & flag) == flag && colnum > 0)
    {
      // Add check for LVS_EDITLABELS
      if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
        EditSubLabel( index, colnum );
    }
    else
      SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED ,
            	LVIS_SELECTED | LVIS_FOCUSED); 
  }
  else
  {
  IconHitTest(point, index, colnum);

  if ((index != -1) && (colnum != -1) && (colnum == CTabModel::VALUE_COLUMN))
  {
      if (!(m_tabModel->getMaterialParameterError())[index].isEmpty())
      {
    _m()->msg((m_tabModel->getMaterialParameterError())[index],
          MB_OK | MB_ICONEXCLAMATION);
      }
  }
  }
}

void CMatLibListCtrl::SetLockedParameterNames(const TNameSet& stLockedParameterNames)
{
  m_stLockedParameterNames = stLockedParameterNames;
}

bool CMatLibListCtrl::IsLockedParameterName(const CString& strParamName) const
{
  return m_stLockedParameterNames.find(strParamName) != m_stLockedParameterNames.end();
}

// private

void CMatLibListCtrl::CellHitTest(const CPoint& pt, int& nRow, int& nCol) const
{
  nRow = -1;
  nCol = -1;

  LVHITTESTINFO lvhti = { 0 };
  lvhti.pt = pt;
  nRow = ListView_SubItemHitTest(m_hWnd, &lvhti);	// SubItemHitTest is non-const
  nCol = lvhti.iSubItem;
  if (!(lvhti.flags & LVHT_ONITEM))
  nRow = -1;
}

void CMatLibListCtrl::IconHitTest(const CPoint& pt, int& nRow, int& nCol) const
{
  nRow = -1;
  nCol = -1;

  LVHITTESTINFO lvhti = { 0 };
  lvhti.pt = pt;
  nRow = ListView_SubItemHitTest(m_hWnd, &lvhti);	// SubItemHitTest is non-const
  nCol = lvhti.iSubItem;
  if (!(lvhti.flags & LVHT_ONITEMICON))
  nRow = -1;
}

BOOL CMatLibListCtrl::OnToolNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
  CPoint pt(GetMessagePos());
  ScreenToClient(&pt);

  int nRow, nCol;
  CellHitTest(pt, nRow, nCol);

  CString tooltip = GetToolTipText(nRow, nCol);
  if (tooltip.IsEmpty())
  return FALSE;

  CToolTipCtrl* pToolTip = AfxGetModuleThreadState()->m_pToolTip;

  if (pToolTip)
  {
  pToolTip->SetMaxTipWidth(SHRT_MAX);
  }

  // Non-unicode applications can receive requests for tooltip-text in unicode
  TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
  TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
#ifndef _UNICODE
  if (pNMHDR->code == TTN_NEEDTEXTA)
  {
  if (m_toolTipA != 0)
  {
      delete m_toolTipA;
  }

  m_toolTipA = new TCHAR[tooltip.GetLength() + 1];
  lstrcpyn(m_toolTipA, tooltip, tooltip.GetLength());
  m_toolTipA[tooltip.GetLength()] = 0;
  pTTTW->lpszText = (WCHAR*) m_toolTipA;
  }
  else
  {
  if (m_toolTipB != 0)
  {
      delete m_toolTipB;
  }

  m_toolTipB = new WCHAR[tooltip.GetLength() + 1];
  mbstowcs(m_toolTipB, tooltip, tooltip.GetLength());
  m_toolTipB[tooltip.GetLength()] = 0;
  pTTTW->lpszText = (WCHAR*) m_toolTipB;
  }
#else
  if (pNMHDR->code == TTN_NEEDTEXTA)
  {
  if (m_toolTipA != 0)
  {
      delete m_toolTipA;
  }

  m_toolTipA = new TCHAR[tooltip.GetLength() + 1];
  _wcstombsz(m_toolTipA, tooltip, tooltip.GetLength());
  m_toolTipA[tooltip.GetLength()] = 0;
  pTTTW->lpszText = (LPTSTR) m_toolTipA;
  }
  else
  {
  if (m_toolTipB != 0)
  {
      delete m_toolTipB;
  }

  m_toolTipB = new WCHAR[tooltip.GetLength() + 1];
  lstrcpyn(m_toolTipB, tooltip, tooltip.GetLength());
  m_toolTipB[tooltip.GetLength()] = 0;
  pTTTW->lpszText = (LPTSTR) m_toolTipB;
  }
#endif
  // If wanting to display a tooltip which is longer than 80 characters,
  // then one must allocate the needed text-buffer instead of using szText,
  // and point the TOOLTIPTEXT::lpszText to this text-buffer.
  // When doing this, then one is required to release this text-buffer again
  return TRUE;
}

bool CMatLibListCtrl::ShowToolTip(const CPoint& pt) const
{
  // Lookup up the cell
  int nRow, nCol;
  CellHitTest(pt, nRow, nCol);

  if (nRow != -1 && nCol != -1)
  return true;
  else
  return false;
}

CString CMatLibListCtrl::GetToolTipText(int nRow, int nCol)
{
  if(nRow != -1 && nCol != -1 && (nCol == CTabModel::VALUE_COLUMN))
  {
  QString toolTip = (m_tabModel->getMaterialParameterError())[nRow];

  return CString(toolTip.toStdString().c_str());
  }
  else
#ifdef _DEBUG
  if (nRow != -1 && nCol != -1)
  return GetItemText(nRow, nCol);	// Cell-ToolTip
  else
#endif  // _DEBUG
  return CString("");
}

void CMatLibListCtrl::PreSubclassWindow()
{
  CListCtrl::PreSubclassWindow();

//  SetExtendedStyle(LVS_EX_INFOTIP | GetExtendedStyle());

  // Disable the CToolTipCtrl of CListCtrl so it won't disturb the CWnd tooltip
  GetToolTips()->Activate(FALSE);

  // Activates the standard CWnd tooltip functionality
  VERIFY(EnableToolTips(TRUE));
}

INT_PTR CMatLibListCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
  CPoint pt(GetMessagePos());
  ScreenToClient(&pt);
  if (!ShowToolTip(pt))
  return -1;

  int nRow, nCol;
  CellHitTest(pt, nRow, nCol);

  //Get the client (area occupied by this control
  RECT rcClient;
  GetClientRect(&rcClient);

  //Fill in the TOOLINFO structure
  pTI->hwnd = m_hWnd;
  pTI->uId = (UINT)(nRow * 1000 + nCol);
  pTI->lpszText = LPSTR_TEXTCALLBACK;	// Send TTN_NEEDTEXT when tooltip should
                                      // be shown
  pTI->rect = rcClient;

  return pTI->uId; // Must return a unique value for each cell
                   // (Marks a new tooltip)
}
