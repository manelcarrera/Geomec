
// attriscene.cpp : implementation file
//

#include "attriscene.h"
#include "OpenInventorSceneNode.h"
#include "geomec.h"
#include "stdafx.h"

// Deformation
#include "WellCasingModel.h" //FIXME
// #include "IT_Geomec_Interface.h" //FIXME
#include "Deformation.h" //FIXME
#include "IGm.h"         //FIXME
#include "List.h"        //FIXME

// #include "ConsistencyGuard.h" // -> is_model_in_scope()

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriScene dialog

// ##ModelId=3BE7AEC80297
CAttriScene::CAttriScene(COpenInventorSceneNode &scene, CWnd *pParent /*=NULL*/)
    : CAttributesTemplate<COpenInventorSceneNode>(CAttriScene::IDD, scene, pParent) {
  //{{AFX_DATA_INIT(CAttriScene)
  // bApplyToAllView = FALSE;

  //}}AFX_DATA_INIT
}

enum eType { Enable, Show };

void show_hide(CWnd *wnd, std::vector<int> control_id_v, eType type_, int val) {
  for (int i = 0; i < control_id_v.size(); i++) {
    if (type_ == Enable)
      wnd->GetDlgItem(control_id_v[i])->EnableWindow(val);
    else
      wnd->GetDlgItem(control_id_v[i])->ShowWindow(val);
  }
}

void fill_stages_combo(CComboBox &combo, Deformation::Data::Input deformation) {
  TDepletionL *depletion_l = gm::list(gm::eList::L_Depletion);

  if (combo.GetCount() != depletion_l->elems().size()) // -> depletion stages has changed
  {
    for (int i = 0; i < depletion_l->elems().size(); i++)
      combo.AddString(depletion_l->elems()[i].c_str());

    if (deformation.stage != -1)
      combo.SetCurSel(deformation.stage);
  }
}

// ##ModelId=3BC55D6500D6
void CAttriScene::DoDataExchange(CDataExchange *pDX) {
  CString strName;
  BOOL bDisplayGnomon, bDisplayBoundary, bDisplayPerspective, bAllowDynamicRotate, bFill,
      bTrackElement; //, bDeformation;
  double dPointSize;
  int iTransparency, iIsoCount;
  int iDraggerTimeOut;
  double dIsoGap;

  // Deformation
  BOOL active; // -> int
  Deformation::Data::Input deformation = Deformation::INVALID_INPUT;

  const CWellCasingModel *model_casing = dynamic_cast<const CWellCasingModel *>(&Copy().Model());
  /*bool is_model_in_scope = false;
  CModelBase* pModel = dynamic_cast<CModelBase*>( &Copy().Model() );
  if( pModel )
  {
    CConsistencyGuard* guard = pModel->GetConsistencyGuard();
    is_model_in_scope = guard && guard->is_model_in_scope( CConsistencyGuard::Deformation );
  }*/

  CDialog::DoDataExchange(pDX);

  if (!pDX->m_bSaveAndValidate) // (1) SHOW
  {
    // -> (a) get values to show
    strName = Copy().Name().toStdString().c_str();
    bDisplayBoundary = Copy().ShowBoundary();
    bDisplayGnomon = Copy().DisplayGnomon();
    bDisplayPerspective = Copy().DisplayPerspective();
    bAllowDynamicRotate = Copy().AllowDynamicRotate();
    dPointSize = Copy().PointSize();
    iTransparency = Copy().Transparency();
    iDraggerTimeOut = Copy().DraggerTimeOut();
    iIsoCount = Copy().IsoCount();
    dIsoGap = Copy().IsoGap();
    bFill = Copy().Fill();
    bTrackElement = Copy().TrackElement();

    deformation = Copy().Deformation(); // TODO: check out
    active = deformation.active;

    BOOL bEnable = Copy().Dimension() == IOpenGLFrame::DIM_3D;
    std::vector<int> control_id_v = {IDC_CK_AXIS,       IDC_CK_BOUNDARY,  IDC_CK_PERSPECTIVE,   IDC_CK_ROTATE,
                                     IDC_CK_SOLID_FILL, IDC_ED_POINTSIZE, IDC_ED_DRAGGERTIMEOUT};
    show_hide(this, control_id_v, Enable, bEnable);

    // Deformation
    if (!model_casing)
    // if( !is_model_in_scope )
    {
      std::vector<int> control_id_v2 = {IDC_DEF_CHECK,     IDC_DEF_COMBO,           IDC_DEF_SCALE_GEN_LABEL,
                                        IDC_DEF_SCALE_GEN, IDC_DEF_SCALE_DEF_LABEL, IDC_DEF_SCALE_DEF};
      show_hide(this, control_id_v2, Show, SW_HIDE);
    }
    // std::vector<int> control_id_v = { IDC_DEF_CHECK, IDC_DEF_COMBO, IDC_DEF_SCALE_GEN, IDC_DEF_SCALE_DEF };
    // show_hide( this, control_id_v, Enable, bEnable );
  }

  //-> (b) link controls with local vars to retrieve values on accept

  //{{AFX_DATA_MAP(CAttriScene)
  // DDX_Check(pDX, IDC_CK_APPLY_ALL_VIEW, bApplyToAllView);
  DDX_Check(pDX, IDC_CK_AXIS, bDisplayGnomon);
  DDX_Check(pDX, IDC_CK_BOUNDARY, bDisplayBoundary);
  DDX_Check(pDX, IDC_CK_PERSPECTIVE, bDisplayPerspective);
  DDX_Check(pDX, IDC_CK_ROTATE, bAllowDynamicRotate);
  DDX_Check(pDX, IDC_CK_SOLID_FILL, bFill);
  DDX_Check(pDX, IDC_CK_TRACKELEMENT, bTrackElement);
  DDX_Text(pDX, IDC_ED_POINTSIZE, dPointSize);
  DDX_Text(pDX, IDC_ED_DRAGGERTIMEOUT, iDraggerTimeOut);
  DDX_Text(pDX, IDC_ED_TRANSPARENCY, iTransparency);
  DDX_Text(pDX, IDC_ED_ISOCOUNT, iIsoCount);
  DDX_Text(pDX, IDC_ED_ISOGAP, dIsoGap);
  DDV_MinMaxDouble(pDX, dPointSize, 0, 1000);
  DDX_Text(pDX, IDC_ST_NAME, strName);

  // Deformation
  if (model_casing)
  // if( is_model_in_scope )
  {
    DDX_Check(pDX, IDC_DEF_CHECK, active);
    DDX_Control(pDX, IDC_DEF_COMBO, m_combo_deformation);
    DDX_Text(pDX, IDC_DEF_SCALE_GEN, deformation.scale.gen);
    DDX_Text(pDX, IDC_DEF_SCALE_DEF, deformation.scale.def);

    if (!pDX->m_bSaveAndValidate)
      fill_stages_combo(m_combo_deformation,
                        deformation); // This must be performed at the end, after linking control and var
  }

  //}}AFX_DATA_MAP

  // (c) save values from linked local vars (linked to controls) -> saved into the 'scene node' / 'node manager'

  if (pDX->m_bSaveAndValidate) // (2) SAVE
  {
    Copy().ShowBoundary(bDisplayBoundary);
    Copy().DisplayGnomon(bDisplayGnomon);
    Copy().DisplayPerspective(bDisplayPerspective);
    Copy().AllowDynamicRotate(bAllowDynamicRotate);
    Copy().TrackElement(bTrackElement);
    Copy().PointSize(dPointSize);
    Copy().DraggerTimeOut(iDraggerTimeOut);
    Copy().Transparency(iTransparency);
    Copy().IsoCount(iIsoCount);
    Copy().IsoGap(dIsoGap);
    Copy().Fill(bFill);
    Copy().UpdateAllViews();

    // Deformation -> send it for the scene to be updated with scale / deformation
    deformation.active = active;
    deformation.stage = m_combo_deformation.GetCurSel();
    Copy().OnDeformation(deformation); // FIXME: This triggers the process
  }
}

BEGIN_MESSAGE_MAP(CAttriScene, CDialog)
//{{AFX_MSG_MAP(CAttriScene)
ON_WM_PAINT()
ON_BN_CLICKED(IDC_ST_AXIS_COLOR, OnClickAxisColor)
ON_BN_CLICKED(IDC_ST_TEXT_COLOR, OnClickTextColor)
ON_BN_CLICKED(IDC_ST_BACKGROUND_COLOR, OnClickBackGroundColor)
ON_BN_CLICKED(IDC_ST_BACKGROUND_COLOR2, OnClickBackGroundColor2)

// ON_BN_CLICKED(IDC_CHECK_BOX_DEFORMATION, OnClickDeformation)
// ON_CBN_SELENDOK(IDC_COMBO_BOX_DEFORMATION, OnComboChanged)

//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriScene message handlers

// ##ModelId=3BC55D6500E0
void CAttriScene::OnPaint() {
  CPaintDC dc(this); // device context for painting

  QRgb axisColor = Copy().AxisColor();
  QRgb textColor = Copy().TextColor();
  QRgb backgroundColor = Copy().BackgroundColor();
  QRgb backgroundColor2 = Copy().BackgroundColor2();
  FillFrame(IDC_ST_AXIS_COLOR, axisColor, dc);
  FillFrame(IDC_ST_TEXT_COLOR, textColor, dc);
  FillFrame(IDC_ST_BACKGROUND_COLOR, backgroundColor, dc);
  FillFrame(IDC_ST_BACKGROUND_COLOR2, backgroundColor2, dc);
}

// ##ModelId=3BE7AEC80322
void CAttriScene::OnClickAxisColor() {
  Copy().AxisColor(SelectColor(Copy().AxisColor()));
  Invalidate();
}

void CAttriScene::OnClickTextColor() {
  Copy().TextColor(SelectColor(Copy().TextColor()));
  Invalidate();
}

// ##ModelId=3BE7AEC80324
void CAttriScene::OnClickBackGroundColor() {
  Copy().BackgroundColor(SelectColor(Copy().BackgroundColor()));
  Invalidate();
  // TODO: Add your control notification handler code here
}

void CAttriScene::OnClickBackGroundColor2() {
  Copy().BackgroundColor2(SelectColor(Copy().BackgroundColor2()));
  Invalidate();
  // TODO: Add your control notification handler code here
}
