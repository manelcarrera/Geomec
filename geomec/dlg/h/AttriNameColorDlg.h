#ifndef _ATTRINAMECOLORDLG_H_
#define _ATTRINAMECOLORDLG_H_

#include "AttributeTempl.h"
#include "resource.h"

// T must be derived from CColorNode
template <class T>
class CAttriNameColorDlg : public CAttributesTemplate<T>
{
public:
  CAttriNameColorDlg(T& source, CWnd* pParent = 0);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnStColor();
  afx_msg void OnPaint();
  DECLARE_MESSAGE_MAP()
};

BEGIN_TEMPLATE_MESSAGE_MAP(CAttriNameColorDlg, T, CDialog)
  ON_BN_CLICKED(IDC_ST_COLOR, OnStColor)
  ON_WM_PAINT()
END_MESSAGE_MAP()

template <class T>
CAttriNameColorDlg<T>::CAttriNameColorDlg(T& source, CWnd* pParent)
: CAttributesTemplate<T>(IDD_ATTRI_NAME_COLOR, source, pParent)
{
}

template <class T>
void CAttriNameColorDlg<T>::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<T>::DoDataExchange(pDX);

  CString strName;
  if(!pDX->m_bSaveAndValidate)
  strName = Copy().Name().toStdString().c_str();

  DDX_Text(pDX, IDC_ED_NAME, strName);

  if(pDX->m_bSaveAndValidate)
  {
  if(strName.IsEmpty())
  {
      AfxMessageBox("Please enter a valid name");
      pDX->PrepareEditCtrl(IDC_ED_NAME);
      pDX->Fail();
  }

  Copy().Name((LPCSTR) strName);
  }
}

template <class T>
void CAttriNameColorDlg<T>::OnStColor()
{
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();
}

template <class T>
void CAttriNameColorDlg<T>::OnPaint()
{
  CPaintDC dc(this); 
  FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

#endif // _ATTRINAMECOLORDLG_H_
