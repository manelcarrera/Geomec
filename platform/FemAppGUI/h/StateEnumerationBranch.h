#ifndef _STATEENUMERATIONBRANCH_H_
#define _STATEENUMERATIONBRANCH_H_

#include "EnumerationBranch.h"
#include <afxcview.h>

template <class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE, class UNDEFINEDICON_PROVIDER>
class CStateEnumerationBranch : public CEnumerationBranch<CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE> {
public:
  CStateEnumerationBranch(CTreeNode &parent_node, const CString &strName, const unsigned int uIcon,
                          HTREEITEM hInsertAfter = TVI_LAST, const BOOL bInitialUpdate = TRUE)
      : CEnumerationBranch<CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(parent_node, strName, uIcon, hInsertAfter,
                                                                       bInitialUpdate) {}

  virtual void ToggleState() {
    if (AllNoState())
      return;

    COperation *pOperation = 0;
    CGraphEntry *pEntry = dynamic_cast<CGraphEntry *>(&ObservedItem());
    if (pEntry)
      pOperation = new COperation(pEntry->Model());

    if (AllSameState()) {
      // Just toggle ...
      TChildren children = Children();
      for (TChildren::iterator it = children.begin(); it != children.end(); it++) {
        if ((*it)->StateIcon() != 0) {
          (*it)->ToggleState();
          (*it)->Update();
        }
      }

      if (pOperation)
        delete pOperation;
      return;
    }

    CTreeView *pView = 0;
    // Take first state as target ...
    TChildren children = Children();
    unsigned int uTarget = (*children.begin())->StateIcon();
    for (TChildren::iterator it = children.begin(); it != children.end(); it++) {
      ITreeObject *pObserver = (ITreeObject *)(*it);

      if (pObserver->StateIcon() != 0 && pObserver->StateIcon() != uTarget) {
        pObserver->ToggleState();
        pObserver->Update();
      }
    }

    if (pOperation)
      delete pOperation;
    assert(AllSameState());
  }

  virtual unsigned int StateIcon() const {
    if (AllNoState())
      return 0;

    if (AllSameState()) {
      int nZero = 0;
      int nDisabled = 0;

      TChildren children = Children();
      for (TChildren::iterator it = children.begin(); it != children.end(); ++it) {
        unsigned int stateIcon = (*it)->StateIcon();

        if (stateIcon != 0) {
          if (stateIcon == IDI_CHECK_DISABLED)
            ++nDisabled;
          else
            return stateIcon;
        } else
          ++nZero;
      }

      if (!nZero && nDisabled)
        return IDI_CHECK_DISABLED;
    }

    UNDEFINEDICON_PROVIDER provider;
    return provider.UndefinedIconId();
  }
};

#endif // _STATEENUMERATIONBRANCH_H_
