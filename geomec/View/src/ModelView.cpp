// ModelView.cpp : implementation file
//


#include "stdafx.h"
#include "geomec.h"
#include "ModelView.h"
#include "Geomecdoc.h"
#include "GeoSurface.h"
#include "hexameshregion.h"
#include "HexaEntryTypes.h"
#include "CrossSection.h"
#include "ModelBase.h"
#include "TetraModel.h"
#include "HexaModel.h"
#include "TetraEntryTypes.h"
#include "BoundaryBase.h"
#include "MainFrm.h"
#include "ZoomWindowDragger.h"
#include "ColorScaleEntry.h"
#include "TnoFileDialog.h"
#include "CrossSection_Delegate.h"
#include "OpenGLSceneBase_Delegate.h"
#include "ColorScaleEntry_Delegate.h"
#include "HexaMeshRegion_Delegate.h"
#include "3DGUI.h"
#include "OpenInventorSceneWrapper.h"
#include "OIV2DScene.h"
#include "MeshMode.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

void CALLBACK TimerCallback(
  HWND hWnd,      // handle of CWnd that called SetTimer
  UINT nMsg,      // WM_TIMER
  UINT_PTR nIDEvent,   // timer identification
  DWORD dwTime    // system time
  )
{
  CWnd * wnd = CWnd::FromHandle(hWnd);

  assert (dynamic_cast<CModelView *> (wnd) != 0);
  CModelView * mv = static_cast<CModelView *> (wnd);

  mv->OnTimerCalled();
}

/////////////////////////////////////////////////////////////////////////////
// CModelView

IMPLEMENT_DYNCREATE(CModelView, COpenGLView)

//##ModelId=3BC55D610110
CModelView::CModelView()
: m_pLegend(0),
  m_pSceneWrapper(0),
  m_timerId(0)
{
}

//##ModelId=3BC55D610116
CModelView::~CModelView()
{
}

void CModelView::OnDraw(CDC* pDC)
{
  if(m_pSceneWrapper)
  {
      if(m_pSceneWrapper->OpenInventorScene() || m_pSceneWrapper->getOIV2DScene())
      {
    MakeCurrent();
    if(m_pSceneWrapper->OpenInventorScene())
          m_pSceneWrapper->OpenInventorScene()->Render();
    else
          m_pSceneWrapper->getOIV2DScene()->render();
    FlipBuffers();
      }
  }
  else
  {
    CFemAppOpenGLView::OnDraw(pDC);
  }

  if (m_timerId == 0)
    m_timerId = SetTimer(2, 1000, &TimerCallback);
}

BEGIN_MESSAGE_MAP(CModelView, COpenGLView)
  //{{AFX_MSG_MAP(CModelView)
  ON_COMMAND(ID_VIEW_ZOOMWINDOW, OnZoomWindow)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMWINDOW, OnUpdateZoomWindow)
  ON_COMMAND(ID_INSERT_MESH_REGION_POLY, OnInsertPolygonalMeshRegion)
  ON_UPDATE_COMMAND_UI(ID_INSERT_MESH_REGION_POLY, OnUpdateInsertMeshRegion)
  ON_COMMAND(ID_INSERT_MESH_REGION_RECT, OnInsertRectangularMeshRegion)
  ON_UPDATE_COMMAND_UI(ID_INSERT_MESH_REGION_RECT, OnUpdateInsertMeshRegion)
  ON_COMMAND(ID_INSERT_X_SECTION, OnInsertXSection)
  ON_UPDATE_COMMAND_UI(ID_INSERT_X_SECTION, OnUpdateInsertXSection)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_DESTROY()
  ON_WM_ERASEBKGND()
  ON_WM_PALETTECHANGED()
  ON_WM_QUERYNEWPALETTE()
  ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
  ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
  ON_WM_KILLFOCUS()
  ON_UPDATE_COMMAND_UI(ID_COPYPICTURE, OnUpdateCopyPicture)
  ON_COMMAND(ID_COPYPICTURE, OnCopyPicture)
  ON_COMMAND(ID_FILE_SAVE_GRAPHICS, OnSaveGraphics) // wjrx mantis 2461
  //}}AFX_MSG_MAP
  ON_COMMAND(ID_BEACHBALL, OnViewBeachBalls)
  ON_COMMAND(ID_VIEW_COLOR, OnViewColor)
  ON_COMMAND(ID_VIEW_FILL, OnViewFill)
  ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
  ON_COMMAND(ID_VIEW_VALUE, OnViewValue)
  ON_UPDATE_COMMAND_UI(ID_VIEW_COLOR, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_SQUARE, OnUpdateViewMenu)
  ON_COMMAND(ID_VIEW_SQUARE, OnViewSquare)
  ON_COMMAND(ID_VIEW_ROTATE, OnViewRotate)
  ON_COMMAND(ID_VIEW_AXIS, OnViewAxis)
  ON_COMMAND(ID_COORD_AXIS, OnViewCoordinateAxis)
  ON_COMMAND(ID_FORMATIONFULL, OnFormationFull)
  ON_COMMAND(ID_FORMATIONSKIN, OnFormationSkin)
  ON_COMMAND(ID_FORMATIONCENTER, OnFormationCenter)
  ON_UPDATE_COMMAND_UI(ID_FORMATIONFULL, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_FORMATIONSKIN, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_FORMATIONCENTER, OnUpdateViewMenu)
  ON_COMMAND(ID_DRAGGERTRACKER_MODE, OnSelectDraggerTracker)
  ON_COMMAND(ID_MOUSETRACKER_MODE, OnSelectMouseTracker)
  ON_COMMAND(ID_HIDELEGEND, OnSelectHideLegend)
  ON_COMMAND(ID_CLIPPLANE_MODE, OnSelectClipPlaneMode)
  ON_COMMAND(ID_CONTOURLINE_MODE, OnSelectContourLineMode)
  ON_COMMAND(ID_MESHLINE_MODE, OnSelectMeshLinesDisplayed)
  ON_COMMAND(ID_FORMATIONOUTLINE_MODE, OnSelectFormationOutlinesDisplayed)
  ON_COMMAND(ID_ELEMENT_MODE, OnShowValuesPerElement)
  ON_COMMAND(ID_VIEW_BOUNDARY, OnViewBoundary)
  ON_COMMAND(ID_VIEW_GLOBAL, OnViewGlobal)
  ON_COMMAND(ID_VIEW_LOCAL, OnViewLocal)
  ON_COMMAND(ID_VIEW_HOTSPOT, OnViewHotspot)
  ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
  ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
  ON_COMMAND(ID_ZOOM_RESET, OnZoomReset)
  ON_COMMAND(ID_VIEW_LOCK_X, OnLockX)
  ON_COMMAND(ID_VIEW_LOCK_Y, OnLockY)
  ON_COMMAND(ID_VIEW_LOCK_Z, OnLockZ)
  ON_COMMAND(ID_UNLOCK_AXES, OnUnlockAxes)
  ON_COMMAND(ID_VIEW_LIGHTING, OnViewLighting)
  ON_COMMAND(ID_VIEW_N_DIREC, OnViewNDirection)
  ON_COMMAND(ID_VIEW_Z_DIREC, OnViewZDirection)
  ON_COMMAND(ID_VIEW_BE_DIREC, OnViewBirdEyeDirection)
  ON_COMMAND(ID_VIEW_E_DIREC, OnViewEDirection)
  ON_COMMAND(ID_BUTTON_GOHOME, OnGoHome)
  ON_COMMAND(ID_BUTTON_SAVEHOME, OnSaveHome)
  ON_COMMAND(ID_VIEW_COLORSCALE, OnViewColorscale)

  ON_COMMAND(ID_TENSOR_FULL, OnViewTensorVectorFull)
  ON_COMMAND(ID_TENSOR_NORMAL, OnViewTensorVectorNormal)
  ON_COMMAND(ID_TENSOR_SHEAR, OnViewTensorVectorShear)
  ON_COMMAND(ID_BASESCALEREFRESH, OnBaseScaleRefresh)

  ON_UPDATE_COMMAND_UI(ID_VIEW_VALUE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ROTATE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_AXIS, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_COORD_AXIS, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_DRAGGERTRACKER_MODE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_MOUSETRACKER_MODE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_HIDELEGEND, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_CLIPPLANE_MODE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_CONTOURLINE_MODE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_MESHLINE_MODE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_FORMATIONOUTLINE_MODE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_ELEMENT_MODE, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_BOUNDARY, OnUpdateViewMenu)
  ON_COMMAND(ID_VIEW_BOUNDARY, OnViewBoundary)
  ON_UPDATE_COMMAND_UI(ID_VIEW_BOUNDARY, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIONS, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_FILL, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_BEACHBALL, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_RESET, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_LOCK_X, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_LOCK_Y, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_LOCK_Z, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_UNLOCK_AXES, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_TENSOR_FULL, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_TENSOR_NORMAL, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_TENSOR_SHEAR, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_BASESCALEREFRESH, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_LIGHTING, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_N_DIREC, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_Z_DIREC, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_E_DIREC, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_VIEW_BE_DIREC, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_BUTTON_GOHOME, OnUpdateViewMenu)
  ON_UPDATE_COMMAND_UI(ID_BUTTON_SAVEHOME, OnUpdateViewMenu)
  ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
  ON_WM_HSCROLL()
  ON_MESSAGE(WM_SCALE_VECTORS, &CModelView::OnScaleVectors)
  ON_MESSAGE(WM_DECIMATE_VECTORS, &CModelView::OnSetVectorVisibleFraction)
  ON_UPDATE_COMMAND_UI(IDC_SCALE_SLIDER, &CModelView::OnUpdateScaleSlider)
  ON_UPDATE_COMMAND_UI(IDC_DECIMATE_SLIDER, &CModelView::OnUpdateDecimateSlider)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelView drawing



/////////////////////////////////////////////////////////////////////////////
// CModelView diagnostics

#ifdef _DEBUG
//##ModelId=3BC55D610120
void CModelView::AssertValid() const
{
  CView::AssertValid();
}

//##ModelId=3BC55D610122
void CModelView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CModelView message handlers

//##ModelId=3BC55D610111
void CModelView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  CGeomecDoc *pDoc = GetGeomecDoc();
  assert(pDoc);

  if(lHint == NEW_CURRENT_SCENE || lHint == SWITCH_TO_OPENINVENTOR)
  {
    if(pHint != 0)
    {
      ISceneWrapper *pScene = reinterpret_cast<ISceneWrapper *>(pHint);
      pDoc->CurrentScene(pScene);
      m_pSceneWrapper = pDoc->CurrentScene();

      assert(m_pSceneWrapper);
      assert(m_pSceneWrapper == pScene);

      CRect rect;
      GetClientRect(&rect);
      m_pSceneWrapper->OnSize(rect.Width(), rect.Height());
    }
    else
    {
      m_pSceneWrapper = 0;
    }
  }

  CFemAppOpenGLView::OnUpdate(pSender, lHint, pHint);
}

const ISceneWrapper& CModelView::SceneWrapper() const
{
  assert(m_pSceneWrapper);
  return *m_pSceneWrapper;
}


void CModelView::EnterView()
{
  if (m_pSceneWrapper)
  {
  if (m_pSceneWrapper->OpenInventorScene())
      m_pSceneWrapper->OpenInventorScene()->EnterView();
  }
}

void CModelView::LeaveView()
{
  if (m_pSceneWrapper)
  {
  if (m_pSceneWrapper->OpenInventorScene())
      m_pSceneWrapper->OpenInventorScene()->LeaveView();
  }
}

bool CModelView::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if (m_pSceneWrapper)
  {
    bool handled = false;

    if(m_pSceneWrapper->OpenInventorScene())
      handled = m_pSceneWrapper->OpenInventorScene()->MousePress(state, button, point);
    else if(m_pSceneWrapper->getOIV2DScene())
      handled = m_pSceneWrapper->getOIV2DScene()->MousePress(state, button, point);

    if(handled)
      Invalidate();

    return handled;
  }

  return CFemAppOpenGLView::MousePress(state, button, point);
}

bool CModelView::MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if (m_pSceneWrapper)
  {
    bool handled = false;

    if(m_pSceneWrapper->OpenInventorScene())
      handled = m_pSceneWrapper->OpenInventorScene()->MouseRelease(state, button, point);
    else if(m_pSceneWrapper->getOIV2DScene())
      handled = m_pSceneWrapper->getOIV2DScene()->MouseRelease(state, button, point);

    if(handled)
      Invalidate();

    return handled;
  }

  return CFemAppOpenGLView::MouseRelease(state, button, point);
}

bool CModelView::MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if (m_pSceneWrapper && m_pSceneWrapper->OpenInventorScene())
  return m_pSceneWrapper->OpenInventorScene()->MouseDblClk(state, button, point);
  else
  return CFemAppOpenGLView::MouseDblClk(state, button, point);
}

bool CModelView::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if (m_pSceneWrapper)
  {
    bool handled = false;

    if(m_pSceneWrapper->OpenInventorScene())
      handled = m_pSceneWrapper->OpenInventorScene()->MouseMove(state, button, point);
    else if(m_pSceneWrapper->getOIV2DScene())
      handled = m_pSceneWrapper->getOIV2DScene()->MouseMove(state, button, point);

    if(handled)
      Invalidate();

    return handled;
  }

  return CFemAppOpenGLView::MouseMove(state, button, point);
}

bool CModelView::MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point)
{
  if (m_pSceneWrapper)
  {
    bool handled  = false;
    if (m_pSceneWrapper && m_pSceneWrapper->OpenInventorScene())
      handled =  m_pSceneWrapper->OpenInventorScene()->MouseWheel(state, nDelta, point);
    else if(m_pSceneWrapper->getOIV2DScene())
      handled = m_pSceneWrapper->getOIV2DScene()->MouseWheel(state, nDelta, point);

    if(handled)
      Invalidate();

    return handled;
  }

  return CFemAppOpenGLView::MouseWheel(state, nDelta, point);
}

bool CModelView::KeyPress(TKeyboardModifiers state, int nChar, TKey key, int nRepCount)
{
  if (m_pSceneWrapper && m_pSceneWrapper->OpenInventorScene())
  return m_pSceneWrapper->OpenInventorScene()->KeyPress(state, nChar, key, nRepCount);
  else
  return CFemAppOpenGLView::KeyPress(state, nChar, key, nRepCount);
}

bool CModelView::KeyRelease(TKeyboardModifiers state, int nChar, TKey key, int nRepCount)
{
  if (m_pSceneWrapper && m_pSceneWrapper->OpenInventorScene())
  return m_pSceneWrapper->OpenInventorScene()->KeyRelease(state, nChar, key, nRepCount);
  else
  return CFemAppOpenGLView::KeyRelease(state, nChar, key, nRepCount);
}


//see if a mesh node is selected in the tree
BOOL CModelView::MeshSelectionMode()
{
  CGeomecDoc* pDoc = (CGeomecDoc*) GetDocument();
  if (pDoc->Model())
  {
    CHexaMeshRegionEntry *pEntry=(CHexaMeshRegionEntry*)pDoc->Model()->GraphEntry(MD_HEXA_MESH_ZONE);

    if(pDoc->SelectedNode()==pEntry)
    {
      TRACE("mesh node entry\n");
      return TRUE;
    }
    else
    {
      TGraphNodeSet stNode = pEntry->GraphEntryNodes();
      for(TGraphNodeSet::iterator it = stNode.begin();it != stNode.end();it++)
      {
        if(*it == pDoc->SelectedNode())
        {
          TRACE("mesh node child\n");
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}


void CModelView::OnZoomWindow() 
{
  if(m_pSceneWrapper)
  m_pSceneWrapper->OnZoomWindow();
}

void CModelView::OnUpdateZoomWindow(CCmdUI* pCmdUI) 
{
  if(m_pSceneWrapper == 0)
    pCmdUI->Enable(FALSE);
  else
  {
    if(m_pSceneWrapper->CanZoomWindow())
      pCmdUI->SetCheck(1);
    else
      pCmdUI->SetCheck(0);
  }
}

void CModelView::OnInsertRectangularMeshRegion()
{
  CHexaMeshRegionEntry& entry = dynamic_cast<CHexaMeshRegionEntry&>(*Model()->GraphEntry(MD_HEXA_MESH_ZONE));

  (static_cast <CHexaMeshRegionEntry_Delegate*> (entry.getDelegate()))->
  CreateMeshRegionRect();
}

void CModelView::OnInsertPolygonalMeshRegion() 
{
  CHexaMeshRegionEntry& entry = dynamic_cast<CHexaMeshRegionEntry&>(*Model()->GraphEntry(MD_HEXA_MESH_ZONE));

  (static_cast <CHexaMeshRegionEntry_Delegate*> (entry.getDelegate()))->
  CreateMeshRegionPoly();
}

void CModelView::OnUpdateInsertMeshRegion(CCmdUI* pCmdUI) 
{
  if(dynamic_cast<CHexaModel*>(Model()))
  {
    CHexaMeshRegionEntry& entry = dynamic_cast<CHexaMeshRegionEntry&>(*Model()->GraphEntry(MD_HEXA_MESH_ZONE));

  pCmdUI->Enable(
      (static_cast <CHexaMeshRegionEntry_Delegate*> (entry.getDelegate()))->
    CanCreateMeshRegion());
  }
  else 
    pCmdUI->Enable(FALSE);
}

CModelBase* CModelView::Model()
{
  return ((CModelBase*) ((CFemAppDoc*)GetDocument())->Model());
}

void CModelView::SwitchToTopView()
{
  if(m_pSceneWrapper)
  m_pSceneWrapper->SwitchToTopView();
}

void CModelView::OnInsertXSection() 
{
  assert(Model());
  // If a mouselistener of this type is already instantiated-> kill him
  {
    CCrossSectionEntry* pEntry = dynamic_cast<CCrossSectionEntry*>(Model()->GraphEntry(MD_BASE_XSECTION));
    assert(pEntry);
    CCrossSectionEntry_Delegate* pEntry_Delegate = new CCrossSectionEntry_Delegate(pEntry);
    assert(pEntry_Delegate);
    pEntry_Delegate->InsertGraphical();
  }
}

void CModelView::OnUpdateInsertXSection(CCmdUI* pCmdUI) 
{
  if(Model())
  {
    CCrossSectionEntry *pEntry = dynamic_cast<CCrossSectionEntry*>(Model()->GraphEntry(MD_BASE_XSECTION));
    CCrossSectionEntry_Delegate *pEntry_Delegate = new CCrossSectionEntry_Delegate(pEntry);
    pCmdUI->Enable( pEntry && pEntry_Delegate && pEntry_Delegate->CanInsertNew() );
  }
  else
    pCmdUI->Enable(FALSE);

  pCmdUI->SetCheck(0);
}

void CModelView::OnKillFocus(CWnd* pNewWnd) 
{
  // Validate all mouselisteners
//	ValidateAllMouseListeners();

  COpenGLView::OnKillFocus(pNewWnd);
}

void CModelView::OnUpdateCopyPicture(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(Model() != 0);
}

void CModelView::OnCopyPicture()
{
  CopyClipboard();
}


void CModelView::OnSaveGraphics()  // wjrx mantis 2461
{
  CString sFileName;
  CTnoFileDialog dlg( FALSE, "bmp", sFileName,
      OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "Bitmap (*.bmp)|*.bmp||", NULL );
  if ( dlg.DoModal() == IDOK )
  {
  CString sPath = dlg.GetPathName();
  if ( CopyBitmapToFile( dlg.GetPathName() ) == FALSE )
  {
      _m()->msg("Save Graphics Screen failed.");
  }
  }
}

//##ModelId=3BC55D63012E
void CModelView::OnViewColor() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ShowColor,true);
}

//##ModelId=3BC55D630132
void CModelView::OnViewValue() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ShowColor,false);
}


void CModelView::PerformInvalidatingOIVSceneWrapperFunction (void (ISceneWrapper::*func_ptr)())
{
  if(m_pSceneWrapper)
  {
  (m_pSceneWrapper->*func_ptr)();
  if (m_pSceneWrapper->OpenInventorScene() || m_pSceneWrapper->getOIV2DScene())
  {
      Invalidate();
  }
  }
}


//##ModelId=3BC55D630151
void CModelView::OnViewBoundary() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ShowBoundary, !m_pSceneWrapper->ShowBoundary());
}

//##ModelId=3BE7AEC30256
void CModelView::OnViewOptions() 
{
  if(m_pSceneWrapper)
  m_pSceneWrapper->ShowOptionsDialog();
}

//##ModelId=3BC55D630143
void CModelView::OnViewRotate() 
{
  if(m_pSceneWrapper)
  m_pSceneWrapper->AllowDynamicRotate(!m_pSceneWrapper->AllowDynamicRotate());
}

//##ModelId=3BC55D63014F
void CModelView::OnViewAxis() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::DisplayGnomon, !m_pSceneWrapper->DisplayGnomon());
}

void CModelView::OnViewCoordinateAxis() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::DisplayCoordinateAxis, !m_pSceneWrapper->DisplayCoordinateAxis());
}

void CModelView::SelectMeshMode( MeshMode mode )
{
  if(m_pSceneWrapper)
  {
  m_pSceneWrapper->SelectMeshMode(mode);
  if (m_pSceneWrapper->OpenInventorScene())
  {
      Invalidate();
  }
  }
}

void CModelView::OnFormationFull() 
{
  SelectMeshMode(FullMesh);
}

void CModelView::OnFormationSkin() 
{
  SelectMeshMode(SkinMesh);
}

void CModelView::OnFormationCenter() 
{
  SelectMeshMode(CenterMesh);
}

void CModelView::OnSelectDraggerTracker() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SelectDraggerTracker, !m_pSceneWrapper->DraggerTrackerSelected());
}

void CModelView::OnSelectMouseTracker() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SelectMouseTracker, !m_pSceneWrapper->MouseTrackerSelected());
}

void CModelView::OnSetDraggerConfined() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SetDraggerConfined, !m_pSceneWrapper->DraggerConfined());
}

void CModelView::OnSelectHideLegend() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::HideLegend, !m_pSceneWrapper->LegendHidden());
}

void CModelView::OnSelectClipPlaneMode()
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SelectClipPlaneMode, !m_pSceneWrapper->ClipPlaneModeSelected());
}

void CModelView::OnSelectContourLineMode()
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SelectContourLineMode, !m_pSceneWrapper->ContourLineModeSelected());
}

void CModelView::OnSelectMeshLinesDisplayed()
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SelectMeshLinesDisplayed, !m_pSceneWrapper->MeshLinesDisplayed());
}

void CModelView::OnSelectFormationOutlinesDisplayed()
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SelectFormationOutlinesDisplayed, !m_pSceneWrapper->FormationOutlinesDisplayed());
}

void CModelView::OnShowValuesPerElement() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ShowValuesPerElement, !m_pSceneWrapper->ShowValuesPerElement());
}

void CModelView::OnViewFill() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::Fill, !m_pSceneWrapper->Fill());
}

void CModelView::OnViewBeachBalls() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ViewBeachBalls, !m_pSceneWrapper->BeachBallsOnView());
}

//##ModelId=3BE7AEC302A6
void CModelView::OnViewLocal()
{
  CColorScaleEntry *pEntry = dynamic_cast<CColorScaleEntry*> (Model()->GraphEntry(MD_BASE_COLOR_SCALE));
  assert(pEntry);
  pEntry->ColorScaleType(CColorScaleEntry::LOCAL);
}

//##ModelId=3BE7AEC30295
void CModelView::OnViewGlobal()
{
  CColorScaleEntry *pEntry = dynamic_cast<CColorScaleEntry*> (Model()->GraphEntry(MD_BASE_COLOR_SCALE));
  assert(pEntry);
  pEntry->ColorScaleType(CColorScaleEntry::GLOBAL);
}

//##ModelId=3BE7AEC302C4
void CModelView::OnViewHotspot()
{
  CColorScaleEntry *pEntry = dynamic_cast<CColorScaleEntry*> (Model()->GraphEntry(MD_BASE_COLOR_SCALE));
  assert(pEntry);
  pEntry->ColorScaleType(CColorScaleEntry::HOTSPOT);
}

void CModelView::OnZoomIn() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ZoomIn);
}

void CModelView::OnTimerCalled() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::TimerCalled);
}

void CModelView::OnZoomOut() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ZoomOut);
}

void CModelView::OnZoomReset() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ZoomReset);
}

void CModelView::OnLockX() 
{
  if(m_pSceneWrapper)
  m_pSceneWrapper->LockRotationVector(geo::CVector::Yaxis);
}

void CModelView::OnLockY() 
{
  if(m_pSceneWrapper)
  m_pSceneWrapper->LockRotationVector(geo::CVector::Xaxis);
}

void CModelView::OnLockZ() 
{
  if(m_pSceneWrapper)
  m_pSceneWrapper->LockRotationVector(geo::CVector::Zaxis);
}

void CModelView::OnUnlockAxes() 
{
  if(m_pSceneWrapper)
  m_pSceneWrapper->LockRotationVector(geo::CVector::NullVector);
}

void CModelView::OnViewTensorVectorFull() 
{
  if(m_pSceneWrapper)
    m_pSceneWrapper->ViewTensorVector(TensorVectorFull);
}

void CModelView::OnViewTensorVectorNormal() 
{
  if(m_pSceneWrapper)
    m_pSceneWrapper->ViewTensorVector(TensorVectorNormal);
}
void CModelView::OnViewTensorVectorShear() 
{
  if(m_pSceneWrapper)
    m_pSceneWrapper->ViewTensorVector(TensorVectorShear);
}

void CModelView::OnViewEDirection() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ViewEDirection);
}

void CModelView::OnViewNDirection() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ViewNDirection);
}

void CModelView::OnViewZDirection() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ViewZDirection);
}

void CModelView::OnGoHome() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::GoHome);
}

void CModelView::OnSaveHome() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SaveHome);
}

void CModelView::OnViewBirdEyeDirection() 
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ViewBirdEyeDirection);
}

void CModelView::OnViewSquare()
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::ViewProportional, !m_pSceneWrapper->ViewProportional());
}

void CModelView::OnViewPerspective()
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::DisplayPerspective, !m_pSceneWrapper->DisplayPerspective());
}

void CModelView::OnSize(unsigned int nType, int cx, int cy)
{
  CFemAppOpenGLView::OnSize(nType, cx, cy);

  if(m_pSceneWrapper)
  m_pSceneWrapper->OnSize(cx, cy);
}

void CModelView::OnUpdateViewMenu(CCmdUI* pCmdUI) 
{
  if(m_pSceneWrapper)
  {
    switch(pCmdUI->m_nID)
    {
    case ID_BUTTON_ZOOMWINDOW:
      pCmdUI->Enable(m_pSceneWrapper->CanZoomWindow());
      break;
    case ID_VIEW_Z_DIREC:
      pCmdUI->Enable(m_pSceneWrapper->CanAllowDynamicRotate());
      break;
    case ID_VIEW_N_DIREC:
      pCmdUI->Enable(m_pSceneWrapper->CanAllowDynamicRotate());
      break;
    case ID_VIEW_E_DIREC:
      pCmdUI->Enable(m_pSceneWrapper->CanAllowDynamicRotate());
      break;
  case ID_BUTTON_GOHOME:
    pCmdUI->Enable(m_pSceneWrapper->CanGoHome());
    break;
  case ID_BUTTON_SAVEHOME:
    pCmdUI->Enable(m_pSceneWrapper->CanSaveHome());
    break;
    case ID_VIEW_BE_DIREC:
      pCmdUI->Enable(m_pSceneWrapper->CanAllowDynamicRotate());
      break;
    case ID_VIEW_AXIS:
          pCmdUI->Enable(m_pSceneWrapper->CanDisplayGnomon());
          pCmdUI->SetCheck(m_pSceneWrapper->DisplayGnomon());
          break;
    case ID_COORD_AXIS:
          pCmdUI->Enable(m_pSceneWrapper->CanDisplayCoordinateAxis());
          pCmdUI->SetCheck(m_pSceneWrapper->DisplayCoordinateAxis());
          break;
    case ID_FORMATIONFULL:
          pCmdUI->Enable(m_pSceneWrapper->CanSelectMeshMode());
          pCmdUI->SetRadio(m_pSceneWrapper->MeshModeSelected() == FullMesh);
          break;
    case ID_FORMATIONSKIN:
          pCmdUI->Enable(m_pSceneWrapper->CanSelectMeshMode());
          pCmdUI->SetRadio(m_pSceneWrapper->MeshModeSelected() == SkinMesh);
          break;
    case ID_FORMATIONCENTER:
          pCmdUI->Enable(m_pSceneWrapper->CanSelectMeshMode());
          pCmdUI->SetRadio(m_pSceneWrapper->MeshModeSelected() == CenterMesh);
          break;
    case ID_DRAGGERTRACKER_MODE:
      pCmdUI->Enable(m_pSceneWrapper->CanSelectDraggerTracker());
      pCmdUI->SetRadio(m_pSceneWrapper->DraggerTrackerSelected() == true);
      break;
    case ID_MOUSETRACKER_MODE:
      pCmdUI->Enable(m_pSceneWrapper->CanSelectMouseTracker());
      pCmdUI->SetRadio(m_pSceneWrapper->MouseTrackerSelected() == true);
      break;
    case ID_HIDELEGEND:
      pCmdUI->Enable(m_pSceneWrapper->CanSelectHideLegend());
      pCmdUI->SetCheck(m_pSceneWrapper->LegendHidden() == true);
      break;
    case ID_ELEMENT_MODE:
          pCmdUI->Enable(m_pSceneWrapper->CanShowValuesPerElement());
          pCmdUI->SetRadio(m_pSceneWrapper->ShowValuesPerElement() == true);
          break;
    case ID_CLIPPLANE_MODE:
          pCmdUI->Enable(m_pSceneWrapper->CanSelectClipPlaneMode());
          pCmdUI->SetRadio(m_pSceneWrapper->ClipPlaneModeSelected());
          break;
    case ID_CONTOURLINE_MODE:
      pCmdUI->Enable(m_pSceneWrapper->CanSelectContourLineMode());
      pCmdUI->SetRadio(m_pSceneWrapper->ContourLineModeSelected());
      break;
    case ID_MESHLINE_MODE:
      pCmdUI->Enable(m_pSceneWrapper->CanSelectMeshLineDisplayMode());
      pCmdUI->SetCheck(m_pSceneWrapper->MeshLinesDisplayed());
      break;
    case ID_FORMATIONOUTLINE_MODE:
      pCmdUI->Enable(m_pSceneWrapper->CanSelectFormationOutlineDisplayMode());
      pCmdUI->SetCheck(m_pSceneWrapper->FormationOutlinesDisplayed());
      break;
    case ID_VIEW_FILL:
          pCmdUI->Enable(m_pSceneWrapper->CanFill());
          pCmdUI->SetCheck(m_pSceneWrapper->Fill());
          break;
    case ID_BEACHBALL:
      pCmdUI->Enable(m_pSceneWrapper->CanViewBeachBalls());
      pCmdUI->SetCheck(m_pSceneWrapper->BeachBallsOnView());
      break;
    case ID_VIEW_COLOR:
      pCmdUI->SetRadio(m_pSceneWrapper->ShowColor());
      break;
    case ID_VIEW_VALUE:
      pCmdUI->SetRadio(!m_pSceneWrapper->ShowColor());
      pCmdUI->Enable(m_pSceneWrapper->CanShowValue());
      break;
    case ID_VIEW_PERSPECTIVE:
      pCmdUI->Enable(m_pSceneWrapper->CanDisplayPerspective());
      pCmdUI->SetCheck(m_pSceneWrapper->DisplayPerspective());
      break;
    case ID_VIEW_ROTATE:
      pCmdUI->Enable(m_pSceneWrapper->CanAllowDynamicRotate());
      pCmdUI->SetCheck(m_pSceneWrapper->AllowDynamicRotate());
      break;
    case ID_VIEW_BOUNDARY:
      pCmdUI->Enable(m_pSceneWrapper->CanShowBoundary());
      pCmdUI->SetCheck(m_pSceneWrapper->ShowBoundary());
      break;
    case ID_VIEW_OPTIONS:
      break;
    case ID_ZOOM_IN:
      break;
    case ID_ZOOM_OUT:
      break;
    case ID_ZOOM_RESET:
      break;
    case ID_VIEW_LOCK_X: // EAST
      pCmdUI->Enable(m_pSceneWrapper->CanLockRotationVector());
      pCmdUI->SetRadio(m_pSceneWrapper->LockRotationVector() == geo::CVector::Yaxis);
      break;
    case ID_VIEW_LOCK_Y: // NORTH
      pCmdUI->Enable(m_pSceneWrapper->CanLockRotationVector());
      pCmdUI->SetRadio(m_pSceneWrapper->LockRotationVector() == geo::CVector::Xaxis);
      break;
    case ID_VIEW_LOCK_Z:
      pCmdUI->Enable(m_pSceneWrapper->CanLockRotationVector());
      pCmdUI->SetRadio(m_pSceneWrapper->LockRotationVector() == geo::CVector::Zaxis);
      break;
    case ID_UNLOCK_AXES:
      pCmdUI->Enable(m_pSceneWrapper->CanLockRotationVector());
      break;
    case ID_TENSOR_FULL:
      pCmdUI->Enable(m_pSceneWrapper->CanViewTensorVector());
      pCmdUI->SetRadio(m_pSceneWrapper->ViewTensorVector() == TensorVectorFull);
      break;
    case ID_TENSOR_NORMAL:
      pCmdUI->Enable(m_pSceneWrapper->CanViewTensorVector());
      pCmdUI->SetRadio(m_pSceneWrapper->ViewTensorVector() == TensorVectorNormal);
      break;
    case ID_TENSOR_SHEAR:
      pCmdUI->Enable(m_pSceneWrapper->CanViewTensorVector());
      pCmdUI->SetRadio(m_pSceneWrapper->ViewTensorVector() == TensorVectorShear);
      break;
    case ID_BASESCALEREFRESH:
      pCmdUI->Enable(m_pSceneWrapper->CanRefreshVectorBaseScale());
      break;
    case ID_VIEW_ATTRIBUTES_GLOBAL:
    case ID_VIEW_ATTRIBUTES_LOCAL:
    case ID_VIEW_ATTRIBUTES_HOTSPOT:
      break;
    case ID_VIEW_LIGHTING:
      {
        bool bEnable = m_pSceneWrapper->CanEnableLighting();
        if( !bEnable )
          m_pSceneWrapper->EnableLighting( bEnable );
        pCmdUI->Enable(bEnable);
        pCmdUI->SetCheck(m_pSceneWrapper->Lighting() && bEnable);
      }
      break;
    case ID_VIEW_SQUARE:
      pCmdUI->SetCheck(!m_pSceneWrapper->ViewProportional());
      break;
    default:
      assert(false);
      break;
    }
  }
  else
  {
    pCmdUI->Enable(FALSE);
  }
}

void CModelView::OnViewLighting()
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::Lighting, !m_pSceneWrapper->Lighting());
}

void CModelView::OnViewColorscale() 
{
  CColorScaleEntry *pEntry = dynamic_cast<CColorScaleEntry*> (Model()->GraphEntry(MD_BASE_COLOR_SCALE));
  assert(pEntry);
  CColorScaleEntry_Delegate* pEntry_Delegate =
  new CColorScaleEntry_Delegate(pEntry);
  assert(pEntry_Delegate);
  pEntry_Delegate->Attributes();
}

afx_msg LRESULT CModelView::OnScaleVectors(WPARAM wParam, LPARAM lParam)
{
  if(m_pSceneWrapper)
  {
  float scale = *((float*)&lParam);
  double dScale = *((double*)&lParam);

  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SetVectorScale, scale);
  }

  return 0;
}


afx_msg LRESULT CModelView::OnSetVectorVisibleFraction(WPARAM wParam, LPARAM lParam)
{
  if(m_pSceneWrapper)
  {
  float factor = *((float*)&lParam);

  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::SetVectorVisibleFraction, factor);
  
  }

  return 0;
}


void CModelView::OnUpdateScaleSlider(CCmdUI *pCmdUI)
{
  // TODO: Add your command update UI handler code here
  pCmdUI->Enable(TRUE);
}


void CModelView::OnUpdateDecimateSlider(CCmdUI *pCmdUI)
{
  // TODO: Add your command update UI handler code here
  pCmdUI->Enable(TRUE);
}

void CModelView::OnBaseScaleRefresh()
{
  PerformInvalidatingOIVSceneWrapperFunction(&ISceneWrapper::RefreshVectorBaseScale);

}

