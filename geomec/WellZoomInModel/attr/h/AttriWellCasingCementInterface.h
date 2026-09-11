#ifndef _ATTRIWELLCASINGCEMENTINTERFACE_H_
#define _ATTRIWELLCASINGCEMENTINTERFACE_H_

class CWellCasingCementInterface;

class CAttriWellCasingCementInterface : public CDialog {
public:
  CAttriWellCasingCementInterface(CWellCasingCementInterface &source, CWnd *pParent = 0);

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();

  afx_msg void OnStColor();
  afx_msg void OnPaint();
  DECLARE_MESSAGE_MAP()

private:
  typedef QRgb TColor;

private:
  TColor SelectColor(TColor nDefaultColor);
  void FillFrame(const unsigned int uFrameId, TColor color, CPaintDC &dc);

private:
  CWellCasingCementInterface &m_source;
  TColor m_color;
  int m_sliptype;
  CString m_strName;
};

#endif // _ATTRIWELLCASINGCEMENTINTERFACE_H_
