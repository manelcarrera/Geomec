#ifndef _OPENINVENTOREVENTSHANDLER_H_
#define _OPENINVENTOREVENTSHANDLER_H_

#include <QPoint>

class SbVec2s;

class SoSceneManager;
class SoSwitch;
class SoGuiAlgoViewers; // Open Inventor class that handles basic viewing algorithms
class SoPerspectiveCamera;
class SoCamera;
class SoScale;
class SoTranslation;
class SoGradientBackground;
class SoGLContext;
class CMeshRefinementBoxesHandler;

#include "Inventor/events/SoButtonEvent.h"

class COpenInventorSceneNode;
class CWorldAction;
class CCrossSection;
class CNewWellPath;
class CFormationBase;
class CColorScaleEntry;
class OIDIMeshNodeManager;
class CoordinateAxis;
class CLegendFrame;
class IResultComponent;

class MeshRefinementDlg;

#include "GeomecPolygonScreenDrawer.h"
#include "MeshMode.h"

#include "Deformation.h"

class COpenInventorEventsHandler {
public:
  static Qt::CursorShape s_cursorShape;
  static void SetCursor(Qt::CursorShape shape);
  static void RefreshCursor();

  typedef IValueDomainScalar::TMinMax TMinMax;

  COpenInventorEventsHandler(COpenInventorSceneNode &sceneNode);
  ~COpenInventorEventsHandler();

  // input events
  typedef QPoint TScreenPoint;
  typedef Qt::MouseButton TMouseButton;
  typedef Qt::KeyboardModifiers TKeyboardModifiers;
  typedef Qt::Key TKey;
  bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);

  bool HandleSceneMousePressEvent(SbVec2s &locator);

  bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);

  bool HandleSceneMouseReleaseEvent(SbVec2s &locator);
  bool HandleSceneMouseDblClkEvent(SbVec2s &locator);

  void ActivatePolygonScreenDrawer(CWorldAction *action, GeomecPolygonScreenDrawer::PolygonMode mode,
                                   std::vector<geo::CPoint> *points = 0);

  void PerformWorldActionOnPolygon(SoPolyLineScreenDrawer *drawer);

  SbVec2f ScreenDrawerToWorldPoint(const SbVec2f &pt);
  SbVec2f WorldToScreenDrawerPoint(const SbVec2f &worldPoint);

  bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);

  void EnterView();
  void LeaveView();

  int LockedSpinDirection();

  void TimerCalled();
  void ResetTimer(const SbVec2s &locator);

  bool HandleSceneMouseMoveEvent(SbVec2s &locator, bool button1Down);

  bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint &point);
  bool KeyPress(TKeyboardModifiers state, int nChar, TKey key, int nRepCount);
  bool KeyRelease(TKeyboardModifiers state, int nChar, TKey key, int nRepCount);

  bool KeyAction(int nChar, SoButtonEvent::State state);

  // Cross section events
  void OnShowCrossSection(const CCrossSection &xsec);
  void OnHideCrossSection(const CCrossSection &xsec);
  void OnCrossSectionChanged(const CCrossSection &xsec);

  // Well path events
  void OnShowWellPath(const CNewWellPath &wellPath);
  void OnHideWellPath(const CNewWellPath &wellPath);
  void OnWellPathChanged(const CNewWellPath &wellPath);

  // viewer events
  void OnNewFormationNode(const CFormationBase &node);
  void OnFormationNodeModified(const CFormationBase &node, ModifiedHint hint);
  void OnFormationNodeDeleted(const CFormationBase &node);
  void OnNewOpenGLNode(const COpenGLNode &node);
  void OnOpenGLNodeModified(const COpenGLNode &node, enum ModifiedHint uHint = Default);
  void OnOpenGLNodeDeleted(const COpenGLNode &node);
  void OnColorScaleChanged();
  void OnValueComponentChanged(const IValueComponentBase *pValueComponent, CDoubleQuantity::UNIT unit,
                               bool settingsChanged = false);

  void OnUnitChanged();
  void OnViewModeChanged();
  void OnViewportSizeChanged(int cx, int cy);

  void UpdateLegend(bool bForceColor = false, bool bUpdatedColorScale = false);
  void UpdateValuesLegend(const IValueComponentBase *valueComponent);

  void OnZoomRectangle();
  bool CanZoomWindow() const;
  void SwitchToTopView();
  void SetCamera(bool bPerspective);
  void CheckCamera();
  void DisplayPerspective(bool bPerspective);
  void OnDeformation(Deformation::Data::Input deformation);
  Deformation::Data::Input Deformation();
  bool DisplayPerspective() const;
  bool CanDisplayPerspective() const;
  void ShowBoundary(bool bShow);
  bool ShowBoundary() const;
  bool CanShowBoundary() const;
  void TrackElement(bool bTrack);
  bool TrackElement() const;
  bool CanTrackElement() const;
  void AllowDynamicRotate(bool bAllow);
  bool AllowDynamicRotate() const;
  bool CanAllowDynamicRotate() const;
  void DisplayGnomon(bool bDisplay);
  bool DisplayGnomon() const;
  bool CanDisplayGnomon() const;
  void DisplayCoordinateAxis(bool bDisplay);
  bool DisplayCoordinateAxis() const;
  bool CanDisplayCoordinateAxis() const;
  void SelectMeshMode(MeshMode mode);
  MeshMode MeshModeSelected() const;
  bool CanSelectMeshMode() const;
  bool DraggerTrackerSelected() const;
  bool CanSelectDraggerTracker() const;
  void SelectDraggerTracker(bool yesOrNo);
  void ActUponNoValuesDisplayedWhenAdded();
  void UpdateValueTrackerDisplay();
  void AdjustView();

  bool MouseTrackerSelected() const;
  bool CanSelectMouseTracker() const;
  void SelectMouseTracker(bool yesOrNo);

  void SetDraggerConfined(bool yesOrNo);
  bool DraggerConfined() const;
  bool CanSetDraggerConfined() const;

  void HideLegend(bool yesOrNo);
  bool LegendHidden() const;
  bool CanSelectHideLegend() const;

  bool HandMode() const;
  bool CanSelectHandMode() const;
  void SelectHandMode(bool yesOrNo);

  bool ClipPlaneModeSelected() const;
  bool CanSelectClipPlaneMode() const;
  void SelectClipPlaneMode(bool yesOrNo);

  bool ContourLineModeSelected() const;
  bool CanSelectContourLineMode() const;
  void SelectContourLineMode(bool yesOrNo);

  bool MeshLinesDisplayed() const;
  bool CanSelectMeshLineDisplayMode() const;
  void SelectMeshLinesDisplayed(bool yesOrNo);

  bool FormationOutlinesDisplayed() const;
  bool CanSelectFormationOutlineDisplayMode() const;
  void SelectFormationOutlinesDisplayed(bool yesOrNo);

  void Fill(bool bFill);
  bool Fill() const;
  bool CanFill() const;
  void ZoomIn();
  void ZoomOut();
  void ZoomReset();
  void ZoomRectangle(SoPolyLineScreenDrawer *drawer);

  void ShowInFull();
  bool ShowTopView() const;
  void ShowTopView(bool show);

  bool PointSetSelected() const;
  void LockRotationVector(const geo::IVector &vector);
  const geo::IVector &LockRotationVector() const;
  bool CanLockRotationVector() const;
  void ViewTensorVector(TensorVectorMode mode);
  TensorVectorMode ViewTensorVector();

  bool CanRefreshVectorBaseScale();
  void RefreshVectorBaseScale();

  void ViewEDirection();
  void ViewNDirection();
  void ViewZDirection();
  virtual void GoHome();
  virtual void SaveHome();
  void ViewBirdEyeDirection();
  void ViewProportional(bool bProportional);
  bool ViewProportional() const;
  void Lighting(bool bLighting);
  bool Lighting() const;
  void CheckLighting() const;
  bool CanEnableLighting() const;
  void ShowValuesPerElement(bool perElement);
  bool ShowValuesPerElement() const;
  bool CanShowValuesPerElement();
  void ViewBeachBalls(bool bView);
  bool BeachBallsOnView() const;
  bool CanViewBeachBalls() const;
  void SetVectorScale(float scale);
  void SetVectorVisibleFraction(float factor);

  void Render();
  void UpdateTranslation();

  // viewport size
  int Width() const;
  int Height() const;

  // value/coloring information
  const IColorScaleNode *ColorScale() const;

  // display color or property values
  bool ShowColor() const;
  bool ShowValue() const;
  void ShowColor(bool show);
  bool CanShowValue();

  bool CanDisplay(const COpenGLNode *pOpenGLNode);

  void SetCoordinateAxisColor(float *rgb);
  void GetCoordinateAxisColor(float *rgb) const;

  void SetBackgroundColor(float *rgb);
  void GetBackgroundColor(float *rgb) const;

  void SetBackgroundColor2(float *rgb);
  void GetBackgroundColor2(float *rgb) const;

  void ResetSceneEnclosureNodes();

  void removeDataSets();
  void unlinkValueComponent();
  void RemoveAllMeshes();

  void OnIsoValueComponentChanged(IValueComponentBase *pComponent, CDoubleQuantity::UNIT unit);

  const double MinValue();
  const double MaxValue();

  const CColorScaleEntry *GetColorScaleEntry() const;

  void ShowGeologyLegend();

  void SetTextColor(float *rgb);
  void GetTextColor(float *rgb) const;

  void PointSize(const double &size);
  const double &PointSize() const;

  int IsoCount() const;
  void IsoCount(int count);

  double IsoGap() const;
  void IsoGap(double gap);

  int Transparency() const;
  void Transparency(int transparency);

  int DraggerTimeOut() const;
  void DraggerTimeOut(int timeOut);

  void removeFromGeologyLegend(const COpenGLNode *pOpenGLNode);
  void addToGeologyLegend(const COpenGLNode *pOpenGLNode);
  void ModifyGeologyLegend(const COpenGLNode *oglNode);

  bool IsVisible(COpenGLNode *pOpenGLNode);

  void ResetRenderAction();
  void RestoreValueComponent(IValueComponentBase *pValueComponent);
  int NumberOfOpenGLNodesLinked();

  void dehighlightMesh();
  void clearValueTrackerCellFacetSet();

  void show_refinement_box_dlg();
  void on_zoom_to_box_cb(const QString &box_name);

private:
  void OnUpdateLegend(bool bForceColor);
  void OnUpdateValueLegend();
  typedef std::set<const COpenGLNode *> TOpenGLNodeSet;
  bool OnInsertNodeInLegend(const COpenGLNode &node) const;
  TOpenGLNodeSet NodesInSceneForLegend() const;
  void AddResultPath(const IResultComponent &result_component);
  TOpenGLNodeSet NodesInScene() const;
  int InsertLinkedNodesInLegend();
  TMinMax MinMaxValue();

  void ScalePerspectiveCameraAngle(float scale);
  static void renderCallback(void *userData, SoSceneManager *mgr);

  void initOIV();

  void InitCoordinateAxis();

  void InitGnomon();

  void InitMeshNodeManager();

  void InitScreenDrawing();
  void InitBoundingBox();
  void ResetSceneBoundingBox(SbBox3f &bbox);

  void ResetCoordAxis(const SbBox3f &bbox);

  template <class ScreenDrawerClass> ScreenDrawerClass *getNewScreenDrawer();

  void InitCamera();
  void setViewport(int width, int height);
  void SetCameraOrientation(float x, float y, float z, float w);
  void RetraceMeshInfo();
  void TraceMeshInfo(SbVec2s &point);

  void GetScreenToWorldConversion(SbVec3f &screenMin, SbVec3f &worldMin, SbVec3f &scaleScreenToWorld);

  SbVec2f GetWorldCoordinates(const SbVec2f &normalizedScreen);
  SbVec2f GetScreenNormalizedCoordinates(const SbVec2f &world);
  void HandleContextMenu(SbVec2s &point, bool bSuppressMenu = false);
  SoSwitch *const &gnomonSwitch() const;

public: // FIXME:
  COpenInventorSceneNode &m_sceneNode;

private:
  CLegendFrame &LegendFrame();

  int m_draggerTimeOut;
  int m_secondsToTimeOut;

  bool m_firstRender;
  bool m_updateLegend;

  bool m_mouseMoved;
  bool m_oivInitialized;
  bool m_cameraIsMoving;
  bool m_dynamicRotate;
  bool m_phongLighting;
  bool m_displayPerspective;
  // bool m_deformation;
  int m_lockedSpinDirection;
  double m_pointSize;

  SbVec2s m_meshInfoScreenPoint;

  CWorldAction *m_worldAction;

  SoGuiAlgoViewers *m_viewerAlgos;

  SoSwitch *m_cameraSwitch;
  SoPerspectiveCamera *m_perspectiveCamera;
  SoOrthographicCamera *m_orthoCamera;
  SoCamera *m_currentCamera;

  SoSeparator *m_rootSceneGraph;
  SoScale *m_scale;

public: // FIXME
  OIDIMeshNodeManager *m_meshNodeManager;
  MeshRefinementDlg *m_refinement_box_dlg;

private:
  SoSwitch *m_gnomonSwitch;

  SoSwitch *m_bboxSwitch;
  SoTranslation *m_bboxTranslation;
  SoScale *m_bboxScale;

  SoGradientBackground *m_background;

  CoordinateAxis *m_coordinateAxis;

  SoSeparator *m_screenDrawerGroup;
  SoPolyLineScreenDrawer *m_screenDrawer;

  SoGLContext *m_oglContext;

  CMeshRefinementBoxesHandler *m_boxes;
};

#endif // _OPENINVENTOREVENTSHANDLER_H_
