#if !defined(AFX_ATTRIBUTETEMPL_H__4DB112CC_5590_40A2_83EE_E97FB334E8EB__INCLUDED_)
#define AFX_ATTRIBUTETEMPL_H__4DB112CC_5590_40A2_83EE_E97FB334E8EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUnitNode;

#include "FemAppDoc.h"

// The attributes template generlize the validation of an
// object.

template <class T> class CAttributesTemplate : public CDialog {
  T &m_gnSource;
  T *m_pCopy;

public:
  // Constructor / Destructor  ....
  CAttributesTemplate(unsigned int uIDTemplate, T &source, CWnd *pParent = NULL)
      : CDialog(uIDTemplate, pParent), m_gnSource(source), m_pCopy(0) {}
  ~CAttributesTemplate() { assert(!m_pCopy); }

protected:
  // Access to copy and source ...
  T &Copy() { return *m_pCopy; }
  T &Source() { return m_gnSource; }
  const T &Copy() const { return *m_pCopy; }
  const T &Source() const { return m_gnSource; }

  // Access to
  typedef QRgb TColor;
  void FillFrame(const unsigned int uFrameId, TColor color, CPaintDC &dc);
  TColor SelectColor(TColor nDefaultColor);
  const CUnitNode &UnitNode() const {
    POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
    CDocTemplate *pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
    POSITION docpos = pDocTemplate->GetFirstDocPosition();
    CFemAppDoc &doc = *(CFemAppDoc *)(pDocTemplate->GetNextDoc(docpos));
    return doc.UnitNode();
  }

  // On validate ...
  virtual void OnOK() {
    if (UpdateData()) {
      if (!(m_gnSource == *m_pCopy)) {
        // Restore
        m_gnSource = *m_pCopy;
        m_gnSource.Modified();
      }

      EndDialog(IDOK);
    }
  }

  virtual BOOL OnInitDialog() {
    assert(!m_pCopy);

    m_pCopy = new T(m_gnSource);

    assert(m_pCopy->IsCopy());

    return CDialog::OnInitDialog();
  }

  virtual void PostNcDestroy() {
    delete m_pCopy;
    m_pCopy = 0;
  }

  void EnterPopup() { PostMessage(WM_SYSKEYDOWN); }

  void LeavePopup() {
    Invalidate();
    UpdateWindow();
  }
};

template <class T> void CAttributesTemplate<T>::FillFrame(const unsigned int uFrameId, TColor color, CPaintDC &dc) {
  CWnd *pColor = GetDlgItem(uFrameId);
  CBrush Brush(RGB(qRed(color), qGreen(color), qBlue(color)));
  CRect rectClient;
  pColor->GetWindowRect(&rectClient);
  ScreenToClient(&rectClient);
  dc.FillRect(&rectClient, &Brush);
}

template <class T> typename CAttributesTemplate<T>::TColor CAttributesTemplate<T>::SelectColor(TColor nDefaultColor) {
  CColorDialog dlg;
  //	COLORREF aCustomColors[16];
  // GetCustomColors( aCustomColors );

  dlg.m_cc.Flags |= CC_RGBINIT;
  dlg.m_cc.rgbResult = RGB(qRed(nDefaultColor), qGreen(nDefaultColor), qBlue(nDefaultColor));
  //	dlg.m_cc.lpCustColors = aCustomColors;

  if (dlg.DoModal() == IDOK) {
    int nColorRet = dlg.GetColor();
    // SetCustomColors( aCustomColors );
    return qRgb(GetRValue(nColorRet), GetGValue(nColorRet), GetBValue(nColorRet));
  }

  return nDefaultColor;
}

#endif // !defined(AFX_ATTRIBUTETEMPL_H__4DB112CC_5590_40A2_83EE_E97FB334E8EB__INCLUDED_)
