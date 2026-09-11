// ModelTreeView.cpp : implementation file
//

#include "ModelTreeView.h"
#include "3DHorizon.h"
#include "3DSupportNode.h"
#include "AnalysisPoint.h"
#include "BaseSupportNode_Delegate.h"
#include "BoundaryBase.h"
#include "BoundaryBase_Delegate.h"
#include "CrossSection.h"
#include "CrossSection_Delegate.h"
#include "FaultPressure.h"
#include "GeomecDianaRunner.h"
#include "GlobalInitialStressNode.h"
#include "GlobalInitialStressNode_Delegate.h"
#include "GlobalPressure.h"
#include "GlobalPressure_Delegate.h"
#include "GlobalTemperature.h"
#include "GlobalTemperature_Delegate.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "HexaMeshRegion_Delegate.h"
#include "HexaModel.h"
#include "HomogenizationBox.h"
#include "HomogenizationBoxTypes.h"
#include "KeyFile.h"
#include "MaterialResultTree.h"
#include "MeasuredTopDisplacementsNode_Delegate.h"
#include "MeshBase.h"
#include "NewWellPath.h"
#include "NodeObserver_Delegate.h"
#include "OpenGLNodeObserver.h"
#include "OpenGLSceneBase_Delegate.h"
#include "OpenInventorSceneWrapper.h"
#include "ResultObserver.h"
#include "ResultTree.h"
#include "SelectionObserver.h"
#include "SelectionObserver_Delegate.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"
#include "TetraModel.h"
#include "TreeCtrlBase.h"
#include "Upscaling.h"
#include "ValueCompositeObserver.h"
#include "ValueType_Delegate.h"
#include "WellCasingCement.h"
#include "WellCasingMesh_Delegate.h"
#include "WellCasingModel.h"
#include "WellCasingModel_Delegate.h"
#include "WellCasingNode.h"
#include "WellCasingRTCI.h"
#include "WellFormation.h"
#include "WellModelEntryTypes.h"
#include "WellZoomInMesh_Delegate.h"
#include "geomec.h"
#include "hexameshregion.h"
#include "stdafx.h"

#include "3DHorizonObserver.h"
#include "AnalysisPointObserver.h"
#include "CrossSectionObserver.h"
#include "ExportResultData.h"
#include "ExportResultDataEntry.h"
#include "ExportResultDataEntry_Delegate.h"
#include "ExportResultData_Delegate.h"
#include "GVTResult.h"
#include "GroupNode.h"
#include "HexaFormationObserver.h"
#include "HexaGUI.h"
#include "IWellGUI.h"
#include "ModelObserver.h"
#include "ModelView.h"
#include "NewWellPathGroupNodeObserver.h"
#include "NewWellPathObserver.h"
#include "NewWellPath_Delegate.h"
#include "NonMeshedSurfaceObserver.h"
#include "OIV2DHistorySceneObserver.h"
#include "OIV2DWellPathGraphSceneObserver.h"
#include "OIVSceneObserver.h"
#include "OpenInventorSceneNodeObserver.h"
#include "TetraFormationObserver.h"
#include "TetraModelObserver.h"
#include "UpscalingObserver.h"
#include "ValueCompositeObserver.h"
#include "WellCasingCementObserver.h"
#include "WellCasingNodeObserver.h"
#include "WellCasingRTCIObserver.h"
#include "WellFormationObserver.h"
#include "ZoominModelEntry.h"
#include "geoSurface_Delegate.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

typedef CNodeObserver_Delegate<CValueType, CValueType_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>
    TValueTypeObserver;
typedef CNodeObserver_Delegate<CSupportDepletion, CSupportDepletion_Delegate, CValueType, TValueTypeObserver, FALSE,
                               UNLINK_ITEM>
    TDepletionObserver;
typedef CNodeObserver_Delegate<CBaseSupportNode, CBaseSupportNode_Delegate, CSupportDepletion, TDepletionObserver,
                               FALSE, FIXED_ITEM>
    TBaseSupportObserver;
typedef COpenGLNodeObserver_Delegate<CBoundaryBase, CBoundaryBase_Delegate, CBaseSupportNode, TBaseSupportObserver,
                                     FALSE, FIXED_ITEM>
    TBoundaryObserver;
typedef COpenGLNodeObserver_Delegate<CHexaMeshRegionBase, CHexaMeshRegionBase_Delegate, CDummyNode, CDummyObserver,
                                     TRUE, UNLINK_ITEM>
    TMeshRegionObserver;
// ##ModelId=3E3E5F020199
typedef CStateBranch_Delegate<CHexaMeshRegionEntry, CHexaMeshRegionEntry_Delegate, CHexaMeshRegionBase,
                              TMeshRegionObserver, CUndefinedIconProvider, TRUE, DELETE_ITEM>
    TMeshRegionEntryObserver;
typedef CStateBranch_Delegate<CCrossSectionEntry, CCrossSectionEntry_Delegate, CCrossSection, TCrossSectionObserver,
                              CUndefinedIconProvider, TRUE, DELETE_ITEM>
    TXSectionEntryObserver;
// ##ModelId=3E3E5F0201A7
typedef CStateBranch_Delegate<CNewWellPathEntry, CNewWellPathEntry_Delegate, CNewWellPath, CNewWellPathObserver,
                              CUndefinedIconProvider, TRUE, DELETE_ITEM>
    TNewWellPathEntryObserver; // wjrx mantis 3185

typedef CSelectionLeafObserver_Delegate<IValueComposite, IValueComposite_Delegate, CGeomecDoc, TRUE> TCompositeObs;
typedef CSelectionBranchObserver_Delegate<CGraphEntry, CGraphEntry_Delegate, IValueComposite, TCompositeObs,
                                          IValueComposite, CGeomecDoc, FALSE, FIXED_ITEM>
    TMeshEntryObs;
typedef CSelectionBranchObserver_Delegate<CGraphEntry, CGraphEntry_Delegate, IValueComposite, TCompositeObs,
                                          IValueComposite, CGeomecDoc, TRUE, DELETE_ITEM>
    TDerivedEntryObs;

typedef CNodeObserver_Delegate<CExportResultData, CExportResultData_Delegate, CDummyNode, CDummyObserver, FALSE,
                               FIXED_ITEM>
    TExportResultDataObserver;
typedef CNodeObserver_Delegate<CExportResultDataEntry, CExportResultDataEntry_Delegate, CExportResultData,
                               TExportResultDataObserver, TRUE, DELETE_ITEM>
    TExportResultDataEntryObserver;

// wjrx mantis 3185
class CNewWellPathEntryObserver : public TNewWellPathEntryObserver {
public:
  // ##ModelId=3E3E5F0201C7
  CNewWellPathEntryObserver(CNewWellPathEntry &node, CTreeNode &parent, const BOOL rename,
                            const enum REMOVE_TYPE remove, HTREEITEM hInsertAfter = TVI_LAST,
                            const BOOL bInitialUpdate = TRUE, BOOL bSortAfterInsertion = TRUE)
      : TNewWellPathEntryObserver(node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion) {}

  // ##ModelId=3E3E5F0201D0
  CNewWellPathEntryObserver(CNewWellPathEntry &node, CTreeCtrl &ctrl, const BOOL rename, const enum REMOVE_TYPE remove,
                            HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST,
                            const BOOL bInitialUpdate = TRUE, BOOL bSortAfterInsertion = TRUE)
      : TNewWellPathEntryObserver(node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate,
                                  bSortAfterInsertion) {}

  // ##ModelId=3E3E5F0201DA
  virtual UINT StateIcon() const {
    POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
    CDocTemplate *pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
    POSITION docpos = pDocTemplate->GetFirstDocPosition();
    CFemAppDoc &doc = *(CFemAppDoc *)(pDocTemplate->GetNextDoc(docpos));
    ISceneWrapper *scene = doc.CurrentScene();

    return TNewWellPathEntryObserver::StateIcon();
  }

  virtual BOOL OnFilter(const CNewWellPath &node) const { return node.Links<CGroupNode>().empty(); }
};

/////////////////////////////////////////////////////////////////////////////
// CModelTreeView

IMPLEMENT_DYNCREATE(CModelTreeView, CTreeViewBase)

CModelTreeView::CModelTreeObject::CModelTreeObject(CFemAppDoc *pDoc, CTreeCtrl &ctrl, HTREEITEM hParent,
                                                   HTREEITEM hInsertAfter)
    : IGraphTreeObject(ctrl, hParent, hInsertAfter), m_pDoc(pDoc) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
}

QString CModelTreeView::CModelTreeObject::Text() const {
  if (m_pDoc->Model())
    return m_pDoc->Model()->documentType();

  // Default no-model loaded tree name
  return getStringTableEntry(IDS_TREE_MODEL);
}

void CModelTreeView::CModelTreeObject::Delete() {
  // Cannot delete a model, so functions should not be called ...
  assert(FALSE);
}

BOOL CModelTreeView::CModelTreeObject::CanEditText() const {
  // No, model labels are fixed ...
  return FALSE;
}

BOOL CModelTreeView::CModelTreeObject::CanDelete() const {
  // Cannot delete a model ...
  return FALSE;
}

unsigned int CModelTreeView::CModelTreeObject::Icon() const {
  if (m_pDoc->Model())
    return m_pDoc->Model()->ModelIconId();

  // Default no-model icon
  return IDI_MODEL;
}

/* NOT OBSOLETE */ void CModelTreeView::CModelTreeObject::AppendContextMenu(CContextMenuInvoker &invoker) {
  CModelBase *pModel = dynamic_cast<CModelBase *>(m_pDoc->Model());
  CGUIBase *pGUI = dynamic_cast<CGUIBase *>(m_pDoc->GUI());
  if (pModel) {
    CHexaModel *pHexa = dynamic_cast<CHexaModel *>(pModel);
    CHexaGUI *pHexaGUI = dynamic_cast<CHexaGUI *>(pGUI);
    const CGeomecDoc *pDoc = dynamic_cast<const CGeomecDoc *>(m_pDoc);
    assert(pDoc != 0);
    if (pHexa) {

#ifdef TETRA_FAULTS_KEYFILE
      bool bHasSlippingHorizons = false;
      if (!pDoc->ExperimentalUnlocked()) {
        const CHexaHorizonEntry::TNodeSet &nodes =
            ((const CHexaHorizonEntry *)pModel->GraphEntry(MD_HEXA_HORIZON))->EntryNodes();

        for (CHexaHorizonEntry::TNodeSet::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
          if ((*it)->Slip()) {
            bHasSlippingHorizons = true;
            break;
          }
        }
      }
#endif
#ifdef TETRA_KEYFILE
      if (pDoc->ExperimentalUnlocked()) {
#endif
#ifdef TETRA_FAULTS_KEYFILE
        if (!bHasSlippingHorizons) {
#endif
          if (!pModel->parentModel()) {
            invoker.AddCommand(_T("Convert to tetramodel"),
                               *(new CSingleCommandTemplate<CHexaGUI>(*pHexaGUI, &CHexaGUI::ConvertToTetraModel,
                                                                      &CHexaGUI::CanConvertToTetra)));
            invoker.AddSeparator();
          }
#ifdef TETRA_FAULTS_KEYFILE
        }
#endif
#ifdef TETRA_KEYFILE
      }
#endif
    }

    if (pModel->parentModel()) {
      invoker.AddCommand(_T("Switch back to parent model"),
                         *new CSingleCommandTemplate<CGUIBase>(*pGUI, &CGUIBase::SwitchToParent));
      invoker.AddCommand(
          _T("Reinitialize depletion stages"),
          *new CSingleCommandTemplate<CModelBase>(*pModel, &CModelBase::MenuInvokedBuildDepletionStages));
      invoker.AddCommand(_T("Set parent model results type"),
                         *new CSingleCommandTemplate<CGUIBase>(*pGUI, &CGUIBase::SetParentModelResults));
    }

    IWellModel *pWellModel = dynamic_cast<IWellModel *>(pModel);
    IWellGUI *pWellGUI = dynamic_cast<IWellGUI *>(pGUI);
    if (pWellModel && pWellGUI) {
      invoker.appendSeparator();
    }

    invoker.AddCommand(_T("Attributes"), *(new CSingleCommandTemplate<CGUIBase>(*pGUI, &CGUIBase::ModelProperties)));
    invoker.AddSeparator();
    invoker.AddCommand(_T("Mesh converter"), *(new CSingleCommandTemplate<CGUIBase>(*pGUI, &CGUIBase::ModelTree)));
    invoker.AddCommand(_T("Mesh refinement"), *(new CSingleCommandTemplate<CGUIBase>(*pGUI, &CGUIBase::MeshRefinement,
                                                                                     &CHexaGUI::Can_MeshRefinement)));
  }
}

/////
CModelTreeView::CViewButton::CViewButton(CModelTreeView &view, TClicked fnClicked, const CString &text,
                                         const CRect &rect, COLORREF color)
    : m_view(view), m_fnClicked(fnClicked), m_text(text), m_rect(rect), m_color(color), m_bEnabled(true),
      m_bVisible(true), m_bDown(false) {}

void CModelTreeView::CViewButton::OnDraw(CDC *pDC) {
  if (m_bVisible) {
    int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
    COLORREF oldTextColor = pDC->SetTextColor(RGB(255, 255, 255));

    COLORREF color = m_color;

    if (!m_bEnabled) {
      color = RGB(200, 200, 200);
    } else if (m_bDown) {
      BYTE r = GetRValue(color);
      BYTE g = GetGValue(color);
      BYTE b = GetBValue(color);

      UpdateColorValue(r);
      UpdateColorValue(g);
      UpdateColorValue(b);

      color = RGB(r, g, b);
    }

    CSize textSize = pDC->GetTextExtent(m_text);

    CBrush brush(color);
    CBrush *pOldBrush = pDC->SelectObject(&brush);

    pDC->Rectangle(m_rect);
    pDC->TextOut(m_rect.left + m_rect.Width() / 2 - textSize.cx / 2, m_rect.top + m_rect.Height() / 2 - textSize.cy / 2,
                 m_text);

    pDC->SelectObject(pOldBrush);
    pDC->SetTextColor(oldTextColor);
    pDC->SetBkMode(nOldBkMode);
  }
}

void CModelTreeView::CViewButton::setEnabled(bool bEnabled) {
  if (bEnabled != m_bEnabled) {
    m_bEnabled = bEnabled;
    if (!bEnabled)
      m_bDown = false;
    Invalidate();
  }
}

bool CModelTreeView::CViewButton::isEnabled() const { return m_bEnabled; }

void CModelTreeView::CViewButton::setVisible(bool bVisible) {
  if (bVisible != m_bVisible) {
    m_bVisible = bVisible;
    Invalidate();
  }
}

bool CModelTreeView::CViewButton::isVisible() const { return m_bVisible; }

void CModelTreeView::CViewButton::OnLButtonDown(UINT nFlags, CPoint point) {
  if (m_bEnabled && m_bVisible) {
    if (m_rect.PtInRect(point))
      m_bDown = true;
    Invalidate();
  }
}

void CModelTreeView::CViewButton::OnLButtonUp(UINT nFlags, CPoint point) {
  if (m_bEnabled && m_bVisible) {
    if (m_bDown && m_rect.PtInRect(point))
      Clicked();

    m_bDown = false;
    Invalidate();
  }
}

void CModelTreeView::CViewButton::OnMouseMove(UINT nFlags, CPoint point) {
  if (m_bEnabled && m_bVisible) {
    if (m_bDown && !m_rect.PtInRect(point)) {
      m_bDown = false;
      Invalidate();
    }
  }
}

void CModelTreeView::CViewButton::OnMouseLeave() {
  if (m_bEnabled && m_bVisible) {
    m_bDown = false;
    Invalidate();
  }
}

void CModelTreeView::CViewButton::Invalidate() { m_view.InvalidateRect(m_rect); }

void CModelTreeView::CViewButton::Clicked() { (m_view.*m_fnClicked)(); }

void CModelTreeView::CViewButton::UpdateColorValue(BYTE &c) {
  int diff = 255 - c;
  c += diff / 2;
}

// ##ModelId=3BC55D61015F
CModelTreeView::CModelTreeView() : m_ButtonHeight(20) {
  for (int i = 0; i < 3; ++i)
    m_viewbuttons[i] = 0;
}

// ##ModelId=3BC55D6101BB
CModelTreeView::~CModelTreeView() {}

BEGIN_MESSAGE_MAP(CModelTreeView, CTreeViewBase)
//{{AFX_MSG_MAP(CModelTreeView)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_MOUSELEAVE()
ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelTreeView drawing

// ##ModelId=3BC55D6101A1
void CModelTreeView::OnDraw(CDC *pDC) {
  if (!m_viewbuttons[0]) {
    CSize deleteSize = pDC->GetTextExtent("Delete");
    CSize cancelSize = pDC->GetTextExtent("Cancel");
    CSize changeSize = pDC->GetTextExtent("Change");
    int maxX = (deleteSize.cx > cancelSize.cx ? deleteSize.cx : cancelSize.cx);
    int maxY = (deleteSize.cy > cancelSize.cy ? deleteSize.cy : cancelSize.cy);
    maxX = (changeSize.cx > maxX ? changeSize.cx : maxX);
    maxY = (changeSize.cy > maxY ? changeSize.cy : maxY);

    int rectW = maxX + 16;
    int rectH = maxY + 5;

    CRect rectButton1 = CRect(1, 1, rectW + 1, rectH);
    CRect rectButton2 = CRect(rectW + 2, 1, 2 * rectW, rectH);

    m_viewbuttons[0] = new CViewButton(*this, &CModelTreeView::OnChangeClicked, "Change", rectButton1, RGB(0, 0, 255));
    m_viewbuttons[1] = new CViewButton(*this, &CModelTreeView::OnDeleteClicked, "Delete", rectButton1, RGB(255, 0, 0));
    m_viewbuttons[2] = new CViewButton(*this, &CModelTreeView::OnCancelClicked, "Cancel", rectButton2, RGB(0, 0, 255));
    m_viewbuttons[1]->setVisible(false);
    m_viewbuttons[2]->setVisible(false);
  }

  CGeomecDoc *pDoc = (CGeomecDoc *)GetDocument();
  m_viewbuttons[0]->setEnabled(pDoc->Model() != 0);
  m_viewbuttons[1]->setEnabled(pDoc->HasChangeSelection());

  for (int i = 0; i < 3; ++i)
    m_viewbuttons[i]->OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CModelTreeView diagnostics

#ifdef _DEBUG
// ##ModelId=3BC55D6101BD
void CModelTreeView::AssertValid() const { CTreeViewBase::AssertValid(); }

// ##ModelId=3BC55D6101BF
void CModelTreeView::Dump(CDumpContext &dc) const { CTreeViewBase::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CModelTreeView message handlers

// ##ModelId=3BC55D61019D
void CModelTreeView::OnInitialUpdate() {
  CGraphTreeView::OnInitialUpdate();
  DeleteTree();

  // Make Model root
  HTREEITEM hModel = 0;

  IGraphTreeObject *pObject = new CModelTreeObject(Document(), GetTreeCtrl(), TVI_ROOT, TVI_LAST);
  hModel = pObject->Handle();

  // Composite for supported objects
  if (Document()->Model()) {
    CModelBase *pModel = (CModelBase *)(Document()->Model());
    CWellZoomInModel *pWZIModel = dynamic_cast<CWellZoomInModel *>(pModel);
    CWellCasingModel *pCasingModel = dynamic_cast<CWellCasingModel *>(pModel);

    if (!pWZIModel && !pCasingModel) {
      typedef CNodeObserver_Delegate<CGlobalInitialStressNode, CGlobalInitialStressNode_Delegate, IValueComposite,
                                     CValueCompositeObserver, TRUE, UNLINK_ITEM>
          TInitStressObs;
      AddRoot(*(new TInitStressObs(pModel->GlobalInitialStress(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST,
                                   TRUE, FALSE)));
    }

    typedef CNodeObserver_Delegate<CGlobalPressure, CGlobalPressure_Delegate, IValueComposite, CValueCompositeObserver,
                                   TRUE, UNLINK_ITEM>
        TInitPressureObs;
    AddRoot(*(new TInitPressureObs(pModel->GlobalPressure(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST, TRUE,
                                   FALSE)));

    typedef CNodeObserver_Delegate<CGlobalTemperature, CGlobalTemperature_Delegate, IValueComposite,
                                   CValueCompositeObserver, TRUE, UNLINK_ITEM>
        TInitTemperatureObs;
    AddRoot(*(new TInitTemperatureObs(pModel->InitialTemperature(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST,
                                      TRUE, FALSE)));

    if (pCasingModel) {
      AddRoot(*new CWellCasingNodeObserver(pCasingModel->CasingNode(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel,
                                           TVI_LAST, FALSE, FALSE));
      AddRoot(*new CWellCasingCementObserver(pCasingModel->CasingCement(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel,
                                             TVI_LAST, FALSE, FALSE));
    }

    if (Document()->Model()->GraphEntry(MD_HEXA_HORIZON))
      //			AddRoot(*(new THorizonEntryObserver((*Document()->Model()->GraphEntry(MD_HEXA_HORIZON)), GetTreeCtrl(),
      // FALSE, DELETE_ITEM, hModel, TVI_LAST, TRUE, FALSE)));
      AddRoot(*(new C3DHorizonEntryObserver<true>((*Document()->Model()->GraphEntry(MD_HEXA_HORIZON)), GetTreeCtrl(),
                                                  FALSE, DELETE_ITEM, hModel, TVI_LAST)));

    if (Document()->Model()->GraphEntry(MD_TETRA_SUB_ALL)) {
      AddRoot(*(new CTetraSubModelBranch(*pObject, (CTetraModel &)(*Document()->Model()))));
    }

    // Observer for the tetra super horizons
    typedef C3DHorizonEntryObserver<false> TTetraHorizonEntryObserver;
    // typedef CStateBranch_Delegate<CTetraSuperHorizonEntry, CTetraSuperHorizonEntry_Delegate, CHorizonBase,
    // CHorizonObserver<FALSE>, CUndefinedIconProvider, TRUE, DELETE_ITEM> TTetraHorizonEntryObserver;
    if (Document()->Model()->GraphEntry(MD_TETRA_SUPER_HORIZON)) {
      CGeomecApp *pApp = dynamic_cast<CGeomecApp *>(AfxGetApp());
#ifdef TETRA_SUPERMODEL_KEYFILE
      if (pApp->KeyFile().Unlocked()) {
#endif
        //				AddRoot(*(new
        // TTetraHorizonEntryObserver((CTetraSuperHorizonEntry&)(*Document()->Model()->GraphEntry(MD_TETRA_SUPER_HORIZON)),
        // GetTreeCtrl(), FALSE, DELETE_ITEM, hModel, TVI_LAST, TRUE, FALSE)));
        AddRoot(*(new TTetraHorizonEntryObserver(
            (CTetraSuperHorizonEntry &)(*Document()->Model()->GraphEntry(MD_TETRA_SUPER_HORIZON)), GetTreeCtrl(), FALSE,
            DELETE_ITEM, hModel, TVI_LAST)));
#ifdef TETRA_SUPERMODEL_KEYFILE
      }
#endif
    }

    if (Document()->Model()->GraphEntry(MD_BASE_NONMESHEDSURFACE))
      AddRoot(*(new TNonMeshedSurfaceEntryObserver(
          (CNonMeshedSurfaceEntry &)(*Document()->Model()->GraphEntry(MD_BASE_NONMESHEDSURFACE)), GetTreeCtrl(), FALSE,
          FIXED_ITEM, hModel, TVI_LAST)));

    if (Document()->Model()->GraphEntry(MD_HEXA_FORMATION)) {
      AddRoot(*(
          new CHexaFormationEntryObserver((CHexaFormationEntry &)(*Document()->Model()->GraphEntry(MD_HEXA_FORMATION)),
                                          GetTreeCtrl(), hModel, TVI_LAST)));
    } else if (Document()->Model()->GraphEntry(MD_TETRA_FORMATION)) {
      AddRoot(*(new CTetraFormationEntryObserver(
          (CTetraFormationEntry &)(*Document()->Model()->GraphEntry(MD_TETRA_FORMATION)), GetTreeCtrl(), hModel,
          TVI_LAST)));
    } else if (Document()->Model()->GraphEntry(MD_WELLMODEL_FORMATION)) {
      AddRoot(*(new CWellFormationEntryObserver(
          (CWellFormationEntry &)(*Document()->Model()->GraphEntry(MD_WELLMODEL_FORMATION)), GetTreeCtrl(), hModel,
          TVI_LAST)));
    }

    if (pWZIModel) {
      typedef COpenGLNodeObserver_Delegate<CWellZoomInMesh, CWellZoomInMesh_Delegate, CDummyNode, CDummyObserver, FALSE,
                                           FIXED_ITEM>
          TZoomInMeshObs;
      new TZoomInMeshObs(pWZIModel->Mesh(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST, TRUE, FALSE);
    }

    if (pCasingModel) {
      typedef COpenGLNodeObserver_Delegate<CWellCasingMesh, CWellCasingMesh_Delegate, CDummyNode, CDummyObserver, FALSE,
                                           FIXED_ITEM>
          TCasingMeshObs;
      new TCasingMeshObs(pCasingModel->Mesh(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST, TRUE, FALSE);
    }

    new TBoundaryObserver(pModel->Boundary(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST, TRUE, FALSE);

    if (pWZIModel) {
      typedef CNodeObserver_Delegate<CWellCasingModel, CWellCasingModel_Delegate, CDummyNode, CDummyObserver, FALSE,
                                     FIXED_ITEM>
          TCasingObs;
      new TCasingObs(pWZIModel->CasingModel(), GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST, TRUE, FALSE);
    }

    if (pCasingModel) {
      AddRoot(*new TWellCasingRTCIEntryObserver((CWellCasingRTCIEntry &)*pCasingModel->GraphEntry(MD_WELLCASING_RTCI),
                                                GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST));
    }

    // upscaling
    if (Document()->Model()->GraphEntry(MD_BASE_UPSCALING))
      AddRoot(*new CUpscalingBranch(*pObject, (CModelBase &)*Document()->Model()));

    if (Document()->Model()->GraphEntry(MD_HEXA_MESH_ZONE)) {
      AddRoot(
          *(new TMeshRegionEntryObserver((CHexaMeshRegionEntry &)(*Document()->Model()->GraphEntry(MD_HEXA_MESH_ZONE)),
                                         GetTreeCtrl(), FALSE, DELETE_ITEM, hModel, TVI_LAST, TRUE, FALSE)));
    }

    if (Document()->Model()->GraphEntry(MD_BASE_ANALYSIS_POINT))
      AddRoot(*(new TAnalysisPointEntryObserver(*Document()->Model()->GraphEntry(MD_BASE_ANALYSIS_POINT), GetTreeCtrl(),
                                                FALSE, DELETE_ITEM, hModel, TVI_LAST, TRUE, FALSE)));

    if (Document()->Model()->GraphEntry(MD_BASE_XSECTION)) {
      AddRoot(*(new TXSectionEntryObserver((CCrossSectionEntry &)(*Document()->Model()->GraphEntry(MD_BASE_XSECTION)),
                                           GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST, TRUE, FALSE)));
    }

    if (Document()->Model()->GraphEntry(MD_NEW_WELLPATH)) // wjrx mantis 3185
    {
      CNewWellPathEntry *wellPathEntry =
          static_cast<CNewWellPathEntry *>(Document()->Model()->GraphEntry(MD_NEW_WELLPATH));
      CGroupNodeEntry *groupNodeEntry =
          static_cast<CGroupNodeEntry *>(Document()->Model()->GraphEntry(MD_BASE_GROUPNODE));

      auto observer = new CNewWellPathEntryObserver(*wellPathEntry, GetTreeCtrl(), FALSE, FIXED_ITEM, hModel, TVI_LAST,
                                                    TRUE, FALSE);
      AddRoot(*observer);

      if (groupNodeEntry) {
        CGroupNodeEntry::TNodeSet nodes = groupNodeEntry->EntryNodes();

        for (CGroupNodeEntry::TNodeSet::iterator node = nodes.begin(); node != nodes.end(); ++node) {
          if ((*node)->GraphEntry() == wellPathEntry)
            new TNewWellPathGroupNodeObserver(**node, *observer);
        }
      }
    }

    if (Document()->Model()->GraphEntry(MD_BASE_ZOOMIN_MODEL))
      AddRoot(
          *(new TZoominModelEntryObserver((CZoominModelEntry &)(*Document()->Model()->GraphEntry(MD_BASE_ZOOMIN_MODEL)),
                                          GetTreeCtrl(), TRUE, FIXED_ITEM, hModel)));

    if (Document()->Model()->GraphEntry(MD_BASE_SCENE_NODE)) {
      AddRoot(*new TOIVSceneEntryObserver((TOIVSceneBaseEntry &)(*Document()->Model()->GraphEntry(MD_BASE_SCENE_NODE)),
                                          GetTreeCtrl(), FALSE, FIXED_ITEM, TVI_ROOT, TVI_LAST));
    }

    CGeomecDoc &doc = (CGeomecDoc &)(*Document());
    AddRoot(*(new TMeshEntryObs(*Document()->Model()->GraphEntry(MD_BASE_MESH_RESULT), GetTreeCtrl(), doc,
                                &CGeomecDoc::SelectComposite, &CGeomecDoc::SelectComposite,
                                &CGeomecDoc::CanSelectComposite)));

    AddRoot(*(new CResultGroupObserver(pModel->MaterialResultTree(), *this, TVI_ROOT, false)));

    AddRoot(*(new CResultGroupObserver(pModel->ResultTree(), *this, TVI_ROOT, true)));

    AddRoot(*(new CGVTResultGroupObserver(pModel->GVTResultTree(), *this, TVI_ROOT, false, false)));

    if (Document()->Model()->GraphEntry(MD_BASE_EXPORT_MACROS)) {
      AddRoot(*(new TExportResultDataEntryObserver(
          (CExportResultDataEntry &)(*Document()->Model()->GraphEntry(MD_BASE_EXPORT_MACROS)), GetTreeCtrl(), FALSE,
          FIXED_ITEM, TVI_ROOT, TVI_LAST, TRUE, FALSE)));
    }

    // Expand the model item in the tree. Solution to mantis error NR. 5
    GetTreeCtrl().Expand(hModel, TVE_EXPAND);
    pObject->Update();
  }
}

// ##ModelId=3BC55D6101AC
void CModelTreeView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint) {
  switch (lHint) {
  case NEW_MODEL:
    DeleteTree();
    OnInitialUpdate();
    break;
  case CURRENT_SCENE_MODIFIED:
    if (pSender != this) {
      GetTreeCtrl().UnSelectStateless();
    }
  case NEW_CURRENT_SCENE:
  case SWITCH_TO_OPENINVENTOR:
    GetTreeCtrl().UpdateTree();
    break;

  default:
    assert(TRUE); // Impossible
    break;
  }
}

// ##ModelId=3BC55D61019F
BOOL CModelTreeView::DestroyWindow() {
  DeleteTree();

  return CTreeViewBase::DestroyWindow();
}

void CModelTreeView::OnSelect(ITreeObject &tree_object) {
  CGeomecDoc &doc = (CGeomecDoc &)(*Document());

  // Try to view component and value type with the OIV viewer
  BOOL bAccepted = FALSE;
  TValueComponentObserver *pComponentObserver = dynamic_cast<TValueComponentObserver *>(&tree_object);
  CValueCompositeObserver *pCompositeObserver = dynamic_cast<CValueCompositeObserver *>(&tree_object);
  if ((pComponentObserver || pCompositeObserver) && !doc.IsDeleting()) {
    assert(doc.Model());
    CFemAppModel *pModel = dynamic_cast<CFemAppModel *>(doc.Model());
    CFemAppGUI *pGUI = dynamic_cast<CFemAppGUI *>(doc.GUI());
    if (pModel && pGUI) {
      CFemAppModel &model = *pModel;
      CFemAppGUI &gui = *pGUI;

      ISceneWrapper *pCurrent = doc.CurrentScene();
      COpenInventorSceneWrapper *pOIVSceneWrapper = dynamic_cast<COpenInventorSceneWrapper *>(pCurrent);

      if (pOIVSceneWrapper != 0) {
        if (pComponentObserver) {
          // if (pOIVSceneWrapper->ValueComponent() != &pComponentObserver->ObservedItem())
          bAccepted = pOIVSceneWrapper->ViewComponent(*pComponentObserver);
        } else {
          IValueComposite &composite = dynamic_cast<IValueComposite &>(pCompositeObserver->ObservedItem());
          // if(pOIVSceneWrapper->ValueComponent() != &composite.Component())
          bAccepted = pOIVSceneWrapper->ViewComposite(*pCompositeObserver);
        }
      }
    }
  }

  if (!bAccepted)
    CGraphTreeView::OnSelect(tree_object);

  doc.UpdateAllViews(this, CURRENT_SCENE_MODIFIED, 0);
}

void CModelTreeView::OnSize(unsigned int nType, int cx, int cy) {
  CView::OnSize(nType, cx, cy);
  GetTreeCtrl().MoveWindow(0, m_ButtonHeight + 2, cx, cy - m_ButtonHeight - 2);
}

void CModelTreeView::OnLButtonDown(UINT nFlags, CPoint point) {
  if (m_viewbuttons[0]) {
    for (int i = 0; i < 3; ++i)
      m_viewbuttons[i]->OnLButtonDown(nFlags, point);
  }
}

void CModelTreeView::OnLButtonUp(UINT nFlags, CPoint point) {
  if (m_viewbuttons[0]) {
    for (int i = 0; i < 3; ++i)
      m_viewbuttons[i]->OnLButtonUp(nFlags, point);
  }
}

void CModelTreeView::OnMouseMove(UINT nFlags, CPoint point) {
  if (m_viewbuttons[0]) {
    for (int i = 0; i < 3; ++i)
      m_viewbuttons[i]->OnMouseMove(nFlags, point);
  }
}

void CModelTreeView::OnMouseLeave() {
  if (m_viewbuttons[0]) {
    for (int i = 0; i < 3; ++i)
      m_viewbuttons[i]->OnMouseLeave();
  }
}

void CModelTreeView::OnChangeClicked() {
  CGeomecDoc *pDoc = (CGeomecDoc *)GetDocument();
  assert(!pDoc->ChangeMode());

  pDoc->EnterChangeMode();

  m_viewbuttons[0]->setVisible(false);
  m_viewbuttons[1]->setVisible(true);
  m_viewbuttons[2]->setVisible(true);
}

void CModelTreeView::OnDeleteClicked() {
  CGeomecDoc *pDoc = (CGeomecDoc *)GetDocument();
  assert(pDoc->ChangeMode());

  pDoc->ChangeModeDeleteSelectedItems();

  m_viewbuttons[0]->setVisible(true);
  m_viewbuttons[1]->setVisible(false);
  m_viewbuttons[2]->setVisible(false);
}

void CModelTreeView::OnCancelClicked() {
  CGeomecDoc *pDoc = (CGeomecDoc *)GetDocument();
  assert(pDoc->ChangeMode());

  pDoc->ExitChangeMode();

  m_viewbuttons[0]->setVisible(true);
  m_viewbuttons[1]->setVisible(false);
  m_viewbuttons[2]->setVisible(false);
}
