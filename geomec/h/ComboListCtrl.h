#ifndef _COMBOLISTCTRL_H_
#define _COMBOLISTCTRL_H_

#include <afxtempl.h>
#include <list>

class CInPlaceCombo;

#include "ListCtrlBase.h"

class CComboListCtrl : public CListCtrlBase
{
public:
  // Constructor
  CComboListCtrl();

  // Destructor
  virtual ~CComboListCtrl();

  // Sets/Resets the column which support the in place combo box
  void SetComboColumns(int iColumnIndex, bool bSet = true);

protected:
  afx_msg void OnHScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar* pScrollBar);
  afx_msg void OnVScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar* pScrollBar);
  afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
//	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

  DECLARE_MESSAGE_MAP()

  virtual void OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList& lstItems) = 0;

private:
  // Returns the row & column index of the column on which mouse click event has occured
//	bool HitTestEx(CPoint& rHitPoint, int* pRowIndex, int* pColumnIndex) const;

  // Creates and displays the in place combo box
  CInPlaceCombo* ShowInPlaceList(int iRowIndex, int iColumnIndex, CStringList& rComboItemsList, 
                   CString strCurSelecetion = "", int iSel = -1);

  // Calculates the cell rect
  void CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect);

  // Checks whether column supports in place combo box
  bool IsCombo(int iColumnIndex);

  // Scrolls the list ctrl to bring the in place ctrl to the view
  void ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& obCellRect);

// Attributes
  
  // List of columns that support the in place combo box
  CList<int, int> m_ComboSupportColumnsList;

  // The window style of the in place combo ctrl
  DWORD m_dwDropDownCtrlStyle;
};

#endif // _COMBOLISTCTRL_H_
