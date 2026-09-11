// MatLibListCtrl.h: interface for the CMatLibListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MATLIBLISTCTRL_H_
#define _MATLIBLISTCTRL_H_

#include <set>

class CTabModel;

void AdjustColumnWidth ( CWnd *wnd , CListCtrl *pListCtrl);
int GetColumnCount( const CListCtrl *pListCtrl);

// This class including its implementation was taken from codeguru.com. Original author (apart from some tweaks ;)
// is Zafir Anjum.
class CMatLibListCtrl : public CListCtrl
{
  typedef std::set<CString> TNameSet;

public:
  CMatLibListCtrl(const CTabModel* tabModel);
  virtual ~CMatLibListCtrl();

  int	HitTestEx(CPoint &point,int *col) const;
  CEdit*	EditSubLabel(int nItem,int nCol);
  virtual void OnHScroll(unsigned int nSBCode,unsigned int nPos,CScrollBar* pScrollBar);
  virtual void OnVScroll(unsigned int nSBCode,unsigned int nPos,CScrollBar* pScrollBar);
  void OnEndLabelEdit(NMHDR* pNMHDR,LRESULT* pResult);
//	virtual void OnLButtonDown(unsigned int nFlags,CPoint point);

  void SetLockedParameterNames(const TNameSet& stLockedParameterNames);
  bool IsLockedParameterName(const CString& strParamName) const;

protected:
  //{{AFX_MSG(CMatLibListCtrl)
  afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  TNameSet m_stLockedParameterNames;
  const CTabModel* m_tabModel;
  TCHAR* m_toolTipA;
  WCHAR* m_toolTipB;

  afx_msg BOOL OnToolNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

  virtual void PreSubclassWindow();

  void CellHitTest(const CPoint& pt, int& nRow, int& nCol) const;
  void IconHitTest(const CPoint& pt, int& nRow, int& nCol) const;
  bool ShowToolTip(const CPoint& pt) const;
  CString GetToolTipText(int nRow, int nCol);
  virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
};

#endif // _MATLIBLISTCTRL_H_
