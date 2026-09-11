#ifndef OIV2DSCENE_H_INCLUDED
#define OIV2DSCENE_H_INCLUDED

#include "OIVSceneBase.h"

class SoNode;
class SoGroup;
class SoGLContext;
class SoGuiAlgoViewers;
class SoOrthographicCamera;
class SoGradientBackground;
class QPoint;

class OIV2DGraph;
class OIV2DLegend;

/**
 * Base class for all OIV 2D scenes
 */
class OIV2DScene : public COIVSceneBase {
  static Qt::CursorShape s_cursorShape;

  SoGLContext *m_oglContext;
  SoGuiAlgoViewers *m_viewerAlgos;
  SoGroup *m_sceneGraph;
  SoOrthographicCamera *m_camera;

  SoGradientBackground *m_background;
  float m_foregroundColor[3];
  int m_lineThickness;
  bool m_markersVisible;

  int m_width;
  int m_height;

  int m_storedLegendFrameWidth;

  void init();

  enum ViewerMode { ViewingMode, PanningMode, ZoomingMode };

  ViewerMode m_viewerMode;

protected:
  OIV2DGraph *m_graph;
  OIV2DLegend *m_legend;

  int m_preferredCurveGroup;

  static void SetCursor(Qt::CursorShape shape);
  static void RefreshCursor();

public:
  OIV2DScene(CFemAppModel &model);
  OIV2DScene(const QString &name, CFemAppModel &model);
  OIV2DScene(unsigned int name, CFemAppModel &model);

  virtual void Activate();
  virtual void Deactivate();

  virtual void setSize(int width, int height);
  int getWidth() const;
  int getHeight() const;

  virtual size_t NumberOfValueComponents() const = 0;
  virtual IValueComponentBase *ValueComponent(size_t index) const = 0;
  virtual bool MultipleValueComponentsAllowed() const = 0;
  virtual bool MultipleStagesAllowed() const = 0;

  virtual bool CanConnectToMultipleItems(const CGraphNode &node) const = 0;

  void setSceneGraph(SoGroup *root, SoOrthographicCamera *camera);
  SoGroup *getSceneGraph() const;
  void createSceneGraph();

  void updateDomains();

  void startIVTune();
  void stopIVTune();

  void zoomIn();
  void zoomOut();

  void viewAll();
  void render();

  // input
  typedef QPoint TScreenPoint;
  typedef Qt::MouseButton TMouseButton;
  typedef Qt::KeyboardModifiers TKeyboardModifiers;
  typedef Qt::Key TKey;

  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint &point);
  virtual bool KeyPress(TKeyboardModifiers state, int nChar, TKey key, int nRepCount);
  virtual bool KeyRelease(TKeyboardModifiers state, int nChar, TKey key, int nRepCount);
  void UpdateAllViews();

  bool UpdateLabel(const TScreenPoint &point);

  QRgb ForegroundColor() const;
  QRgb BackgroundColor() const;
  QRgb BackgroundColor2() const;

  void ForegroundColor(QRgb color);
  void BackgroundColor(QRgb color);
  void BackgroundColor2(QRgb color);

  void setLineThickness(int t);
  int getLineThickness() const;

  void preferBottomOrRightAxis();
  bool canPreferBottomOrRightAxis() const;
  bool bottomOrRightAxisPreferred() const;

  void preferTopOrLeftAxis();
  bool canPreferTopOrLeftAxis() const;
  bool topOrLeftAxisPreferred() const;

  void toggleMarkers();
  bool getMarkersVisible() const;
  void setMarkersVisible(bool value);

  void SetGraphRanges();

  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);

  virtual int SelectMode() const = 0;
  virtual void updateLegend() = 0;

  float getAspect();
  virtual void updateGraph();

  void SaveColorToStream(QRgb (OIV2DScene::*func_ptr)() const, TSTREAM &stream);
  void RestoreColorFromStream(void (OIV2DScene::*func_ptr)(QRgb), TSTREAM &stream);

  template <class T> void SaveValueToStream(T (OIV2DScene::*func_ptr)() const, TSTREAM &stream) {
    stream << (this->*func_ptr)();
  }

  // specialization to account for that we cannot stream to a bool
  template <> void SaveValueToStream(bool (OIV2DScene::*func_ptr)() const, TSTREAM &stream) {
    stream << int((this->*func_ptr)());
  }

  template <class T> void RestoreValueFromStream(void (OIV2DScene::*func_ptr)(T), TSTREAM &stream) {
    T value;
    stream >> value;
    (this->*func_ptr)(value);
  }

  // specialization to account for that we cannot stream to a bool
  template <> void RestoreValueFromStream(void (OIV2DScene::*func_ptr)(bool), TSTREAM &stream) {
    int value;
    stream >> value;
    (this->*func_ptr)(value);
  }
};

#endif
