#ifndef _OPENINVENTORSCENENODE_H_
#define _OPENINVENTORSCENENODE_H_

class COpenInventorEventsHandler;

#include "3DFormation.h"
#include "FormationNodeAssistant.h"
#include "MainMeshNodeAssistant.h"
#include "MeshMode.h"
#include "OIVSceneBase.h"
#include "ValueCompositeObserver.h"

#include "Deformation.h"

// FIXME
class OIDIMeshNodeManager;
#include "OpenInventorEventsHandler.h"

class COpenInventorSceneNode : public COIVSceneBase {
  Casing3DFormation *m_steelFormation;

public:
  COpenInventorSceneNode(CFemAppModel &model);
  COpenInventorSceneNode(const QString &strName, CFemAppModel &model);
  COpenInventorSceneNode(unsigned int uName, CFemAppModel &model);

  ~COpenInventorSceneNode();

  virtual void Activate();

  // viewport size
  int Width() const;
  int Height() const;

  void SaveColorToStream(QRgb (COpenInventorSceneNode::*func_ptr)() const, TSTREAM &stream);
  void RestoreColorFromStream(void (COpenInventorSceneNode::*func_ptr)(QRgb), TSTREAM &stream);

  template <class T> void SaveValueToStream(T (COpenInventorSceneNode::*func_ptr)() const, TSTREAM &stream) {
    stream << (this->*func_ptr)();
  }

  // specialization to account for that we cannot stream to a bool
  template <> void SaveValueToStream(bool (COpenInventorSceneNode::*func_ptr)() const, TSTREAM &stream) {
    stream << int((this->*func_ptr)());
  }

  template <class T> void RestoreValueFromStream(void (COpenInventorSceneNode::*func_ptr)(T), TSTREAM &stream) {
    T value;
    stream >> value;
    (this->*func_ptr)(value);
  }

  template <> void RestoreValueFromStream(void (COpenInventorSceneNode::*func_ptr)(MeshMode), TSTREAM &stream) {
    int value;
    stream >> value;
    (this->*func_ptr)((MeshMode)value);
  }

  // specialization to account for that we cannot stream to a bool
  template <> void RestoreValueFromStream(void (COpenInventorSceneNode::*func_ptr)(bool), TSTREAM &stream) {
    int value;
    stream >> value;
    (this->*func_ptr)(value);
  }

  // load/save
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  void SaveConnectedNodes(TSTREAM &stream, TPROGRESS &progress);

  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);

  void RestoreConnectedNodes(TSTREAM &stream, TPROGRESS &progress);

  // graphnode connects

  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &node);
  virtual bool CanConnectToMultipleItems(const CGraphNode &node) const;
  bool NodeCanBeDisplayed(const COpenGLNode *oglNode) const;

  enum DisplayMode { NONEXCLUSIVE = 0, EXCLUSIVE = 1, EXCLUSIVEBYTYPE = 2 };

  DisplayMode ToBeDisplayedByItself(COpenGLNode *pOpenGLNode);
  bool IsVisible(COpenGLNode *pOpenGLNode);

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  void SetNonDrawing();
  void SetDrawing();
  void SetRedrawing(bool redrawing);

  // input
  typedef QPoint TScreenPoint;
  typedef Qt::MouseButton TMouseButton;
  typedef Qt::KeyboardModifiers TKeyboardModifiers;
  typedef Qt::Key TKey;
  bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint &point);
  bool KeyPress(TKeyboardModifiers state, int nChar, TKey key, int nRepCount);
  bool KeyRelease(TKeyboardModifiers state, int nChar, TKey key, int nRepCount);

  void EnterView();
  void LeaveView();

  // events from the view
  void OnZoomWindow();
  bool CanZoomWindow() const;
  void SwitchToTopView();
  void DisplayPerspective(bool bPerspective);
  void OnDeformation(Deformation::Data::Input deformation);
  Deformation::Data::Input Deformation();
  bool DisplayPerspective() const;
  bool CanDisplayPerspective() const;
  void ShowBoundary(bool bShow);
  bool ShowBoundary() const;
  bool CanShowBoundary() const;
  void TrackElement(bool bShow);
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
  bool MouseTrackerSelected() const;
  bool CanSelectMouseTracker() const;
  void SelectMouseTracker(bool yesOrNo);
  void SetDraggerConfined(bool yesOrNo);
  bool DraggerConfined() const;
  bool CanSetDraggerConfined() const;
  void HideLegend(bool yesOrNo);
  bool LegendHidden() const;
  void ShowLegend(bool yesOrNo);
  bool LegendShown() const;
  void UpdateLegend();
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
  void SelectMeshLinesDisplayed(bool yesOrNo);
  bool CanSelectMeshLineDisplayMode() const;
  bool MeshLinesDisplayed() const;
  void SelectFormationOutlinesDisplayed(bool yesOrNo);
  bool CanSelectFormationOutlineDisplayMode() const;
  bool FormationOutlinesDisplayed() const;
  void Fill(bool bFill);
  bool Fill() const;
  bool CanFill() const;
  void ZoomIn();
  void ZoomOut();
  void ZoomReset();
  void LockRotationVector(const geo::IVector &vector);
  const geo::IVector &LockRotationVector() const;
  bool CanLockRotationVector() const;
  void ViewEDirection();
  void ViewTensorVector(TensorVectorMode mode);
  TensorVectorMode ViewTensorVector();

  bool CanRefreshVectorBaseScale();
  void RefreshVectorBaseScale();

  void ViewNDirection();
  void ViewZDirection();
  virtual void GoHome();
  virtual void SaveHome();
  void ViewBirdEyeDirection();
  void ViewProportional(bool bProportional);
  bool ViewProportional() const;
  void Lighting(bool bLighting);
  bool Lighting() const;
  bool CanEnableLighting() const;
  void OnSize(int cx, int cy);
  void ShowValuesPerElement(bool perElement);
  bool ShowValuesPerElement() const;
  bool CanShowValuesPerElement();
  void ViewBeachBalls(bool bView);
  bool BeachBallsOnView() const;
  bool CanViewBeachBalls() const;
  void SetCursor(Qt::CursorShape shape);
  void UpdateAllViews();
  void UpdateModelView();
  void SetVectorScale(float scale);
  void SetVectorVisibleFraction(float factor);

  QRgb AxisColor() const;

  void AxisColor(QRgb color);

  void GetRGBColor(float *rgb, QRgb color) const;

  virtual QRgb BackgroundColor() const;
  virtual void BackgroundColor(QRgb color);

  virtual QRgb BackgroundColor2() const;
  virtual void BackgroundColor2(QRgb color);

  virtual QRgb TextColor() const;
  virtual void TextColor(QRgb color);

  QRgb GetQColor(float *rgb) const;

  virtual IOpenGLFrame::DIMENSION Dimension() const;

  void Dimension(IOpenGLFrame::DIMENSION dim);

  const double &PointSize() const;
  void PointSize(double dNewSize);

  int IsoCount() const;
  void IsoCount(int count);

  double IsoGap() const;
  void IsoGap(double gap);

  int Transparency() const;
  void Transparency(int transparency);

  int DraggerTimeOut() const;
  void DraggerTimeOut(int timeOut);

  BOOL ViewComponent(TValueComponentObserver &observer);
  BOOL ViewComposite(CValueCompositeObserver &observer);
  COpenGLNode *FindProjectionNode(CTreeNode &observer);

  void SetMainMeshNode();
  void ConnectFormationNodeAssistant();
  void DisconnectFormationNodeAssistant(enum ModifiedHint uHint = Default);
  void Render();

  // clear the entire scene (unlink from opengl nodes)
  void Clear();

  // show color or value
  bool ShowColor() const;
  void ShowColor(bool bShowColor);
  bool ShowValue() const;
  void ShowValue(bool bShowValue);
  bool CanShowValue();

  const IValueComponentBase *ValueComponent() const;
  IValueComponentBase *ValueComponent();

private:
  const IValueComposite *ValueComposite() const;
  IValueComposite *ValueComposite();

public:
  const CColorScaleEntry *GetColorScaleEntry() const;

  const IColorScaleNode *ColorScale() const;

private:
  const CUnitNode *UnitNode() const;

public:
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  const double MinValue();
  const double MaxValue();

  void unlinkValueComponent();
  void RemoveAllMeshes();
  void ResetRenderAction();

  void ClearAll(CGraphNode *node = 0);
  void DisconnectUnconnectables();
  void ClearNodesToBeDisplayedByItself();
  void ClearInvisibleNodes();

  void ForceValueRedraw();

  void CreateHexaMeshRegion(bool rectangular);
  void CreatePolygonHexaMeshRegion(void *user_data, const std::vector<geo::CPoint> &points);

  void EditHexaSubMeshRegion(CHexaSubMeshRegion &hexaSubMeshRegion);
  void EditHexaMainMeshRegion(CHexaMainMeshRegion &hexaMainMeshRegion);

  void EditVerticalNewWellPath(CNewWellPath *pWellPath);
  void EditVerticalNewWellPath(void *user_data, const std::vector<geo::CPoint> &points);

  void ModifyPolygonHexaSubMeshRegion(void *user_data, const std::vector<geo::CPoint> &points);
  void ModifyRectangleHexaMainMeshRegion(void *user_data, const std::vector<geo::CPoint> &points);

  void TimerCalled();
  int NumberOfOpenGLNodesLinked() const;
  bool containsSupportNodes() const;

  void ConnectScheduledItems();

  typedef std::set<const COpenGLNode *> TOpenGLNodeSet;
  const TOpenGLNodeSet &NodesInScene() const;
  typedef std::set<const CGraphNode *> TLinkedGraphNodeSet;
  const TLinkedGraphNodeSet &GraphNodesInScene() const;

  // COpenInventorEventsHandler* OIV_evt_handler(){ return m_pHandler; } //FIXME
  OIDIMeshNodeManager *OIV_node_manager() { return m_pHandler->m_meshNodeManager; } // FIXME

  void show_refinement_box_dlg() { m_pHandler->show_refinement_box_dlg(); };
  MeshRefinementDlg *mesh_refinement_box_dlg() { return m_pHandler->m_refinement_box_dlg; };
  bool can_mesh_refinement();

private:
  // initialize scene
  void init(CFemAppModel &model);

  void ShowGeologyLegend();
  void OnIsoValueComponentChanged(IValueComponentBase *pComponent, CDoubleQuantity::UNIT unit);

  void RemoveFormationNodesFromGeologyLegend();
  void AddFormationNodesToGeologyLegend();
  void ShowHexaFormationTopMesh();
  void HideHexaFormationTopMesh();
  void ShowTetraFormationTopMesh();
  void HideTetraFormationTopMesh();
  void ShowHexaMeshRegions();
  void AllowExclusiveDisplay(bool allow);
  bool AllowExclusiveDisplay();
  void dehighlightMesh();
  void clearValueTrackerCellFacetSet();
  void ScheduleConnectItem(COpenGLNodeBase &pNode);

private:
  // items scheduled to be connected
  // workaround(?) for a bug to connect items only after an initial render ...
  std::vector<COpenGLNodeBase *> m_scheduledItems;

  IValueComponentBase *m_pValue;
  IValueComponentBase *m_pValueInterrupted;
  IValueComposite *m_pComposite;
  IColorScaleNode *m_pColorScale;
  IColorScaleEntry *m_pColorScaleEntry;
  CUnitNode *m_pUnit;
  CUnitNode m_unitCurrent;

  IValueComponentBase *m_pValueBlocked;
  IValueComponentBase *m_pIsoValueBlocked;
  bool m_forceSettings;

  int m_sizeX;
  int m_sizeY;
  COpenInventorEventsHandler *m_pHandler;
  CMainMeshNodeAssistant m_meshNodeAssistant;
  CFormationNodeAssistant m_formationNodeAssistant;

  TOpenGLNodeSet m_stOpenGLNodes;

  // set of non-OpenGLNodes
  TLinkedGraphNodeSet m_stGraphNodes;

  bool m_allowExclusiveDisplay;
  bool m_adjustView;
  bool m_redrawing;

  bool m_keyPressed[256];
};

#endif // _OPENINVENTORSCENENODE_H_
