#ifndef _ISCENEWRAPPER_H_
#define _ISCENEWRAPPER_H_

#include "GraphNode.h"
#include "GeomecDoc.h"
#include "MeshMode.h"

namespace geo {
  class IObject;
  class CVector;
}

class IOpenGLFrame;
class CFemAppDoc;
class CFemAppModel;
class COpenGLSceneBase;
class IValueComponentBase;
class COpenInventorSceneNode;
class OIV2DScene;

#include "ValueCompositeObserver.h"


class ISceneWrapper
{

protected:

  class CDocumentLink : public CGraphNode
  {
    CGeomecDoc *m_pDoc;

  public:
    CDocumentLink() : CGraphNode(_T("")), m_pDoc(GetGeomecDoc()) {}

    virtual void OnNewNeighbour(const CGraphNode& node)
    {
      CGraphNode::OnNewNeighbour(node);
#if 0
      // At this point m_pDoc points to a deleted scene wrapper, here we are in the process of creating a new one!!
      ISceneWrapper * currentScene = m_pDoc->CurrentScene();
      int mode = (currentScene && currentScene->OpenInventorScene()) ? SWITCH_TO_OPENINVENTOR : NEW_CURRENT_SCENE;
      m_pDoc->UpdateAllViews(0, mode, (CObject*)(currentScene));
#endif

    }

    virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
    {
      CGraphNode::OnNeighbourModified(node, uHint);
      m_pDoc->UpdateAllViews(0, CURRENT_SCENE_MODIFIED, (CObject*)(m_pDoc->CurrentScene()));
    }

    virtual void OnNeighbourDeleted(const CGraphNode& node)
    {
      CGraphNode::OnNeighbourDeleted(node);
      m_pDoc->UpdateAllViews(0, NEW_CURRENT_SCENE, 0);
    }

    virtual unsigned int IconId() const { return 0; }
    virtual unsigned int TypeId() const { return 0; }

  };

  CDocumentLink m_DocLink;


public:
  ISceneWrapper() {}
  ISceneWrapper(COpenGLSceneBase& scene) {}

  virtual ~ISceneWrapper(){};

  virtual void Activate() {}
  virtual void Deactivate() {}

  //virtual COpenGLSceneBase* Scene() { return 0; }
  //virtual CGraphNode *Node() = 0;
  virtual COpenGLSceneBase* OpenGLScene() { return 0; }
  virtual COpenInventorSceneNode* OpenInventorScene() { return 0; }
  virtual OIV2DScene* getOIV2DScene() const { return 0; }

  virtual bool IsOpenInventorScene() const = 0;
  virtual bool IsOIV2DWellPathGraphScene() const { return false; }
  virtual bool IsOIV2DHistoryScene() const { return false; }

  virtual BOOL ViewComponent( TValueComponentObserver & observer ) { return FALSE ;}
  virtual BOOL ViewComposite( CValueCompositeObserver & observer ) { return FALSE; }

  virtual bool ReParent(IOpenGLFrame* pParent) = 0;
  virtual bool IsLinkedTo(const CGraphNode& node) = 0;
  virtual bool CanConnectItem(const CGraphNode& node) = 0;
  virtual bool CanConnectToMultipleItems(const CGraphNode& node)
  {
      return false;
  }

  virtual bool ConnectItem(const CGraphNode& node) = 0;
  virtual void LinkTo(const CGraphNode& node) = 0;
  virtual void UnLink(const CGraphNode& node) = 0;

  virtual void SetNonDrawing() {}
  virtual void SetDrawing() {}
  

  virtual CFemAppModel& Model() = 0;
  virtual IValueComponentBase* ValueComponent() const = 0;
  virtual bool MultipleStagesAllowed() const
  {
      return false;
  }
  virtual bool MultipleValueComponentsAllowed() const
  {
      return false;
  }
  virtual size_t NumberOfValueComponents() const
  {
      return ValueComponent() != nullptr ? 1 : 0;
  }

  virtual IValueComponentBase* ValueComponent(size_t index) const
  {
      return index == 0 ? ValueComponent() : 0;
  }

  virtual std::pair<geo::CValue, geo::CValue> MinMaxValue() = 0;
  virtual const double MinValue() = 0;
  virtual const double MaxValue() = 0;

  virtual int  SelectMode() = 0;
  virtual void OnSelect(CGraphNode& node) = 0;
  virtual void invalidate(const geo::IObject &list, bool bUpdate = true) {}


  virtual void Clear() = 0;
  virtual int  Dimension() = 0;
  virtual void ResetPan() = 0;
  virtual void ResetScale() = 0;

  virtual void OnZoomWindow() = 0;
  virtual bool CanZoomWindow() const = 0;
  virtual void SwitchToTopView() = 0;
  virtual void DisplayPerspective(bool bPerspective) = 0;
  virtual bool DisplayPerspective() const = 0;
  virtual bool CanDisplayPerspective() const = 0;

  virtual bool ShowColor() const = 0;

  virtual void ShowColor(bool bValue) = 0;
  virtual bool ShowValue() const = 0;
  virtual bool CanShowValue() const = 0;
  virtual void ShowBoundary(bool bShow) = 0;
  virtual bool ShowBoundary() const = 0;
  virtual bool CanShowBoundary() const = 0;
  virtual bool ShowOptionsDialog() = 0;
  virtual void AllowDynamicRotate(bool bAllow) = 0;
  virtual bool AllowDynamicRotate() const = 0;
  virtual bool CanAllowDynamicRotate() const = 0;
  virtual bool CanGoHome() const = 0;
  virtual bool CanSaveHome() const = 0;
  virtual void DisplayGnomon(bool bDisplay) = 0;
  virtual bool DisplayGnomon() const = 0;
  virtual bool CanDisplayGnomon() const = 0;
  virtual void DisplayCoordinateAxis(bool bDisplay) = 0;
  virtual bool DisplayCoordinateAxis() const = 0;
  virtual bool CanDisplayCoordinateAxis() const = 0;
  virtual void SelectMeshMode( MeshMode mode ) = 0;
  virtual MeshMode MeshModeSelected() const = 0;
  virtual bool CanSelectMeshMode() const = 0;
  virtual void SelectDraggerTracker(bool yesOrNo) = 0;
  virtual bool DraggerTrackerSelected() const = 0;
  virtual bool CanSelectDraggerTracker() const = 0;
  virtual void SelectMouseTracker(bool yesOrNo) = 0;
  virtual bool MouseTrackerSelected() const = 0;
  virtual bool CanSelectMouseTracker() const = 0;

  virtual void SetDraggerConfined(bool yesOrNo) = 0;
  virtual bool DraggerConfined() const = 0;
  virtual bool CanSetDraggerConfined() const = 0;

  virtual void HideLegend(bool yesOrNo) = 0;
  virtual bool LegendHidden() const = 0;
  virtual bool CanSelectHideLegend() const = 0;

  virtual void SelectHandMode(bool yesOrNo) = 0;
  virtual bool HandMode() const = 0;
  virtual void SelectClipPlaneMode(bool yesOrNo) = 0;
  virtual bool CanSelectClipPlaneMode() const = 0;
  virtual bool ClipPlaneModeSelected() const = 0;
  virtual void SelectContourLineMode(bool yesOrNo) = 0;
  virtual bool CanSelectContourLineMode() const = 0;
  virtual bool ContourLineModeSelected() const = 0;
  virtual bool MeshLinesDisplayed() const = 0;
  virtual void SelectMeshLinesDisplayed(bool yesOrNo) = 0;
  virtual bool CanSelectMeshLineDisplayMode() const = 0;
  virtual bool FormationOutlinesDisplayed() const = 0;
  virtual void SelectFormationOutlinesDisplayed(bool yesOrNo) = 0;
  virtual bool CanSelectFormationOutlineDisplayMode() const = 0;

  virtual bool CanSelectHandMode() const = 0;
  virtual void ViewBeachBalls(bool bFill) = 0;
  virtual bool BeachBallsOnView() const = 0;
  virtual bool CanViewBeachBalls() const = 0;
  virtual void Fill(bool bFill) = 0;
  virtual bool Fill() const = 0;
  virtual bool CanFill() const = 0;
  virtual void TimerCalled () = 0;
  virtual void ZoomIn() = 0;
  virtual void ZoomOut() = 0;
  virtual void ZoomReset() = 0;
  virtual void ShowValuesPerElement(bool perElement) = 0;
  virtual bool ShowValuesPerElement() const = 0;
  virtual bool CanShowValuesPerElement() const = 0;

  virtual const geo::CVector &DisplacementVector() const = 0;
  virtual void DisplacementVector(const geo::CVector &newval) = 0;
  virtual const geo::CVector &ModelScaleVector() const = 0;
  virtual void ModelScaleVector(const geo::CVector &newval) = 0;
  virtual const geo::CVector &ScaleVector() const = 0;
  virtual void ScaleVector(const geo::CVector &newVal) = 0;
  virtual const geo::CVector &RotateLockVector() const = 0;
  virtual void RotateLockVector(const geo::CVector& vec) = 0;
  virtual void LockRotationVector(const geo::IVector& vector) = 0;
  virtual const geo::IVector& LockRotationVector() const = 0;
  virtual bool CanLockRotationVector() const = 0;
  
  virtual void ViewEDirection() = 0;
  virtual void ViewNDirection() = 0;
  virtual void ViewZDirection() = 0;
  virtual void GoHome() = 0;
  virtual void SaveHome() = 0;
  virtual void ViewBirdEyeDirection() = 0;
  virtual void ViewProportional(bool bProportional) = 0;
  virtual bool ViewProportional() const = 0;
  virtual void Lighting(bool bLighting) = 0;
  virtual bool Lighting() const = 0;
  virtual bool CanEnableLighting() const = 0;
  virtual void EnableLighting(bool lighting) = 0;
  virtual bool LightingEnabled() const = 0;
  virtual void OnSize(int cx, int cy) = 0;

  virtual void SetVectorScale(float scale) = 0;
  virtual void SetVectorVisibleFraction(float factor) = 0;

  virtual void ViewTensorVector( TensorVectorMode mode ) = 0;
  virtual TensorVectorMode ViewTensorVector() = 0;
  virtual bool CanViewTensorVector() = 0;

  virtual void RefreshVectorBaseScale() = 0; 
  virtual bool CanRefreshVectorBaseScale() = 0;

};

#endif // _ISCENEWRAPPER_H_
