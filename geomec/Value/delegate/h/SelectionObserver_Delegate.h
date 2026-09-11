#ifndef _SelectionObserver_Delegate_h_
#define _SelectionObserver_Delegate_h_

#include "NodeObserver_Delegate.h"
#include "resourceIDI.h"

template <class OBSERVED_TYPE, class DELEGATE_TYPE, class STATE_HOLDER, bool IS_RADIO>
class CSelectionLeafObserver_Delegate
    : public CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> {
public:
  typedef bool (STATE_HOLDER::*TGetSelected)(const OBSERVED_TYPE &) const;
  typedef void (STATE_HOLDER::*TSetSelected)(const OBSERVED_TYPE &, bool);

  CSelectionLeafObserver_Delegate(OBSERVED_TYPE &node, CTreeNode &parent, const BOOL rename,
                                  const enum REMOVE_TYPE remove, HTREEITEM hInsertAfter = TVI_LAST,
                                  BOOL bInitialUpdate = TRUE, BOOL bSortAfterInsertion = TRUE)
      : CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>(
            node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion),
        m_pHolder(0), m_selected(0), m_enabled(0), m_select(0) {
    // Called by base ...

    assert(false);
  }

  CSelectionLeafObserver_Delegate(OBSERVED_TYPE &node, CTreeNode &parent, STATE_HOLDER &state, TGetSelected isSelected,
                                  TSetSelected setSelected, TGetSelected enabled, BOOL rename, enum REMOVE_TYPE remove)
      : CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>(
            node, parent, rename, remove),
        m_pHolder(&state), m_selected(isSelected), m_enabled(enabled), m_select(setSelected) {}

  virtual unsigned int StateIcon() const {
    assert(m_pHolder);

    const OBSERVED_TYPE &item = (const OBSERVED_TYPE &)ObservedItem();

    bool mvcAllowed = m_pHolder->MultipleValueComponentsAllowed();

    if (m_enabled) {
      if (!(m_pHolder->*m_enabled)(item)) {
        return mvcAllowed ? IDI_CHECK_DISABLED : IDI_RADIO_DISABLED;
      }
    }

    if ((m_pHolder->*m_selected)(item)) {
      return mvcAllowed ? IDI_CHECKED : IDI_RADIO_CHECKED;
    }

    return mvcAllowed ? IDI_UNCHECKED : IDI_RADIO_UNCHECKED;
  }

  virtual void ToggleState() {
    assert(m_pHolder);

    (m_pHolder->*m_select)((OBSERVED_TYPE &)ObservedItem(),
                           !(m_pHolder->*m_selected)((const OBSERVED_TYPE &)ObservedItem()));

    // Update whole tree

    CTreeCtrlBase *pCtrl = dynamic_cast<CTreeCtrlBase *>(&Ctrl());

    assert(pCtrl);

    pCtrl->UpdateTree();
  }

private:
  STATE_HOLDER *m_pHolder;
  TGetSelected m_selected;
  TGetSelected m_enabled;
  TSetSelected m_select;
};

template <class OBSERVED_TYPE, class DELEGATE_TYPE, class CHILD_TYPE, class CHILD_OBS_TYPE, class SELECTED_TYPE,
          class STATE_HOLDER, BOOL RENAME, enum REMOVE_TYPE REMOVE>
class CSelectionBranchObserver_Delegate
    : public CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE> {
public:
  typedef bool (STATE_HOLDER::*TGetSelected)(const SELECTED_TYPE &) const;
  typedef void (STATE_HOLDER::*TSetSelected)(const SELECTED_TYPE &, bool);

  // Construction nested trees

  CSelectionBranchObserver_Delegate(OBSERVED_TYPE &node, CTreeNode &parent, const BOOL rename,
                                    const enum REMOVE_TYPE remove, HTREEITEM hInsertAfter = TVI_LAST,
                                    BOOL bInitialUpdate = TRUE, BOOL bSortAfterInsertion = TRUE)
      : CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(
            node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion),
        m_pHolder(0), m_selected(0), m_enabled(0), m_select(0) {
    assert(false);
  }

  // Construction for root

  CSelectionBranchObserver_Delegate(OBSERVED_TYPE &node, CTreeCtrl &view, STATE_HOLDER &state, TGetSelected isSelected,
                                    TSetSelected setSelected, TGetSelected enabled = 0, BOOL rename = FALSE,
                                    enum REMOVE_TYPE remove = FIXED_ITEM)
      : CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(
            node, view, rename, remove, TVI_ROOT, TVI_LAST, FALSE),
        m_pHolder(&state), m_selected(isSelected), m_enabled(enabled), m_select(setSelected) {
    Update();
  }

  // Construction for root

  CSelectionBranchObserver_Delegate(OBSERVED_TYPE &node, CTreeNode &parent, STATE_HOLDER &state,
                                    TGetSelected isSelected, TSetSelected setSelected, TGetSelected enabled,
                                    BOOL rename, enum REMOVE_TYPE remove)
      : CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, FALSE, FIXED_ITEM>(
            node, parent, rename, remove, TVI_LAST, FALSE),
        m_pHolder(&state), m_selected(isSelected), m_enabled(enabled), m_select(setSelected) {
    Update();
  }

  virtual CTreeNode *InsertChild(CHILD_TYPE &child) {
    return new CHILD_OBS_TYPE(child, *this, *m_pHolder, m_selected, m_select, m_enabled, RENAME, REMOVE);
  }

  typedef enum { NO_STATE, TRI_STATE, CHECKED_STATE, UNCHECKED_STATE } TState;

  TState State() const {
    TState state = NO_STATE;
    TChildren children = Children();

    for (TChildren::iterator it = children.begin(); it != children.end(); it++) {
      if ((*it)->StateIcon() == IDI_CHECKED) {
        if (state == NO_STATE) {
          state = CHECKED_STATE;
        }

        if (state == UNCHECKED_STATE) {
          state = TRI_STATE;
        }
      }

      if ((*it)->StateIcon() == IDI_UNCHECKED) {
        if (state == NO_STATE) {
          state = UNCHECKED_STATE;
        }

        if (state == CHECKED_STATE) {
          state = TRI_STATE;
        }
      }

      if ((*it)->StateIcon() == IDI_TRI_CHECKED) {
        state = TRI_STATE;
      }
    }

    return state;
  }

  virtual unsigned int StateIcon() const {
    switch (State()) {
    case UNCHECKED_STATE:
      return IDI_UNCHECKED;
    case CHECKED_STATE:
      return IDI_CHECKED;
    case TRI_STATE:
      return IDI_TRI_CHECKED;
    }

    return 0;
  }

  virtual void ToggleState() {
    unsigned int icon = IDI_CHECKED;

    if (State() == CHECKED_STATE) {
      icon = IDI_UNCHECKED;
    }

    Update();

    if (m_pHolder)
      m_pHolder->BranchToggle(true);

    TChildren children = Children();

    for (TChildren::iterator it = children.begin(); it != children.end(); it++) {
      ITreeObject *pObserver = (ITreeObject *)(*it);

      if (pObserver->StateIcon() != icon) {
        pObserver->ToggleState();
      }
    }

    if (m_pHolder)
      m_pHolder->BranchToggle(false);
  }

private:
  STATE_HOLDER *m_pHolder;
  TGetSelected m_selected;
  TGetSelected m_enabled;
  TSetSelected m_select;
};

#endif // _SelectionObserver_Delegate_h_
