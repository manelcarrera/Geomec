#ifndef _OIV2DSceneWrapper_h_
#define _OIV2DSceneWrapper_h_

#include "ISceneWrapper.h"

class OIV2DSceneWrapper : public ISceneWrapper
{
    OIV2DScene& m_scene;

    geo::CVector m_dummyVector;

public:

    OIV2DSceneWrapper(OIV2DScene& scene);

    virtual void Activate();
    virtual void Deactivate();

    virtual bool IsOpenInventorScene() const;
    virtual OIV2DScene* getOIV2DScene() const;

    virtual bool ReParent(IOpenGLFrame* pParent);
    virtual bool IsLinkedTo(const CGraphNode& node);
    virtual bool CanConnectItem(const CGraphNode& node);
    virtual bool CanConnectToMultipleItems(const CGraphNode& node);
   
    virtual bool ConnectItem(const CGraphNode& node);
    virtual void LinkTo(const CGraphNode& node);
    virtual void UnLink(const CGraphNode& node);
  
    virtual CFemAppModel& Model();
    virtual IValueComponentBase* ValueComponent() const;
    virtual bool MultipleValueComponentsAllowed() const;
    virtual bool MultipleStagesAllowed() const;

    virtual size_t NumberOfValueComponents() const;
   
    virtual IValueComponentBase* ValueComponent(size_t index) const;
   
    virtual std::pair<geo::CValue, geo::CValue> MinMaxValue();
    virtual const double MinValue();
    virtual const double MaxValue();

    virtual int SelectMode();

    virtual void OnSelect(CGraphNode& node);
    virtual void invalidate(const geo::IObject &list, bool bUpdate = true);

    virtual void Clear();
    virtual int  Dimension();
    virtual void ResetPan();
    virtual void ResetScale();

    virtual void OnZoomWindow();
    virtual bool CanZoomWindow() const;
    virtual void SwitchToTopView();
    virtual void DisplayPerspective(bool bPerspective);
    virtual bool DisplayPerspective() const;
    virtual bool CanDisplayPerspective() const;

    virtual bool ShowColor() const;
    virtual void ShowColor(bool bValue);
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
    virtual void SelectMeshMode( MeshMode mode );
    virtual MeshMode MeshModeSelected() const;
    virtual bool CanSelectMeshMode() const;
    virtual void SelectValueTracker(bool yesOrNo);
    virtual bool ValueTrackerSelected() const;
    virtual bool CanSelectValueTracker() const;

    virtual void SelectDraggerTracker(bool yesOrNo);
    virtual bool DraggerTrackerSelected() const;
    virtual bool CanSelectDraggerTracker() const;
    virtual void SelectMouseTracker(bool yesOrNo);
    virtual bool MouseTrackerSelected() const;
    virtual bool CanSelectMouseTracker() const;

    virtual void SetDraggerConfined(bool yesOrNo);
    virtual bool DraggerConfined() const;
    virtual bool CanSetDraggerConfined() const;

    virtual void HideLegend(bool yesOrNo);
    virtual bool LegendHidden() const;
    virtual bool CanSelectHideLegend() const;

    virtual void SelectHandMode(bool yesOrNo);
    virtual bool HandMode() const;
    virtual void SelectClipPlaneMode(bool yesOrNo);
    virtual bool CanSelectClipPlaneMode() const;
    virtual bool ClipPlaneModeSelected() const;
    virtual void SelectContourLineMode(bool yesOrNo);
    virtual bool CanSelectContourLineMode() const;
    virtual bool ContourLineModeSelected() const;
    virtual bool MeshLinesDisplayed() const;
    virtual void SelectMeshLinesDisplayed(bool yesOrNo);
    virtual bool CanSelectMeshLineDisplayMode() const;
    virtual bool FormationOutlinesDisplayed() const;
    virtual void SelectFormationOutlinesDisplayed(bool yesOrNo);
    virtual bool CanSelectFormationOutlineDisplayMode() const;

    virtual bool CanSelectHandMode() const;
    virtual void ViewBeachBalls(bool bFill);
    virtual bool BeachBallsOnView() const;
    virtual bool CanViewBeachBalls() const;
    virtual void Fill(bool bFill);
    virtual bool Fill() const;
    virtual bool CanFill() const;
    virtual void TimerCalled ();
    virtual void ZoomIn();
    virtual void ZoomOut();
    virtual void ZoomReset();
    virtual void ShowValuesPerElement(bool perElement);
    virtual bool ShowValuesPerElement() const;
    virtual bool CanShowValuesPerElement() const;

    virtual const geo::CVector &DisplacementVector() const;
    virtual void DisplacementVector(const geo::CVector &newval);
    virtual const geo::CVector &ModelScaleVector() const;
    virtual void ModelScaleVector(const geo::CVector &newval);
    virtual const geo::CVector &ScaleVector() const;
    virtual void ScaleVector(const geo::CVector &newVal);
    virtual const geo::CVector &RotateLockVector() const;
    virtual void RotateLockVector(const geo::CVector& vec);
    virtual void LockRotationVector(const geo::IVector& vector);
    virtual const geo::IVector& LockRotationVector() const;
    virtual bool CanLockRotationVector() const;
  
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

    virtual void SetVectorScale(float scale);
    virtual void SetVectorVisibleFraction(float factor);

    virtual void ViewTensorVector( TensorVectorMode mode );
    virtual TensorVectorMode ViewTensorVector();
    virtual bool CanViewTensorVector();

    virtual void RefreshVectorBaseScale(); 
    virtual bool CanRefreshVectorBaseScale();
};

#endif  // _OIV2DSceneWrapper_h_
