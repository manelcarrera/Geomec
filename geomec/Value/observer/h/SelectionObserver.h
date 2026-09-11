#if !defined(SELECTION_OBSERVER_INCLUDED_)
#define SELECTION_OBSERVER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NodeObserver.h"

/*!
  Observes the leaf of an selection tree
*/
template <class OBSERVED_TYPE, class STATE_HOLDER, bool IS_RADIO>
class CSelectionLeafObserver : public CNodeObserver<OBSERVED_TYPE, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>
{
public:
  typedef bool (STATE_HOLDER::*TGetSelected)( const OBSERVED_TYPE& ) const;
  typedef void (STATE_HOLDER::*TSetSelected)( const OBSERVED_TYPE&, bool );
private:
  STATE_HOLDER* m_pHolder;
  TGetSelected  m_selected;
  TGetSelected  m_enabled;
  TSetSelected  m_select;
public:
  CSelectionLeafObserver(OBSERVED_TYPE& node,
               CTreeNode &parent,
               const BOOL rename,
               const enum REMOVE_TYPE remove,
               HTREEITEM hInsertAfter = TVI_LAST,
               BOOL bInitialUpdate = TRUE,
               BOOL bSortAfterInsertion = TRUE )
  : CNodeObserver<OBSERVED_TYPE, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>(node,parent,rename,remove,hInsertAfter,bInitialUpdate,bSortAfterInsertion), 
    m_pHolder( 0 ),
    m_selected( 0 ),
    m_enabled( 0 ),
    m_select( 0 ) 
  { assert(false); }	// Called by base ...

  CSelectionLeafObserver(OBSERVED_TYPE& node,
               CTreeNode &parent,
                           STATE_HOLDER& state,
                           TGetSelected  isSelected,
               TSetSelected  setSelected,
               TGetSelected  enabled,
               BOOL rename,
               enum REMOVE_TYPE remove)
  : CNodeObserver<OBSERVED_TYPE, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>(node,parent, rename, remove), 
    m_pHolder( &state ),
    m_selected( isSelected ),
    m_enabled( enabled ),
    m_select( setSelected ) 
  {  }

  virtual unsigned int StateIcon() const 
  {
    assert(m_pHolder);
    const OBSERVED_TYPE& item = (const OBSERVED_TYPE&)ObservedItem();
    if( IS_RADIO ) {
      if( m_enabled ) {
        if(!(m_pHolder->*m_enabled)(item)) return IDI_RADIO_DISABLED;
      }
      if((m_pHolder->*m_selected)(item))
        return IDI_RADIO_CHECKED;
      return IDI_RADIO_UNCHECKED;	
    }
    if((m_pHolder->*m_selected)(item))
      return IDI_CHECKED;
    return IDI_UNCHECKED;
  }

  virtual void ToggleState()
  {
    assert(m_pHolder);
    (m_pHolder->*m_select)((OBSERVED_TYPE&)ObservedItem(), !(m_pHolder->*m_selected)( (const OBSERVED_TYPE&)ObservedItem() ) );
    // Update whole tree
    CTreeCtrlBase *pCtrl = dynamic_cast<CTreeCtrlBase*>(&Ctrl());
    assert(pCtrl);
    pCtrl->UpdateTree();
  }
};


template<class OBSERVED_TYPE,     // Currently observed type 
         class CHILD_TYPE,	      // Child type
     class CHILD_OBS_TYPE,    // Observer type for child
     class SELECTED_TYPE,     // Type in leaf subject of selection
     class STATE_HOLDER,      // Type of holder of states
     BOOL RENAME,			  // Rename the child or not
     enum REMOVE_TYPE REMOVE> // Remove action
class CSelectionBranchObserver : public CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>
{
public:
  typedef bool (STATE_HOLDER::*TGetSelected)( const SELECTED_TYPE& ) const;
  typedef void (STATE_HOLDER::*TSetSelected)( const SELECTED_TYPE&, bool );
private:
  STATE_HOLDER* m_pHolder;
  TGetSelected  m_selected;
  TGetSelected  m_enabled;
  TSetSelected  m_select;
public:
  // Construction nested trees
  CSelectionBranchObserver(OBSERVED_TYPE& node,
           	     CTreeNode &parent,
                 const BOOL rename,
                 const enum REMOVE_TYPE remove,
                 HTREEITEM hInsertAfter = TVI_LAST,
                 BOOL bInitialUpdate = TRUE,
                 BOOL bSortAfterInsertion = TRUE )
  : CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(node,parent,rename,remove,hInsertAfter,bInitialUpdate,bSortAfterInsertion),
    m_pHolder( 0 ),
    m_selected( 0 ),
    m_enabled( 0 ),
    m_select( 0 ) { assert(false); }
  // Construction for root
  CSelectionBranchObserver( OBSERVED_TYPE&       node,
                CTreeCtrl&           view,
                              STATE_HOLDER&        state,
                              TGetSelected         isSelected,
                TSetSelected         setSelected,
                TGetSelected         enabled = 0,
                BOOL                 rename = FALSE,
                  enum REMOVE_TYPE     remove = FIXED_ITEM )
  : CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(node,view, rename, remove, TVI_ROOT, TVI_LAST, FALSE),
    m_pHolder( &state ),
    m_selected( isSelected ),
    m_enabled( enabled ),
    m_select( setSelected ) { Update(); }

  // Construction for root
  CSelectionBranchObserver( OBSERVED_TYPE&       node,
                CTreeNode&           parent,
                              STATE_HOLDER&        state,
                              TGetSelected         isSelected,
                TSetSelected         setSelected,
                TGetSelected         enabled,
                  BOOL                 rename,
                  enum REMOVE_TYPE     remove )
  : CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, FALSE, FIXED_ITEM>(node,parent, rename, remove, TVI_LAST, FALSE),
    m_pHolder( &state ),
    m_selected( isSelected ),
    m_enabled( enabled ),
    m_select( setSelected ) { Update(); }

  virtual CTreeNode* InsertChild( CHILD_TYPE& child )
  { return new CHILD_OBS_TYPE( child, *this, *m_pHolder, m_selected, m_select, m_enabled, RENAME, REMOVE ); }
public:
  typedef enum { NO_STATE, TRI_STATE, CHECKED_STATE, UNCHECKED_STATE } TState;
  TState State() const 
  {
    TState state = NO_STATE;
    TChildren children = Children();
    for(TChildren::iterator it = children.begin(); it != children.end(); it++)
    {
      if((*it)->StateIcon() == IDI_CHECKED) {
        if( state == NO_STATE )        state = CHECKED_STATE;
        if( state == UNCHECKED_STATE)  state = TRI_STATE;
      }
      if((*it)->StateIcon() == IDI_UNCHECKED) {
        if( state == NO_STATE )      state = UNCHECKED_STATE;
        if( state == CHECKED_STATE)  state = TRI_STATE;
      }
    }
    return state;
  }

  virtual unsigned int StateIcon() const
  {
    switch( State() ) {
    case UNCHECKED_STATE: return IDI_UNCHECKED;
    case CHECKED_STATE:   return IDI_CHECKED;
    case TRI_STATE:       return IDI_TRI_CHECKED;
    }
    return 0;
  }

  virtual void ToggleState() 
  {
    unsigned int icon = IDI_CHECKED;
    if( State() == CHECKED_STATE ) icon = IDI_UNCHECKED;
    TChildren children = Children();
    for(TChildren::iterator it = children.begin(); it != children.end(); it++) {
      ITreeObject *pObserver = (ITreeObject*)(*it);
      while(pObserver->StateIcon() != icon) pObserver->ToggleState();
    }
  }
};

#endif // !defined(SELECTION_OBSERVER_INCLUDED_)
