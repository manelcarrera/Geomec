#ifndef _ATTRIWELLZOOMINMODEL_H_
#define _ATTRIWELLZOOMINMODEL_H_

#include "AttributesTemplate.h"
#include "IWellModel.h"

class CAttriWellZoomInModelDlg : public CDialog
{
public:
  CAttriWellZoomInModelDlg(IWellModel& model, const CString& strCaption, CWnd* pParent = 0);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  virtual void PostNcDestroy();

  afx_msg void OnStartDefault();
  afx_msg void OnStartAHD();
  afx_msg void OnStartFormation();
  afx_msg void OnSelChangeStartFormation();
  afx_msg void OnEndDefault();
  afx_msg void OnEndAHD();
  afx_msg void OnEndFormation();
  afx_msg void OnSelChangeEndFormation();
  afx_msg void OnChangeStartAHD();
  afx_msg void OnChangeEndAHD();
 	DECLARE_MESSAGE_MAP()

private:
  IWellModel& m_model;
  IWellModel::CPositionDef* m_pStartPosition;
  IWellModel::CPositionDef* m_pEndPosition;
  double m_dRadius;
  CString m_strCaption;
};

#endif // _ATTRIWELLZOOMINMODEL_H_
