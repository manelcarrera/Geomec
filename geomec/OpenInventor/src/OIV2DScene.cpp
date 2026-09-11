#include "stdafx.h"

#include "OIV2DScene.h"

#include <Inventor/sys/SoGL.h>
#include <Inventor/devices/SoGLContext.h>
#include <Inventor/Gui/viewers/SoGuiAlgoViewers.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

//#define USE_IVTUNE
#ifdef USE_IVTUNE
#include <IvTune/SoIvTune.h>
#endif

#include <QtCore/qglobal.h>
#include <QtCore/qpoint.h>
#include "GeomecDoc.h"
#include "ModelView.h"
#include "LegendView.h"
#include "OpenGLSplitView.h"
#include "Inventor/nodes/SoGradientBackground.h"
#include "MeshViz/PoMeshViz.h"
#include "Inventor/nodes/SoSeparator.h"
#include "OIV2DGraph.h"
#include "OIV2DLegend.h"
#include "Inventor/actions/SoRayPickAction.h"
#include "Inventor/SoPickedPoint.h"
#include "Curve.h"
#include "CurveGroup.h"

#include "GraphLimitsDlg.h"


Qt::CursorShape OIV2DScene::s_cursorShape = Qt::ArrowCursor;

static QRgb GetQColor( const float * rgb )
{
  return qRgb ((int) (rgb[0] * 255), (int) (rgb[1] * 255), (int) (rgb[2] * 255) );
}

static void GetRGBColor( float * rgb, const QRgb color )
{
  rgb[0] = qRed(color) / 255.0f;
  rgb[1] = qGreen(color) / 255.0f;
  rgb[2] = qBlue(color) / 255.0f;
}

void OIV2DScene::init()
{
  m_camera = 0;
  m_sceneGraph = 0;
  m_oglContext = 0;
  m_preferredCurveGroup = 0;

  m_width = 0;
  m_height = 0;

  m_viewerAlgos = new SoGuiAlgoViewers;
  m_viewerAlgos->ref();
  m_viewerAlgos->setViewerType(SoGuiAlgoViewers::PLANE);

  m_viewerMode = ViewingMode;
  m_foregroundColor[0] = m_foregroundColor[1] = m_foregroundColor[2] = 1.0f;
}

OIV2DScene::OIV2DScene(CFemAppModel& model)
  : COIVSceneBase(model)
  , m_background(0)
  , m_graph(0)
  , m_legend(0)
  , m_lineThickness (1)
  , m_markersVisible(true)
  , m_storedLegendFrameWidth(0)
{
  init();
}

OIV2DScene::OIV2DScene(const QString& name, CFemAppModel& model)
  : COIVSceneBase(name, model)
  , m_background(0)
  , m_graph(0)
  , m_legend(0)
  , m_lineThickness (1)
  , m_storedLegendFrameWidth(0)
{
  init();
}

void OIV2DScene::Activate()
{
  LegendFrame().ClearLegendFrames();
  
  CGeomecDoc* pDoc = GetGeomecDoc();
  assert(pDoc);

  COpenGLSplitView *pView = pDoc->GetLegendModelView();
  assert(pView);

  int min;
  pView->GetColumnInfo(0, m_storedLegendFrameWidth, min);
  pView->SetColumnInfo(0, 1, 0); // Setting it to 0 leads to the legend frame no longer showing fonts after we restore the width in Deactivate (TBI)
}

void OIV2DScene::Deactivate()
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  assert(pDoc);

  COpenGLSplitView *pView = pDoc->GetLegendModelView();
  assert(pView);

  pView->SetColumnInfo(0, m_storedLegendFrameWidth, 0);
}

void OIV2DScene::SetCursor (Qt::CursorShape shape)
{
  s_cursorShape = shape;
  RefreshCursor();
}

float OIV2DScene::getAspect ()
{
  int width = getWidth() - 250;
  int height = getHeight();

  if (width <= 0 || height <= 0)
    return 1.0f;

  return std::max (0.2f, (std::min (5.0f, float (width) / height)));
}

void OIV2DScene::RefreshCursor()
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  if (!pDoc) return;
  CModelView *mv = pDoc->GetModelView();
  if (mv) mv->SetCursor(s_cursorShape);
}

OIV2DScene::OIV2DScene(unsigned int name, CFemAppModel& model)
  : COIVSceneBase(name, model)
{
  init();
}

void OIV2DScene::setSize(int width, int height)
{
  m_width = width;
  m_height = height;

  SbViewportRegion viewport(width, height);
  m_viewerAlgos->getSceneManager()->setViewportRegion(viewport);
  m_viewerAlgos->setGlxSize(SbVec2s(width, height));
}

int OIV2DScene::getWidth() const 
{ 
  return m_width; 
}

int OIV2DScene::getHeight() const 
{ 
  return m_height; 
}

void OIV2DScene::setSceneGraph(SoGroup* root, SoOrthographicCamera* camera)
{
  m_sceneGraph = root;
  m_camera = camera;

  m_viewerAlgos->setSceneGraph(root);
  viewAll();
}

SoGroup* OIV2DScene::getSceneGraph() const
{
  return m_sceneGraph;
}

void OIV2DScene::createSceneGraph()
{
  if(!PoMeshViz::isInitialized())
    PoMeshViz::init();

  m_background = new SoGradientBackground;
  m_background->color0 = SbColor(0.0f, 0.0f, 0.0f);
  m_background->color1 = SbColor(0.0f, 0.0f, 0.0f);

  SoOrthographicCamera* camera = new SoOrthographicCamera;

  SoSeparator* root = new SoSeparator;
  root->addChild(camera);
  root->addChild(m_background);

  setSceneGraph(root, camera);
}

void OIV2DScene::zoomIn()
{
  m_camera->scaleHeight(0.8f);
}

void OIV2DScene::zoomOut()
{
  m_camera->scaleHeight(1.25f);
}

void OIV2DScene::preferBottomOrRightAxis()
{
  m_preferredCurveGroup = 1;
  m_graph->setPreferredGroup (m_preferredCurveGroup);
}

bool OIV2DScene::canPreferBottomOrRightAxis() const
{
  return m_preferredCurveGroup != 1;
}

bool OIV2DScene::bottomOrRightAxisPreferred() const
{
  return m_preferredCurveGroup == 1;
}

void OIV2DScene::preferTopOrLeftAxis()
{
  m_preferredCurveGroup = 0;
  m_graph->setPreferredGroup (m_preferredCurveGroup);
}

bool OIV2DScene::canPreferTopOrLeftAxis() const
{
  return m_preferredCurveGroup != 0;
}

bool OIV2DScene::topOrLeftAxisPreferred() const
{
  return m_preferredCurveGroup == 0;
}

void OIV2DScene::viewAll()
{
  if(m_viewerAlgos != 0 && m_camera != 0 && m_width > 0 && m_height > 0)
  {
    SbViewportRegion region(m_width, m_height);

    SoGetBoundingBoxAction bba(region);
    bba.apply(m_sceneGraph);
    SbBox3f bbox = bba.getBoundingBox();
    SbVec3f bboxSize = bbox.getSize();

    float height = bboxSize[1];
    if(m_width * bboxSize[1] < m_height * bboxSize[0])
    {
      float viewportAspect = (float)m_width / (float)m_height;
      height = bboxSize[0] / viewportAspect;
    }

    const float scale = 1.3f;
    m_camera->viewAll(bbox, region);
    m_camera->height = scale * height;
  }
}

void OIV2DScene::render()
{
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

  m_viewerAlgos->actualRedraw();
}

QRgb OIV2DScene::ForegroundColor() const
{
  return GetQColor(m_foregroundColor);
}


void OIV2DScene::ForegroundColor(QRgb color)
{
  GetRGBColor(m_foregroundColor, color);
  if (m_graph) m_graph->setForegroundColor(m_foregroundColor);
  if (m_legend) m_legend->setForegroundColor(m_foregroundColor);
}

QRgb OIV2DScene::BackgroundColor() const
{
  const SbColor & color = m_background->color0.getValue();

  float rgb[3];
  color.getValue(rgb[0], rgb[1], rgb[2]);

  QRgb rgbColor = GetQColor(rgb);

  return rgbColor;
}

QRgb OIV2DScene::BackgroundColor2() const
{
  const SbColor & color = m_background->color1.getValue();

  float rgb[3];
  color.getValue(rgb[0], rgb[1], rgb[2]);

  QRgb rgbColor = GetQColor(rgb);

  return rgbColor;
}

void OIV2DScene::BackgroundColor(QRgb color)
{
  float rgb[3];
  GetRGBColor(rgb, color);
  m_background->color0.setValue(rgb);
}

void OIV2DScene::BackgroundColor2(QRgb color)
{
  float rgb[3];
  GetRGBColor(rgb, color);
  m_background->color1.setValue(rgb);
}

void OIV2DScene::setLineThickness( int t )
{
  if (t == m_lineThickness) return;
  m_lineThickness = t;

  if (m_graph) m_graph->setLineThickness (t);
}

int  OIV2DScene::getLineThickness() const
{
  return m_lineThickness;
}

void OIV2DScene::toggleMarkers()
{
  setMarkersVisible(!getMarkersVisible());
}

bool OIV2DScene::getMarkersVisible() const
{
  return m_markersVisible;
}

void OIV2DScene::setMarkersVisible(bool value)
{
  m_markersVisible = value;
  if(m_graph)
    m_graph->showMarkers(m_markersVisible);
}

void OIV2DScene::SetGraphRanges()
{
  OIV2DRangeInterface& rangeInterface = m_graph->RangeInterface();

  CGraphLimitsDlg dlg(rangeInterface);

  rangeInterface.Enter();

  bool revert = dlg.DoModal() == IDCANCEL;

  rangeInterface.Leave(revert);
}

void OIV2DScene::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  progress.Step();
  SaveValueToStream (& OIV2DScene::getLineThickness, stream);
  SaveValueToStream (& OIV2DScene::getMarkersVisible, stream);

  SaveColorToStream(&OIV2DScene::ForegroundColor, stream);
  SaveColorToStream(&OIV2DScene::BackgroundColor, stream);
  SaveColorToStream(&OIV2DScene::BackgroundColor2, stream);
}

void OIV2DScene::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  if (version > CStreamVersion(4, 1, 29))
  {
    progress.Step();
    RestoreValueFromStream(& OIV2DScene::setLineThickness, stream);
    RestoreValueFromStream(& OIV2DScene::setMarkersVisible, stream);
    RestoreColorFromStream(static_cast<void (OIV2DScene::*)(QRgb)>(& OIV2DScene::ForegroundColor), stream);
    RestoreColorFromStream(static_cast<void (OIV2DScene::*)(QRgb)>(& OIV2DScene::BackgroundColor), stream);
    RestoreColorFromStream(static_cast<void (OIV2DScene::*)(QRgb)>(& OIV2DScene::BackgroundColor2), stream);
  }
}

bool OIV2DScene::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  SbVec2s position(point.x(), point.y());
  m_viewerAlgos->setCurrentMousePositionLocator(position);

  if(button == Qt::LeftButton)
  {
       
    m_viewerMode = PanningMode;
    m_viewerAlgos->activatePanning();
    m_viewerAlgos->interactiveCountInc();
    return true;
  }
  if(button == Qt::MiddleButton)
  {
    if((state & Qt::ControlModifier) == Qt::ControlModifier)
    {
      m_viewerMode = PanningMode;
      m_viewerAlgos->activatePanning();
      m_viewerAlgos->interactiveCountInc();
      return true;
    }
    else
    {
      m_viewerMode = ZoomingMode;
      m_viewerAlgos->interactiveCountInc();
      return true;
    }
  }

  return false;
}

bool OIV2DScene::MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  m_viewerMode = ViewingMode;
  m_viewerAlgos->interactiveCountDec();
  return true;
}

bool OIV2DScene::MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  return false;
}

bool OIV2DScene::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  RefreshCursor();
  if(m_viewerMode == PanningMode)
  {
    float x = (float)point.x() / m_width;
    float y = (float)point.y() / m_height;
    m_viewerAlgos->panCamera(SbVec2f(x, y));

    return true;
  }
  else if(m_viewerMode == ZoomingMode)
  {
    SbVec2s prevPos = m_viewerAlgos->getCurrentMousePositionLocator();
    SbVec2s currPos(point.x(), point.y());

    float scale = powf(2.0f, -.01f * (currPos[1] - prevPos[1]));
    m_camera->scaleHeight(scale);
    m_viewerAlgos->setCurrentMousePositionLocator(currPos);

    return true;
  }

  return false;
}

bool OIV2DScene::UpdateLabel( const TScreenPoint &point )
{
  if (!m_graph) 
    return false;

  SbViewportRegion region(getWidth(), getHeight());
  SoRayPickAction action(region);
  action.setPickAll(true);
  action.setPoint(SbVec2s(point.x(), point.y()));
  action.apply(getSceneGraph());

  const SoPickedPointList& pointList = action.getPickedPointList();
  if(pointList.getLength() == 0)
  {
    m_graph->showLabel(false);
    return false;
  }

  m_graph->clearLabel();

  std::vector<Curve*> pickedCurves;
  SbVec3f objectPoint;
  int n = pointList.getLength();
  for(int i=0; i < n; ++i)
  {
    SoPickedPoint* pickedPoint = pointList[i];
    SoPath* path = pickedPoint->getPath();
    SbVec3f objPoint = pickedPoint->getObjectPoint();

    Curve * curve = dynamic_cast<Curve *>(path->getTail());
    if(curve != 0 && std::find(pickedCurves.begin(), pickedCurves.end(), curve) == pickedCurves.end())
    {
      // A single curve can show up multiple times in the picked points list
      pickedCurves.push_back(curve);

      // The CurveGroup is the grandparent of the curve. We need this in order
      // to pass the domain to the info label.
      CurveGroup* group = static_cast<CurveGroup*>(path->getNodeFromTail(3));

      m_graph->updateLabel(curve, group->m_domain, objPoint[0], objPoint[1]);
      m_graph->showLabel(true);
    }
  }

  return true;
}
bool OIV2DScene::MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point)
{
  if (nDelta > 0)
    m_camera->scaleHeight(0.95f);
  else
    m_camera->scaleHeight(1.05263f);

  return true;
}

bool OIV2DScene::KeyPress(TKeyboardModifiers state, int nChar, TKey key, int nRepCount)
{
  return false;
}

bool OIV2DScene::KeyRelease(TKeyboardModifiers state, int nChar, TKey key, int nRepCount)
{
  return false;
}

void OIV2DScene::UpdateAllViews ()
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  if (!pDoc) return;
  pDoc->UpdateAllViews(0, CURRENT_SCENE_MODIFIED, 0);
}

void OIV2DScene::startIVTune()
{
#ifdef USE_IVTUNE
  SoIvTune::start(m_viewerAlgos->getSceneManager());
#endif
}

void OIV2DScene::stopIVTune()
{
#ifdef USE_IVTUNE
  SoIvTune::stop();
#endif
}

void OIV2DScene::updateGraph()
{
  if (m_graph)
  {
    m_graph->setForegroundColor(m_foregroundColor);
    m_graph->setLineThickness(m_lineThickness);
    m_graph->showMarkers(m_markersVisible);
  }
}

void OIV2DScene::updateDomains()
{
  if (m_graph)
  {
    m_graph->updateDomains();
    m_graph->setLineThickness(m_lineThickness);
  }
}

void OIV2DScene::SaveColorToStream (QRgb (OIV2DScene::*func_ptr)() const, TSTREAM& stream)
{
  int iTemp;
  QColor color = QColor((this->* func_ptr)());
  iTemp = color.red();
  stream << iTemp;
  iTemp = color.green();
  stream << iTemp;
  iTemp = color.blue();
  stream << iTemp;
}

void OIV2DScene::RestoreColorFromStream (void (OIV2DScene::*func_ptr)(QRgb), TSTREAM& stream)
{
  int R,G,B;
  stream >> R >> G >> B;
  (this->* func_ptr)(qRgb(R,G,B));
}



