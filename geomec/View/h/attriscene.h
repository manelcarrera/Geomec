#if !defined(AFX_ATTRISCENE_H__80FD1288_B027_4724_91C3_5FAF38D993FF__INCLUDED_)
#define AFX_ATTRISCENE_H__80FD1288_B027_4724_91C3_5FAF38D993FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attriscene.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriScene dialog
#include "AttributesTemplate.h"
#include "OpenGLSceneBase.h"
#include "resource.h"



//##ModelId=3BC55D6500B1
class CAttriScene : public CAttributesTemplate<COpenInventorSceneNode>
{
  CComboBox m_combo_deformation; // It must be a member, it can't be a local var in 'DoDataExchange()' as the rest

// Construction
public:
  //##ModelId=3BE7AEC80297
  CAttriScene(COpenInventorSceneNode &scene, CWnd* pParent = NULL);   // standard constructor

  
// Dialog Data
  //{{AFX_DATA(CAttriScene)
  enum { IDD = IDD_ATTRI_SCENE };
  //##ModelId=3BE7AEC802A7
  // BOOL	bApplyToAllView;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriScene)
  protected:
  //##ModelId=3BC55D6500D6
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CAttriScene)
  //##ModelId=3BC55D6500E0
  afx_msg void OnPaint();
  //##ModelId=3BE7AEC80322
  afx_msg void OnClickAxisColor();
  afx_msg void OnClickTextColor();
  //##ModelId=3BE7AEC80324
  afx_msg void OnClickBackGroundColor();
  afx_msg void OnClickBackGroundColor2();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedCkTrackelement();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRISCENE_H__80FD1288_B027_4724_91C3_5FAF38D993FF__INCLUDED_)
