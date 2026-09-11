#if !defined(AFX_MFCTREELEAFOBSERVERTEMP_H__INCLUDED_)
#define AFX_MFCTREELEAFOBSERVERTEMP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MfcTreeObjectObserver.h"

class CModelObjectScene;
template<unsigned int uIconId>
class CMfcTreeLeafObserverTemp : public CMfcTreeObjectObserver
{
protected:
  virtual unsigned int onIconId() const { return uIconId; }
public:
  CMfcTreeLeafObserverTemp(IModelObject& observed_object, CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
  : CMfcTreeObjectObserver(observed_object, ctrl, hParent, hInsertAfter) {  }

  CMfcTreeLeafObserverTemp(IModelObject& observed_object, ITreeObject &parent, HTREEITEM hInsertAfter)
  : CMfcTreeObjectObserver(observed_object, parent, hInsertAfter) {  }

  CMfcTreeLeafObserverTemp(IModelObject& observed_object, CMfcTreeObjectObserver& observer)
  : CMfcTreeObjectObserver(observed_object, observer) {  }

  virtual int childSize() const { return 0; }
};

#endif // !defined(AFX_MFCTREELEAFOBSERVERTEMP_H__INCLUDED_)
