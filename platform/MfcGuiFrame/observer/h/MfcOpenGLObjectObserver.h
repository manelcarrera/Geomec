// MfcOpenGLObjectObserver.h: interface for the CMfcOpenGLObjectObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCOPENGLOBJECTOBSERVER_H__83AB876A_B27F_49F1_AC82_D9C610A3C68B__INCLUDED_)
#define AFX_MFCOPENGLOBJECTOBSERVER_H__83AB876A_B27F_49F1_AC82_D9C610A3C68B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLObjectObserver.h"

class CContextMenuInvoker;
class CMfcOpenGLObjectObserver : public COpenGLObjectObserver 
{
protected:
  virtual void onContextMenu(const TScreenPoint& point);
  virtual void appendContextMenu(CContextMenuInvoker& invoker);

public:
  void invokeContextMenu(int global_x,int global_y);

  CMfcOpenGLObjectObserver(IModelObject& observed_object, CModelObjectScene& scene);
  CMfcOpenGLObjectObserver(IModelObject& observed_object, COpenGLObjectObserver& parent);
  virtual ~CMfcOpenGLObjectObserver();


};

#endif // !defined(AFX_MFCOPENGLOBJECTOBSERVER_H__83AB876A_B27F_49F1_AC82_D9C610A3C68B__INCLUDED_)
