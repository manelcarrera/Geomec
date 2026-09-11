#include "stdafx.h"

#include <Inventor/drawers/SoPolyLineScreenDrawer.h>

#include "PolyLineScreenDrawerRetainer.h"
#include "OpenInventorEventsHandler.h"

PolyLineScreenDrawerRetainer * PolyLineScreenDrawerRetainer::s_activeRetainer = nullptr;

PolyLineScreenDrawerRetainer::PolyLineScreenDrawerRetainer( COpenInventorEventsHandler * eventHandler, SoPolyLineScreenDrawer * drawer ) : m_handler(eventHandler), m_drawer(drawer)
{
  if (!CanWork()) return;

  // we have an active retainer
  s_activeRetainer = this;

  m_points = m_drawer->point.startEditing();
  auto pointCount = m_drawer->point.getNum();
  
  for (auto i = 0; m_points != nullptr && i < pointCount; ++i)
  {
    m_points[i] = m_handler->ScreenDrawerToWorldPoint(m_points[i]);
  }
}

PolyLineScreenDrawerRetainer::~PolyLineScreenDrawerRetainer()
{
  if (s_activeRetainer != this) return;

  // this is the active retainer
  s_activeRetainer = nullptr;

  auto pointCount = m_drawer->point.getNum();
  for (auto i = 0; m_points != nullptr && i < pointCount; ++i)
  {
    m_points[i] = m_handler->WorldToScreenDrawerPoint(m_points[i]);
  }
  m_drawer->point.finishEditing();

  if (dynamic_cast<GeomecPolygonScreenDrawer *> (m_drawer))
    static_cast<GeomecPolygonScreenDrawer *> (m_drawer)->updateMarkers();
}

bool PolyLineScreenDrawerRetainer::CanWork()
{
  assert (m_handler);
  if (!m_handler->ShowTopView()) return false;
  if (!m_drawer) return false;
  auto pointCount = m_drawer->point.getNum();
  if (pointCount == 0) return false;
  if (s_activeRetainer != nullptr) return false;

  return true;
}
