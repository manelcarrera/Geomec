//{{AFX_INCLUDES()
#include "wellspreadsheet.h"
//}}AFX_INCLUDES
#if !defined(AFX_SPREADVIEW_H__E66C09F5_B876_4E0A_AEE5_6A69235A49BB__INCLUDED_)
#define AFX_SPREADVIEW_H__E66C09F5_B876_4E0A_AEE5_6A69235A49BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpreadView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpreadView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "resource.h"

class well::CWellPathBase;
class CWellEditDlg;

enum DEF_TYPE { TMD_INC, TMD_TVD };

class AFX_EXT_CLASS CSpreadView : public CFormView {
  friend class CWellEditDlg;

protected:
  CSpreadView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CSpreadView)

  // Form Data
public:
  //{{AFX_DATA(CSpreadView)
  enum { IDD = UT_IDD_FORMVIEW };
  // CSpreadSheet	m_SpreadSheet;
  // }}AFX_DATA

  // Attributes
public:
  void Init(well::CWellPathBase &, QU::UNIT us);
  void SetDefType(DEF_TYPE type, bool update = TRUE);
  void SetAzimuth(bool azimuth, bool update = TRUE);
  bool IsDirty();
  int GetNumberOfValidRows();
  void UpdateCells();
  // Operations
public:
  void UpdateEnabledCells();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSpreadView)
public:
  virtual void OnInitialUpdate();
  virtual BOOL OnCmdMsg(unsigned int nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  virtual ~CSpreadView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
  //{{AFX_MSG(CSpreadView)
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg void OnLeaveCell(long Col, long Row, long NewCol, long NewRow, BOOL FAR *Cancel);
  afx_msg void OnKeyDown(short FAR *KeyCode, short Shift);
  afx_msg void OnId3dvCopypicture();
  DECLARE_EVENTSINK_MAP()
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  virtual void OnLeaveRow(long Row, BOOL RowWasLast, BOOL RowChanged, BOOL AllCellsHaveData, long NewRow,
                          long NewRowIsLast, BOOL FAR *Cancel);
  virtual void OnChangeSpread(long Col, long Row);
  virtual void OnRightClickSpread(short ClickType, long Col, long Row, long MouseX, long MouseY);
  virtual void OnButtonClickedSpread(long Col, long Row, short ButtonDown);
  virtual void OnDblClickSpread(long Col, long Row);

  // internal funcks
  void ClearColumn(int ColNr, int StartRow = 1);
  void UpdateWellPath();
  void UpdateAzimuth();

  void ConvertTwipsToPixels(long &width, long &height);

  well::CWellPathBase *m_pWellpath;
  QU::UNIT m_Us;
  DEF_TYPE m_Type;
  bool m_Azimuth;
  bool m_CellChanged;
  bool m_Dirty;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPREADVIEW_H__E66C09F5_B876_4E0A_AEE5_6A69235A49BB__INCLUDED_)
