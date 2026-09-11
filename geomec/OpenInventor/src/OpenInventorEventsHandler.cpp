#include "stdafx.h"

#include <Inventor/Gui/viewers/SoGuiAlgoViewers.h>

#include <Inventor/devices/SoGLContext.h>

#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoGradientBackground.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoRotation.h>

#include <Inventor/SoPickedPoint.h>

#include <Inventor/details/SoDetail.h>

#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>

#include <MeshVizXLM/mapping/details/MoFaceDetailI.h>
#include <MeshVizXLM/mapping/details/MoLineDetailI.h>

#include "OpenInventorEventsHandler.h"
#include "OIDIMeshNodeManager.h"
#include "OpenInventorSceneNode.h"
#include "PolyLineScreenDrawerRetainer.h"
#include "Inventor/events/SoKeyboardEvent.h"
#include "ModelView.h"
#include "BusinessAction.h"
#include "CoordinateAxis.h"
#include "GeomecOIV.h"
#include "Gnomon.h"
#include "FemAppEntryTypes.h"
#include "ColorScaleEntry.h"
#include "ColorGradient.h"
#include "HotSpot.h"
#include "FormationBase.h"
#include "MeshResultTree.h"
#include "CrossSection.h"
#include "NewWellPath.h"
#include "TnoWinApp.h"
#include "FieldFactor.h"

#include "MeshRefinementBoxesHandler.h"
#include "MeshRefinementDlg.h"
#include "TetraModel.h"
#include <Inventor/draggers/SoTabBoxDragger.h>

#include "treectrlbase.h"

//#define USE_IVTUNE
#ifdef USE_IVTUNE
#include <IvTune/SoIvTune.h>
#endif

typedef std::function< void( const QString& ) > TFunction;

Qt::CursorShape COpenInventorEventsHandler::s_cursorShape = Qt::ArrowCursor;

COpenInventorEventsHandler::COpenInventorEventsHandler(COpenInventorSceneNode& sceneNode)
    : m_sceneNode(sceneNode)
    , m_oivInitialized(false)
    , m_cameraIsMoving(false)

    , m_draggerTimeOut(5)
    , m_secondsToTimeOut(m_draggerTimeOut)
    , m_firstRender(true)
    , m_updateLegend(false)
    , m_mouseMoved(false)
    , m_dynamicRotate(true)
    , m_phongLighting(true)
    , m_displayPerspective(true)
    , m_viewerAlgos(0)
    , m_cameraSwitch(0)
    , m_perspectiveCamera(0)
    , m_orthoCamera(0)
    , m_currentCamera(0)
    , m_rootSceneGraph(0)
    , m_meshNodeManager(0)
    , m_gnomonSwitch(0)
    , m_bboxSwitch(0)
    , m_bboxScale(0)
    , m_bboxTranslation(0)
    , m_coordinateAxis(0)
    , m_background(0)
    , m_lockedSpinDirection(-1)
    , m_worldAction(0)
    , m_screenDrawerGroup (0)
    , m_screenDrawer(0)
    , m_pointSize(18)
    , m_meshInfoScreenPoint(-1, -1)
    , m_oglContext(0)
	, m_refinement_box_dlg( nullptr )
	, m_boxes( nullptr )
{
    initOIV();
}

COpenInventorEventsHandler::~COpenInventorEventsHandler()
{
    LegendFrame().ClearLegendFrames();

	if( m_boxes ) // Boxes handler
		delete m_boxes;

	if( m_refinement_box_dlg )
		delete m_refinement_box_dlg;
}

// input events
bool COpenInventorEventsHandler::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
    SbVec2s locator(point.x(),  point.y());
    m_viewerAlgos->setCurrentMousePositionLocator( locator );

    m_mouseMoved = false;
    if (button == Qt::LeftButton)
    {
        if (HandleSceneMousePressEvent(locator))
        {
            return true;
        }

        else if (m_dynamicRotate && !ShowTopView())
        {
            m_cameraIsMoving = true;
            m_viewerAlgos->activateSpinning();
            //return true;
        }

    }
    else if (/* button == Qt::MiddleButton || */ button == Qt::RightButton)
    {
        if (!m_meshNodeManager->handleRightMousePress(point))
        {
            m_viewerAlgos->activatePanning();
        }
    }

#if 0
    if (button == Qt::RightButton)
    {
        TraceMeshInfo(locator);
        return true;
    }
#endif

    return false;
}

bool COpenInventorEventsHandler::MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
    SbVec2s locator(point.x(),  point.y());
    m_viewerAlgos->setCurrentMousePositionLocator( locator );


    if (button == Qt::LeftButton)
    {
        if (m_cameraIsMoving)
        {
            m_cameraIsMoving = false;
        }
        if (!m_mouseMoved && MouseTrackerSelected())
        {
            TraceMeshInfo(locator);
        }
        if (!m_mouseMoved && m_sceneNode.containsSupportNodes())
        {
          HandleContextMenu(locator, true);
        }
        HandleSceneMouseReleaseEvent(locator);
    }

    if (button == Qt::RightButton && !m_mouseMoved)
    {
        if (m_sceneNode.containsSupportNodes())
        {
            HandleContextMenu(locator);
        }       
    }

    return true;
}

bool COpenInventorEventsHandler::MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
    SbVec2s locator(point.x(),  point.y());
    m_viewerAlgos->setCurrentMousePositionLocator( locator );
    // TraceMeshInfo(locator);

    if (button == Qt::LeftButton)
    {
        if (m_cameraIsMoving)
        {
            m_cameraIsMoving = false;
        }
        HandleSceneMouseDblClkEvent(locator);

        return true;
    }
   
    return false;
}

bool COpenInventorEventsHandler::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
    RefreshCursor(); // Cursor symbol refresh

    // - We want mx and my in the range [0;1]
    SbVec2s locator(point.x(),  point.y());

    if (m_viewerAlgos->getCurrentMousePositionLocator() == locator)
        return false;

    m_mouseMoved = true;

    ResetTimer(locator);
    m_viewerAlgos->setCurrentMousePositionLocator( locator );

    if (HandleSceneMouseMoveEvent(locator, button == Qt::LeftButton))
    {
        return true;
    }

    switch (button)
    {
    case Qt::LeftButton:
        if (m_cameraIsMoving)
        {
            float mx = (float)point.x() / (float)Width(); 
            float my = (float)point.y() / (float)Height();
            SbVec2f cameraLocator(mx, my);

            if (LockedSpinDirection() < 0)
            {
                m_viewerAlgos->spinCamera(cameraLocator);
            }
            else
            {
                m_viewerAlgos->spinConstrainedCamera(cameraLocator, LockedSpinDirection());
            }
            return true;
        }
        break;
    case Qt::RightButton:
        {
            PolyLineScreenDrawerRetainer retainer(this, m_screenDrawer);
            float mx = (float)point.x() / (float)Width(); 
            float my = (float)point.y() / (float)Height();

            SbVec2f cameraLocator(mx, my);
            m_viewerAlgos->panCamera(cameraLocator);
            return true;
        }
        break;
    case Qt::MiddleButton:
        break;
    default:
        break;
    }
    return false;
}

bool COpenInventorEventsHandler::MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point)
{
  SbVec2s locator = m_viewerAlgos->getCurrentMousePositionLocator();

  int adjust = state & Qt::ControlModifier ? 1 : 5;

  locator[0] -= adjust;
  locator[1] -= adjust;

  PolyLineScreenDrawerRetainer retainer(this, m_screenDrawer);

  if (nDelta > 0)
    m_viewerAlgos->dollyCamera (locator);
  else
    m_viewerAlgos->reverseDollyCamera (locator);

  return true;
}

SoKeyboardEvent::Key GetKeyCode (int nChar)
{
    // to be expanded

    SoKeyboardEvent::Key keycode;

    switch (nChar)
    {
    case 13: // Return
        keycode = SoKeyboardEvent::RETURN;
        break;
    case 27: // Return
        keycode = SoKeyboardEvent::ESCAPE;
        break;
    default:
        keycode = SoKeyboardEvent::ANY;
    }

    return keycode;
}

bool COpenInventorEventsHandler::KeyPress(TKeyboardModifiers state, int nChar, TKey key, int nRepCount)
{
   return KeyAction(nChar, SoButtonEvent::DOWN);
}

bool COpenInventorEventsHandler::KeyRelease(TKeyboardModifiers state, int nChar, TKey key, int nRepCount)
{
    return KeyAction(nChar, SoButtonEvent::UP);
}

void COpenInventorEventsHandler::on_zoom_to_box_cb( const QString& box_name )
{
	dynamic_cast<SoPerspectiveCamera*> (m_currentCamera)->heightAngle = float (PI / 4);
    SbViewportRegion vpregion(Width(), Height());

	if( m_boxes->boxes_m().contains( box_name ) )
	{
		SoNode* node = m_boxes->boxes_m()[ box_name ];
		m_currentCamera->viewAll( node, vpregion );
	}
}

void COpenInventorEventsHandler::show_refinement_box_dlg()
{
	if( !m_sceneNode.can_mesh_refinement() )
		return;
	
	if( !m_refinement_box_dlg )
	{
		CTetraModel* model = dynamic_cast< CTetraModel* >( &m_sceneNode.Model() );
		if( model )
		{
			m_boxes = new CMeshRefinementBoxesHandler( model->m_mesh_refinement_data );
			m_rootSceneGraph->addChild( m_boxes );

			m_refinement_box_dlg = new MeshRefinementDlg( m_boxes );
			TFunction function_cb = std::bind( &COpenInventorEventsHandler::on_zoom_to_box_cb, this, std::placeholders::_1 );
			m_refinement_box_dlg->register_cb( function_cb );

		}
	}
	m_refinement_box_dlg->show();
}

bool COpenInventorEventsHandler::KeyAction( int nChar, SoButtonEvent::State state )
{
    if (!m_viewerAlgos) return false;

	//FIXME: Just for TEST purpose
	if( state == SoButtonEvent::DOWN && nChar == 122 ) // 'F11'
		show_refinement_box_dlg();

	//FIXME: Just for TEST purpose
	//if( state == SoButtonEvent::DOWN && nChar == 120 ) // 'F9'
	//	on_zoom_to_box_cb( "new_box_1" );

    SoKeyboardEvent::Key keycode = GetKeyCode(nChar);

    SbVec2s locator = m_viewerAlgos->getCurrentMousePositionLocator();

    SoKeyboardEvent keyboardEvent;
    keyboardEvent.setKey(keycode);
    keyboardEvent.setState(state);

    keyboardEvent.setPosition(locator);
    keyboardEvent.setTime(SbTime::getTimeOfDay());

    return m_viewerAlgos->getSceneManager()->processEvent(&keyboardEvent) == TRUE;
}

void COpenInventorEventsHandler::SetCursor (Qt::CursorShape shape)
{
    s_cursorShape = shape;
    RefreshCursor();
}

void COpenInventorEventsHandler::RefreshCursor()
{
    CGeomecDoc* pDoc = GetGeomecDoc();
    if (!pDoc) return;
    CModelView *mv = pDoc->GetModelView();
    if (mv) mv->SetCursor(s_cursorShape);
}

void COpenInventorEventsHandler::OnShowCrossSection(const CCrossSection& xsec)
{
    UpdateTranslation();
    m_meshNodeManager->OnShowCrossSection(xsec);
    //m_meshNodeManager->OnValueComponentChanged(ValueComponent());
    //m_meshNodeManager->OnIsoValueComponentChanged();
}

void COpenInventorEventsHandler::OnHideCrossSection(const CCrossSection& xsec)
{
    m_meshNodeManager->OnHideCrossSection(xsec);
    //m_meshNodeManager->OnValueComponentChanged(ValueComponent());
    //m_meshNodeManager->OnIsoValueComponentChanged();
}

void COpenInventorEventsHandler::OnCrossSectionChanged(const CCrossSection& xsec)
{
    m_meshNodeManager->OnCrossSectionChanged(xsec);
}

void COpenInventorEventsHandler::OnShowWellPath(const CNewWellPath& wellPath)
{
  m_meshNodeManager->OnShowWellPath(wellPath);
}

void COpenInventorEventsHandler::OnHideWellPath(const CNewWellPath& wellPath)
{
  m_meshNodeManager->OnHideWellPath(wellPath);
}

void COpenInventorEventsHandler::OnWellPathChanged(const CNewWellPath& wellPath)
{
  m_meshNodeManager->OnWellPathChanged(wellPath);
}

void COpenInventorEventsHandler::OnNewFormationNode(const CFormationBase& node)
{
  m_meshNodeManager->OnNewFormationNode(node);
}

void COpenInventorEventsHandler::OnFormationNodeModified(const CFormationBase& node, ModifiedHint hint)
{
  m_meshNodeManager->OnFormationNodeModified(node, hint);
}

void COpenInventorEventsHandler::OnFormationNodeDeleted(const CFormationBase& node)
{
  m_meshNodeManager->OnFormationNodeDeleted(node);
}

// viewer events
void COpenInventorEventsHandler::OnNewOpenGLNode(const COpenGLNode& node)
{
  const CNewWellPath* pNWP = dynamic_cast<const CNewWellPath*>(&node);
  if(pNWP)
  {
    OnShowWellPath(*pNWP);
  }
  else
  {
    m_meshNodeManager->OnNewOpenGLNode(node);
    //m_meshNodeManager->OnValueComponentChanged(ValueComponent());
   // m_meshNodeManager->OnIsoValueComponentChanged();

    RetraceMeshInfo();
  }

  UpdateTranslation();
}

void COpenInventorEventsHandler::OnOpenGLNodeModified(const COpenGLNode& node, enum ModifiedHint uHint)
{
  const CNewWellPath* pNWP = dynamic_cast<const CNewWellPath*>(&node);
  if(pNWP)
  {
    OnWellPathChanged(*pNWP);
  }
  else
  {
    m_meshNodeManager->OnOpenGLNodeModified(node, uHint);
//    if (uHint == MeshCleared) return;
    //m_meshNodeManager->OnValueComponentChanged(ValueComponent());
    //m_meshNodeManager->OnIsoValueComponentChanged();
    // RetraceMeshInfo();
  }
  UpdateTranslation();
}

void COpenInventorEventsHandler::OnOpenGLNodeDeleted(const COpenGLNode& node)
{
  const CNewWellPath* pNWP = dynamic_cast<const CNewWellPath*>(&node);
  if(pNWP)
  {
    OnHideWellPath(*pNWP);
  }
  else
  {
    m_meshNodeManager->OnOpenGLNodeDeleted(node);
    RetraceMeshInfo();
  }
  UpdateTranslation();
}

void COpenInventorEventsHandler::OnColorScaleChanged()
{
    if (!m_meshNodeManager) return;
    m_meshNodeManager->OnColorScaleChanged();
}

void COpenInventorEventsHandler::OnValueComponentChanged(const IValueComponentBase *pValueComponent, CDoubleQuantity::UNIT unit, bool settingsChanged)
{ 
    m_meshNodeManager->OnValueComponentChanged(pValueComponent, unit, settingsChanged);
    if (pValueComponent)
      m_meshNodeManager->ShowColor(false);
    RetraceMeshInfo();
}

void COpenInventorEventsHandler::OnUnitChanged()
{
  if (DisplayCoordinateAxis())
    ResetSceneEnclosureNodes();
}

void COpenInventorEventsHandler::OnViewModeChanged()
{
}

void COpenInventorEventsHandler::OnViewportSizeChanged(int cx, int cy)
{
    if(m_viewerAlgos)
    {
        setViewport(cx, cy);
        UpdateLegend();

    }
}

SbBool ZoomRectangle (SoHandleEventAction *action, SoPolyLineScreenDrawer *drawer, void *userData)
{
    assert (userData);
    assert (drawer);
    drawer->setUserData(0);
    COpenInventorEventsHandler * eventHandler = (COpenInventorEventsHandler *) userData;
    eventHandler->ZoomRectangle (drawer);
    return TRUE;
}

void COpenInventorEventsHandler::ZoomRectangle( SoPolyLineScreenDrawer * drawer )
{
    SetCursor(Qt::ArrowCursor);

    int numPoints = drawer->point.getNum();

    SbVec2f pt[2];
    SbVec2f odd_pt[2];
    SbVec2f pixelPoint[2];
    if (numPoints == 4)
    {
        for (int p = 0; p < 2; ++p)
        {
            pt[p] = drawer->point[p*2];
            odd_pt[p] = drawer->point[p*2 + 1];

            pixelPoint[p][0] = (pt[p][0] * Width() + Width()) / 2;

            pixelPoint[p][1] = (pt[p][1] * Height() + Height()) / 2;
            pixelPoint[p][1] = Height() - pixelPoint[p][1];
        }
        m_viewerAlgos->doBoxZoom (pixelPoint[0][0], pixelPoint[0][1], pixelPoint[1][0], pixelPoint[1][1]);
    }

    drawer->clear();
}

void COpenInventorEventsHandler::OnZoomRectangle()
{
    GeomecPolygonScreenDrawer * drawer = getNewScreenDrawer<GeomecPolygonScreenDrawer>();
    assert (drawer);

    drawer->setUserData(this); // to signal a zoom operation is active (bit of a hack, to get the zoom button right).

    SetCursor(Qt::CrossCursor);

    drawer->setPolygonMode(GeomecPolygonScreenDrawer::Rectangular);
    drawer->setDrawerMode(GeomecPolygonScreenDrawer::Creating);
    drawer->setFinishCallback( &::ZoomRectangle, this );
}

SbBool PerformWorldActionOnPolygon( SoHandleEventAction* action, SoPolyLineScreenDrawer* drawer, void* userData ) 
{
    assert (userData);
    assert (drawer);
    COpenInventorEventsHandler * eventHandler = (COpenInventorEventsHandler *) userData;
    eventHandler->PerformWorldActionOnPolygon (drawer);
    return TRUE;
}

void COpenInventorEventsHandler::PerformWorldActionOnPolygon( SoPolyLineScreenDrawer * drawer )
{
    if (!ShowTopView()) return;
    int numPoints = drawer->point.getNum();

    if (m_worldAction && numPoints != 0)
    {
        std::vector<geo::CPoint> points;
        for (int p = 0; p < numPoints; ++p)
        {
            SbVec2f pt = drawer->point[p];
            SbVec2f worldPoint = ScreenDrawerToWorldPoint(pt);

            points.push_back( geo::CPoint(worldPoint[0], worldPoint[1]) );
        }

        (*m_worldAction)(points);
    }

    drawer->clear();
    
    delete m_worldAction;
    m_worldAction = 0;
}


void COpenInventorEventsHandler::ActivatePolygonScreenDrawer( CWorldAction * action, GeomecPolygonScreenDrawer::PolygonMode mode, std::vector<geo::CPoint> * points )
{
    GeomecPolygonScreenDrawer * drawer = getNewScreenDrawer<GeomecPolygonScreenDrawer>();
    assert (drawer);

    m_worldAction = action;
    drawer->setFinishCallback( ::PerformWorldActionOnPolygon, this );
    drawer->setPolygonMode(mode);

    drawer->setDrawerMode(GeomecPolygonScreenDrawer::Creating);
    if (points && points->size() != 0)
    {
        drawer->setDrawerMode(GeomecPolygonScreenDrawer::Modifying);

        for (size_t p = 0; p < points->size(); ++p)
        {
            geo::CPoint point = (*points)[p];

            SbVec2f worldPoint(point.X(), point.Y());
            SbVec2f drawerPoint = WorldToScreenDrawerPoint(worldPoint);

            drawer->addPoint(drawerPoint);
        }
    }
}

bool COpenInventorEventsHandler::CanZoomWindow() const
{
    return m_screenDrawer && m_screenDrawer->getUserData() != 0;
}

void COpenInventorEventsHandler::SwitchToTopView()
{
}

void COpenInventorEventsHandler::CheckCamera()
{
    SetCamera(DisplayPerspective());
}

void COpenInventorEventsHandler::SetCamera(bool bPerspective)
{
    if (bPerspective == (m_currentCamera == m_perspectiveCamera)) return; // Nothing to switch
    if(bPerspective)
    {
        // Switch to perspective; convert orthocamera height to angle
        float height = m_orthoCamera->height.getValue();
        float angle = atanf(height / (2.0f * m_orthoCamera->focalDistance.getValue()));
        m_perspectiveCamera->heightAngle = 2 * angle;

        m_cameraSwitch->whichChild = 0;
        m_currentCamera = m_perspectiveCamera;
    }
    else
    {
        // convert field-of-view angle to ortho height
        float angle = m_perspectiveCamera->heightAngle.getValue();
        float height = m_perspectiveCamera->focalDistance.getValue() * tanf(.5f * angle);
        m_orthoCamera->height = 2 * height;

        m_cameraSwitch->whichChild = 1;
        m_currentCamera = m_orthoCamera;
    }

    m_viewerAlgos->setCamera(m_currentCamera);
}

void COpenInventorEventsHandler::DisplayPerspective(bool bPerspective)
{
    m_displayPerspective = bPerspective;

    CheckCamera();
}

void COpenInventorEventsHandler::OnDeformation( Deformation::Data::Input deformation ){ m_meshNodeManager->OnDeformation( deformation ); }
Deformation::Data::Input COpenInventorEventsHandler::Deformation(){ return m_meshNodeManager->Deformation_(); }

bool COpenInventorEventsHandler::DisplayPerspective() const
{
    return m_displayPerspective && !ShowTopView();
}

bool COpenInventorEventsHandler::CanDisplayPerspective() const
{
    return !ShowTopView();
}

void COpenInventorEventsHandler::ResetSceneEnclosureNodes()
{
    SbBox3f bbox = m_meshNodeManager->GetAllMeshesBoundingBox();

    ResetSceneBoundingBox(bbox);
    ResetCoordAxis(bbox);

}

void COpenInventorEventsHandler::ResetSceneBoundingBox( SbBox3f &bbox )
{
    float sizeX, sizeY, sizeZ;
    float originX, originY, originZ;


    bbox.getOrigin (originX, originY, originZ);
    bbox.getSize(sizeX, sizeY, sizeZ);

    m_bboxScale->scaleFactor.setValue (sizeX, sizeY, sizeZ);
    m_bboxTranslation->translation.setValue (originX, originY, originZ);
}

void COpenInventorEventsHandler::ShowBoundary(bool bShow)
{
    ResetSceneEnclosureNodes();
    if (bShow)
    {
        m_bboxSwitch->whichChild = SO_SWITCH_ALL;
        if (NumberOfOpenGLNodesLinked() == 0 && !DisplayCoordinateAxis() && !ShowTopView())
            ViewBirdEyeDirection();
    }
    else
    {
        m_bboxSwitch->whichChild = SO_SWITCH_NONE;
    }
}

bool COpenInventorEventsHandler::ShowBoundary() const
{
    return (m_bboxSwitch != 0 && m_bboxSwitch->whichChild.getValue() == SO_SWITCH_ALL);
}

bool COpenInventorEventsHandler::CanShowBoundary() const
{
    if (!m_meshNodeManager) return false;
    return true;
}

void COpenInventorEventsHandler::TrackElement(bool bTrack)
{
    m_meshNodeManager->TrackElement(bTrack);
}

bool COpenInventorEventsHandler::TrackElement() const
{
    return m_meshNodeManager->TrackElement();
}

bool COpenInventorEventsHandler::CanTrackElement() const
{
    if (!m_meshNodeManager) return false;
    return true;
}
void COpenInventorEventsHandler::AllowDynamicRotate(bool bAllow)
{
    m_dynamicRotate = bAllow;
}

bool COpenInventorEventsHandler::AllowDynamicRotate() const
{
    return (m_dynamicRotate && !ShowTopView());
}

bool COpenInventorEventsHandler::CanAllowDynamicRotate() const
{
    return !ShowTopView();
}

void COpenInventorEventsHandler::DisplayGnomon(bool bDisplay)
{
    if(gnomonSwitch() != 0)
        gnomonSwitch()->whichChild = (bDisplay ? SO_SWITCH_ALL : SO_SWITCH_NONE);
}

bool COpenInventorEventsHandler::DisplayGnomon() const
{
    return (gnomonSwitch() != 0 && gnomonSwitch()->whichChild.getValue() == SO_SWITCH_ALL);
}

bool COpenInventorEventsHandler::CanDisplayGnomon() const
{
    return true;
}


bool COpenInventorEventsHandler::HandMode() const
{
    return false;
}

bool COpenInventorEventsHandler::CanSelectHandMode() const
{
    return false;
}

void COpenInventorEventsHandler::SelectHandMode(bool yesOrNo)
{

}

bool COpenInventorEventsHandler::ClipPlaneModeSelected() const
{
    if (!m_meshNodeManager) return false;
  return m_meshNodeManager->ClipPlaneModeSelected();
}

bool COpenInventorEventsHandler::CanSelectClipPlaneMode() const
{
    if (!m_meshNodeManager) return false;
  return m_meshNodeManager->CanSelectClipPlaneMode();
}

void COpenInventorEventsHandler::SelectClipPlaneMode(bool yesOrNo)
{
  m_meshNodeManager->SelectClipPlaneMode(yesOrNo);
}

bool COpenInventorEventsHandler::ContourLineModeSelected() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->ContourLineModeSelected();
}

bool COpenInventorEventsHandler::CanSelectContourLineMode() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanSelectContourLineMode();
}

void COpenInventorEventsHandler::SelectContourLineMode(bool yesOrNo)
{
    m_meshNodeManager->SelectContourLineMode(yesOrNo);
}

bool COpenInventorEventsHandler::MeshLinesDisplayed() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->MeshLinesDisplayed();
}

bool COpenInventorEventsHandler::CanSelectMeshLineDisplayMode() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanSelectMeshLineDisplayMode();
}

void COpenInventorEventsHandler::SelectMeshLinesDisplayed(bool yesOrNo)
{
    m_meshNodeManager->SelectMeshLinesDisplayed(yesOrNo);
}

bool COpenInventorEventsHandler::FormationOutlinesDisplayed() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->FormationOutlinesDisplayed();
}

bool COpenInventorEventsHandler::CanSelectFormationOutlineDisplayMode() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanSelectFormationOutlineDisplayMode();
}

void COpenInventorEventsHandler::SelectFormationOutlinesDisplayed(bool yesOrNo)
{
    m_meshNodeManager->SelectFormationOutlinesDisplayed(yesOrNo);
}

bool COpenInventorEventsHandler::DraggerTrackerSelected() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->DraggerTrackerSelected();
}

bool COpenInventorEventsHandler::CanSelectDraggerTracker() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanSelectDraggerTracker();
}

void COpenInventorEventsHandler::SelectDraggerTracker(bool yesOrNo)
{
    m_meshNodeManager->SelectDraggerTracker(yesOrNo);
}

bool COpenInventorEventsHandler::MouseTrackerSelected() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->MouseTrackerSelected();
}

bool COpenInventorEventsHandler::CanSelectMouseTracker() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanSelectMouseTracker();
}

void COpenInventorEventsHandler::SelectMouseTracker(bool yesOrNo)
{
    m_meshNodeManager->SelectMouseTracker(yesOrNo);
}

bool COpenInventorEventsHandler::DraggerConfined() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->DraggerConfined();
}

bool COpenInventorEventsHandler::CanSetDraggerConfined() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanSetDraggerConfined();
}

void COpenInventorEventsHandler::SetDraggerConfined(bool yesOrNo)
{
    m_meshNodeManager->SetDraggerConfined(yesOrNo);
}

bool COpenInventorEventsHandler::LegendHidden() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->LegendHidden();
}

bool COpenInventorEventsHandler::CanSelectHideLegend() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanSelectHideLegend();
}

void COpenInventorEventsHandler::HideLegend(bool yesOrNo)
{
    m_meshNodeManager->HideLegend(yesOrNo);
}

bool COpenInventorEventsHandler::DisplayCoordinateAxis() const
{
    return (m_coordinateAxis != 0 && m_coordinateAxis->whichChild.getValue() == SO_SWITCH_ALL);
}

bool COpenInventorEventsHandler::CanDisplayCoordinateAxis() const
{
    if (!m_meshNodeManager) return false;
    return true;
}

void COpenInventorEventsHandler::DisplayCoordinateAxis(bool bDisplay)
{
    if (bDisplay)
    {
        ResetSceneEnclosureNodes();
        m_coordinateAxis->whichChild = SO_SWITCH_ALL;
        if (NumberOfOpenGLNodesLinked() == 0 && !ShowBoundary() && !ShowTopView())
            ViewBirdEyeDirection();
    }
    else
    {
        m_coordinateAxis->whichChild = SO_SWITCH_NONE;
    }
}

void COpenInventorEventsHandler::Fill(bool bFill)
{
    m_meshNodeManager->Fill(bFill);
}

// true -> currently filled
// false -> currently line
bool COpenInventorEventsHandler::Fill() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->Fill();
}

bool COpenInventorEventsHandler::CanFill() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanFill();
}


void COpenInventorEventsHandler::ScalePerspectiveCameraAngle (float scale)
{
    float angle = dynamic_cast<SoPerspectiveCamera*> (m_currentCamera)->heightAngle.getValue();
    if (angle > PI/2)
    {
        // scale the difference with PI
        float complementAngle = PI - angle;
        complementAngle /= scale;
        angle = PI - complementAngle;
    }
    else
    {
        // scale the difference with zero
        angle *= scale;
    }
    dynamic_cast<SoPerspectiveCamera*> (m_currentCamera)->heightAngle = angle;
}

void COpenInventorEventsHandler::ResetTimer(const SbVec2s & locator)
{
    // filters out ghost events
    SbVec2s locatorBefore = m_viewerAlgos->getCurrentMousePositionLocator();
    short x, y;
    (locator - locatorBefore).getValue(x, y);
    if (x == 0 && y == 0)
        return;

    if (m_secondsToTimeOut == 0)
        m_meshNodeManager->resetTimeOut();
    m_secondsToTimeOut = m_draggerTimeOut;
}

void COpenInventorEventsHandler::TimerCalled()
{
    if (m_secondsToTimeOut > 0) 
    {
        if (--m_secondsToTimeOut == 0)
        {
            m_meshNodeManager->timeOut();
        }
    }
}

void COpenInventorEventsHandler::ZoomIn()
{
    PolyLineScreenDrawerRetainer retainer(this, m_screenDrawer);
    if (m_currentCamera->isOfType(SoPerspectiveCamera::getClassTypeId()))
    {
        ScalePerspectiveCameraAngle (float(1/1.2));	
    }
    else
    {
        SetCamera(true);
        ZoomIn();
        SetCamera(false);
    }
}

void COpenInventorEventsHandler::ZoomOut()
{
    PolyLineScreenDrawerRetainer retainer(this, m_screenDrawer);
    if (m_currentCamera->isOfType(SoPerspectiveCamera::getClassTypeId()))
    {
        ScalePerspectiveCameraAngle (float (1.2));
    }
    else
    {
        SetCamera(true);
        ZoomOut();
        SetCamera(false);
    }
}

void COpenInventorEventsHandler::ZoomReset()
{
    PolyLineScreenDrawerRetainer retainer(this, m_screenDrawer);
    if (m_currentCamera->isOfType(SoPerspectiveCamera::getClassTypeId()))
    {
        dynamic_cast<SoPerspectiveCamera*> (m_currentCamera)->heightAngle = float (PI / 4);
        SbViewportRegion vpregion(Width(), Height());
        m_currentCamera->viewAll(m_rootSceneGraph, vpregion);
    }
    else
    {
        SetCamera(true);
        ZoomReset();
        SetCamera(false);
    }
}

void COpenInventorEventsHandler::LockRotationVector(const geo::IVector& vector)
{
    if (vector[0] == 1.0)
    {
        m_lockedSpinDirection = 0;
    }
    else if (vector[1] == 1.0)
    {
        m_lockedSpinDirection = 1;
    }
    else if (vector[2] == 1.0)
    {
        m_lockedSpinDirection = 2;
    }
    else
    {
        m_lockedSpinDirection = -1;
    }
}

const geo::IVector& COpenInventorEventsHandler::LockRotationVector() const
{
    if (m_lockedSpinDirection == 0)
    {
        return geo::CVector::Xaxis;
    }
    else if (m_lockedSpinDirection == 1)
    {
        return geo::CVector::Yaxis;
    }
    else if (m_lockedSpinDirection == 2)
    {
        return geo::CVector::Zaxis;
    }
    else
        return geo::CVector::NullVector;
}

bool COpenInventorEventsHandler::CanLockRotationVector() const
{
    //TODO
    return true;
}

void COpenInventorEventsHandler::ViewTensorVector( TensorVectorMode mode )
{
    if (m_meshNodeManager) m_meshNodeManager->ViewTensorVector(mode);
}

TensorVectorMode COpenInventorEventsHandler::ViewTensorVector()
{
    if (!m_meshNodeManager) return TensorVectorFull;
    return m_meshNodeManager->ViewTensorVector();
}

bool COpenInventorEventsHandler::CanRefreshVectorBaseScale()
{
    if (m_meshNodeManager) return m_meshNodeManager->CanRefreshVectorBaseScale(); 
    return false;
}

void COpenInventorEventsHandler::RefreshVectorBaseScale()
{
    if (m_meshNodeManager)  m_meshNodeManager->RefreshVectorBaseScale();
}

void COpenInventorEventsHandler::ViewEDirection()
{
    ZoomReset();
    SetCameraOrientation(0.0f, -0.70710678f, 0.70710678f, 0.0f);
}

void COpenInventorEventsHandler::ViewNDirection()
{
    ZoomReset();
    SetCameraOrientation(-0.5f, -0.5f, 0.5f, 0.5f);
}

void COpenInventorEventsHandler::ViewZDirection()
{
    ZoomReset();
    SetCameraOrientation(-0.70710678f, -0.70710678f, 0.0f, 0.0f);
}

void COpenInventorEventsHandler::GoHome()
{
    m_viewerAlgos->resetToHomePosition();
}

void COpenInventorEventsHandler::SaveHome()
{
    m_viewerAlgos->saveHomePosition();
}
void COpenInventorEventsHandler::ViewBirdEyeDirection()
{
    ZoomReset();
    SetCameraOrientation(-0.34914f, -0.84062f, 0.38218f, 0.15935f); // determined empirically!?
}

void COpenInventorEventsHandler::ViewProportional(bool bProportional)
{
    if (m_meshNodeManager) m_meshNodeManager->ViewProportional(bProportional);
}

bool COpenInventorEventsHandler::ViewProportional() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->ViewProportional();
}

void COpenInventorEventsHandler::CheckLighting() const
{
    if (m_meshNodeManager) m_meshNodeManager->usePhongLightModel(Lighting());
}

void COpenInventorEventsHandler::Lighting(bool bLighting)
{
    m_phongLighting = bLighting;
    CheckLighting();
}

bool COpenInventorEventsHandler::Lighting() const
{ 
    if (!m_meshNodeManager) return false;
    return m_phongLighting && !PointSetSelected() && !ShowTopView();
}

bool COpenInventorEventsHandler::CanEnableLighting() const
{
    return !PointSetSelected();
}

void COpenInventorEventsHandler::ShowValuesPerElement(bool perElement)
{
  if (m_meshNodeManager) m_meshNodeManager->ShowValuesPerElement(perElement);
}

bool COpenInventorEventsHandler::ShowValuesPerElement() const
{
  if (!m_meshNodeManager) return false;
  return m_meshNodeManager->ShowValuesPerElement();
}

bool COpenInventorEventsHandler::CanShowValuesPerElement()
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanShowValuesPerElement();
}

void COpenInventorEventsHandler::ViewBeachBalls(bool view)
{
    if (m_meshNodeManager) m_meshNodeManager->ViewBeachBalls(view);
}

bool COpenInventorEventsHandler::BeachBallsOnView() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->BeachBallsOnView();
}

bool COpenInventorEventsHandler::CanViewBeachBalls() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanViewBeachBalls();
}

void COpenInventorEventsHandler::SetVectorScale(float scale)
{
  if(!m_meshNodeManager)
    return;

  m_meshNodeManager->SetVectorScale(scale);
}

void COpenInventorEventsHandler::SetVectorVisibleFraction(float factor)
{
  if(!m_meshNodeManager)
    return;

  m_meshNodeManager->SetVectorVisibleFraction(factor);
}

void COpenInventorEventsHandler::Render()
{
  if (m_updateLegend)
  {
    m_updateLegend = false;
    // This causes a problem in the legend: apparently we lose the glList for the font.
    // We detect that (OpenGLView:1021) and create a replacement list.
    // But that one is not always correct for currently unknown reasons,
    // and the text is invisible.
    // It doesn't appear to happen if we don't update the legend here,
    // BUT (TODO) we still need to handle the discrepancy between legend and selected components.
    //
    // Better would be to fix the whole font thing and use Qt for OpenGL.
    //UpdateLegend();
  }
    // Setup OpenGL context
    if(!m_oglContext)
    {
      m_oglContext = SoGLContext::getCurrent(true);
      m_oglContext->setNoGLContextDelete();
      m_viewerAlgos->getGLRenderAction()->setCacheContext(m_oglContext->getSharedId());
    }

    if(!m_oglContext->isCurrent())
      m_oglContext->bind();

    assert(m_oglContext->isValid());
    assert(m_oglContext->isValidForCurrent());

#ifdef _DEBUG
    CGeomecDoc* pDoc = GetGeomecDoc();
    CModelView *mv = pDoc->GetModelView();
    assert(m_oglContext == SoGLContext::findGLContext(mv->getHGLRC()));
#endif

    glViewport(0, 0, Width(), Height());
    m_viewerAlgos->actualRedraw();

    if (m_firstRender)
    {
        const_cast<COpenInventorSceneNode &> (m_sceneNode).ConnectScheduledItems();
        m_firstRender = false;
    }
}

void COpenInventorEventsHandler::UpdateTranslation()
{
  m_meshNodeManager->SetTranslation(true);
}

// viewport size
int COpenInventorEventsHandler::Width() const
{
    return m_sceneNode.Width();
}

int COpenInventorEventsHandler::Height() const
{
    return m_sceneNode.Height();
}

// value/coloring information

const IColorScaleNode* COpenInventorEventsHandler::ColorScale() const
{
    return m_sceneNode.ColorScale();
}

// display color or property values
bool COpenInventorEventsHandler::ShowColor() const
{
    if (m_meshNodeManager) return m_meshNodeManager->ShowColor();
    else return true;
}

bool COpenInventorEventsHandler::ShowValue() const
{
  return !ShowColor();
    //throw std::logic_error("The method or operation is not implemented.");
}

bool COpenInventorEventsHandler::CanShowValue()
{
    return m_meshNodeManager && m_meshNodeManager->CanShowValue();
}

bool COpenInventorEventsHandler::CanDisplay(const COpenGLNode* pOpenGLNode)
{
  assert(false);  // not implemented!

  return false;
}

void COpenInventorEventsHandler::ShowColor (bool show)
{
	if( !CTreeCtrlBase::AllowPaint() )
		return;
    if (m_meshNodeManager)
    {
        if (show)
        {
            m_meshNodeManager->DisplayGeologyLegend();
        }
        else
        {
            m_meshNodeManager->DisplayValuesLegend();
        }

        UpdateLegend();
    }
}

void COpenInventorEventsHandler::renderCallback(void* userData, SoSceneManager* mgr)
{
    COpenInventorEventsHandler* handler = (COpenInventorEventsHandler*)userData;

    if(handler)
        handler->Render();

    //TODO
    //swapBuffers(); ???
}


void COpenInventorEventsHandler::initOIV()
{
    if (m_oivInitialized) 
        return;

	GeomecOIV::init();

    m_viewerAlgos = new SoGuiAlgoViewers;
    m_viewerAlgos->ref();

    CGeomecDoc* pDoc = GetGeomecDoc();
    CModelView *mv = pDoc->GetModelView();
    mv->MakeCurrent();

    m_oglContext = SoGLContext::getCurrent(true);
    assert (m_oglContext); // should be there!
    assert(m_oglContext->getGLContext() == mv->getHGLRC());
    m_oglContext->setNoGLContextDelete();

    m_viewerAlgos->setViewerType(SoGuiAlgoViewers::EXAMINER);
    m_viewerAlgos->setHeadlight(true);

    m_viewerAlgos->getSceneManager()->reinitialize();
    m_viewerAlgos->resetRenderAction();
    m_viewerAlgos->getSceneManager()->setNeedToSetViewport(0);

    setViewport(Width(), Height());

    // Create a simple scene graph
    m_rootSceneGraph = new SoSeparator;
    m_rootSceneGraph->renderCaching = false; // renderCaching interferes with MoMeshTensor rendering
    m_rootSceneGraph->setName("rootSceneGraph");
    m_rootSceneGraph->ref();

    InitCamera(); 
    m_rootSceneGraph->addChild(m_cameraSwitch);

    m_background = new SoGradientBackground;
    m_rootSceneGraph->addChild(m_background);

    float black[3] = { 0.0f, 0.0f, 0.0f };

    m_background->color0.setValue(black);
    m_background->color1.setValue(black);

    // This group contains the scene
    InitMeshNodeManager();

    Lighting(true);

    // bounding box
    InitBoundingBox();
    // Coordinate axes
    InitCoordinateAxis();

    InitScreenDrawing();

    // Initialize the sundial
    InitGnomon();

    // We setup the scene manager with our render callback.
    SoSceneManager* sceneManager = m_viewerAlgos->getSceneManager();
    sceneManager->setRenderCallback( renderCallback, this );
    sceneManager->activate();

    // Ok we are done with the assembly now we can inform the managing
    // objects about this scene graph.
    m_viewerAlgos->setSceneGraph(m_rootSceneGraph);
    m_viewerAlgos->setCamera(m_currentCamera);

    SbViewportRegion vpregion(Width(), Height());
    m_perspectiveCamera->viewAll(m_rootSceneGraph, vpregion);
    m_orthoCamera->viewAll(m_rootSceneGraph, vpregion);

#ifdef USE_IVTUNE
    SoIvTune::start(sceneManager);
#endif

    m_oivInitialized = true;
}


void COpenInventorEventsHandler::setViewport(int width, int height)
{
    SbVec2s viewportSize(width, height);
    SbViewportRegion vpRegion(viewportSize);
    m_viewerAlgos->getSceneManager()->setViewportRegion(vpRegion);

    // We must also inform the algorithms of the modification for the internal computation.
    m_viewerAlgos->setGlxSize(viewportSize);
}


void COpenInventorEventsHandler::InitBoundingBox()
{
    SoSeparator * bboxGroup = new SoSeparator;
    bboxGroup->setName("bboxGroup");

    // We want everything but our draggers to be unpickable
    SoPickStyle * unpickableStyle = new SoPickStyle;
    unpickableStyle->style = SoPickStyle::UNPICKABLE;

    bboxGroup->addChild(unpickableStyle);

    m_bboxSwitch = new SoSwitch;
    m_bboxSwitch->setName("bboxSwitch");
    bboxGroup->addChild(m_bboxSwitch);

    const SbColor white (1.0f, 1.0f, 1.0f);

    SoMaterial*  bboxMaterial = new SoMaterial;


    bboxMaterial->diffuseColor = white;
    bboxMaterial->emissiveColor = white;

    m_bboxSwitch->addChild (bboxMaterial);

    SoDrawStyle * bboxDrawStyle = new SoDrawStyle;
    bboxDrawStyle->style.setValue(SoDrawStyle::LINES);
    bboxDrawStyle->lineWidth.setValue(2);
    bboxDrawStyle->linePattern.setValue(0xf0f0);
    m_bboxSwitch->addChild (bboxDrawStyle);

    SoScale * scale = m_meshNodeManager->getScale();
    assert (scale != 0);
    m_bboxSwitch->addChild(scale);

    // to translate the bounding box to the right position
    m_bboxTranslation = new SoTranslation;
    m_bboxSwitch->addChild (m_bboxTranslation);

    // Necessary to scale to the bounding box size of the mesh
    m_bboxScale = new SoScale;
    m_bboxSwitch->addChild(m_bboxScale);

    // Make a bounding box cube of size (1, 1, 1) positioned at (0, 0, 0)
    SoScale * cubeScale = new SoScale;
    cubeScale->scaleFactor.setValue (0.5f, 0.5f, 0.5f);
    m_bboxSwitch->addChild (cubeScale);

    SoTranslation * cubeTranslation = new SoTranslation;
    cubeTranslation->translation.setValue(1.0f, 1.0f, 1.0f);
    m_bboxSwitch->addChild (cubeTranslation);

    SoCube * bbox = new SoCube;
    m_bboxSwitch->addChild(bbox);

    SoPickStyle * pickableStyle = new SoPickStyle;
    pickableStyle->style = SoPickStyle::SHAPE;
    bboxGroup->addChild(pickableStyle);

    int childIndex = m_rootSceneGraph->findChild(m_meshNodeManager);
    if (childIndex < 0)
        m_rootSceneGraph->addChild (bboxGroup);
    else
        m_rootSceneGraph->insertChild (bboxGroup, childIndex);
}

void COpenInventorEventsHandler::InitScreenDrawing()
{
    m_screenDrawerGroup = new SoSeparator;
    m_screenDrawerGroup->setName("ScreenDrawerSeparator");

    SoDrawStyle * screenDrawerDrawStyle = new SoDrawStyle;
    screenDrawerDrawStyle->style.setValue(SoDrawStyle::LINES);
    screenDrawerDrawStyle->lineWidth.setValue(1);
    screenDrawerDrawStyle->linePattern.setValue(0xffff);
    m_screenDrawerGroup->addChild (screenDrawerDrawStyle);

    m_rootSceneGraph->addChild (m_screenDrawerGroup);
}

void COpenInventorEventsHandler::InitCamera()
{
    // Setup cameras. Add both a perspective and an orthographic camera to a switch node, so
    // we can switch easily between the 2 at runtime.
    m_perspectiveCamera = new SoPerspectiveCamera;
    m_orthoCamera = new SoOrthographicCamera;

    // Connect fields from both, so when switching we have the same orientation etc.
    m_perspectiveCamera->orientation.connectFrom(&m_orthoCamera->orientation);
    m_orthoCamera->orientation.connectFrom(&m_perspectiveCamera->orientation);
    m_perspectiveCamera->position.connectFrom(&m_orthoCamera->position);
    m_orthoCamera->position.connectFrom(&m_perspectiveCamera->position);
    m_perspectiveCamera->focalDistance.connectFrom(&m_orthoCamera->focalDistance);
    m_orthoCamera->focalDistance.connectFrom(&m_perspectiveCamera->focalDistance);

    m_cameraSwitch = new SoSwitch;
    m_cameraSwitch->setName("cameraSwitch");
    m_cameraSwitch->addChild(m_perspectiveCamera);
    m_cameraSwitch->addChild(m_orthoCamera);
    m_cameraSwitch->whichChild = 0;
    m_currentCamera = m_perspectiveCamera;
}

void COpenInventorEventsHandler::SetCameraOrientation(float x, float y, float z, float w)
{
    m_currentCamera->orientation.setValue(x, y, z, w);

    SbViewportRegion vpregion(Width(), Height());
    m_currentCamera->viewAll(m_rootSceneGraph, vpregion);
}

struct Pickability
{
    Pickability (SoPickStyle::Style style, OIDIMeshNodeManager * meshNodeManager)
        : m_meshNodeManager(meshNodeManager)
    {
        m_savedStyle = m_meshNodeManager->GetPickStyle();
        m_meshNodeManager->SetPickStyle(style);
    }

    ~Pickability()
    {
        m_meshNodeManager->SetPickStyle(m_savedStyle);
    }

private:
    OIDIMeshNodeManager * m_meshNodeManager;
    SoPickStyle::Style m_savedStyle;

};

int WorldToScreenIndex (int index)
{
    assert (index >= 0 && index < 2);

    const int WorldToScreen[2] = { 1 , 0 };
    return WorldToScreen[index];
}

/* x <-> N, y <-> E */
void COpenInventorEventsHandler::GetScreenToWorldConversion (SbVec3f & screenMin, SbVec3f & worldMin, SbVec3f & scaleScreenToWorld)
{
    assert (ShowTopView()); /* does not work for anything else */

    SbViewVolume viewVolume = m_currentCamera->getViewVolume();

    screenMin  = SbVec3f (0.0f, 0.0f, 0.0f );
    SbVec3f screenMax (1.0f, 1.0f, 0.0f );

    SbVec3f worldMax;

    viewVolume.projectFromScreen(screenMin, worldMin);
    viewVolume.projectFromScreen(screenMax, worldMax);

    const SbVec3d& translation = m_meshNodeManager->GetTranslation();

    for (size_t i = 0; i < 3; ++i)
    {
      worldMin[i] += translation[i];
      worldMax[i] += translation[i];
    }

    float width = (float) Width();
    float height = (float) Height();

    if (width > height)
    {
        screenMax[0] = (screenMax[0] - 0.5) * height / width + 0.5;
        screenMin[0] = (screenMin[0] - 0.5) * height / width + 0.5;
    }
    else
    {
        screenMax[1] = (screenMax[1] - 0.5) * width / height + 0.5;
        screenMin[1] = (screenMin[1] - 0.5) * width / height + 0.5;
    }

    for (int i = 0; i < 2; ++i)
    {
        scaleScreenToWorld[i] = (worldMax[i] - worldMin[i]) / (screenMax[WorldToScreenIndex(i)] - screenMin[WorldToScreenIndex(i)]);
    }
}

SbVec2f COpenInventorEventsHandler::GetWorldCoordinates(const SbVec2f & screenNormalized /* x, y */)
{
   
    SbVec3f screenMin (0.0f, 0.0f, 0.0f );
    
    SbVec3f worldMin;

    SbVec3f scaleScreenToWorld;
    GetScreenToWorldConversion(screenMin, worldMin, scaleScreenToWorld);
    
    SbVec2f world;
    for (int i = 0; i < 2; ++i)
    {
        world[i] = worldMin[i] +  scaleScreenToWorld[i] * (screenNormalized[WorldToScreenIndex(i)] - screenMin[WorldToScreenIndex(i)]);
    }

    return world;
}

/* x <-> N, y <-> E */
SbVec2f COpenInventorEventsHandler::GetScreenNormalizedCoordinates(const SbVec2f & world /* x, y */)
{
    SbVec3f screenMin (0.0f, 0.0f, 0.0f );

    SbVec3f worldMin;

    SbVec3f scaleScreenToWorld;
    GetScreenToWorldConversion(screenMin, worldMin, scaleScreenToWorld);

    SbVec2f screenNormalized;
    for (int i = 0; i < 2; ++i)
    {
        screenNormalized[WorldToScreenIndex(i)] = screenMin[WorldToScreenIndex(i)] + (world[i] - worldMin[i]) / scaleScreenToWorld[i];
    }

    return screenNormalized;
}

void COpenInventorEventsHandler::RetraceMeshInfo()
{
    if (MouseTrackerSelected())
    {
        TraceMeshInfo(m_meshInfoScreenPoint);
    }
}

void COpenInventorEventsHandler::TraceMeshInfo( SbVec2s & point )
{
    m_meshInfoScreenPoint = point;

    m_meshNodeManager->invalidateValueTrackerCellMarkers();
    SbViewportRegion vpregion(Width(), Height());
    SoRayPickAction pickAction (vpregion);
    pickAction.setPoint(point);

    Pickability modifiedPickability (SoPickStyle::SHAPE, m_meshNodeManager);

    pickAction.setPickAll(true);
    pickAction.apply(m_rootSceneGraph); // requires a camera to be in the scene
    
    // loop not really necessary ...

    long long closestPickedPointIndex = -1;
    double closestDistance = DBL_MAX;

    SoPickedPoint * pickedPoint;
    for (size_t i = 0; (pickedPoint = pickAction.getPickedPoint(i)) != 0; ++i)
    {
        const SbVec3f & coordinates = pickedPoint->getPoint();
        const SoDetail* detail = pickedPoint->getDetail();
        if (!detail || !detail->isOfType(MoFaceDetailI::getClassTypeId())) continue;
       

        // A face of a mesh representation of a unstructured mesh 
        // has been picked. Get detail about the picked face.
        MoFaceDetailI* fdetail = (MoFaceDetailI*) detail;

        // Get the picked cell and the value at the picked point.
        size_t cellId = fdetail->getCellIndex();
        double value = fdetail->getValue(pickedPoint->getPoint());

        // Get detail about the picked mesh 
        const MoMeshDetail* mdetail = fdetail->getMeshDetail();
        const MiMesh* pickedMesh = mdetail->getMesh();
        if (dynamic_cast<const OIDIPointSetMesh *> (pickedMesh) != 0) continue;;

        MeshType meshType = mdetail->getMeshType();

        MbVec3d mvCoordinates(coordinates[0], coordinates[1], coordinates[2]);

        size_t meshNodeIndexClosest = 0;
        

        double distance = m_meshNodeManager->computeDistanceClosestNode(dynamic_cast<const MiMeshUnstructuredI *> (pickedMesh), mvCoordinates, cellId, meshNodeIndexClosest);

        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestPickedPointIndex = i;
        }
    }

    if (closestPickedPointIndex < 0)
    {
        m_meshNodeManager->updateValueTracker(0, 0, MbVec3d(-1e-8), (size_t) -1);
        return;
    }

    pickedPoint = pickAction.getPickedPoint(closestPickedPointIndex);
    const SbVec3f & coordinates = pickedPoint->getPoint();

    const SoDetail* detail = pickedPoint->getDetail();
    assert (detail);
    assert (detail->isOfType(MoFaceDetailI::getClassTypeId()));
    
    // A face of a mesh representation of a unstructured mesh 
    // has been picked. Get detail about the picked face.
    MoFaceDetailI* fdetail = (MoFaceDetailI*) detail;

    // Get the picked cell and the value at the picked point.
    size_t cellId = fdetail->getCellIndex();
    double value = fdetail->getValue(pickedPoint->getPoint());

    // Get detail about the picked mesh 
    const MoMeshDetail* mdetail = fdetail->getMeshDetail();
    const MiMesh* pickedMesh = mdetail->getMesh();

    MeshType meshType = mdetail->getMeshType();

    MbVec3d mvCoordinates(coordinates[0], coordinates[1], coordinates[2]);

    size_t meshNodeIndexClosest = 0;
    MbVec3d meshNodeCoordinatesClosest;
    if (dynamic_cast<const OIDIPointSetMesh *> (pickedMesh) != 0) return;

    
    m_meshNodeManager->computeValueTrackerCellMarkerSet(dynamic_cast<const MiMeshUnstructuredI *> (pickedMesh), mvCoordinates, cellId, meshNodeIndexClosest, meshNodeCoordinatesClosest);

    m_meshNodeManager->computeValueTrackerCellFacetSet( dynamic_cast<const OIDIMeshVU *> (pickedMesh), cellId );
    m_meshNodeManager->computeValueTrackerCellFacetSet( dynamic_cast<const OIDIMeshSU *> (pickedMesh), cellId );
    m_meshNodeManager->computeValueTrackerCellFacetSet( dynamic_cast<const OIDIPointSetMesh *> (pickedMesh), cellId );


    m_meshNodeManager->updateValueTracker(dynamic_cast<const MiMeshUnstructuredI *> (pickedMesh), dynamic_cast<const OIDIMesh *> (pickedMesh), mvCoordinates, cellId);

}

void COpenInventorEventsHandler::HandleContextMenu(SbVec2s & point, bool bSuppressMenu)
{
    SbViewportRegion vpregion(Width(), Height());
    SoRayPickAction pickAction (vpregion);
    pickAction.setPoint(point);

    Pickability modifiedPickability (SoPickStyle::SHAPE, m_meshNodeManager);

    pickAction.apply(m_rootSceneGraph); // requires a camera to be in the scene

    SoPickedPoint * pickedPoint = pickAction.getPickedPoint(0); 
    if (!pickedPoint) return;

    const SbVec3f coordinates = pickedPoint->getPoint();

    const SoDetail* detail = pickedPoint->getDetail();
    if (!detail) return;
    const MoMeshDetail* mdetail = 0;
    size_t cellId;
    if (detail->isOfType(MoFaceDetailI::getClassTypeId()))
    {
        // A face of a mesh representation of a unstructured mesh 
        // has been picked. Get detail about the picked face.
        MoFaceDetailI* fdetail = (MoFaceDetailI*) detail;

        // Get the picked cell and the value at the picked point.
        cellId = fdetail->getCellIndex();
        double value = fdetail->getValue(pickedPoint->getPoint());

        // Get detail about the picked mesh 
         mdetail = fdetail->getMeshDetail();
    }
       
    else if (detail->isOfType(MoLineDetailI::getClassTypeId()))
    {
        // A face of a mesh representation of a unstructured mesh 
        // has been picked. Get detail about the picked face.
        MoLineDetailI* ldetail = (MoLineDetailI*) detail;

        // Get the picked cell and the value at the picked point.
        cellId = ldetail->getCellIndex();
        double value = ldetail->getValue(pickedPoint->getPoint());

        // Get detail about the picked mesh 
        mdetail = ldetail->getMeshDetail();
    }
    if (!mdetail) return;

    const MiMesh* pickedMesh = mdetail->getMesh();
    MbVec3d mvCoordinates(coordinates[0], coordinates[1], coordinates[2]);

    m_meshNodeManager->dehighlightMesh();
    if (!m_meshNodeManager->highlightMesh (dynamic_cast<const OIDIMesh *> (pickedMesh)))
    {
        TrackElement(true);
        m_meshNodeManager->clearValueTrackerCellFacetSet();

        size_t meshNodeIndexClosest = 0;
        MbVec3d meshNodeCoordinatesClosest;
        m_meshNodeManager->computeValueTrackerCellMarkerSet(dynamic_cast<const MiMeshUnstructuredI *> (pickedMesh), mvCoordinates, cellId, meshNodeIndexClosest, meshNodeCoordinatesClosest);
    }

    if (!bSuppressMenu)
    {
      MeshType meshType = mdetail->getMeshType();

      QPoint qpoint(point[0], point[1]);
      m_meshNodeManager->handleContext(dynamic_cast<const OIDIMesh *> (pickedMesh), cellId, qpoint, mvCoordinates);
    }
}


bool COpenInventorEventsHandler::HandleSceneMousePressEvent( SbVec2s & locator )
{
    if (!m_viewerAlgos) return false;

    SoMouseButtonEvent buttonEvent;

    buttonEvent.setButton(SoMouseButtonEvent::BUTTON1);
    buttonEvent.setState(SoButtonEvent::DOWN);
    buttonEvent.setButton1Down(TRUE);
    buttonEvent.setPosition(locator);
    buttonEvent.setTime(SbTime::getTimeOfDay());
    return m_viewerAlgos->getSceneManager()->processEvent(&buttonEvent) == TRUE;
}

bool COpenInventorEventsHandler::HandleSceneMouseReleaseEvent( SbVec2s & locator )
{
    if (!m_viewerAlgos) return false;

    SoMouseButtonEvent buttonEvent;

    buttonEvent.setButton(SoMouseButtonEvent::BUTTON1);
    buttonEvent.setState(SoButtonEvent::UP);
    buttonEvent.setPosition(locator);
    buttonEvent.setTime(SbTime::getTimeOfDay());

    return m_viewerAlgos->getSceneManager()->processEvent(&buttonEvent) == TRUE;
}

bool COpenInventorEventsHandler::HandleSceneMouseDblClkEvent( SbVec2s & locator )
{
    if (!m_viewerAlgos) return false;

    SoMouseButtonEvent buttonEvent;

    buttonEvent.setButton(SoMouseButtonEvent::BUTTON1);
    buttonEvent.setState(SoButtonEvent::DBCLK);
    buttonEvent.setPosition(locator);
    buttonEvent.setTime(SbTime::getTimeOfDay());

    return m_viewerAlgos->getSceneManager()->processEvent(&buttonEvent) == TRUE;
}

bool COpenInventorEventsHandler::HandleSceneMouseMoveEvent( SbVec2s & locator, bool button1Down )
{
    if (!m_viewerAlgos) return false;

    SoLocation2Event mouseMoveEvent;

    mouseMoveEvent.setEventSource(SoLocation2Event::MOUSE_MOVE);
    mouseMoveEvent.setButton1Down(button1Down ? TRUE : FALSE);
    mouseMoveEvent.setPosition(locator);
    mouseMoveEvent.setTime(SbTime::getTimeOfDay());

    SbBool processed = m_viewerAlgos->getSceneManager()->processEvent(&mouseMoveEvent);

    return processed == TRUE;
}

void COpenInventorEventsHandler::EnterView()
{
  m_meshNodeManager->EnterView();
}

void COpenInventorEventsHandler::LeaveView()
{
  m_meshNodeManager->LeaveView();
}

void COpenInventorEventsHandler::InitMeshNodeManager()
{
    m_meshNodeManager = new OIDIMeshNodeManager(this);
    m_rootSceneGraph->addChild( m_meshNodeManager );
//    UpdateLegend();
}

void COpenInventorEventsHandler::InitGnomon()
{
    SoSeparator * gnomon = makeGnomon();
    m_gnomonSwitch = dynamic_cast<SoSwitch *> (gnomon->getChild(0));
    assert (gnomonSwitch());

    m_rootSceneGraph->addChild(gnomon);
}

void COpenInventorEventsHandler::SelectMeshMode( MeshMode mode )
{
    m_meshNodeManager->SelectMeshMode(mode);
}

MeshMode COpenInventorEventsHandler::MeshModeSelected() const
{
    if (!m_meshNodeManager) return SkinMesh;
    return m_meshNodeManager->MeshModeSelected();

}

bool COpenInventorEventsHandler::CanSelectMeshMode() const
{
    if (!m_meshNodeManager) return false;
    return m_meshNodeManager->CanSelectMeshMode();
}

void COpenInventorEventsHandler::ResetCoordAxis( const SbBox3f & bbox )
{
  SbVec3d translation = m_meshNodeManager->GetTranslation();

  CGeomecDoc& doc = *GetGeomecDoc();
  if (doc.UnitNode().Unit() == CQuantity::FIELD_UNIT)
    for (size_t i = 0; i < 3; ++i)
      translation[i] *= FF_FACTOR_LENGTH;

  m_coordinateAxis->reset (bbox, translation);
}

void COpenInventorEventsHandler::UpdateLegend(bool bForceColor, bool bUpdatedColorScale)
{
  if (!CTreeCtrlBase::AllowPaint())
  {
    m_updateLegend = true;
    return;
  }

  CTnoWinApp* app = (CTnoWinApp*)AfxGetApp();
  if(!app->IsExiting())
  {
    if (bForceColor || (!bUpdatedColorScale && m_meshNodeManager->ShowColor()))
      m_meshNodeManager->ShowGeologyLegend();
    else
      m_meshNodeManager->UpdateValuesLegend();

	LegendFrame().ClearLegendFrames();
    OnUpdateLegend(bForceColor);
    LegendFrame().UpdateFrame();
  }
}

void COpenInventorEventsHandler::OnUpdateLegend(bool bForceColor)
{
	// Insert caption "legend"
	new CLegendFrame::legend_label(LegendFrame(), "Legend");
	new CLegendFrame::legend_separator(LegendFrame(), 20);

	TOpenGLNodeSet stNode = NodesInScene();
	TOpenGLNodeSet::iterator itNode;
	for(itNode = stNode.begin(); itNode != stNode.end(); itNode++)
	{
		const CDisplacementSupportNode *dnode = dynamic_cast<const CDisplacementSupportNode *> (*itNode);
		const CPressureSupportNode *pnode = dynamic_cast<const CPressureSupportNode *> (*itNode);
		assert(!(dnode && pnode));

		if(dnode != 0)
		{
			new CLegendFrame::legend_separator(LegendFrame(), 15);
			new CLegendFrame::legend_label(LegendFrame(), "Select nodes, right-click to configure");
			new CLegendFrame::legend_separator(LegendFrame(), 15);
			new CLegendFrame::legend_index(LegendFrame(), 10, 10, qRgb(255, 0, 0), "Selected node");
			new CLegendFrame::legend_index(LegendFrame(), 10, 10, qRgb(0, 255, 0), "Reference node");
			new CLegendFrame::legend_index(LegendFrame(), 10, 10, qRgb(0, 0, 255), "Selectable node");
			return;
		}
		else if(pnode != 0)
		{
			new CLegendFrame::legend_separator(LegendFrame(), 15);
			new CLegendFrame::legend_label(LegendFrame(), "Select surfaces, right-click to configure");
			new CLegendFrame::legend_separator(LegendFrame(), 15);
			new CLegendFrame::legend_index(LegendFrame(), 10, 10, qRgb(255, 255, 0), "Selected surface");
			return;
		}
	}

	// Are there items in the scene?
	if(NodesInScene().size() == 0)
	{
		new CLegendFrame::legend_separator(LegendFrame(), 15);		
		new CLegendFrame::legend_label(LegendFrame(), QString("No viewable items selected"));
		new CLegendFrame::legend_separator(LegendFrame(), 15);
		return;
	}
	if(bForceColor || ShowColor())
	{
		new CLegendFrame::legend_label(LegendFrame(), "View : Geology");
    if (bForceColor)
      new CLegendFrame::legend_label(LegendFrame(), "(press 'refresh' to update')");
		InsertLinkedNodesInLegend();
	}
	else if (m_sceneNode.ValueComponent()) // we may not always have one
	{
		new CLegendFrame::legend_label(LegendFrame(), "View : Value");
		OnUpdateValueLegend();
		//UpdateFrame();
	}
}

void COpenInventorEventsHandler::OnUpdateValueLegend()
{
    CColorScaleEntry *pEntry = dynamic_cast<CColorScaleEntry*>(((CFemAppModel&)m_sceneNode.Model()).GraphEntry(MD_BASE_COLOR_SCALE));
    assert(pEntry);
    CGeomecDoc& doc = *GetGeomecDoc();

    new CLegendFrame::legend_separator(LegendFrame(), 3);
    new CLegendFrame::legend_label(LegendFrame(), QString("Unit type : ") + doc.UnitNode().Name());
    new CLegendFrame::legend_separator(LegendFrame(), 3);

    switch(pEntry->ColorScaleType())
    {
    case CColorScaleEntry::LOCAL:
        new CLegendFrame::legend_label(LegendFrame(), "Color scale : Local");
        break;
    case CColorScaleEntry::GLOBAL:
        new CLegendFrame::legend_label(LegendFrame(),"Color scale : Global");
        new CLegendFrame::legend_separator(LegendFrame(), 3);
        new CLegendFrame::legend_label(LegendFrame(), QString("Color scale name : ") + pEntry->CurrentColorScale().Name() );
        break;
    case CColorScaleEntry::HOTSPOT:
        new CLegendFrame::legend_label(LegendFrame(), "Color scale : Hotspot");
        new CLegendFrame::legend_separator(LegendFrame(), 3);
        new CLegendFrame::legend_label(LegendFrame(), QString("Hotspot name : ") + pEntry->CurrentColorScale().Name() );
        break;
    default:
        assert(FALSE);	// Unknown
        break;
    }

    // Do we have a result component
    new CLegendFrame::legend_separator(LegendFrame(), 15);

    const IResultComponent *pResultComponent = dynamic_cast<const IResultComponent*> (m_sceneNode.ValueComponent());
    if(pResultComponent)
    {
        // Result
        new CLegendFrame::legend_label(LegendFrame(), "Result tree path:");
        AddResultPath(*pResultComponent);
        new CLegendFrame::legend_separator(LegendFrame(), 6);
        new CLegendFrame::legend_label(LegendFrame(), QString("Depletion stage : ") + pResultComponent->Stage().Name());
        new CLegendFrame::legend_separator(LegendFrame(), 3);

        if(pResultComponent->AnalysisType().Valid())
        {
            new CLegendFrame::legend_label(LegendFrame(), QString("Analysis : ") + pResultComponent->AnalysisType().Label());
            if(pResultComponent->AnalysisType() == CAnalysisType::AT_NONLIN)
                new CLegendFrame::legend_label(LegendFrame(), "Step type : Time");
        }
    }
    else
    {
        // Other value object
        QString sValueName;
        if(m_sceneNode.ValueComponent()->Parent().TypeId() && !dynamic_cast<CMeshResult*>(&m_sceneNode.ValueComponent()->Parent()))
            sValueName = m_sceneNode.ValueComponent()->Parent().TypeName();
        else
            sValueName = m_sceneNode.ValueComponent()->Parent().Name();	// Derived result
        new CLegendFrame::legend_label(LegendFrame(), QString("Value : ") + sValueName);
        if(m_sceneNode.ValueComponent()->Parent().ComponentSize() != 1)
        {
            new CLegendFrame::legend_separator(LegendFrame(), 3);
            new CLegendFrame::legend_label(LegendFrame(), QString("Component : ") + m_sceneNode.ValueComponent()->Name());
        }
    }

    new CLegendFrame::legend_separator(LegendFrame(), 3);
    new CLegendFrame::legend_label(LegendFrame(), QString("Unit : ") + m_sceneNode.ValueComponent()->UnitName(doc.UnitNode().Unit()));
    new CLegendFrame::legend_separator(LegendFrame(), 15);

    if(InsertLinkedNodesInLegend() == 0)
        return;

    CColorGradient *pGradient = pEntry->ColorScaleType() == CColorScaleEntry::LOCAL ? pEntry->LocalColorGradient() : pEntry->GlobalColorGradient();
    assert (pGradient);
    switch(pEntry->ColorScaleType())
    {
    case CColorScaleEntry::LOCAL:
#if 0
        {
            // Get the color gradient
            CColorGradient *pGradient = pEntry->LocalColorGradient();
            assert(pGradient);
            pGradient->SetMinMaxValue(MinMaxValue().first.Value(), MinMaxValue().second.Value());

            // Get the range of the scene ... 
            if(MinMaxValue().second.Valid() && MinMaxValue().first.Valid())
            {
                double dFraction = (MinMaxValue().second.Value() - MinMaxValue().first.Value()) / (pGradient->MinMaxValue().second - pGradient->MinMaxValue().first);
                CLegendFrame::legend_scale::TColorMap mpColor;
                for(CColorGradient::color_iterator it = pGradient->Begin(); it != pGradient->End(); it++)
                {
                    // Fill color map
                    double value = MinMaxValue().first.Value() + dFraction * (it->first - pGradient->MinMaxValue().first);
                    mpColor.insert(CLegendFrame::legend_scale::TColorMap::value_type(value, it->second));
                }

                assert(mpColor.size() != 0);

                if(mpColor.size() == 1)	// Min and max are equal -> single value
                {
                    QString label = QString("%1").arg(MinMaxValue().first.Value());	
                    new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MinColor(), label);
                    label = QString("%1").arg(MinMaxValue().second.Value());	
                    new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MaxColor(), label);			
                }
                else
                {
                    // Create color scale
                    new CLegendFrame::legend_scale(LegendFrame(), mpColor, 10, 15, 300);
                    new CLegendFrame::legend_separator(LegendFrame(), 15);
                }
            }
            else
            {
                // Result is invalid ....
                QString label("Not defined");
                new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MinColor(), label);
                new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MaxColor(), label);	
            }
        }
        break;
#endif
    case CColorScaleEntry::GLOBAL:
        {
#if 0
            // Get the color gradient

            CColorGradient *pGradient = pEntry->GlobalColorGradient();
            assert(pGradient);
#endif
            if(MinMaxValue().second.Valid() && MinMaxValue().first.Valid())
            {
                if(pGradient->AutoExtremes())
                    pGradient->SetMinMaxValue(MinMaxValue().first.Value(), MinMaxValue().second.Value());

                CLegendFrame::legend_scale::TColorMap mpColor;
                for(CColorGradient::color_iterator it = pGradient->Begin(); it != pGradient->End(); it++)
                {
                    // Fill color map
                    mpColor.insert(CLegendFrame::legend_scale::TColorMap::value_type(it->first, it->second));
                }

                assert(mpColor.size() > 0);

                // Create color scale
                new CLegendFrame::legend_scale(LegendFrame(), mpColor, 10, 15, 300);
                new CLegendFrame::legend_separator(LegendFrame(), 15);
            }
            else
            {
                // Result is invalid ....
                QString label("Not defined"); //wjrx 
                new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MinColor(), label);
                new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MaxColor(), label);	
            }

        }
        break;
    case CColorScaleEntry::HOTSPOT:
        {
            const CHotSpot *pHotSpot = pEntry->HotSpot(); 
            assert(pHotSpot); // has to be a hotspot...
            if(pHotSpot->CenterIsTransparent())
            {
                QString label = QString("< %1").arg(pHotSpot->MinMaxValue().first);
                new CLegendFrame::legend_index(LegendFrame(), 15, 15, pHotSpot->MinColor(), label);
                new CLegendFrame::legend_separator(LegendFrame(), 3);
                label = QString("> %1").arg(pHotSpot->MinMaxValue().second);
                new CLegendFrame::legend_index(LegendFrame(), 15, 15, pHotSpot->MaxColor(), label);
            }
            else
            {
                if(pHotSpot->MinMaxValue().first != pHotSpot->MinMaxValue().second)
                {
                    CLegendFrame::legend_scale::TColorMap mpColor;
                    mpColor.insert(CLegendFrame::legend_scale::TColorMap::value_type(pHotSpot->MinMaxValue().first, pHotSpot->MinColor()));
                    mpColor.insert(CLegendFrame::legend_scale::TColorMap::value_type(pHotSpot->MinMaxValue().second, pHotSpot->MaxColor()));
                    new CLegendFrame::legend_scale(LegendFrame(), mpColor, 10, 15, 300);
                    new CLegendFrame::legend_separator(LegendFrame(), 3);
                }

                QString label = QString("> %1").arg( pHotSpot->MinMaxValue().second);
                new CLegendFrame::legend_index(LegendFrame(), 15, 15, pHotSpot->MaxColor(), label);	
                new CLegendFrame::legend_separator(LegendFrame(), 15);
            }
        }
        break;
    default:
        assert(FALSE);	// Unknown
        break;
    }

}

int COpenInventorEventsHandler::InsertLinkedNodesInLegend()
{
	// We fill the node map and divide the nodes in groups based on the TypeName()
	typedef std::set<const COpenGLNode*, CGraphNode::CLess> TNodeSet;
	typedef std::map<QString, TNodeSet> TColorMap;
	TColorMap mpNode;
	int nRejectedNodes = 0;

	TOpenGLNodeSet stNode = NodesInSceneForLegend();
	TOpenGLNodeSet::iterator itNode;
	for(itNode = stNode.begin(); itNode != stNode.end(); itNode++)
	{
		if(OnInsertNodeInLegend(**itNode))
		{
			QString sTypeName = (*itNode)->TypeName();
			if((sTypeName.length() > 0) && ((*itNode)->DisplayListSize() > 0))
			{
				TColorMap::iterator it = mpNode.insert(TColorMap::value_type(sTypeName, TNodeSet())).first;
				it->second.insert(*itNode);
			}

		}
		else
			nRejectedNodes++;
	}

	// Insert the nodes in the legend
	for(TColorMap::iterator it = mpNode.begin(); it != mpNode.end(); it++)
	{
		TNodeSet& node_set = it->second;
		QString sName = it->first;

		// Create caption
		new CLegendFrame::legend_separator(LegendFrame(), 20);
		new CLegendFrame::legend_label(LegendFrame(), sName);
		new CLegendFrame::legend_separator(LegendFrame(), 10);
		for(TNodeSet::iterator ob_it = node_set.begin(); ob_it != node_set.end(); ob_it++)
		{
			new CLegendFrame::legend_index(LegendFrame(), 15, 15, (*ob_it)->Color(), (*ob_it)->Name());
			new CLegendFrame::legend_separator(LegendFrame(), 3);
		}
	}

	if(mpNode.size() > 0)
		new CLegendFrame::legend_separator(LegendFrame(), 20);

	return nRejectedNodes;
}

bool COpenInventorEventsHandler::OnInsertNodeInLegend(const COpenGLNode& node) const
{
	if(ShowValue() && m_sceneNode.ValueComponent() != 0)
		return !m_sceneNode.ValueComponent()->CanMap(node);
	return true;
}

COpenInventorEventsHandler::TOpenGLNodeSet COpenInventorEventsHandler::NodesInSceneForLegend() const
{
  TOpenGLNodeSet stNodes = NodesInScene();

  // add formations
  TOpenGLNodeSet stFormations;

  for(TOpenGLNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    const IFormationElementSet* pFormationElementSet = dynamic_cast<const IFormationElementSet*>(*it);
    if(pFormationElementSet)
      stFormations.insert(&pFormationElementSet->Formation());
  }

  stNodes.insert(stFormations.begin(), stFormations.end());

  return stNodes;
}

void COpenInventorEventsHandler::AddResultPath(const IResultComponent& result_component)
{
	std::vector<QString> vcPath;
	
	// Get result of component
	const IResult& result = dynamic_cast<const IResult&>(result_component.Parent());
	
	// Is the result a scalar?
	if(result.ComponentSize() > 1)
		vcPath.push_back(result_component.Name());

	vcPath.push_back(result.Name());
	const CResultGroup *pGroup = result.Parent();
	while(pGroup)
	{
		vcPath.push_back(pGroup->Name());
		pGroup = pGroup->Parent();
	}

	// Reverse insert of result path
	new CLegendFrame::legend_label(LegendFrame(), vcPath[vcPath.size() - 1]);

	CLegendFrame::legend_tree_item* pItem = 0;
	for(int i = 1; i < vcPath.size(); i++)
	{
		int nIndex = vcPath.size() - i - 1;
		if(pItem)
			pItem = new CLegendFrame::legend_tree_item(vcPath[nIndex], pItem->child_position(), LegendFrame());
		else
			pItem = new CLegendFrame::legend_tree_item(vcPath[nIndex], 0, LegendFrame());
	}
}

COpenInventorEventsHandler::TOpenGLNodeSet COpenInventorEventsHandler::NodesInScene() const
{
  TOpenGLNodeSet stNodes = m_sceneNode.NodesInScene();

  const COpenInventorSceneNode::TLinkedGraphNodeSet& stGraphNodes = m_sceneNode.GraphNodesInScene();
  for(COpenInventorSceneNode::TLinkedGraphNodeSet::const_iterator it = stGraphNodes.begin(); it != stGraphNodes.end(); ++it)
  {
    const CCrossSection* pCS = dynamic_cast<const CCrossSection*>(*it);
    if(pCS)
    {
      std::vector<CCrossSection::CCSObject*> vcCSObjects = pCS->CSObjects();
      for(int i = 0; i < vcCSObjects.size(); ++i)
      {
        const CCrossSection::CCSFormation* pCSFormation = dynamic_cast<const CCrossSection::CCSFormation*>(vcCSObjects[i]);
        if(pCSFormation)
        {
          stNodes.insert(pCSFormation);
        }
      }
    }
  }

  return stNodes;
}

COpenInventorEventsHandler::TMinMax COpenInventorEventsHandler::MinMaxValue()
{
  if(m_meshNodeManager)
  {
    double min, max;
    m_meshNodeManager->GetMeshDataMinMax(min, max);
    return TMinMax(min, max);
  }
  return TMinMax();
}

void COpenInventorEventsHandler::UpdateValuesLegend(const IValueComponentBase *valueComponent)
{
  m_meshNodeManager->UpdateValuesLegendAdornments(valueComponent);
  //m_meshNodeManager->DisplayValuesLegend(); // this causes a crash later on during render
}

const CColorScaleEntry * COpenInventorEventsHandler::GetColorScaleEntry() const
{
    return m_sceneNode.GetColorScaleEntry();
}

const double COpenInventorEventsHandler::MinValue()
{
    double minimum, maximum;
    m_meshNodeManager->GetMeshDataMinMax(minimum, maximum);
    return minimum;
}

const double COpenInventorEventsHandler::MaxValue()
{
    double minimum, maximum;
    m_meshNodeManager->GetMeshDataMinMax(minimum, maximum);
    return maximum;
}

void COpenInventorEventsHandler::ShowGeologyLegend()
{
    m_meshNodeManager->ShowGeologyLegend();
}

void COpenInventorEventsHandler::SetTextColor( float * rgb )
{
    m_meshNodeManager->SetTextColor(rgb); 
}

void COpenInventorEventsHandler::GetTextColor( float * rgb ) const
{
    m_meshNodeManager->GetTextColor(rgb);
}

void COpenInventorEventsHandler::SetCoordinateAxisColor( float * rgb )
{
    m_coordinateAxis->SetAxisColor(rgb); 
}

void COpenInventorEventsHandler::GetCoordinateAxisColor( float * rgb ) const
{
    m_coordinateAxis->GetAxisColor(rgb);
}

void COpenInventorEventsHandler::SetBackgroundColor( float * rgb )
{
    m_background->color0.setValue(rgb); 
}

void COpenInventorEventsHandler::GetBackgroundColor( float * rgb ) const
{
    const SbColor & color = m_background->color0.getValue();
    color.getValue(rgb[0], rgb[1], rgb[2]);
}

void COpenInventorEventsHandler::SetBackgroundColor2( float * rgb )
{
    m_background->color1.setValue(rgb); 
}

void COpenInventorEventsHandler::GetBackgroundColor2( float * rgb ) const
{
    const SbColor & color = m_background->color1.getValue();
    color.getValue(rgb[0], rgb[1], rgb[2]);
}

void COpenInventorEventsHandler::PointSize( const double & size )
{
    m_pointSize = size;
    m_coordinateAxis->PointSize(m_pointSize);
    m_meshNodeManager->PointSize(m_pointSize);
}

const double & COpenInventorEventsHandler::PointSize() const
{
    return m_pointSize;
}

int COpenInventorEventsHandler::DraggerTimeOut() const
{
    return m_draggerTimeOut;
}

void COpenInventorEventsHandler::DraggerTimeOut(int timeOut)
{
    m_draggerTimeOut = timeOut;
}

int COpenInventorEventsHandler::Transparency() const
{
    return m_meshNodeManager->Transparency();
}

void COpenInventorEventsHandler::Transparency(int transparency)
{
    m_meshNodeManager->Transparency (transparency);
}

int COpenInventorEventsHandler::IsoCount() const
{
    return m_meshNodeManager->IsoCount();
}

void COpenInventorEventsHandler::IsoCount(int isoCount)
{
    m_meshNodeManager->IsoCount (isoCount);
}

double COpenInventorEventsHandler::IsoGap() const
{
    return m_meshNodeManager->IsoGap();
}

void COpenInventorEventsHandler::IsoGap(double gap)
{
    m_meshNodeManager->IsoGap (gap);
}

void COpenInventorEventsHandler::removeDataSets()
{
    m_meshNodeManager->removeDataSets();
}

void COpenInventorEventsHandler::unlinkValueComponent()
{
    const_cast<COpenInventorSceneNode *> (& m_sceneNode)->unlinkValueComponent();
}

void COpenInventorEventsHandler::OnIsoValueComponentChanged( IValueComponentBase * pComponent, CDoubleQuantity::UNIT unit )
{
    m_meshNodeManager->OnIsoValueComponentChanged(pComponent, unit);
}


void COpenInventorEventsHandler::RemoveAllMeshes()
{
    if (m_meshNodeManager) m_meshNodeManager->RemoveAllMeshes();
}

void COpenInventorEventsHandler::ResetRenderAction()
{
    m_viewerAlgos->resetRenderAction();
}


bool COpenInventorEventsHandler::IsVisible( COpenGLNode* pOpenGLNode )
{
    return m_meshNodeManager->IsVisible (pOpenGLNode);
}

void COpenInventorEventsHandler::removeFromGeologyLegend( const COpenGLNode * pOpenGLNode )
{
    m_meshNodeManager->removeFromGeologyLegend(* pOpenGLNode);
}

void COpenInventorEventsHandler::addToGeologyLegend( const COpenGLNode * pOpenGLNode )
{
    m_meshNodeManager->addToGeologyLegend(* pOpenGLNode);
}

void COpenInventorEventsHandler::ModifyGeologyLegend( const COpenGLNode * pOpenGLNode )
{
    m_meshNodeManager->ModifyGeologyLegend(* pOpenGLNode);
}


SoSwitch* const & COpenInventorEventsHandler::gnomonSwitch() const
{
    return m_gnomonSwitch;
}

void COpenInventorEventsHandler::RestoreValueComponent( IValueComponentBase* pValueComponent )
{
    m_meshNodeManager->restoreValueComponent(pValueComponent);
}

void COpenInventorEventsHandler::ShowInFull()
{
    if (ShowTopView())
    {
       ViewZDirection(); 
    }
    else
    {
        ZoomReset();
    }

}

bool COpenInventorEventsHandler::ShowTopView() const
{
    return m_meshNodeManager->ShowTopView();
}

void COpenInventorEventsHandler::ShowTopView(bool show)
{
    m_meshNodeManager->ShowTopView(show);
    CheckLighting();
}

bool COpenInventorEventsHandler::PointSetSelected() const
{
    return m_meshNodeManager->PointSetSelected();
}

int COpenInventorEventsHandler::LockedSpinDirection()
{
    return m_lockedSpinDirection;
}

template <class ScreenDrawerClass>
ScreenDrawerClass * COpenInventorEventsHandler::getNewScreenDrawer()
{
    // we want one (only one!) in a pristine state, first remove any old screen drawer
    if (m_screenDrawer)
        m_screenDrawerGroup->removeChild(m_screenDrawer);

    ScreenDrawerClass * drawer = new ScreenDrawerClass;
    m_screenDrawerGroup->addChild(drawer);
    m_screenDrawer = drawer;

    return drawer;
}

int COpenInventorEventsHandler::NumberOfOpenGLNodesLinked()
{
    return m_sceneNode.NumberOfOpenGLNodesLinked();
}

void COpenInventorEventsHandler::dehighlightMesh()
{
    m_meshNodeManager->dehighlightMesh();
}

void COpenInventorEventsHandler::clearValueTrackerCellFacetSet()
{
    m_meshNodeManager->clearValueTrackerCellFacetSet();
}

void COpenInventorEventsHandler::InitCoordinateAxis()
{
    m_coordinateAxis = new CoordinateAxis(m_meshNodeManager);
    m_coordinateAxis->PointSize(m_pointSize);

    int childIndex = m_rootSceneGraph->findChild(m_meshNodeManager);
    if (childIndex < 0)
        m_rootSceneGraph->addChild (m_coordinateAxis);
    else
        m_rootSceneGraph->insertChild (m_coordinateAxis, childIndex);
}

void COpenInventorEventsHandler::ActUponNoValuesDisplayedWhenAdded()
{
  m_meshNodeManager->ActUponNoValuesDisplayedWhenAdded();
}

void COpenInventorEventsHandler::UpdateValueTrackerDisplay()
{
    m_meshNodeManager->UpdateValueTrackerDisplay();
}

void COpenInventorEventsHandler::AdjustView()
{
    ResetSceneEnclosureNodes();
    if (ShowTopView())
    {
        ShowInFull();
    }
    else
    {
        if (m_meshNodeManager->NumberOfOpenGLNodesLinked() == 1)
            ViewBirdEyeDirection();
    }
}

SbVec2f COpenInventorEventsHandler::ScreenDrawerToWorldPoint( const SbVec2f & pt )
{
    SbVec2f normalizedScreenPoint;

    // pt[..] lies between -1 and +1
    normalizedScreenPoint[0] = (pt[0] + 1) * 0.5;
    normalizedScreenPoint[1] = (pt[1] + 1) * 0.5; 

    SbVec2f worldPoint = GetWorldCoordinates(normalizedScreenPoint);

    return worldPoint;
}

SbVec2f COpenInventorEventsHandler::WorldToScreenDrawerPoint( const SbVec2f & worldPoint )
{
    SbVec2f normalizedScreenPoint = GetScreenNormalizedCoordinates( worldPoint); /* [0, 1] */
    SbVec2f drawerPoint;

    drawerPoint[0] = 2 * normalizedScreenPoint[0] - 1; /* [-1, 1] */
    drawerPoint[1] = 2 * normalizedScreenPoint[1] - 1;

    return drawerPoint;
}

CLegendFrame& COpenInventorEventsHandler::LegendFrame()
{
  return m_sceneNode.LegendFrame();
}
