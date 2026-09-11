// attri2dscene.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "attri2dscene.h"
#include "OIV2DHistoryScene.h"
#include "OIV2DWellPathGraphScene.h"


#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif


template class CAttri2DScene<OIV2DHistoryScene>;
template class CAttri2DScene<OIV2DWellPathGraphScene>;

/////////////////////////////////////////////////////////////////////////////
// CAttri2DScene dialog


//##ModelId=3BE7AEC80297

template <class OIV2DSceneType>
CAttri2DScene<OIV2DSceneType>::CAttri2DScene(OIV2DSceneType &scene, CWnd* pParent /*=NULL*/)
  : CAttributesTemplate<OIV2DSceneType>(CAttri2DScene::IDD, scene, pParent)
{
  //{{AFX_DATA_INIT(CAttri2DScene)
  // bApplyToAllView = FALSE;

  //}}AFX_DATA_INIT
}


//##ModelId=3BC55D6500D6
template <class OIV2DSceneType>
void CAttri2DScene<OIV2DSceneType>::DoDataExchange(CDataExchange* pDX)
{
  CString strName;
  
  CDialog::DoDataExchange(pDX);

  if(!pDX->m_bSaveAndValidate)
  {
    strName = Copy().Name().toStdString().c_str();
    m_lineThickness = Copy().getLineThickness();
  }


  //{{AFX_DATA_MAP(CAttri2DScene)
  // DDX_Check(pDX, IDC_CK_APPLY_ALL_VIEW, bApplyToAllView);

  DDX_Text(pDX, IDC_ST_NAME, strName);
  DDX_Text(pDX, IDC_ED_LINETHICKNESS, m_lineThickness);
  //}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate)
  {
    Copy().setLineThickness(m_lineThickness);
  }
}


// BEGIN_MESSAGE_MAP(CAttri2DScene<OIV2DHistoryScene>, CDialog)
BEGIN_TEMPLATE_MESSAGE_MAP(CAttri2DScene, OIV2DSceneType, CListBox)

  //{{AFX_MSG_MAP(CAttri2DScene)
  ON_WM_PAINT()
  ON_BN_CLICKED(IDC_ST_TEXT_COLOR, OnClickForeGroundColor)
  ON_BN_CLICKED(IDC_ST_BACKGROUND_COLOR, OnClickBackGroundColor)
  ON_BN_CLICKED(IDC_ST_BACKGROUND_COLOR2, OnClickBackGroundColor2)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BT_INVERTCOLORS, OnBnClickedBtInvertcolors)
  ON_BN_CLICKED(IDOK, OnOK)
  ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttri2DScene message handlers

//##ModelId=3BC55D6500E0
template <class OIV2DSceneType>
void CAttri2DScene<OIV2DSceneType>::OnPaint() 
{
  CPaintDC dc(this); // device context for painting

  QRgb foregroundColor = Copy().ForegroundColor();
  QRgb backgroundColor = Copy().BackgroundColor();
  QRgb backgroundColor2 = Copy().BackgroundColor2();
  
  FillFrame(IDC_ST_TEXT_COLOR, foregroundColor, dc);
  FillFrame(IDC_ST_BACKGROUND_COLOR, backgroundColor, dc);	
  FillFrame(IDC_ST_BACKGROUND_COLOR2, backgroundColor2, dc);
}


template <class OIV2DSceneType>
void CAttri2DScene<OIV2DSceneType>::OnClickForeGroundColor() 
{
  Copy().ForegroundColor(SelectColor(Copy().ForegroundColor()));
  Invalidate();

}

//##ModelId=3BE7AEC80324
template <class OIV2DSceneType>
void CAttri2DScene<OIV2DSceneType>::OnClickBackGroundColor() 
{
  Copy().BackgroundColor(SelectColor(Copy().BackgroundColor()));
  Invalidate();
  // TODO: Add your control notification handler code here

}

template <class OIV2DSceneType>
void CAttri2DScene<OIV2DSceneType>::OnClickBackGroundColor2() 
{
  Copy().BackgroundColor2(SelectColor(Copy().BackgroundColor2()));
  Invalidate();
  // TODO: Add your control notification handler code here

}


QRgb InvertQrgb (QRgb color)
{
  return (color ^ RGB_MASK);
}

template <class OIV2DSceneType>
void CAttri2DScene<OIV2DSceneType>::OnBnClickedBtInvertcolors()
{
  UpdateData(true);
  Copy().ForegroundColor(InvertQrgb(Copy().ForegroundColor()));
  Copy().BackgroundColor(InvertQrgb(Copy().BackgroundColor()));
  Copy().BackgroundColor2(InvertQrgb(Copy().BackgroundColor2()));
  UpdateData(false);
  Invalidate();
}

template <class OIV2DSceneType>
void CAttri2DScene<OIV2DSceneType>::OnOK()
{
  CAttributesTemplate<OIV2DSceneType>::OnOK();
}

template <class OIV2DSceneType>
void CAttri2DScene<OIV2DSceneType>::OnCancel()
{
  CAttributesTemplate<OIV2DSceneType>::OnCancel();
}
