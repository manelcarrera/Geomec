#pragma once

// attri2dscene.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttri2DScene dialog
#include "AttributesTemplate.h"
#include "OpenGLSceneBase.h"
#include "resource.h"

// ##ModelId=3BC55D6500B1
template <class OIV2DSceneType> class CAttri2DScene : public CAttributesTemplate<OIV2DSceneType> {
  int m_lineThickness;
  // Construction
public:
  // ##ModelId=3BE7AEC80297
  CAttri2DScene(OIV2DSceneType &scene, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CAttri2DScene)
  enum { IDD = IDD_ATTRI_2DSCENE };
  // ##ModelId=3BE7AEC802A7
  //  BOOL	bApplyToAllView;
  // }}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttri2DScene)
protected:
  // ##ModelId=3BC55D6500D6
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAttri2DScene)
  // ##ModelId=3BC55D6500E0
  afx_msg void OnPaint();
  // ##ModelId=3BE7AEC80322
  afx_msg void OnClickForeGroundColor();
  // ##ModelId=3BE7AEC80324
  afx_msg void OnClickBackGroundColor();
  afx_msg void OnClickBackGroundColor2();
  afx_msg void OnOK();
  afx_msg void OnCancel();

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedBtInvertcolors();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
