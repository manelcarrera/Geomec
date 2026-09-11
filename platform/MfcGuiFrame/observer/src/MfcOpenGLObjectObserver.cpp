// MfcOpenGLObjectObserver.cpp: implementation of the CMfcOpenGLObjectObserver class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ModelObjectScene.h"
#include "IModelObject.h"
#include "ICommand.h"
#include "ContextMenuInvoker.h"
#include "MfcOpenGLObjectObserver.h"
#include "FemAppMainWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMfcOpenGLObjectObserver::CMfcOpenGLObjectObserver(IModelObject& observed_object, CModelObjectScene& scene)
: COpenGLObjectObserver(observed_object, scene)
{
}

CMfcOpenGLObjectObserver::CMfcOpenGLObjectObserver(IModelObject& observed_object, COpenGLObjectObserver& parent)
: COpenGLObjectObserver(observed_object, parent)
{
}

CMfcOpenGLObjectObserver::~CMfcOpenGLObjectObserver()
{

}

void CMfcOpenGLObjectObserver::invokeContextMenu(int global_x,int global_y)
{
  CContextMenuInvoker invoker;
  appendContextMenu(invoker);
  invoker.TrackMenu(global_x, global_y, *FemAppGetMainWnd());

}

void CMfcOpenGLObjectObserver::onContextMenu(const TScreenPoint& point)
{
  CContextMenuInvoker invoker;
  appendContextMenu(invoker);
  TScreenPoint global = scene().GlobalScreenPoint(point);
  invoker.TrackMenu(global.x(), global.y(), *FemAppGetMainWnd());
}

void CMfcOpenGLObjectObserver::appendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate<IModelObject> TObjectCommand;
  typedef CSingleCommandTemplate<CModelObjectObserver> TObserverCommand;

  invoker.AddCommand("Delete", *new TObjectCommand(observedObject(), &IModelObject::destroy, &IModelObject::canDestroy));

  invoker.AddSeparator();
  invoker.AddCommand("Properties", *new TObserverCommand(*this, &CModelObjectObserver::showProperties, &CModelObjectObserver::canShowProperties));
}

