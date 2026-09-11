#ifndef _OPENINVENTORSCENEWRAPPER_H_
#define _OPENINVENTORSCENEWRAPPER_H_

#include "ISceneWrapper.h"
#include "MeshMode.h"
#include "OpenGLSceneBase.h"
#include "OpenInventorSceneNode.h"
#include "ValueCompositeObserver.h"

class COpenInventorSceneNode;
// class TValueComponentObserver;
// class CValueCompositeObserver;

class COpenInventorSceneWrapper : public ISceneWrapper {
  geo::CVector dummy;

public:
  COpenInventorSceneWrapper(COpenInventorSceneNode &scene);
  ~COpenInventorSceneWrapper();

  virtual void Activate();
  virtual void Deactivate();

  BOOL ViewComponent(TValueComponentObserver &observer);
  BOOL ViewComposite(CValueCompositeObserver &observer);

  // virtual COpenGLSceneBase* Scene() { return &dummy2; }
  // virtual CGraphNode* Node() { return &m_scene; }
  virtual COpenInventorSceneNode *OpenInventorScene();

  virtual bool IsOpenInventorScene() const;

  virtual bool ReParent(IOpenGLFrame *pParent);
  virtual bool IsLinkedTo(const CGraphNode &node);
  virtual void LinkTo(const CGraphNode &node);
  virtual void UnLink(const CGraphNode &node);
  virtual bool CanConnectItem(const CGraphNode &node);
  virtual bool ConnectItem(const CGraphNode &node);
  virtual bool CanConnectToMultipleItems(const CGraphNode &node);

  virtual void SetNonDrawing();
  virtual void SetDrawing();

  virtual int SelectMode();
  virtual void OnSelect(CGraphNode &node);

  virtual void invalidate(const geo::IObject &list, bool bUpdate = true);

  virtual CFemAppModel &Model();
  virtual IValueComponentBase *ValueComponent() const;
  virtual std::pair<geo::CValue, geo::CValue> MinMaxValue();
  virtual const double MinValue();
  virtual const double MaxValue();
  virtual void Clear();

  virtual int Dimension();
  virtual void ResetPan();
  virtual void ResetScale();

  virtual void OnZoomWindow();
  virtual bool CanZoomWindow() const;
  virtual void SwitchToTopView();
  virtual void DisplayPerspective(bool bPerspective);
  virtual bool DisplayPerspective() const;
  virtual bool CanDisplayPerspective() const;
  virtual void ShowColor(bool bValue);
  virtual bool ShowColor() const;
  virtual void ShowValue(bool bValue);
  virtual bool ShowValue() const;
  virtual bool CanShowValue() const;
  virtual void ShowBoundary(bool bShow);
  virtual bool ShowBoundary() const;
  virtual bool CanShowBoundary() const;
  virtual bool ShowOptionsDialog();
  virtual void AllowDynamicRotate(bool bAllow);
  virtual bool AllowDynamicRotate() const;
  virtual bool CanAllowDynamicRotate() const;
  virtual bool CanGoHome() const;
  virtual bool CanSaveHome() const;
  virtual void DisplayGnomon(bool bDisplay);
  virtual bool DisplayGnomon() const;
  virtual bool CanDisplayGnomon() const;
  virtual void DisplayCoordinateAxis(bool bDisplay);
  virtual bool DisplayCoordinateAxis() const;
  virtual bool CanDisplayCoordinateAxis() const;
  virtual void SelectMeshMode(MeshMode mode);
  virtual MeshMode MeshModeSelected() const;
  virtual bool CanSelectMeshMode() const;
  virtual bool DraggerTrackerSelected() const;
  virtual bool CanSelectDraggerTracker() const;
  virtual void SelectDraggerTracker(bool yesOrNo);
  virtual bool MouseTrackerSelected() const;
  virtual bool CanSelectMouseTracker() const;
  virtual void SelectMouseTracker(bool yesOrNo);
  virtual void SetDraggerConfined(bool yesOrNo);
  virtual bool DraggerConfined() const;
  virtual bool CanSetDraggerConfined() const;
  virtual void HideLegend(bool yesOrNo);
  virtual bool LegendHidden() const;
  virtual bool CanSelectHideLegend() const;
  virtual bool HandMode() const;
  virtual bool CanSelectHandMode() const;
  virtual void SelectClipPlaneMode(bool yesOrNo);
  virtual bool CanSelectClipPlaneMode() const;
  virtual bool ClipPlaneModeSelected() const;
  virtual void SelectContourLineMode(bool yesOrNo);
  virtual bool CanSelectContourLineMode() const;
  virtual bool ContourLineModeSelected() const;
  virtual void SelectMeshLinesDisplayed(bool yesOrNo);
  virtual bool CanSelectMeshLineDisplayMode() const;
  virtual bool MeshLinesDisplayed() const;
  virtual void SelectFormationOutlinesDisplayed(bool yesOrNo);
  virtual bool CanSelectFormationOutlineDisplayMode() const;
  virtual bool FormationOutlinesDisplayed() const;
  virtual void SelectHandMode(bool yesOrNo);
  virtual void Fill(bool bFill);
  virtual bool Fill() const;
  virtual bool CanFill() const;
  virtual void TimerCalled();
  virtual void ZoomIn();
  virtual void ZoomOut();
  virtual void ZoomReset();
  virtual void DisplacementVector(const geo::CVector &newval);
  virtual const geo::CVector &DisplacementVector() const;
  virtual const geo::CVector &ModelScaleVector() const;
  virtual void ModelScaleVector(const geo::CVector &newval);
  virtual const geo::CVector &ScaleVector() const;
  virtual void ScaleVector(const geo::CVector &newVal);
  virtual void RotateLockVector(const geo::CVector &vec);
  virtual const geo::CVector &RotateLockVector() const;
  virtual void LockRotationVector(const geo::IVector &vector);
  virtual const geo::IVector &LockRotationVector() const;
  virtual bool CanLockRotationVector() const;
  virtual void ViewTensorVector(TensorVectorMode mode);
  virtual TensorVectorMode ViewTensorVector();
  virtual bool CanViewTensorVector();
  virtual void RefreshVectorBaseScale();
  virtual bool CanRefreshVectorBaseScale();
  virtual void ViewEDirection();
  virtual void ViewNDirection();
  virtual void ViewZDirection();
  virtual void GoHome();
  virtual void SaveHome();
  virtual void ViewBirdEyeDirection();
  virtual void ViewProportional(bool bProportional);
  virtual bool ViewProportional() const;
  virtual void Lighting(bool bLighting);
  virtual bool Lighting() const;
  virtual bool CanEnableLighting() const;
  virtual void EnableLighting(bool lighting);
  virtual bool LightingEnabled() const;
  virtual void OnSize(int cx, int cy);
  virtual void ShowValuesPerElement(bool perElement);
  virtual bool ShowValuesPerElement() const;
  virtual bool CanShowValuesPerElement() const;
  virtual void ViewBeachBalls(bool bView);
  virtual bool BeachBallsOnView() const;
  virtual bool CanViewBeachBalls() const;

  virtual void SetVectorScale(float scale);
  virtual void SetVectorVisibleFraction(float factor);

private:
  COpenInventorSceneNode &m_scene;
};

#endif // _OPENINVENTORSCENEWRAPPER_H_
