#ifndef _ATTRIWELLZOOMINPARENTRESULTS_H_
#define _ATTRIWELLZOOMINPARENTRESULTS_H_

class CModelBase;

class CAttriWellZoomInParentResultsDlg : public CDialog {
public:
  CAttriWellZoomInParentResultsDlg(CModelBase &model, CWnd *pParent);

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();

private:
  CModelBase &m_model;
  int m_nSelection;
};

#endif // _ATTRIWELLZOOMINPARENTRESULTS_H_
