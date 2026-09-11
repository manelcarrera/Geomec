#pragma once
#include "afxcmn.h"

#define WM_SCALE_VECTORS    (WM_USER + 0x0400)
#define WM_DECIMATE_VECTORS (WM_USER + 0x0401)

// CVectorDialogBar 
// http://support.microsoft.com/kb/185672

class CVectorDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CVectorDialogBar)

public:
	CVectorDialogBar();   // standard constructor
	virtual ~CVectorDialogBar();

// Dialog Data
	enum { IDD = IDD_VECTOR_DIALOGBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CSliderCtrl m_scaleSlider;
  CSliderCtrl m_decimateSlider;

  afx_msg LRESULT OnInitDialog (WPARAM, LPARAM);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

