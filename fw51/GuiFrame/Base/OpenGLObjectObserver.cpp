 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include <QObject>
#include <cassert>
#include "IModelObject.h"
#include "ModelObjectScene.h"
#include "OpenGLObjectObserver.h"
#include "LegendFrameBase.h"

//////////////////////////////////////////////////////////////////////
// Implementation of the CDrawDef class
//////////////////////////////////////////////////////////////////////

COpenGLObjectObserver::CObserverDrawDef::CObserverDrawDef(COpenGLObjectObserver &observer)
: m_observer(observer)
{
}

const COpenGLObjectObserver& COpenGLObjectObserver::CObserverDrawDef::Observer() const
{
  return m_observer;

}

COpenGLObjectObserver& COpenGLObjectObserver::CObserverDrawDef::Observer()
{
  return m_observer;
}

std::vector<COpenGLObjectObserver::CObserverDrawDef::TColor> COpenGLObjectObserver::CObserverDrawDef::Color(const geo::IObject &object) const
{
  // Pass function call
  return m_observer.onColor(object);
}

bool COpenGLObjectObserver::CObserverDrawDef::Visible() const
{
  return TRUE;
}

bool COpenGLObjectObserver::CObserverDrawDef::PolyFillFront() const
{
  return m_observer.scene().fill();
}

bool COpenGLObjectObserver::CObserverDrawDef::PolyFillBack() const
{
  return m_observer.scene().fill();
}

bool COpenGLObjectObserver::CObserverDrawDef::CurvedDraw() const
{
  return m_observer.scene().curvedDraw();
}

bool COpenGLObjectObserver::CObserverDrawDef::LineStipple() const
{
  return false;
}

float COpenGLObjectObserver::CObserverDrawDef::LineWidth() const
{
  if(m_observer.isSelected())
    return 2.5;
  return 1.0;
}

float COpenGLObjectObserver::CObserverDrawDef::PointSize() const
{
  if(m_observer.isSelected())
    return 5.0;
  return 1.0;
}

bool COpenGLObjectObserver::CObserverDrawDef::PolyDrawFront() const
{
  return TRUE;
}

bool COpenGLObjectObserver::CObserverDrawDef::PolyDrawBack() const
{
  return TRUE;
}

int COpenGLObjectObserver::CObserverDrawDef::DepthFunc() const
{
  return GL_LESS;
}

bool COpenGLObjectObserver::CObserverDrawDef::Value() const
{
  return TRUE;
}

bool COpenGLObjectObserver::CObserverDrawDef::IsSelectable(const geo::IObject& object) const
{
  for(int i = 0; i < m_observer.displayListSize(); i++)
  {
    if(&m_observer.displayList(i) == &object)
      return true;
  }

  return false;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COpenGLObjectObserver::COpenGLObjectObserver(IModelObject& observed_object, CModelObjectScene& scene)
: CModelObjectObserver(observed_object, 0), m_scene(scene),  m_inserted(false)
{
  m_pDrawDef = new CObserverDrawDef(*this);
  scene.m_observers.push_back(this);
}

COpenGLObjectObserver::COpenGLObjectObserver(IModelObject& observed_object, COpenGLObjectObserver& parent)
: CModelObjectObserver(observed_object, &parent), m_scene(parent.m_scene),  m_inserted(false)
{
  m_pDrawDef = new CObserverDrawDef(*this);
  for(size_t i = 0; i < m_scene.m_observers.size(); i++)
  {
    if(m_scene.m_observers[i] == this)
      m_scene.m_observers.erase(m_scene.m_observers.begin() + i);
  }
}

COpenGLObjectObserver::~COpenGLObjectObserver()
{
  for(size_t i = 0; i < m_inserted.size(); i++)
    scene().erase(*m_inserted[i], false);

  // Remove ourselves from the scene
  for(size_t i = 0; i < m_scene.m_observers.size(); i++)
  {
    if(m_scene.m_observers[i] == this)
      m_scene.m_observers.erase(m_scene.m_observers.begin() + i);
  }


  delete m_pDrawDef;
}

void COpenGLObjectObserver::updateDisplayList(int nIndex)
{
  if(nIndex < displayListSize())
  {
    if(m_inserted[nIndex] != &displayList(nIndex))
    {
      scene().erase(*m_inserted[nIndex], false);
      scene().insert(displayList(nIndex), drawDef(nIndex));
      m_inserted[nIndex] = &displayList(nIndex);
    }
    else
      scene().invalidate(displayList(nIndex), false);
  }
  else
    scene().erase(*m_inserted[nIndex], false);

}

void COpenGLObjectObserver::updateDisplayList()
{
  // Check current display list
  int i;
  for(i = 0; i < m_inserted.size(); i++)
    updateDisplayList(i);

  int nBegin = (int)m_inserted.size();
  m_inserted.resize(displayListSize());
  for(i = nBegin; i < displayListSize(); i++)
  {
    scene().insert(displayList(i), drawDef(i));
    m_inserted[i] = &displayList(i);
  }
}

void COpenGLObjectObserver::removeDisplayList(geo::IObject& displayList)
{
  for(size_t i = 0; i < m_inserted.size(); i++)
  {
    if((m_inserted[i] = &displayList))
    {
      scene().erase(*m_inserted[i], false);
      m_inserted[i] = m_inserted[m_inserted.size()-1];
      m_inserted.resize(m_inserted.size()-1);	
      return;
    }
  }
}

/*!
  The legend of the scene is cleared, filled by appendLegend() and
  resized to its contents.

  The legend is updated upon change of display and geometry.

  \sa appendLegend, CModelOBjectScene::legend
*/
void COpenGLObjectObserver::updateLegend()
{
  scene().legend().ClearLegendFrames();
  appendLegend( scene().legend() );
  scene().legend().OnResizeParent();
}

const CModelObjectScene& COpenGLObjectObserver::scene() const
{
  return m_scene;
}

CModelObjectScene& COpenGLObjectObserver::scene()
{
  return m_scene;
}


void COpenGLObjectObserver::select()
{
  CModelObjectObserver::select();
  selectChildren();

}

void COpenGLObjectObserver::deSelect()
{
  CModelObjectObserver::deSelect();
  deSelectChildren();
}


void COpenGLObjectObserver::onDisplayChanged()
{
  updateDisplayList();
  updateLegend();
}

void COpenGLObjectObserver::onGeometryChanged()
{
  updateDisplayList();
  updateLegend();
}



std::vector<CDrawDef::TColor> COpenGLObjectObserver::onColor(const geo::IObject &object) const
{
  const geo::IElement *pElement = dynamic_cast<const geo::IElement*>(&object);

  if(pElement)
    return std::vector<CDrawDef::TColor>(pElement->NrOfPoints(), observedObject().color());

  return std::vector<CDrawDef::TColor>(1, observedObject().color());
}

int COpenGLObjectObserver::displayListSize() const
{
  return observedObject().geometrySize();
}

const geo::IObject& COpenGLObjectObserver::displayList(int nIndex) const
{
  return observedObject().geometry(nIndex);
}

const IDrawDef& COpenGLObjectObserver::drawDef(int /*nIndex*/) const
{
  return *m_pDrawDef;
}

/*!
  This function is automatically called when the legend needs an update.
  Override this function to put items in the legend.
  \sa CLegendFrame
*/
void COpenGLObjectObserver::appendLegend( CLegendFrame& /*legend*/ ) const
{
}

bool COpenGLObjectObserver::hitTest(const geo::IObject& object) const
{
  for(size_t i = 0; i < m_inserted.size(); i++)
  {
    if(m_inserted[i] == &object)
      return true;
  }
  
  for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
  {
    COpenGLObjectObserver* pObserver = dynamic_cast<COpenGLObjectObserver*>(*it);
    if(pObserver)
    {
      if(pObserver->hitTest(object))
        return true;
    }
  }

  return false;
}

COpenGLObjectObserver::TObjectVec COpenGLObjectObserver::filterSelection(const COpenGLObjectObserver& observer, const TObjectVec& vcSelection) const
{
  TObjectVec vcHit;
  // Is the list selected?
  for(size_t i = 0; i < vcSelection.size(); i++)
  {
    // Do the hit test ...
    if(observer.hitTest(*vcSelection[i]))
      vcHit.push_back(vcSelection[i]);
  }

  return vcHit;
}

bool COpenGLObjectObserver::mousePress(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit)
{
  if(vcHit.size())
  {
    for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
    {
          COpenGLObjectObserver* pObserver =  dynamic_cast<COpenGLObjectObserver*>(*it);
      if(pObserver)
      {
         	 TObjectVec vcObject  = pObserver->filterSelection(*pObserver, vcHit);
          	if(vcObject.size() > 0)
      {
        if(pObserver->mousePress(modifiers, button, point, vcObject))
          return true;
      }
      }
    }
  }

  return false;
}

/*virtual*/ bool COpenGLObjectObserver::isSelectionState(TKeyboardModifiers modifiers,TMouseButton button) const
{
  if((modifiers & Qt::ControlModifier) && (button & Qt::LeftButton))
    return true;

  return false;
}


bool COpenGLObjectObserver::mouseRelease(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit)
{
  if( isSelectionState(modifiers , button) )
  {
    for(size_t i = 0; i < vcHit.size(); i++)
    {
      for(size_t j = 0; j < m_inserted.size(); j++)
      {
        if(vcHit[i] == m_inserted[j])
        {
          if(isSelected())
      deSelect();
          else
            select();
          return true;
        }
      }
    }
  }

  if(button & Qt::RightButton)
  {
    for(size_t i = 0; i < vcHit.size(); i++)
    {
      for(size_t j = 0; j < m_inserted.size(); j++)
      {
        if(vcHit[i] == m_inserted[j])
        {
          if(!isSelected())
          {
            select();
          }

          onContextMenu(point);
//					if(bDeSelect)
//					{
//						deSelect();
//						scene().UpdateFrame();
//					}
          return true;
        }
      }
    }
  }

  // Let the selection process do it job
  if(vcHit.size())
  {
    for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
    {
          COpenGLObjectObserver* pObserver =  dynamic_cast<COpenGLObjectObserver*>(*it);
      if(pObserver)
      {
          	TObjectVec vcObject  = pObserver->filterSelection(*pObserver, vcHit);
          	if(vcObject.size() > 0)
      {
        if(pObserver->mouseRelease(modifiers, button, point, vcObject))
          return true;
      }
      }
    }
  }

  return false;
}

bool COpenGLObjectObserver::mouseDblClk(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit)
{
  if(vcHit.size())
  {
    for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
    {
          COpenGLObjectObserver* pObserver =  dynamic_cast<COpenGLObjectObserver*>(*it);
      if(pObserver)
      {
          	TObjectVec vcObject  = pObserver->filterSelection(*pObserver, vcHit);
          	if(vcObject.size() > 0)
      {
        if(pObserver->mouseDblClk(modifiers, button, point, vcObject))
          return true;
      }
      }
    }
  }

  return false;
}

bool COpenGLObjectObserver::mouseMove(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit)
{
  if(vcHit.size())
  {
    for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
    {
          COpenGLObjectObserver* pObserver =  dynamic_cast<COpenGLObjectObserver*>(*it);
      if(pObserver)
      {
          	TObjectVec vcObject  = pObserver->filterSelection(*pObserver, vcHit);
          	if(vcObject.size() > 0)
      {
        if(pObserver->mouseMove(modifiers, button, point, vcObject))
          return true;
      }
      }
    }
  }

  return false;
}

void COpenGLObjectObserver::onContextMenu(const TScreenPoint& /*point*/)
{

}

