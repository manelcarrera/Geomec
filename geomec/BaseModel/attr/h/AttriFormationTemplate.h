// AttriFormationTemplate.h: interface for the CAttriFormationTemplate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTRIFORMATIONTEMPLATE_H__B3921941_0C77_4C09_967A_9081ACE8B0AB__INCLUDED_)
#define AFX_ATTRIFORMATIONTEMPLATE_H__B3921941_0C77_4C09_967A_9081ACE8B0AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AttributeTempl.h"
#include "BranchState.h"

template <class T> class CAttriFormationTemplate : public CAttributesTemplate<T> {
public:
  CAttriFormationTemplate(unsigned int uIDTemplate, T &source, CWnd *pParent = NULL)
      : CAttributesTemplate<T>(uIDTemplate, source, pParent) {}
  virtual BOOL OnInitDialog();
  //	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
  virtual void DoDataExchange(CDataExchange *pDX);
};

template <class T> void CAttriFormationTemplate<T>::DoDataExchange(CDataExchange *pDX) {
  CString strName;
  int nFormationType;

  CAttributesTemplate<T>::DoDataExchange(pDX);

  if (!pDX->m_bSaveAndValidate) {
    strName = Copy().Name().toStdString().c_str();
    nFormationType = Copy().FormationType();
  }

  DDX_Text(pDX, IDC_EB_NAME, strName);

  if (pDX->m_bSaveAndValidate) {
    Copy().Name((LPCSTR)strName);

    CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_CB_FORMATIONTYPE);
    nFormationType = pBox->GetCurSel();
    Copy().FormationType((CFormationBase::TFormationType)nFormationType);
  }
}

template <class T> BOOL CAttriFormationTemplate<T>::OnInitDialog() {
  CAttributesTemplate<T>::OnInitDialog();

  // Select formation type
  CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_CB_FORMATIONTYPE);
  VERIFY(pBox->AddString("Not Specified") == CFormationBase::FT_NOTSPECIFIED);
  VERIFY(pBox->AddString("Shale") == CFormationBase::FT_SHALE);
  VERIFY(pBox->AddString("Sand") == CFormationBase::FT_SAND);
  VERIFY(pBox->AddString("Chalk") == CFormationBase::FT_CHALK);
  VERIFY(pBox->AddString("Salt") == CFormationBase::FT_SALT);
  VERIFY(pBox->AddString("Limestone") == CFormationBase::FT_LIMESTONE);
  VERIFY(pBox->AddString("Sandstone") == CFormationBase::FT_SANDSTONE);
  pBox->SetCurSel(Copy().FormationType());

  return TRUE;
}

#endif // !defined(AFX_ATTRIFORMATIONTEMPLATE_H__B3921941_0C77_4C09_967A_9081ACE8B0AB__INCLUDED_)
