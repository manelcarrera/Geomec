#pragma once

#include "ISceneWrapper.h"

#include "GeomecDoc.h"
#include "GraphNode.h"
#include "MeshMode.h"

#include <utility> // std::pair

#include "FemAppModel.h"

namespace geo {
class IObject;
class CVector;
} // namespace geo

/*class IOpenGLFrame;
class CFemAppDoc;
class CFemAppModel;
class COpenGLSceneBase;
class IValueComponentBase;
class COpenInventorSceneNode;
class OIV2DScene;

#include "ValueCompositeObserver.h"*/

class CEmptySceneWrapper : public ISceneWrapper {

public:
  CEmptySceneWrapper() {}
  // CEmptySceneWrapper(COpenGLSceneBase& scene) {}

  virtual ~CEmptySceneWrapper() {};

  virtual bool IsOpenInventorScene() const { return false; }; // = 0

  virtual bool ReParent(IOpenGLFrame *pParent) { return true; };         // = 0;
  virtual bool IsLinkedTo(const CGraphNode &node) { return false; };     // = 0;
  virtual bool CanConnectItem(const CGraphNode &node) { return false; }; // = 0;

  virtual bool ConnectItem(const CGraphNode &node) { return false; }; // = 0
  virtual void LinkTo(const CGraphNode &node) {};                     // = 0;
  virtual void UnLink(const CGraphNode &node) {};                     // = 0;

  //
  virtual CFemAppModel &Model() {
    // static CFemAppModel model; // ->abstract
    // return model;

    // FIXME
    return *GetGeomecDoc()->Model();
  }; // = 0;
  virtual IValueComponentBase *ValueComponent() const { return nullptr; }; // = 0;

  //
  virtual std::pair<geo::CValue, geo::CValue> MinMaxValue() { return std::pair<geo::CValue, geo::CValue>(); }; // = 0;

  virtual const double MinValue() { return 0; }; // = 0;
  virtual const double MaxValue() { return 0; }; // = 0;

  virtual int SelectMode() { return 0; };     // = 0;
  virtual void OnSelect(CGraphNode &node) {}; // = 0;

  virtual void Clear() {};               // = 0;
  virtual int Dimension() { return 0; }; // = 0
  virtual void ResetPan() {};            // = 0;
  virtual void ResetScale() {};          // = 0;

  virtual void OnZoomWindow() {};                               // = 0;
  virtual bool CanZoomWindow() const { return false; };         // = 0
  virtual void SwitchToTopView() {};                            // = 0;
  virtual void DisplayPerspective(bool bPerspective) {};        // = 0;
  virtual bool DisplayPerspective() const { return false; };    // = 0
  virtual bool CanDisplayPerspective() const { return false; }; // = 0

  virtual bool ShowColor() const { return false; }; // = 0

  virtual void ShowColor(bool bValue) {};                          // = 0;
  virtual bool ShowValue() const { return false; };                // = 0
  virtual bool CanShowValue() const { return false; };             // = 0
  virtual void ShowBoundary(bool bShow) {};                        // = 0;
  virtual bool ShowBoundary() const { return false; };             // = 0
  virtual bool CanShowBoundary() const { return false; };          // = 0
  virtual bool ShowOptionsDialog() { return false; };              // = 0
  virtual void AllowDynamicRotate(bool bAllow) {};                 // = 0;
  virtual bool AllowDynamicRotate() const { return false; };       // = 0
  virtual bool CanAllowDynamicRotate() const { return false; };    // = 0
  virtual bool CanGoHome() const { return false; };                // = 0
  virtual bool CanSaveHome() const { return false; };              // = 0
  virtual void DisplayGnomon(bool bDisplay) {};                    // = 0;
  virtual bool DisplayGnomon() const { return false; };            // = 0
  virtual bool CanDisplayGnomon() const { return false; };         // = 0
  virtual void DisplayCoordinateAxis(bool bDisplay) {};            // = 0
  virtual bool DisplayCoordinateAxis() const { return false; };    // = 0
  virtual bool CanDisplayCoordinateAxis() const { return false; }; // = 0
  virtual void SelectMeshMode(MeshMode mode) {};                   // = 0;
  //
  virtual MeshMode MeshModeSelected() const { return FullMesh; }  // = 0;
  virtual bool CanSelectMeshMode() const { return false; };       // = 0
  virtual void SelectDraggerTracker(bool yesOrNo) {};             // = 0;
  virtual bool DraggerTrackerSelected() const { return false; };  // = 0
  virtual bool CanSelectDraggerTracker() const { return false; }; // = 0
  virtual void SelectMouseTracker(bool yesOrNo) {};               // = 0;
  virtual bool MouseTrackerSelected() const { return false; };    // = 0
  virtual bool CanSelectMouseTracker() const { return false; };   // = 0

  virtual void SetDraggerConfined(bool yesOrNo) {};             // = 0;
  virtual bool DraggerConfined() const { return false; };       // = 0
  virtual bool CanSetDraggerConfined() const { return false; }; // = 0

  virtual void HideLegend(bool yesOrNo) {};                   // = 0;
  virtual bool LegendHidden() const { return false; };        // = 0
  virtual bool CanSelectHideLegend() const { return false; }; // = 0

  virtual void SelectHandMode(bool yesOrNo) {};                                // = 0;
  virtual bool HandMode() const { return false; };                             // = 0
  virtual void SelectClipPlaneMode(bool yesOrNo) {};                           // = 0;
  virtual bool CanSelectClipPlaneMode() const { return false; };               // = 0
  virtual bool ClipPlaneModeSelected() const { return false; };                // = 0
  virtual void SelectContourLineMode(bool yesOrNo) {};                         // = 0;
  virtual bool CanSelectContourLineMode() const { return false; };             // = 0
  virtual bool ContourLineModeSelected() const { return false; };              // = 0
  virtual bool MeshLinesDisplayed() const { return false; };                   // = 0
  virtual void SelectMeshLinesDisplayed(bool yesOrNo) {};                      // = 0;
  virtual bool CanSelectMeshLineDisplayMode() const { return false; };         // = 0
  virtual bool FormationOutlinesDisplayed() const { return false; };           // = 0
  virtual void SelectFormationOutlinesDisplayed(bool yesOrNo) {};              // = 0;
  virtual bool CanSelectFormationOutlineDisplayMode() const { return false; }; // = 0

  virtual bool CanSelectHandMode() const { return false; };       // = 0
  virtual void ViewBeachBalls(bool bFill) {};                     // = 0;
  virtual bool BeachBallsOnView() const { return false; };        // = 0
  virtual bool CanViewBeachBalls() const { return false; };       // = 0
  virtual void Fill(bool bFill) {};                               // = 0;
  virtual bool Fill() const { return false; };                    // = 0
  virtual bool CanFill() const { return false; };                 // = 0
  virtual void TimerCalled() {};                                  // = 0;
  virtual void ZoomIn() {};                                       // = 0;
  virtual void ZoomOut() {};                                      // = 0;
  virtual void ZoomReset() {};                                    // = 0;
  virtual void ShowValuesPerElement(bool perElement) {};          // = 0;
  virtual bool ShowValuesPerElement() const { return false; };    // = 0
  virtual bool CanShowValuesPerElement() const { return false; }; // = 0

  //
  virtual const geo::CVector &DisplacementVector() const {
    static geo::CVector v;
    return v;
  }; // const = 0;
  virtual void DisplacementVector(const geo::CVector &newval) {}; // = 0;
  virtual const geo::CVector &ModelScaleVector() const {
    static geo::CVector v;
    return v;
  }; // const = 0;
  virtual void ModelScaleVector(const geo::CVector &newval) {}; // = 0;
  virtual const geo::CVector &ScaleVector() const {
    static geo::CVector v;
    return v;
  }; // const = 0;
  virtual void ScaleVector(const geo::CVector &newVal) {}; // = 0;
  virtual const geo::CVector &RotateLockVector() const {
    static geo::CVector v;
    return v;
  }; // const = 0;
  virtual void RotateLockVector(const geo::CVector &vec) {};      // = 0;
  virtual void LockRotationVector(const geo::IVector &vector) {}; // = 0;
  virtual const geo::IVector &LockRotationVector() const {
    static geo::CVector v;
    return v;
  }; // const = 0;
  virtual bool CanLockRotationVector() const { return false; }; // = 0

  virtual void ViewEDirection() {};                         // = 0;
  virtual void ViewNDirection() {};                         // = 0;
  virtual void ViewZDirection() {};                         // = 0;
  virtual void GoHome() {};                                 // = 0;
  virtual void SaveHome() {};                               // = 0;
  virtual void ViewBirdEyeDirection() {};                   // = 0;
  virtual void ViewProportional(bool bProportional) {};     // = 0;
  virtual bool ViewProportional() const { return false; };  // = 0
  virtual void Lighting(bool bLighting) {};                 // = 0;
  virtual bool Lighting() const { return false; };          // = 0
  virtual bool CanEnableLighting() const { return false; }; // = 0
  virtual void EnableLighting(bool lighting) {};            // = 0;
  virtual bool LightingEnabled() const { return false; };   // = 0
  virtual void OnSize(int cx, int cy) {};                   // = 0;

  virtual void SetVectorScale(float scale) {};            // = 0;
  virtual void SetVectorVisibleFraction(float factor) {}; // = 0;

  virtual void ViewTensorVector(TensorVectorMode mode) {}; // = 0;
  //
  virtual TensorVectorMode ViewTensorVector() { return TensorVectorFull; }; // = 0;
  virtual bool CanViewTensorVector() { return false; };                     // = 0

  virtual void RefreshVectorBaseScale() {};                   // = 0;
  virtual bool CanRefreshVectorBaseScale() { return false; }; // = 0
};