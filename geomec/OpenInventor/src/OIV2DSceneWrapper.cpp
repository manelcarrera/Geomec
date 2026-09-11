#include "stdafx.h"

#include "OIV2DSceneWrapper.h"
#include "OIV2DScene.h"
#include "OIV2DHistoryScene.h"
#include "OIV2DWellPathGraphScene.h"
#include "OIV2DHistoryScene_Delegate.h"
#include "OIV2DWellPathGraphScene_Delegate.h"

OIV2DSceneWrapper::OIV2DSceneWrapper(OIV2DScene& scene)
  : m_scene (scene)
{
}

void OIV2DSceneWrapper::Activate()
{
  m_scene.Activate();
}

void OIV2DSceneWrapper::Deactivate()
{
  m_scene.Deactivate();
}

bool OIV2DSceneWrapper::IsOpenInventorScene() const
{
  return false;
}

OIV2DScene* OIV2DSceneWrapper::getOIV2DScene() const
{
  return &m_scene;
}

bool OIV2DSceneWrapper::ReParent(IOpenGLFrame* pParent)
{
  return true;
}

bool OIV2DSceneWrapper::IsLinkedTo(const CGraphNode& node)
{
  return m_scene.IsLinkedTo(node);
}

bool OIV2DSceneWrapper::CanConnectItem(const CGraphNode& node)
{
  return m_scene.CanConnectItem(node);
}

bool OIV2DSceneWrapper::CanConnectToMultipleItems(const CGraphNode& node)
{
  return m_scene.CanConnectToMultipleItems(node);
}

bool OIV2DSceneWrapper::ConnectItem(const CGraphNode& node)
{
  if (m_scene.ConnectItem(node))
  {
    m_scene.Modified();
    return true;
  } 

  return false;
}

void OIV2DSceneWrapper::LinkTo(const CGraphNode& node)
{
  m_scene.LinkTo(const_cast<CGraphNode&>(node));
}

void OIV2DSceneWrapper::UnLink(const CGraphNode& node)
{
  m_scene.UnLink(const_cast<CGraphNode&>(node));;
  m_scene.Modified();
}
  
CFemAppModel& OIV2DSceneWrapper::Model()
{
  return m_scene.Model();
}

IValueComponentBase* OIV2DSceneWrapper::ValueComponent() const
{
  // It's unclear what to return here in case of multiple valuecomponents, so
  // for now just return null.
  return m_scene.NumberOfValueComponents() >= 1 ? m_scene.ValueComponent(0) : 0;
}

bool OIV2DSceneWrapper::MultipleStagesAllowed() const
{
  return m_scene.MultipleStagesAllowed();
}

bool OIV2DSceneWrapper::MultipleValueComponentsAllowed() const
{
  return m_scene.MultipleValueComponentsAllowed();
}

size_t OIV2DSceneWrapper::NumberOfValueComponents() const
{
  return m_scene.NumberOfValueComponents();
}

IValueComponentBase* OIV2DSceneWrapper::ValueComponent(size_t index) const
{
  return m_scene.ValueComponent(index);
}

std::pair<geo::CValue, geo::CValue> OIV2DSceneWrapper::MinMaxValue()
{
  return std::make_pair(geo::CValue(), geo::CValue());
}

const double OIV2DSceneWrapper::MinValue()
{
  return 0.0;
}

const double OIV2DSceneWrapper::MaxValue()
{
  return 0.0;
}


void OIV2DSceneWrapper::OnSelect(CGraphNode& node)
{
}

int OIV2DSceneWrapper::SelectMode()
{
  return m_scene.SelectMode();
}

void OIV2DSceneWrapper::invalidate(const geo::IObject &list, bool bUpdate)
{
}

void OIV2DSceneWrapper::Clear()
{
}

int  OIV2DSceneWrapper::Dimension()
{
  return 0;
}

void OIV2DSceneWrapper::ResetPan()
{
}

void OIV2DSceneWrapper::ResetScale()
{
}

void OIV2DSceneWrapper::OnZoomWindow()
{
}

bool OIV2DSceneWrapper::CanZoomWindow() const
{
  return true;
}

void OIV2DSceneWrapper::SwitchToTopView()
{
}

void OIV2DSceneWrapper::DisplayPerspective(bool bPerspective)
{
}

bool OIV2DSceneWrapper::DisplayPerspective() const
{
  return false;
}

bool OIV2DSceneWrapper::CanDisplayPerspective() const
{
  return false;
}

bool OIV2DSceneWrapper::ShowColor() const
{
  return false;
}

void OIV2DSceneWrapper::ShowColor(bool bValue)
{
}

bool OIV2DSceneWrapper::ShowValue() const
{
  return false;
}

bool OIV2DSceneWrapper::CanShowValue() const
{
  return false;
}

void OIV2DSceneWrapper::ShowBoundary(bool bShow)
{
}

bool OIV2DSceneWrapper::ShowBoundary() const
{
  return false;
}

bool OIV2DSceneWrapper::CanShowBoundary() const
{
  return false;
}

bool OIV2DSceneWrapper::ShowOptionsDialog()
{
  auto historyScene = dynamic_cast<OIV2DHistoryScene *>(&m_scene);
  auto wellPathGraphScene = dynamic_cast<OIV2DWellPathGraphScene *>(&m_scene);

  if (historyScene)
  {
    OIV2DHistoryScene_Delegate* pDelegate =
      dynamic_cast <OIV2DHistoryScene_Delegate*> (
        historyScene->getDelegate());
    return (pDelegate != 0) ? pDelegate->Attributes() : false;
  }
  if (wellPathGraphScene)
  {
    OIV2DWellPathGraphScene_Delegate* pDelegate =
      dynamic_cast <OIV2DWellPathGraphScene_Delegate*> (
        wellPathGraphScene->getDelegate());
    return (pDelegate != 0) ? pDelegate->Attributes() : false;
  }

  return false;
}

void OIV2DSceneWrapper::AllowDynamicRotate(bool bAllow)
{
}

bool OIV2DSceneWrapper::AllowDynamicRotate() const
{
  return false;
}

bool OIV2DSceneWrapper::CanAllowDynamicRotate() const
{
  return false;
}

bool OIV2DSceneWrapper::CanGoHome() const
{
  return false;
}

bool OIV2DSceneWrapper::CanSaveHome() const
{
  return false;
}

void OIV2DSceneWrapper::DisplayGnomon(bool bDisplay)
{
}

bool OIV2DSceneWrapper::DisplayGnomon() const
{
  return false;
}

bool OIV2DSceneWrapper::CanDisplayGnomon() const
{
  return false;
}

void OIV2DSceneWrapper::DisplayCoordinateAxis(bool bDisplay)
{
}

bool OIV2DSceneWrapper::DisplayCoordinateAxis() const
{
  return false;
}

bool OIV2DSceneWrapper::CanDisplayCoordinateAxis() const
{
  return false;
}

void OIV2DSceneWrapper::SelectMeshMode( MeshMode mode )
{
}

MeshMode OIV2DSceneWrapper::MeshModeSelected() const
{
  return FullMesh; //???
}

bool OIV2DSceneWrapper::CanSelectMeshMode() const
{
  return false;
}

void OIV2DSceneWrapper::SelectValueTracker(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::ValueTrackerSelected() const
{
  return false;
}

bool OIV2DSceneWrapper::CanSelectValueTracker() const
{
  return false;
}

void OIV2DSceneWrapper::SelectDraggerTracker(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::DraggerTrackerSelected() const
{
  return false;
}

bool OIV2DSceneWrapper::CanSelectDraggerTracker() const
{
  return false;
}

void OIV2DSceneWrapper::SelectMouseTracker(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::MouseTrackerSelected() const
{
  return false;
}

bool OIV2DSceneWrapper::CanSelectMouseTracker() const
{
  return false;
}

void OIV2DSceneWrapper::SetDraggerConfined(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::DraggerConfined() const
{
  return false;
}

bool OIV2DSceneWrapper::CanSetDraggerConfined() const
{
  return false;
}


void OIV2DSceneWrapper::HideLegend(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::LegendHidden() const
{
  return false;
}

bool OIV2DSceneWrapper::CanSelectHideLegend() const
{
  return false;
}

void OIV2DSceneWrapper::SelectHandMode(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::HandMode() const
{
  return false;
}

void OIV2DSceneWrapper::SelectClipPlaneMode(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::CanSelectClipPlaneMode() const
{
  return false;
}

bool OIV2DSceneWrapper::ClipPlaneModeSelected() const
{
  return false;
}

void OIV2DSceneWrapper::SelectContourLineMode(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::CanSelectContourLineMode() const
{
  return false;
}

bool OIV2DSceneWrapper::ContourLineModeSelected() const
{
  return false;
}

bool OIV2DSceneWrapper::MeshLinesDisplayed() const
{
  return false;
}

void OIV2DSceneWrapper::SelectMeshLinesDisplayed(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::CanSelectMeshLineDisplayMode() const
{
  return false;
}

bool OIV2DSceneWrapper::FormationOutlinesDisplayed() const
{
  return false;
}

void OIV2DSceneWrapper::SelectFormationOutlinesDisplayed(bool yesOrNo)
{
}

bool OIV2DSceneWrapper::CanSelectFormationOutlineDisplayMode() const
{
  return false;
}

bool OIV2DSceneWrapper::CanSelectHandMode() const
{
  return false;
}

void OIV2DSceneWrapper::ViewBeachBalls(bool bFill)
{
}

bool OIV2DSceneWrapper::BeachBallsOnView() const
{
  return false;
}

bool OIV2DSceneWrapper::CanViewBeachBalls() const
{
  return false;
}

void OIV2DSceneWrapper::Fill(bool bFill)
{
}

bool OIV2DSceneWrapper::Fill() const
{
  return false;
}

bool OIV2DSceneWrapper::CanFill() const
{
  return false;
}

void OIV2DSceneWrapper::TimerCalled ()
{
}

void OIV2DSceneWrapper::ZoomIn()
{
  m_scene.zoomIn();
}

void OIV2DSceneWrapper::ZoomOut()
{
  m_scene.zoomOut();
}

void OIV2DSceneWrapper::ZoomReset()
{
  m_scene.viewAll();
}

void OIV2DSceneWrapper::ShowValuesPerElement(bool perElement)
{
}

bool OIV2DSceneWrapper::ShowValuesPerElement() const
{
  return false;
}

bool OIV2DSceneWrapper::CanShowValuesPerElement() const
{
  return false;
}

const geo::CVector& OIV2DSceneWrapper::DisplacementVector() const
{
  return m_dummyVector;
}

void OIV2DSceneWrapper::DisplacementVector(const geo::CVector &newval)
{
}

const geo::CVector& OIV2DSceneWrapper::ModelScaleVector() const
{
  return m_dummyVector;
}

void OIV2DSceneWrapper::ModelScaleVector(const geo::CVector &newval)
{
}

const geo::CVector& OIV2DSceneWrapper::ScaleVector() const
{
  return m_dummyVector;
}

void OIV2DSceneWrapper::ScaleVector(const geo::CVector &newVal)
{
}

const geo::CVector& OIV2DSceneWrapper::RotateLockVector() const
{
  return m_dummyVector;
}

void OIV2DSceneWrapper::RotateLockVector(const geo::CVector& vec)
{
}

void OIV2DSceneWrapper::LockRotationVector(const geo::IVector& vector)
{
}

const geo::IVector& OIV2DSceneWrapper::LockRotationVector() const
{
  return m_dummyVector;
}

bool OIV2DSceneWrapper::CanLockRotationVector() const
{
  return false;
}

void OIV2DSceneWrapper::ViewEDirection()
{
}

void OIV2DSceneWrapper::ViewNDirection()
{
}

void OIV2DSceneWrapper::ViewZDirection()
{
}

void OIV2DSceneWrapper::GoHome()
{
}

void OIV2DSceneWrapper::SaveHome()
{
}

void OIV2DSceneWrapper::ViewBirdEyeDirection()
{
}

void OIV2DSceneWrapper::ViewProportional(bool bProportional)
{
}

bool OIV2DSceneWrapper::ViewProportional() const
{
  return false;
}

void OIV2DSceneWrapper::Lighting(bool bLighting)
{
}

bool OIV2DSceneWrapper::Lighting() const
{
  return false;
}

bool OIV2DSceneWrapper::CanEnableLighting() const
{
  return false;
}

void OIV2DSceneWrapper::EnableLighting(bool lighting)
{
}

bool OIV2DSceneWrapper::LightingEnabled() const
{
  return false;
}

void OIV2DSceneWrapper::OnSize(int cx, int cy)
{
  m_scene.setSize(cx, cy);
}

void OIV2DSceneWrapper::SetVectorScale(float scale)
{
}

void OIV2DSceneWrapper::SetVectorVisibleFraction(float factor)
{
}

void OIV2DSceneWrapper::ViewTensorVector( TensorVectorMode mode )
{
}

TensorVectorMode OIV2DSceneWrapper::ViewTensorVector()
{
  return TensorVectorFull;
}

bool OIV2DSceneWrapper::CanViewTensorVector()
{
  return false;
}

void OIV2DSceneWrapper::RefreshVectorBaseScale()
{
}

bool OIV2DSceneWrapper::CanRefreshVectorBaseScale()
{
  return false;
}
