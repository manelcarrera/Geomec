#if !defined(AFX_MFCOPENGLOBJECTOBSERVERTEMP_H__INCLUDED_)
#define AFX_MFCOPENGLOBJECTOBSERVERTEMP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MfcOpenGLObjectObserver.h"

class CModelObjectScene;
template <class CHILD_TYPE, class CHILD_OBS_TYPE> class CMfcOpenGLObjectObserverTemp : public CMfcOpenGLObjectObserver {
public:
  CMfcOpenGLObjectObserverTemp(IModelObject &observed_object, CModelObjectScene &scene)
      : CMfcOpenGLObjectObserver(observed_object, scene) {
    updateChildren();
  }

  CMfcOpenGLObjectObserverTemp(IModelObject &observed_object, COpenGLObjectObserver &parent)
      : CMfcOpenGLObjectObserver(observed_object, parent) {
    updateChildren();
  }

  virtual CModelObjectObserver *insertChild(IModelObject &object) {
    CHILD_OBS_TYPE *pObserver = new CHILD_OBS_TYPE(object, *this);
    pObserver->updateDisplayList();
    return pObserver;
  }

  virtual int childSize() const {
    int lRet = 0;
    for (int i = 0; i < observedObject().childSize(); i++) {
      if (dynamic_cast<const CHILD_TYPE *>(&observedObject().childAt(i)) != 0)
        lRet++;
    }

    for (int j = 0; j < observedObject().referenceSize(); j++) {
      if (dynamic_cast<const CHILD_TYPE *>(&observedObject().referenceAt(i)) != 0)
        lRet++;
    }

    return lRet;
  }

  virtual IModelObject &childAt(int nIndex) {
    int lRet = 0;
    for (int i = 0; i < observedObject().childSize(); i++) {
      if (dynamic_cast<const CHILD_TYPE *>(&observedObject().childAt(i)) != 0) {
        if (lRet == nIndex)
          return observedObject().childAt(i);
        lRet++;
      }
    }

    for (int j = 0; j < observedObject().referenceSize(); j++) {
      if (dynamic_cast<const CHILD_TYPE *>(&observedObject().referenceAt(j)) != 0) {
        if (lRet == nIndex)
          return observedObject().referenceAt(j);
        lRet++;
      }
    }

    IModelObject *pBogus = 0;
    return *pBogus;
  }
};

#endif // !defined(AFX_MFCOPENGLOBJECTOBSERVERTEMP_H__INCLUDED_)
