#if !defined(AFX_MFCTREEBRANCHOBSERVERTEMP_H__INCLUDED_)
#define AFX_MFCTREEBRANCHOBSERVERTEMP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MfcTreeObjectObserver.h"

class CModelObjectScene;
template<unsigned int uIconId, class CHILD_TYPE, class CHILD_OBS_TYPE>
class CMfcTreeBranchObserverTemp : public CMfcTreeObjectObserver
{
protected:
  virtual unsigned int onIconId() const { return uIconId; }
public:
  CMfcTreeBranchObserverTemp(IModelObject& observed_object, CTreeCtrl &ctrl, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST)
  : CMfcTreeObjectObserver(observed_object, ctrl, hParent, hInsertAfter) { updateChildren(); }

  CMfcTreeBranchObserverTemp(IModelObject& observed_object, ITreeObject &parent, HTREEITEM hInsertAfter)
  : CMfcTreeObjectObserver(observed_object, parent, hInsertAfter) { updateChildren(); }

  CMfcTreeBranchObserverTemp(IModelObject& observed_object, CMfcTreeObjectObserver& observer)
  : CMfcTreeObjectObserver(observed_object, observer) { updateChildren(); }

  virtual CModelObjectObserver* insertChild(IModelObject& object)
  {
    CHILD_OBS_TYPE* pObserver = new CHILD_OBS_TYPE(object, *this);
    return pObserver;
  }

  virtual bool IsValidObservedChild(const IModelObject& observed_child_object) const
  {
    return dynamic_cast<const CHILD_TYPE*>(&observed_child_object) != 0;
  }

  virtual int childSize() const
  {
    int lRet = 0;
    for(int i = 0; i < observedObject().childSize(); i++)
    {
      if(IsValidObservedChild(observedObject().childAt(i)))
        lRet++;
    }

    for(int j = 0; j < observedObject().referenceSize(); j++)
    {
      if(IsValidObservedChild(observedObject().referenceAt(j)))
        lRet++;
    }

    return lRet;
  }

  virtual IModelObject& childAt(int nIndex)
  {
    int lRet = 0;
    for(int i = 0; i < observedObject().childSize(); i++)
    {
      if(IsValidObservedChild(observedObject().childAt(i)))
      {
        if(lRet == nIndex)
          return observedObject().childAt(i);
        lRet++;
      }
    }

    for(int j = 0; j < observedObject().referenceSize(); j++)
    {
      if(IsValidObservedChild(observedObject().referenceAt(j)))
      {
        if(lRet == nIndex)
          return const_cast<IModelObject&>(observedObject().referenceAt(j));
        lRet++;
      }
    }

    IModelObject* pBogus = 0;
    return *pBogus;
  }
};

#endif // !defined(AFX_MFCTREEBRANCHOBSERVERTEMP_H__INCLUDED_)
