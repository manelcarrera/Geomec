#include "stdafx.h"

#include "OpenInventorSceneWrapper.h"
#include "OpenInventorSceneNode.h"
#include "ValueCompositeObserver.h"
#include "ModelView.h"
#include "ModelTreeView.h"
#include "GeomecDoc.h"
#include "Geomec.h"
#include "OpenInventorSceneNode_Delegate.h"


// Note: some const methods return bool, but will be used as int (0, 1) in ModelView.
// Visual Studio leaves const bool as it is, and apparently we can end up with
// values larger than 1, causing an ASSERT. Therefore, we return bValue ? 1 : 0
// where appropriate.


COpenInventorSceneWrapper::COpenInventorSceneWrapper(COpenInventorSceneNode& scene)
  : m_scene(scene)
{
  m_DocLink.LinkTo(m_scene);
}

COpenInventorSceneWrapper::~COpenInventorSceneWrapper()
{
  m_DocLink.UnLink(m_scene);
}

void COpenInventorSceneWrapper::Activate()
{
  m_scene.Activate();
}

void COpenInventorSceneWrapper::Deactivate()
{
  m_scene.Deactivate();
}

COpenInventorSceneNode* COpenInventorSceneWrapper::OpenInventorScene()
{
  return &m_scene;
}

bool COpenInventorSceneWrapper::IsOpenInventorScene() const
{
  return true;
}

bool COpenInventorSceneWrapper::ReParent(IOpenGLFrame* pParent)
{
  // not implemented in m_scene
  // m_scene.reParent(pParent) is unrelated to IOpenGLFrame
  return true;
}

bool COpenInventorSceneWrapper::IsLinkedTo(const CGraphNode& node)
{
  return m_scene.IsLinkedTo(node);
}

void COpenInventorSceneWrapper::LinkTo(const CGraphNode& node)
{
  m_scene.LinkTo(const_cast<CGraphNode&>(node));
}

void COpenInventorSceneWrapper::UnLink(const CGraphNode& node)
{
  m_scene.UnLink(const_cast<CGraphNode&>(node));;
  m_scene.Modified();
}

bool COpenInventorSceneWrapper::CanConnectItem(const CGraphNode& node)
{
  return m_scene.CanConnectItem(node);
}

bool COpenInventorSceneWrapper::ConnectItem(const CGraphNode& node)
{
  if (m_scene.ConnectItem(node))
  {
    m_scene.Modified();
    return true;
  } 

  return false;
}

bool COpenInventorSceneWrapper::CanConnectToMultipleItems( const CGraphNode& node )
{
  return m_scene.CanConnectToMultipleItems(node);
}

void COpenInventorSceneWrapper::SetNonDrawing()
{
  m_scene.SetNonDrawing();
}

void COpenInventorSceneWrapper::SetDrawing()
{
  m_scene.SetDrawing();
}


int COpenInventorSceneWrapper::SelectMode()
{
  return COpenGLSceneNode::SM_CHECK;
}

void COpenInventorSceneWrapper::OnSelect(CGraphNode& node)
{
  // not implemented in m_scene
}


void COpenInventorSceneWrapper::invalidate(const geo::IObject &list, bool bUpdate)
{
  // not implemented in m_scene
}


CFemAppModel& COpenInventorSceneWrapper::Model()
{
  return m_scene.Model();
}

IValueComponentBase* COpenInventorSceneWrapper::ValueComponent() const
{
  return m_scene.ValueComponent();
}

std::pair<geo::CValue, geo::CValue> COpenInventorSceneWrapper::MinMaxValue()
{
  // not implemented in m_scene
  return std::make_pair(geo::CValue(), geo::CValue());
}

const double COpenInventorSceneWrapper::MinValue()
{
  return m_scene.MinValue();
}

const double COpenInventorSceneWrapper::MaxValue()
{
  return m_scene.MaxValue();
}

void COpenInventorSceneWrapper::Clear()
{
  m_scene.Clear();
}


int COpenInventorSceneWrapper::Dimension()
{
  // not implemented in m_scene, assume DIM_3D
  return IOpenGLFrame::DIM_3D;
}

void COpenInventorSceneWrapper::ResetPan()
{
  // not implemented in m_scene;
}

void COpenInventorSceneWrapper::ResetScale()
{
  // not implemented in m_scene;
}

void COpenInventorSceneWrapper::OnZoomWindow()
{
  m_scene.OnZoomWindow();
}

bool COpenInventorSceneWrapper::CanZoomWindow() const
{
  return m_scene.CanZoomWindow();
}

void COpenInventorSceneWrapper::SwitchToTopView()
{
  m_scene.SwitchToTopView();
}

void COpenInventorSceneWrapper::DisplayPerspective(bool bPerspective)
{
  m_scene.DisplayPerspective(bPerspective);
}

bool COpenInventorSceneWrapper::DisplayPerspective() const
{
  return m_scene.DisplayPerspective() ? 1 : 0;
}

bool COpenInventorSceneWrapper::CanDisplayPerspective() const
{
  return m_scene.CanDisplayPerspective();
}

bool COpenInventorSceneWrapper::ShowColor() const
{
  return m_scene.ShowColor();
}

void COpenInventorSceneWrapper::ShowColor(bool bValue)
{
  m_scene.ShowColor(bValue);
}

bool COpenInventorSceneWrapper::ShowValue() const
{
  return m_scene.ShowValue();
}

void COpenInventorSceneWrapper::ShowValue(bool bValue)
{
  m_scene.ShowValue(bValue);
}

bool COpenInventorSceneWrapper::CanShowValue() const
{
  return m_scene.CanShowValue();
}

void COpenInventorSceneWrapper::ShowBoundary(bool bShow)
{
  m_scene.ShowBoundary(bShow);
}

bool COpenInventorSceneWrapper::ShowBoundary() const
{
  return m_scene.ShowBoundary() ? 1 : 0;
}

bool COpenInventorSceneWrapper::CanShowBoundary() const
{
  return m_scene.CanShowBoundary();
}

bool COpenInventorSceneWrapper::ShowOptionsDialog()
{
  COpenInventorSceneNode_Delegate* pDelegate =
    dynamic_cast <COpenInventorSceneNode_Delegate*> (m_scene.getDelegate());
  return (pDelegate != 0) ? pDelegate->Attributes() : false;
}

void COpenInventorSceneWrapper::AllowDynamicRotate(bool bAllow)
{
  m_scene.AllowDynamicRotate(bAllow);
}

bool COpenInventorSceneWrapper::AllowDynamicRotate() const
{
  return m_scene.AllowDynamicRotate() ? 1 : 0;
}

bool COpenInventorSceneWrapper::CanAllowDynamicRotate() const
{
  return m_scene.CanAllowDynamicRotate();
}

bool COpenInventorSceneWrapper::CanGoHome() const
{
  return true;
}

bool COpenInventorSceneWrapper::CanSaveHome() const
{
  return true;
}


void COpenInventorSceneWrapper::DisplayGnomon(bool bDisplay)
{
  m_scene.DisplayGnomon(bDisplay);
}

bool COpenInventorSceneWrapper::DisplayGnomon() const
{
  return m_scene.DisplayGnomon() ? 1 : 0;
}

bool COpenInventorSceneWrapper::CanDisplayGnomon() const
{
  return m_scene.CanDisplayGnomon();
}

void COpenInventorSceneWrapper::DisplayCoordinateAxis(bool bDisplay)
{
  m_scene.DisplayCoordinateAxis(bDisplay);
}

bool COpenInventorSceneWrapper::DisplayCoordinateAxis() const
{
  return m_scene.DisplayCoordinateAxis() ? 1 : 0;
}

bool COpenInventorSceneWrapper::CanDisplayCoordinateAxis() const
{
  return m_scene.CanDisplayCoordinateAxis();
}

void COpenInventorSceneWrapper::SelectMeshMode( MeshMode mode )
{
  m_scene.SelectMeshMode( mode );
}

MeshMode COpenInventorSceneWrapper::MeshModeSelected() const
{
  return m_scene.MeshModeSelected();
}

bool COpenInventorSceneWrapper::CanSelectMeshMode() const
{
  return m_scene.CanSelectMeshMode();
}

void COpenInventorSceneWrapper::SelectDraggerTracker(bool bDisplay)
{
  m_scene.SelectDraggerTracker(bDisplay);
}

bool COpenInventorSceneWrapper::DraggerTrackerSelected() const
{
  return m_scene.DraggerTrackerSelected() ? 1 : 0;
}

bool COpenInventorSceneWrapper::CanSelectDraggerTracker() const
{
  return m_scene.CanSelectDraggerTracker();
}

void COpenInventorSceneWrapper::SelectMouseTracker(bool bDisplay)
{
  m_scene.SelectMouseTracker(bDisplay);
}

bool COpenInventorSceneWrapper::MouseTrackerSelected() const
{
  return m_scene.MouseTrackerSelected() ? 1 : 0;
}

bool COpenInventorSceneWrapper::CanSelectMouseTracker() const
{
  return m_scene.CanSelectMouseTracker();
}

bool COpenInventorSceneWrapper::DraggerConfined() const
{
  return m_scene.DraggerConfined();
}

bool COpenInventorSceneWrapper::CanSetDraggerConfined() const
{
  return m_scene.CanSetDraggerConfined();
}

void COpenInventorSceneWrapper::SetDraggerConfined(bool yesOrNo)
{
  m_scene.SetDraggerConfined(yesOrNo);
}

bool COpenInventorSceneWrapper::LegendHidden() const
{
  return m_scene.LegendHidden();
}

bool COpenInventorSceneWrapper::CanSelectHideLegend() const
{
  return m_scene.CanSelectHideLegend();
}

void COpenInventorSceneWrapper::HideLegend(bool yesOrNo)
{
  m_scene.HideLegend(yesOrNo);
}

void COpenInventorSceneWrapper::SelectHandMode(bool bDisplay)
{
  m_scene.SelectHandMode(bDisplay);
}

bool COpenInventorSceneWrapper::HandMode() const
{
  return m_scene.HandMode() ? 1 : 0;
}

void COpenInventorSceneWrapper::SelectClipPlaneMode(bool bDisplay)
{
  m_scene.SelectClipPlaneMode(bDisplay);
}

bool COpenInventorSceneWrapper::CanSelectClipPlaneMode() const
{
  return m_scene.CanSelectClipPlaneMode();
}

bool COpenInventorSceneWrapper::ClipPlaneModeSelected() const
{
  return m_scene.ClipPlaneModeSelected();
}

void COpenInventorSceneWrapper::SelectContourLineMode(bool bDisplay)
{
  m_scene.SelectContourLineMode(bDisplay);
}

bool COpenInventorSceneWrapper::CanSelectContourLineMode() const
{
  return m_scene.CanSelectContourLineMode();
}

bool COpenInventorSceneWrapper::ContourLineModeSelected() const
{
  return m_scene.ContourLineModeSelected();
}

bool COpenInventorSceneWrapper::MeshLinesDisplayed() const
{
  return m_scene.MeshLinesDisplayed();
}

bool COpenInventorSceneWrapper::CanSelectMeshLineDisplayMode() const
{
  return m_scene.CanSelectMeshLineDisplayMode();
}

void COpenInventorSceneWrapper::SelectMeshLinesDisplayed(bool yesOrNo)
{
  m_scene.SelectMeshLinesDisplayed(yesOrNo);
}

bool COpenInventorSceneWrapper::FormationOutlinesDisplayed() const
{
  return m_scene.FormationOutlinesDisplayed();
}

bool COpenInventorSceneWrapper::CanSelectFormationOutlineDisplayMode() const
{
  return m_scene.CanSelectFormationOutlineDisplayMode();
}

void COpenInventorSceneWrapper::SelectFormationOutlinesDisplayed(bool yesOrNo)
{
  m_scene.SelectFormationOutlinesDisplayed(yesOrNo);
}

bool COpenInventorSceneWrapper::CanSelectHandMode() const
{
  return m_scene.CanSelectHandMode();
}

void COpenInventorSceneWrapper::Fill(bool bFill)
{
  m_scene.Fill(bFill);
}

bool COpenInventorSceneWrapper::Fill() const
{
  return m_scene.Fill() ? 1 : 0;
}

bool COpenInventorSceneWrapper::CanFill() const
{
  return m_scene.CanFill();
}

void COpenInventorSceneWrapper::TimerCalled()
{
  m_scene.TimerCalled();
}

void COpenInventorSceneWrapper::ZoomIn()
{
  m_scene.ZoomIn();
}

void COpenInventorSceneWrapper::ZoomOut()
{
  m_scene.ZoomOut();
}

void COpenInventorSceneWrapper::ZoomReset()
{
  m_scene.ZoomReset();
}

void COpenInventorSceneWrapper::DisplacementVector(const geo::CVector &newval)
{
  // not implemented in m_scene
}

const geo::CVector &COpenInventorSceneWrapper::DisplacementVector() const
{
  // not implemented in m_scene
  return dummy;
}

const geo::CVector &COpenInventorSceneWrapper::ModelScaleVector() const
{
  // not implemented in m_scene
  return dummy;
}

void COpenInventorSceneWrapper::ModelScaleVector(const geo::CVector &newval)
{
  // not implemented in m_scene
}

const geo::CVector &COpenInventorSceneWrapper::ScaleVector() const
{
  // not implemented in m_scene
  return dummy;
}

void COpenInventorSceneWrapper::ScaleVector(const geo::CVector &newVal)
{
  // not implemented in m_scene
}

void COpenInventorSceneWrapper::RotateLockVector(const geo::CVector& vec)
{
  // not implemented in m_scene
}

const geo::CVector &COpenInventorSceneWrapper::RotateLockVector() const
{
  // not implemented in m_scene
  return dummy;
}

void COpenInventorSceneWrapper::LockRotationVector(const geo::IVector& vector)
{
  m_scene.LockRotationVector(vector);
}

const geo::IVector& COpenInventorSceneWrapper::LockRotationVector() const
{
  return m_scene.LockRotationVector();
}

bool COpenInventorSceneWrapper::CanLockRotationVector() const
{
  return m_scene.CanLockRotationVector();
}

void COpenInventorSceneWrapper::ViewTensorVector( TensorVectorMode mode )
{
  m_scene.ViewTensorVector(mode);
}

TensorVectorMode COpenInventorSceneWrapper::ViewTensorVector()
{
  return m_scene.ViewTensorVector();
}

bool COpenInventorSceneWrapper::CanViewTensorVector()
{
  return true;
}

void COpenInventorSceneWrapper::RefreshVectorBaseScale()
{
  m_scene.RefreshVectorBaseScale();
}
bool COpenInventorSceneWrapper::CanRefreshVectorBaseScale()
{
  return m_scene.CanRefreshVectorBaseScale();
}

void COpenInventorSceneWrapper::ViewEDirection()
{
  m_scene.ViewEDirection();
}

void COpenInventorSceneWrapper::ViewNDirection()
{
  m_scene.ViewNDirection();
}

void COpenInventorSceneWrapper::ViewZDirection()
{
  m_scene.ViewZDirection();
}

void COpenInventorSceneWrapper::SaveHome()
{
  m_scene.SaveHome();
}

void COpenInventorSceneWrapper::GoHome()
{
  m_scene.GoHome();
}

void COpenInventorSceneWrapper::ViewBirdEyeDirection()
{
  m_scene.ViewBirdEyeDirection();
}

void COpenInventorSceneWrapper::ViewProportional(bool bProportional)
{
  m_scene.ViewProportional(bProportional);
}

bool COpenInventorSceneWrapper::ViewProportional() const
{
  return m_scene.ViewProportional();
}

void COpenInventorSceneWrapper::Lighting(bool bLighting)
{
  m_scene.Lighting(bLighting);
}

bool COpenInventorSceneWrapper::Lighting() const
{
  return m_scene.Lighting();
}

bool COpenInventorSceneWrapper::CanEnableLighting() const
{
  return m_scene.CanEnableLighting();
}

void COpenInventorSceneWrapper::EnableLighting(bool lighting)
{
  m_scene.Lighting(lighting);
}

bool COpenInventorSceneWrapper::LightingEnabled() const
{
  return m_scene.Lighting();
}

void COpenInventorSceneWrapper::OnSize(int cx, int cy)
{
  m_scene.OnSize(cx, cy);
}


void COpenInventorSceneWrapper::ShowValuesPerElement(bool perElement)
{
  m_scene.ShowValuesPerElement(perElement);
}

bool COpenInventorSceneWrapper::ShowValuesPerElement() const
{
  return m_scene.ShowValuesPerElement();
}

bool COpenInventorSceneWrapper::CanShowValuesPerElement() const
{
  return m_scene.CanShowValuesPerElement();
}

void COpenInventorSceneWrapper::ViewBeachBalls(bool view)
{
  m_scene.ViewBeachBalls(view);
}

bool COpenInventorSceneWrapper::BeachBallsOnView() const
{
  return m_scene.BeachBallsOnView();
}

bool COpenInventorSceneWrapper::CanViewBeachBalls() const
{
  return m_scene.CanViewBeachBalls();
}

void COpenInventorSceneWrapper::SetVectorScale(float scale)
{
  m_scene.SetVectorScale(scale);
}

void COpenInventorSceneWrapper::SetVectorVisibleFraction(float factor)
{
  m_scene.SetVectorVisibleFraction(factor);
}

BOOL COpenInventorSceneWrapper::ViewComponent( TValueComponentObserver &observer )
{
  return m_scene.ViewComponent (observer);
}

BOOL COpenInventorSceneWrapper::ViewComposite( CValueCompositeObserver &observer )
{
  return m_scene.ViewComposite (observer);
}

