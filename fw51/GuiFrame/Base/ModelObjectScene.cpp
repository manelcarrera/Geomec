/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ModelObjectScene.h"
#include "LegendFrameBase.h"
#include "OpenGLObjectObserver.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelObjectScene::CModelObjectScene(bool bDisplayAxis)
    : COpenGLScene(bDisplayAxis), m_fill(false), m_curvedDraw(true), m_legend(0) {}

CModelObjectScene::~CModelObjectScene() {
  while (m_observers.size() > 0)
    delete m_observers[0];
}

void CModelObjectScene::setCurvedDraw(bool curvedDraw) {
  bool update = (m_curvedDraw != curvedDraw);
  m_curvedDraw = curvedDraw;
  if (update) {
    FlushDisplayList();
    UpdateFrame();
  }
}

bool CModelObjectScene::curvedDraw() const { return m_curvedDraw; }

void CModelObjectScene::setFill(bool fill) {
  m_fill = fill;
  {
    FlushDisplayList();
    UpdateFrame();
  }
}

bool CModelObjectScene::fill() const { return m_fill; }

CModelObjectScene::TObjectVec CModelObjectScene::filterSelection(const COpenGLObjectObserver &observer,
                                                                 const TObjectVec &vcSelection) const {
  TObjectVec vcHit;
  // Is the list selected?
  for (int i = 0; i < vcSelection.size(); i++) {
    // Do the hit test ...
    if (observer.hitTest(*vcSelection[i]))
      vcHit.push_back(vcSelection[i]);
  }

  return vcHit;
}

bool CModelObjectScene::MousePress(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point) {
  if (MouseListener()) {
    if (MouseListener()->MousePress(modifiers, button, point))
      return true;
  }

  TObjectVec vcSelection = ProcessSelection(point);
  for (int nNode = 0; nNode < m_observers.size(); nNode++) {
    TObjectVec vcHit = filterSelection(*m_observers[nNode], vcSelection);
    if (m_observers[nNode]->mousePress(modifiers, button, point, vcHit))
      return true;
  }

  for (int i = 0; i < ChildSize(); i++) {
    if (Child(i).HitTest(point)) {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());
      if (Child(i).MousePress(modifiers, button, child_point))
        return true;
    }
  }

  return COpenGLScene::MousePress(modifiers, button, point);
}

bool CModelObjectScene::MouseRelease(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point) {
  if (MouseListener()) {
    if (MouseListener()->MouseRelease(modifiers, button, point))
      return true;
  }

  TObjectVec vcSelection = ProcessSelection(point);
  for (int nNode = 0; nNode < m_observers.size(); nNode++) {
    TObjectVec vcHit = filterSelection(*m_observers[nNode], vcSelection);
    if (m_observers[nNode]->mouseRelease(modifiers, button, point, vcHit))
      return true;
  }

  for (int i = 0; i < ChildSize(); i++) {
    if (Child(i).HitTest(point)) {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());
      if (Child(i).MouseRelease(modifiers, button, child_point))
        return true;
    }
  }

  return COpenGLScene::MouseRelease(modifiers, button, point);
}

bool CModelObjectScene::MouseDblClk(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point) {
  if (MouseListener()) {
    if (MouseListener()->MouseDblClk(modifiers, button, point))
      return true;
  }

  TObjectVec vcSelection = ProcessSelection(point);
  for (int nNode = 0; nNode < m_observers.size(); nNode++) {
    TObjectVec vcHit = filterSelection(*m_observers[nNode], vcSelection);
    if (m_observers[nNode]->mouseDblClk(modifiers, button, point, vcHit))
      return true;
  }

  for (int i = 0; i < ChildSize(); i++) {
    if (Child(i).HitTest(point)) {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());
      if (Child(i).MouseDblClk(modifiers, button, child_point))
        return true;
    }
  }

  return COpenGLScene::MouseDblClk(modifiers, button, point);
}

bool CModelObjectScene::MouseMove(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point) {
  if (MouseListener()) {
    if (MouseListener()->MouseMove(modifiers, button, point))
      return true;
  }

  TObjectVec vcSelection = ProcessSelection(point);
  for (int nNode = 0; nNode < m_observers.size(); nNode++) {
    TObjectVec vcHit = filterSelection(*m_observers[nNode], vcSelection);
    if (m_observers[nNode]->mouseMove(modifiers, button, point, vcHit))
      return true;
  }

  for (int i = 0; i < ChildSize(); i++) {
    if (Child(i).HitTest(point)) {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());
      if (Child(i).MouseMove(modifiers, button, child_point))
        return true;
    }
  }

  return COpenGLScene::MouseMove(modifiers, button, point);
}

std::vector<CModelObjectScene::TColor> CModelObjectScene::onColor(const COpenGLObjectObserver & /*node*/,
                                                                  const geo::IObject & /*object*/) const {
  std::vector<TColor> vcColor;
  return vcColor;
}

/*!
  Clients can decorate the scene with a legend. If needed, a new
  CLegendFrame is created in the topleft corner of the scene.
*/
CLegendFrame &CModelObjectScene::legend() {
  if (!m_legend) {
    m_legend = new CLegendFrame();
    m_legend->ReParent(this);
  }
  return *m_legend;
}

/*!
  Clients can decorate the scene with a legend. If needed, a new
  CLegendFrame is created in the topleft corner of the scene.
*/
const CLegendFrame &CModelObjectScene::legend() const {
  if (!m_legend) {
    m_legend = new CLegendFrame();
    m_legend->ReParent(const_cast<CModelObjectScene *>(this));
  }
  return *m_legend;
}
