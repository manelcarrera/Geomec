 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
  // OpenGLObjectObserver.h: interface for the COpenGLObjectObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLOBJECTOBSERVER_H__6D9D38EB_EC90_4EF9_8180_93773DD1038C__INCLUDED_)
#define AFX_OPENGLOBJECTOBSERVER_H__6D9D38EB_EC90_4EF9_8180_93773DD1038C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "IDrawDef.h"
#include "ModelObjectObserver.h"

#include "GuiFrameExports.h"
namespace geo { class IObject; }

class CLegendFrame;
class CModelObjectScene;
class GUIFRAME_EXPORT COpenGLObjectObserver : public CModelObjectObserver
{
public:
  class GUIFRAME_EXPORT CObserverDrawDef : public IDrawDef
  {
    COpenGLObjectObserver& m_observer;
  public:
    CObserverDrawDef(COpenGLObjectObserver& observer);
    virtual std::vector<TColor> Color(const geo::IObject &object) const;
    const COpenGLObjectObserver& Observer() const;
    COpenGLObjectObserver& Observer();
    virtual bool Visible() const;
    virtual bool PolyFillFront() const;
    virtual bool PolyFillBack() const;
    virtual bool CurvedDraw() const;
    virtual bool LineStipple() const;
    virtual float LineWidth() const;
    virtual float PointSize() const;
    virtual bool PolyDrawFront() const;
    virtual bool PolyDrawBack() const;
    virtual int DepthFunc() const;
    virtual bool Value() const;
    virtual bool IsSelectable(const geo::IObject &object) const;		
  };
public:
  COpenGLObjectObserver(IModelObject& observed_object, CModelObjectScene& scene);
  COpenGLObjectObserver(IModelObject& observed_object, COpenGLObjectObserver& parent);
  virtual ~COpenGLObjectObserver();

  // Scene
  const CModelObjectScene& scene() const;
  CModelObjectScene& scene();

  
  // Selection
  virtual void select();
  virtual void deSelect();

  // Reinitialise the scene
  void updateDisplayList();
  void updateDisplayList(int nIndex);
  void updateLegend();

  bool hitTest(const geo::IObject& object) const;

  // Mouse handlers
  typedef QPoint TScreenPoint;
  typedef Qt::MouseButton TMouseButton;
  typedef Qt::KeyboardModifiers TKeyboardModifiers;
  typedef std::vector<const geo::IObject*> TObjectVec;
  virtual bool mousePress(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit);
  virtual bool mouseRelease(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit);
  virtual bool mouseDblClk(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit);
  virtual bool mouseMove(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit);

  //react on different state to select objects
  virtual bool isSelectionState(TKeyboardModifiers modifiers, TMouseButton button) const;

  // Geometry to display
  virtual int displayListSize() const;
  virtual const geo::IObject& displayList(int nIndex) const;
  virtual const IDrawDef& drawDef(int nIndex) const;
  void removeDisplayList(geo::IObject& displayList);

  // Legend to display
  virtual void appendLegend( CLegendFrame& legend ) const;

  // Color override
  typedef QRgb TColor;
  virtual std::vector<TColor> onColor(const geo::IObject &object) const;


protected:
  virtual void onDisplayChanged();	//*!< Emitted when the display properties of an object are changed.
  virtual void onGeometryChanged();	//*!< The geometry of a the object is changed.
  virtual void onContextMenu(const TScreenPoint& point);




private:
  CObserverDrawDef* m_pDrawDef;
  CModelObjectScene& m_scene;

  std::vector<const geo::IObject*> m_inserted;
  TObjectVec filterSelection(const COpenGLObjectObserver& observer, const TObjectVec& vcSelection) const;
};

#endif // !defined(AFX_OPENGLOBJECTOBSERVER_H__6D9D38EB_EC90_4EF9_8180_93773DD1038C__INCLUDED_)
