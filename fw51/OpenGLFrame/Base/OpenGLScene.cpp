/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "OpenGLScene.h"
#include "Line.h"
#include "OpenGLAxis.h"
#include "Sphere.h"
#include "Vector.h"
#include "dimple.h"
#include <GL/glu.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ##ModelId=3B653BCA036C
COpenGLScene::COpenGLScene(bool bDisplayAxis)
    : m_dPointSize(5.0), m_BackgroundColor(qRgb(0, 0, 0)), m_bDisplayBoundingBox(false), m_bDisplayPerspective(FALSE),
      m_bAllowDynamicRotate(TRUE), m_bAllowDynamicZoom(TRUE), m_bAllowDynamicPan(TRUE), m_bProportional(true),
      m_bUseFixedBoundingBox(false), m_dimension(DIM_3D), m_ScaleVector(geo::CVector(1, 1, 1)),
      m_DisplacementVector(geo::CVector::NullVector), m_ViewVector(geo::CVector::Zaxis),
      m_UpVector(geo::CVector::Yaxis), m_RotateLockVector(geo::CVector::NullVector),
      m_ModelScaleVector(geo::CVector(1, 1, 1)), m_pAxis(0), m_RotateSmoothFactor(1.0), m_bRecalcMaxMin(TRUE),
      m_bDuringDynamicRotate(false), m_bDuringDynamicZoom(false), m_bDuringDynamicPan(false) {
  if (bDisplayAxis)
    DisplayAxis(bDisplayAxis);
}

// ##ModelId=3B653BCA036D
COpenGLScene::~COpenGLScene() { emit OnDestroy(*this); }

bool COpenGLScene::ReParent(IOpenGLFrame *pParent) {
  InvalidateAllDisplayLists(false);
  return IOpenGLFrame::ReParent(pParent);
}

bool COpenGLScene::insert(const geo::IObject &list, const IDrawDef &def) {
  m_bRecalcMaxMin = true;
  return m_mpDisplay.insert(TDisplayMap::value_type(&list, &def)).second;
}

void COpenGLScene::InvalidateMaxMin() { m_bRecalcMaxMin = true; }

void COpenGLScene::UseFixedBoundingBox(bool bUseFixedBB) {
  m_bUseFixedBoundingBox = bUseFixedBB && (Dimension() == DIM_3D);
}

void COpenGLScene::CalcMaxMin() const {
  m_bRecalcMaxMin = false;

  m_ptMin = geo::CPoint();
  m_ptMax = geo::CPoint();

  for (const_iterator i = begin(); i != end(); i++) {
    m_ptMax = m_ptMax.Max(i->first->Max());
    m_ptMin = m_ptMin.Min(i->first->Min());
  }

  if (m_ptMax.Empty() || m_ptMin.Empty()) {
    m_ptMin = geo::CPoint(0.5, 0.5, 0.5);
    m_ptMax = geo::CPoint(-0.5, -0.5, -0.5);
  }

  if (m_ptMax == m_ptMin) {
    // When the view contains a single point, the min and max are equal. This causes trouble later on so adjust them.
    m_ptMax = m_ptMin + geo::CPoint(0.5, 0.5, 0.5);
    m_ptMin = m_ptMax - geo::CPoint(1, 1, 1);
  }

  if (UseFixedBoundingBox()) {
    m_ptMax = GetFixedBoundingBox().Max();
    m_ptMin = GetFixedBoundingBox().Min();
  }

  m_ptMid = (m_ptMax + m_ptMin) * 0.5;
}

const geo::IPoint &COpenGLScene::Mid() const {
  if (m_bRecalcMaxMin)
    CalcMaxMin();

  return m_ptMid;
}

const geo::IPoint &COpenGLScene::Max() const {
  if (m_bRecalcMaxMin)
    CalcMaxMin();

  return m_ptMax;
}

const geo::IPoint &COpenGLScene::Min() const {
  if (m_bRecalcMaxMin)
    CalcMaxMin();

  return m_ptMin;
}

void COpenGLScene::ViewVector(const geo::IVector &vec) { m_ViewVector = vec; }

const geo::CVector &COpenGLScene::ViewVector() const { return m_ViewVector; }

void COpenGLScene::UpVector(const geo::CVector &vec) { m_UpVector = vec; }

const geo::CVector &COpenGLScene::UpVector() const { return m_UpVector; }

bool COpenGLScene::DisplayBoundingBox() const { return m_bDisplayBoundingBox; }
void COpenGLScene::DisplayBoundingBox(bool newval) { m_bDisplayBoundingBox = newval; }

bool COpenGLScene::DisplayAxis() const { return Axis().Visible(); }

void COpenGLScene::DisplayAxis(bool newval) { Axis().Visible(newval); }

const COpenGLAxis &COpenGLScene::Axis() const {
  COpenGLScene *pThis = const_cast<COpenGLScene *>(this);
  return pThis->Axis();
}

COpenGLAxis &COpenGLScene::Axis() {
  if (!m_pAxis) {
    m_pAxis = new COpenGLAxis(qRgb(255, 0, 0));
    m_pAxis->SetViewPort(0, 0, 100, 100);
    m_pAxis->ReParent(this);
  }
  return *m_pAxis;
}

COpenGLScene::TColor COpenGLScene::BackgroundColor() const { return m_BackgroundColor; }

/*virtual*/ void COpenGLScene::BackgroundColor(TColor color) { m_BackgroundColor = color; }

void COpenGLScene::AxisColor(TColor color) {
  Axis().AxisColor(geo::CVector::Xaxis, color);
  Axis().AxisColor(geo::CVector::Yaxis, color);
  Axis().AxisColor(geo::CVector::Zaxis, color);
}

COpenGLScene::TColor COpenGLScene::AxisColor() const { return Axis().AxisColor(geo::CVector::Xaxis); }

void COpenGLScene::Dimension(DIMENSION dim) {
  m_dimension = dim;
  if (dim == DIM_2D) {
    RotateLockVector(geo::CVector::Zaxis);
    m_bUseFixedBoundingBox = false;
  } else {
    RotateLockVector(geo::CVector::NullVector);
  }
}

COpenGLScene::DIMENSION COpenGLScene::Dimension() const { return m_dimension; }

bool COpenGLScene::DisplayPerspective() const { return m_bDisplayPerspective; }

void COpenGLScene::DisplayPerspective(bool newval) { m_bDisplayPerspective = newval; }

const geo::CVector &COpenGLScene::ModelScaleVector() const { return m_ModelScaleVector; }

void COpenGLScene::ModelScaleVector(const geo::CVector &newval) {
  DIA_ASSERT(!(newval.X() == 0));
  DIA_ASSERT(!(newval.Y() == 0));
  DIA_ASSERT(!(newval.Z() == 0));

  m_ModelScaleVector = newval;
}

const geo::CVector &COpenGLScene::ScaleVector() const { return m_ScaleVector; }

void COpenGLScene::ScaleVector(const geo::CVector &newval) { m_ScaleVector = newval; }

void COpenGLScene::PointSize(const double &dNewSize) { m_dPointSize = dNewSize; }

const double &COpenGLScene::PointSize() const { return m_dPointSize; }

void COpenGLScene::AllowDynamicZoom(bool newval) { m_bAllowDynamicZoom = newval; }

bool COpenGLScene::AllowDynamicZoom() const { return m_bAllowDynamicZoom; }

const geo::CVector &COpenGLScene::DisplacementVector() const { return m_DisplacementVector; }

void COpenGLScene::DisplacementVector(const geo::CVector &newval) { m_DisplacementVector = newval; }

bool COpenGLScene::AllowDynamicPan() const { return m_bAllowDynamicPan; }

void COpenGLScene::AllowDynamicPan(bool newval) { m_bAllowDynamicPan = newval; }

void COpenGLScene::ResetRotate() {
  ViewVector(geo::CVector::Zaxis);
  UpVector(geo::CVector::Yaxis);
}

void COpenGLScene::ResetScale() { m_ScaleVector = geo::CVector(1, 1, 1); }

void COpenGLScene::ResetPan() { m_DisplacementVector = geo::CVector::NullVector; }

void COpenGLScene::EraseAllDisplayLists() {
  while (m_mpDisplay.size()) {
    COpenGLScene::iterator it = m_mpDisplay.begin();
    invalidate(*it->first);
    m_mpDisplay.erase(it);
  }
}

bool COpenGLScene::IsDisplayList(const geo::IObject &object) const {
  COpenGLScene::const_iterator it = m_mpDisplay.find(&object);
  return (it != COpenGLScene::end());
}

bool COpenGLScene::erase(const geo::IObject &object, bool bUpdate) {
  // Find displaylist and erase it...
  COpenGLScene::iterator it = m_mpDisplay.find(&object);

  if (it != COpenGLScene::end()) {
    invalidate(object, bUpdate);
    m_mpDisplay.erase(it);
    return true;
  }

  return false;
}

void COpenGLScene::FlushDisplayList() {
  InvalidateAllDisplayLists(false);
  IOpenGLFrame::FlushDisplayList();
}

void COpenGLScene::InvalidateAllDisplayLists(bool bUpdate) {
  m_bRecalcMaxMin = true;

  // Kill the displaylists in memory ...
  for (TDisplayListMap::iterator it = m_mpDisplayList.begin(); it != m_mpDisplayList.end(); it++)
    DeleteList(it->second);

  m_mpDisplayListRev.clear();
  m_mpDisplayList.clear();

  if (bUpdate)
    UpdateFrame();
}

void COpenGLScene::invalidate(const geo::IObject &object, bool bUpdate) {
  m_bRecalcMaxMin = true;

  TDisplayListMap::iterator it = m_mpDisplayList.find(&object);
  if (it != m_mpDisplayList.end()) {
    // explicit use of iterator after delete is not specified so first get the value from the iterator and use
    // it afterwards.
    int nIndex = it->second;
    DeleteList(nIndex);
    m_mpDisplayListRev.erase(nIndex);
    m_mpDisplayList.erase(&object);
    DIA_ASSERT(m_mpDisplayList.find(&object) == m_mpDisplayList.end());
    DIA_ASSERT(m_mpDisplayListRev.find(nIndex) == m_mpDisplayListRev.end());
  }

  if (bUpdate)
    UpdateFrame();
}

const IDrawDef &COpenGLScene::DrawDef(const geo::IObject &object) const {
  COpenGLScene::const_iterator it = m_mpDisplay.find(&object);

  DIA_ASSERT(it != COpenGLScene::end());

  return *it->second;
}

// replace a drawdef
void COpenGLScene::DrawDef(const IDrawDef &drawdef, const geo::IObject &list) {
  COpenGLScene::iterator it = m_mpDisplay.find(&list);

  DIA_ASSERT(it != COpenGLScene::end());

  it->second = &drawdef;
}

void COpenGLScene::RotateLockVector(const geo::CVector &vec) { m_RotateLockVector = vec; }

const geo::CVector &COpenGLScene::RotateLockVector() const { return m_RotateLockVector; }

COpenGLScene::COpenGLScene(const COpenGLScene &rhs)
    : QObject(), // QObject(const QObject&) is private,
                 // hence QObject(QObject* parent = 0)
      m_dPointSize(rhs.PointSize()), m_BackgroundColor(rhs.BackgroundColor()),
      m_bDisplayBoundingBox(rhs.DisplayBoundingBox()), m_bDisplayPerspective(rhs.DisplayPerspective()),
      m_bAllowDynamicRotate(rhs.AllowDynamicRotate()), m_bAllowDynamicZoom(rhs.AllowDynamicZoom()),
      m_bAllowDynamicPan(rhs.AllowDynamicPan()), m_bUseFixedBoundingBox(rhs.m_bUseFixedBoundingBox),
      m_FixedBoundingBox(rhs.m_FixedBoundingBox), m_dimension(rhs.m_dimension), m_ScaleVector(rhs.ScaleVector()),
      m_DisplacementVector(rhs.DisplacementVector()), m_ViewVector(rhs.ViewVector()), m_UpVector(rhs.UpVector()),
      m_RotateLockVector(rhs.RotateLockVector()), m_ModelScaleVector(rhs.m_ModelScaleVector), m_pAxis(0),
      m_RotateSmoothFactor(1.0) {
  EnableLighting(rhs.LightingEnabled());
  DisplayAxis(rhs.DisplayAxis());
  AxisColor(rhs.AxisColor());
}

COpenGLScene &COpenGLScene::operator=(const COpenGLScene &rhs) {
  BackgroundColor(rhs.BackgroundColor());
  DisplayAxis(rhs.DisplayAxis());
  m_bDisplayBoundingBox = rhs.DisplayBoundingBox();
  m_bDisplayPerspective = rhs.DisplayPerspective();
  m_bAllowDynamicRotate = rhs.AllowDynamicRotate();
  m_bAllowDynamicZoom = rhs.AllowDynamicZoom();
  m_bAllowDynamicPan = rhs.AllowDynamicPan();
  m_ScaleVector = rhs.ScaleVector();
  m_DisplacementVector = rhs.DisplacementVector();
  m_ViewVector = rhs.ViewVector();
  m_UpVector = rhs.UpVector();
  m_RotateLockVector = rhs.RotateLockVector();
  m_bUseFixedBoundingBox = rhs.m_bUseFixedBoundingBox;
  m_FixedBoundingBox = rhs.m_FixedBoundingBox;
  Dimension(rhs.Dimension());
  EnableLighting(rhs.LightingEnabled());
  m_dPointSize = rhs.PointSize();
  m_ModelScaleVector = rhs.m_ModelScaleVector;
  m_RotateSmoothFactor = rhs.m_RotateSmoothFactor;
  AxisColor(rhs.AxisColor());
  return *this;
}

bool COpenGLScene::operator==(const COpenGLScene &rhs) const {
  return ((LightingEnabled() == rhs.LightingEnabled()) && (PointSize() == rhs.PointSize()) &&
          (AxisColor() == rhs.AxisColor()) && (BackgroundColor() == rhs.BackgroundColor()) &&
          (DisplayAxis() == rhs.DisplayAxis()) && (DisplayBoundingBox() == rhs.DisplayBoundingBox()) &&
          (DisplayPerspective() == rhs.DisplayPerspective()) && (AllowDynamicRotate() == rhs.AllowDynamicRotate()) &&
          (AllowDynamicZoom() == rhs.AllowDynamicZoom()) && (AllowDynamicPan() == rhs.AllowDynamicPan()) &&
          (ScaleVector() == rhs.ScaleVector()) && (DisplacementVector() == rhs.DisplacementVector()) &&
          (ViewVector() == rhs.ViewVector()) && (UpVector() == rhs.UpVector()) &&
          (RotateLockVector() == rhs.RotateLockVector()) && (Dimension() == rhs.Dimension())); // &&
  //(GetFixedBoundingBox() == rhs.GetFixedBoundingBox())); TODO: ADD == operator for geo::CBox
}

void COpenGLScene::OnResizeParent() {
  assert(Parent());
  IOpenGLFrame::OnResizeParent();
}

void COpenGLScene::SceneToClipboard() {
  /*  if(Parent())
    {
    // Invalidate the attached view ...
    COpenGLView* pView = (COpenGLView*)(Parent());
    DIA_ASSERT(pView);
    pView->ClientToClipBoard();
    }*/
}

void COpenGLScene::GeometryObjectUpdated(const geo::IObject *pObject, IDrawDef *pDrawDef) {
  if (!IsDisplayList(*pObject))
    insert(*pObject, *pDrawDef);

  invalidate(*pObject);
}

void COpenGLScene::GeometryObjectRemoved(const geo::IObject *pObject) { erase(*pObject); }

int COpenGLScene::OnCountDrawSteps() const {
  int lCount = IOpenGLFrame::OnCountDrawSteps();
  for (TDisplayMap::const_iterator it = m_mpDisplay.begin(); it != m_mpDisplay.end(); it++) {
    const geo::IObject &object = *it->first;
    const IDrawDef &draw_def = *it->second;
    if (draw_def.RequireCompile() && m_mpDisplayList.find(&object) == m_mpDisplayList.end())
      lCount += CountObject(object, draw_def);
  }

  return lCount;
}

void COpenGLScene::DrawScene() {
  //  geo::CPoint MaxPoint;
  //  geo::CPoint MinPoint;
  //  geo::CPoint MidPoint;
  //  GetMaxMinScene(MaxPoint,MinPoint,MidPoint);

  for (TDisplayMap::iterator it = m_mpDisplay.begin(); it != m_mpDisplay.end(); it++) {
    const geo::IObject &object = *it->first;
    const IDrawDef &draw_def = *it->second;

    // Is the displaylist valid or are there changes ...
    if (draw_def.RequireCompile()) {
      TDisplayListMap::iterator mapIt = m_mpDisplayList.find(&object);
      int nIndex = -1;
      if (mapIt == m_mpDisplayList.end()) {
        nIndex = CompileList(object, draw_def, true);
        VERIFY(m_mpDisplayList.insert(TDisplayListMap::value_type(&object, nIndex)).second);
        VERIFY(m_mpDisplayListRev.insert(TDisplayListMapRev::value_type(nIndex, &object)).second);
      } else {
        nIndex = mapIt->second;
      }

      // wjrx mantis 2533; moved RegisterSelectableObject below CompileList
      // see if we are in render mode and if object is selectable
      bool bRegistered = false;
      if (draw_def.IsSelectable(object))
        bRegistered = RegisterSelectableObject(object);

      // call the display list
      DrawList(nIndex, draw_def);

      if (bRegistered)
        glPopName();
    } else {
      // We don't render during rotate, zoom or panning
      if (!m_bDuringDynamicRotate && !m_bDuringDynamicZoom && !m_bDuringDynamicPan)
        DrawObject(object, draw_def, false);
    }
  }

  DrawBoundingBox();

  IOpenGLFrame::DrawScene();
}

void COpenGLScene::DrawBoundingBox() {
  int iRenderMode;
  glGetIntegerv(GL_RENDER_MODE, &iRenderMode);
  if ((iRenderMode == GL_RENDER) && DisplayBoundingBox()) {
    glDisable(GL_LIGHTING);
    // Determ bounding box
    geo::CPoint ptMin, ptMax;
    if (UseFixedBoundingBox()) {
      ptMin = GetFixedBoundingBox().Min();
      ptMax = GetFixedBoundingBox().Max();
    } else {
      ptMin = Min();
      ptMax = Max();
    }

    if (ptMax.Empty())
      ptMax = geo::CPoint(0.5, 0.5, 0.5);

    if (ptMin.Empty())
      ptMin = geo::CPoint(-0.5, -0.5, -0.5);

    if (ptMax == ptMin) {
      // When the view contains a single point, the min and max are equal. This causes trouble later on so adjust them.
      ptMax = ptMin + geo::CPoint(0.5, 0.5, 0.5);
      ptMin = ptMax - geo::CPoint(1, 1, 1);
    }

    geo::CBox box(ptMin, ptMax);

    if ((box.Depth() == 0.0) || (box.Height() == 0.0) || (box.Width() == 0.0))
      return;

    CDrawDef dd(qRgb(140, 140, 140));

    dd.DepthFunc(GL_ALWAYS); // dont use depth_buffer for drawing
    dd.LineWidth(1);
    dd.PolyFillFront(FALSE);
    dd.PolyFillBack(FALSE);

    dd.LineStipple(TRUE);
    dd.PolyDrawFront(TRUE);
    dd.PolyDrawBack(FALSE);

    DrawObject(box, dd); // draw a stipeled box (only back faces are drawn!!)

    dd.LineWidth(2);
    dd.LineStipple(FALSE);
    dd.PolyDrawFront(FALSE);
    dd.PolyDrawBack(TRUE);

    DrawObject(box, dd); // draw a solid box (only front faces are drawn!!)

    if (LightingEnabled())
      glEnable(GL_LIGHTING);
  }
}

bool COpenGLScene::DynamicZoomMouseState(TKeyboardModifiers modifiers, TMouseButton button,
                                         const TScreenPoint & /*point*/) const {
  return ((modifiers & Qt::ShiftModifier) && (button == Qt::LeftButton));
}

bool COpenGLScene::DynamicPanMouseState(TKeyboardModifiers modifiers, TMouseButton button,
                                        const TScreenPoint & /*point*/) const {
  return ((modifiers & Qt::ControlModifier) && (button == Qt::LeftButton));
}

bool COpenGLScene::DynamicRotateMouseState(TKeyboardModifiers /*modifiers*/, TMouseButton button,
                                           const TScreenPoint & /*point*/) const {
  return (button == Qt::LeftButton);
}

bool COpenGLScene::MouseMove(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point) {
  if (!IOpenGLFrame::MouseMove(modifiers, button, point)) {
    // Handler for dynamic zoom
    if (DynamicZoomMouseState(modifiers, button, point)) {
      if (AllowDynamicZoom()) {
        if (m_bDuringDynamicZoom)
          DoDuringDynamicZoom(point);
        else
          DoStartDynamicZoom(point);
        return true;
      }
    } else {
      if (m_bDuringDynamicZoom) {
        DoEndDynamicZoom();
        return true;
      }
    }

    // DYNAMIC PAN
    if (DynamicPanMouseState(modifiers, button, point)) {
      if (AllowDynamicPan()) {
        if (m_bDuringDynamicPan)
          DoDuringDynamicPan(point);
        else
          DoStartDynamicPan(point);
        return true;
      }
    } else {
      if (m_bDuringDynamicPan) {
        DoEndDynamicPan();
        return true;
      }
    }

    // Handler for dynamic rotate
    if (DynamicRotateMouseState(modifiers, button, point)) {
      if (AllowDynamicRotate()) {
        if (m_bDuringDynamicRotate)
          DoDuringDynamicRotate(point);
        else
          DoStartDynamicRotate(point);
        return true;
      }
    } else {
      if (m_bDuringDynamicRotate) {
        DoEndDynamicRotate();
        return true;
      }
    }
  }

  return false;
}

bool COpenGLScene::MouseRelease(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point) {
  if (m_bDuringDynamicZoom)
    DoEndDynamicZoom();

  if (m_bDuringDynamicPan)
    DoEndDynamicPan();

  if (m_bDuringDynamicRotate)
    DoEndDynamicRotate();

  return IOpenGLFrame::MouseRelease(modifiers, button, point);
}

void COpenGLScene::DoStartDynamicZoom(const TScreenPoint &screenpoint) {
  m_ptZoomRefPoint = screenpoint;
  m_ptFirstZoomPoint = screenpoint;
  m_bDuringDynamicZoom = true;
}

void COpenGLScene::DoDuringDynamicZoom(const TScreenPoint &point) {
  geo::CLine l = GetViewingLine(m_ptZoomRefPoint);
  geo::CVector dv = geo::CVector(Mid(), l.Project(Mid()));

  m_ptSecondZoomPoint = point;

  double Factor = (double)m_ptFirstZoomPoint.y() / (double)m_ptSecondZoomPoint.y();
  ScaleVector(ScaleVector() / Factor);
  UpdateFrame();
  m_ptFirstZoomPoint = m_ptSecondZoomPoint;
}

void COpenGLScene::DoEndDynamicZoom() {
  UpdateFrame();
  m_bDuringDynamicZoom = false;
}

void COpenGLScene::DoStartDynamicRotate(const TScreenPoint &point) {
  m_ptFirstSelectedScreenPoint = point;
  m_bDuringDynamicRotate = true;
}

void COpenGLScene::DoDuringDynamicRotate(const TScreenPoint &point) {
  m_ptSecondSelectedScreenPoint = point;
  geo::CPoint RefPoint = Mid();

  if (ModelScaleVector().UnitVector() == geo::CVector(1, 1, 1).UnitVector()) {
    // isotrophic scaling
    m_ptSecondSelectedWorldPoint = GetSelectedModelSpherePoint(m_ptSecondSelectedScreenPoint);
    m_ptFirstSelectedWorldPoint = GetSelectedModelSpherePoint(m_ptFirstSelectedScreenPoint);
  } else {
    // anisotrophic scaling
    m_ptSecondSelectedWorldPoint = UnProject(m_ptSecondSelectedScreenPoint);
    m_ptFirstSelectedWorldPoint = UnProject(m_ptFirstSelectedScreenPoint);

    m_ptSecondSelectedWorldPoint.Multiply(ModelScaleVector());
    m_ptFirstSelectedWorldPoint.Multiply(ModelScaleVector());
    RefPoint.Multiply(ModelScaleVector());
  }

  geo::CVector First;
  geo::CVector Second;

  double RotAngle;

  if (RotateLockVector() == geo::CVector::NullVector) {
    // free rotate arround midpoint
    First = geo::CVector(RefPoint - m_ptFirstSelectedWorldPoint).UnitVector();
    Second = geo::CVector(RefPoint - m_ptSecondSelectedWorldPoint).UnitVector();
  } else {
    // restrict rotation arround a lockvector
    geo::CLine l = geo::CLine(RefPoint, RotateLockVector());
    geo::CPoint p = l.Project(m_ptFirstSelectedWorldPoint);
    First = geo::CVector(m_ptFirstSelectedWorldPoint - p).UnitVector();
    p = l.Project(m_ptSecondSelectedWorldPoint);
    Second = geo::CVector(m_ptSecondSelectedWorldPoint - p).UnitVector();
  }

  geo::CVector RotVec = First.CrossProduct(Second);

  if (!(RotVec == geo::CVector::NullVector)) {
    // rotate the viewingvector/upvector
    RotAngle = First.AngleDeg(Second) * m_RotateSmoothFactor;
    geo::CVector NewVec;

    NewVec = ViewVector();
    NewVec.Rotate(RotVec, -RotAngle);
    ViewVector(NewVec);

    NewVec = UpVector();
    NewVec.Rotate(RotVec, -RotAngle);
    UpVector(NewVec);

    NewVec = DisplacementVector();
    NewVec.Rotate(RotVec, -RotAngle);
    DisplacementVector(NewVec);

    // update the scene
    UpdateFrame();
  }
  m_ptFirstSelectedScreenPoint = m_ptSecondSelectedScreenPoint;
}

void COpenGLScene::DoEndDynamicRotate() {
  m_ptSecondSelectedWorldPoint = geo::CPoint();
  UpdateFrame();
  m_bDuringDynamicRotate = FALSE;
}

void COpenGLScene::DoStartDynamicPan(const TScreenPoint &point) {
  m_ptFirstSelectedScreenPoint = point;
  m_ptFirstSelectedWorldPoint = UnProject(point);
  m_bDuringDynamicPan = TRUE;
}

void COpenGLScene::DoDuringDynamicPan(const TScreenPoint &point) {
  m_ptSecondSelectedWorldPoint = UnProject(point);
  geo::CVector displ = geo::CVector(m_ptSecondSelectedWorldPoint - m_ptFirstSelectedWorldPoint);
  displ.Multiply(ModelScaleVector());
  DisplacementVector(DisplacementVector() + displ);
  UpdateFrame();
}

void COpenGLScene::DoEndDynamicPan() {
  m_ptSecondSelectedWorldPoint = geo::CPoint();
  m_ptFirstSelectedWorldPoint = geo::CPoint();

  UpdateFrame();
  m_bDuringDynamicPan = false;
}

geo::CLine COpenGLScene::GetViewingLine(const TScreenPoint &screenpoint) {
  geo::CPoint p1 = UnProject(screenpoint);
  geo::CPoint p1_far = UnProject(screenpoint, 1);
  return geo::CLine(p1, geo::CVector(p1 - p1_far));
}

// Project screenpoint on the model sphere
geo::CPoint COpenGLScene::GetSelectedModelSpherePoint(const TScreenPoint &screenpoint) {
  geo::CPoint Point1 = UnProject(screenpoint, 0.0);
  geo::CPoint Point2 = UnProject(screenpoint, 1.0);

  geo::CLine SelectionLine = geo::CLine(Point1, ViewVector());
  return GetSelectedPointOnModelSphere(SelectionLine);
}

// calculates the intersection of a line with the modelsphere
geo::CPoint COpenGLScene::GetSelectedPointOnModelSphere(const geo::CLine &line) {
  geo::CPoint RefPoint = Mid();

  // create modelsphere
  geo::CSphere sphere(RefPoint, Max().Distance(Mid()));

  // calculate the intersection
  geo::CArray<geo::CPoint> intersec = sphere.Intersection(line);

  if (intersec.Size() == 0) {
    // no intersection
    // project the center of the sphere on the line.
    // and calculate the intersection of a line( given by the center of the sphere
    // and the projection point) and the sphere.
    geo::CPoint project = line.Project(RefPoint);
    geo::CLine l(project, RefPoint);
    intersec = sphere.Intersection(l);
    assert(intersec.Size() != 0);
    if (intersec.Size() == 1) {
      return intersec.Object(0);
    } else {
      if (project.SquareDistance(intersec.Object(0)) < project.SquareDistance(intersec.Object(1))) {
        return intersec.Object(0);
      } else {
        return intersec.Object(1);
      }
    }
  } else if (intersec.Size() == 1) {
    // line touches the sphere
    return intersec.Object(0);
  } else if (intersec.Size() == 2) {
    // two intersections,take the closest to the first point of the line
    if (line.First().SquareDistance(intersec.Object(0)) < line.First().SquareDistance(intersec.Object(1))) {
      return intersec.Object(0);
    } else {
      return intersec.Object(1);
    }
  } else {
    DIA_ASSERT(FALSE);
  }
  return geo::CPoint();
}

void COpenGLScene::ZoomWindow(const TScreenPoint &ScreenPoint1, const TScreenPoint &ScreenPoint2) {
  geo::CPoint p1 = UnProject(ScreenPoint1);
  geo::CPoint p1_far = UnProject(ScreenPoint1, 1);
  geo::CPoint p2 = UnProject(ScreenPoint2);
  geo::CPoint p2_far = UnProject(ScreenPoint2, 1);

  geo::CPoint mid = (p1 + p2) / 2.0;
  geo::CPoint mid_far = (p1_far + p2_far) / 2.0;

  geo::CLine l(mid, geo::CVector(mid - mid_far));
  geo::CPoint mid2 = l.Project(Mid());

  long w1 = ViewPortWidth();
  long h1 = ViewPortHeight();

  long w2 = abs(ScreenPoint1.x() - ScreenPoint2.x());
  long h2 = abs(ScreenPoint1.y() - ScreenPoint2.y());

  double factor = std::min(double(w1) / double(w2), double(h1) / double(h2));

  geo::CVector dv(Mid() - mid2);

  dv.Multiply(ModelScaleVector());
  DisplacementVector(dv);
  ScaleVector(ScaleVector() * factor);
}

void COpenGLScene::SetupProjectionMatrix() const {
  MakeCurrent();

  geo::CPoint MaxPoint(Max());
  geo::CPoint MinPoint(Min());
  geo::CPoint MidPoint(Mid());

  MaxPoint.Multiply(ModelScaleVector());
  MinPoint.Multiply(ModelScaleVector());
  MidPoint.Multiply(ModelScaleVector());

  double dFactor = 1.05;
  double RangeMax = MaxPoint.Distance(MidPoint);
  long w = ViewPortWidth();
  long h = ViewPortHeight();

  RangeMax = RangeMax * dFactor;

  double whRatio = double(w) / double(h);
  double sfx = 1.0 / ScaleVector().X();
  double sfy = 1.0 / ScaleVector().Y();
  if (!m_bProportional) {
    sfx *= (RangeMax / (Max().X() - Min().X()));
    sfy *= (RangeMax / (Max().Y() - Min().Y()));
  }

  // 3D viewing matrix is set up here....
  if (DisplayPerspective()) {
    // set up a perspective frustum
    m_dNear = 0.99 * RangeMax; // near clipping plane
    m_dFar = 3.01 * RangeMax;  // far  clipping plane
    if (w <= h)
      glFrustum(-RangeMax * sfx, RangeMax * sfx, -RangeMax / whRatio * sfy, RangeMax / whRatio * sfy, m_dNear, m_dFar);
    else
      glFrustum(-RangeMax * whRatio * sfx, RangeMax * whRatio * sfx, -RangeMax * sfy, RangeMax * sfy, m_dNear, m_dFar);
    // gluPerspective(35.0,whRatio,m_dNear,m_dFar);
  } else {
    // set up a orthographic view
    m_dNear = 0.99 * RangeMax; // near clipping plane
    m_dFar = 3.01 * RangeMax;  // far  clipping plane
    if (w <= h)
      glOrtho(-RangeMax * sfx, RangeMax * sfx, -RangeMax / whRatio * sfy, RangeMax / whRatio * sfy, m_dNear, m_dFar);
    else
      glOrtho(-RangeMax * whRatio * sfx, RangeMax * whRatio * sfx, -RangeMax * sfy, RangeMax * sfy, m_dNear, m_dFar);
  }
}

void COpenGLScene::SetupModelMatrix() const {
  MakeCurrent();

  // set-up the modelview
  glLoadIdentity();

  geo::CPoint RefPoint = Mid();
  RefPoint.Multiply(ModelScaleVector());
  geo::CPoint EyePoint = RefPoint;

  if (Dimension() == IOpenGLFrame::DIM_3D) {
    EyePoint = EyePoint + geo::CPoint(ViewVector().UnitVector() * (m_dNear + m_dFar) /
                                      2.0); // make the matrix, regular 3D viewing matrix.....
  } else {
    RefPoint.Z(0);
    EyePoint.Z((m_dNear + m_dFar) / 2.0);
  }

  // make the matrix, regular 3D viewing matrix.....
  gluLookAt(EyePoint.X(), EyePoint.Y(), EyePoint.Z(), RefPoint.X(), RefPoint.Y(), RefPoint.Z(), UpVector().X(),
            UpVector().Y(), UpVector().Z());

  glTranslated(DisplacementVector().X(), DisplacementVector().Y(), DisplacementVector().Z());

  glScaled(ModelScaleVector().X(), ModelScaleVector().Y(), ModelScaleVector().Z());

  bool bUseFixedLightPos = true;

  if (LightingEnabled()) {
    if (bUseFixedLightPos) {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
    }

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    // http://www.sjbaker.org/steve/omniv/opengl_lighting.html
    //
    GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    // reverse normal of back facing polygons
    // wjrx mantis 2384
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    GLfloat ambientLight0[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);

    GLfloat diffuseLight0[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);

    GLfloat LightPos0[] = {0.0f, 0.0f, 1.0f, 0.0f}; // mantis0400
    glLightfv(GL_LIGHT0, GL_POSITION, LightPos0);

    glEnable(GL_LIGHT0);

    glEnable(GL_AUTO_NORMAL); // new
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    if (bUseFixedLightPos) {
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }
  }

  if (!(LightingEnabled())) {
    glDisable(GL_LIGHTING);
  }
}
