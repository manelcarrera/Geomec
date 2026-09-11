#if !defined(AFX_CHILDENUMERATOR__INCLUDED_)
#define AFX_CHILDENUMERATOR__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TreeNode.h"
#include "resourceIDI.h"

template<class CHILD_TYPE>
class CChildEnumerator : public CTreeNode
{
	std::set<CTreeNode*> m_stGenerated;
public:
	typedef CHILD_TYPE child_type;						// Type of the child ...
	CChildEnumerator(CTreeCtrl &ctrl,
					 HTREEITEM hParent,
					 HTREEITEM hInsertAfter);			// Constructor ...
	CChildEnumerator(IGraphTreeObject &tree_object,
					 HTREEITEM hInsertAfter);			
	virtual CTreeNode* InsertChild(CHILD_TYPE& t) = 0;		// Insertion of child in the tree
	virtual void OnChildModified(CTreeNode &child);			// Called when child is modified
	virtual void OnChildDeleted(CTreeNode &child);			// Called when child is deleted
	virtual void Update();									// Called when a child owner(observed node) changed
	virtual BOOL OnFilter(const CHILD_TYPE& t) const;
	virtual void OnExpand() { Update(); } 
	virtual void OnCollapse() { Update(); } 

protected:
	BOOL AllNoState() const
	{
		TChildren children = Children();
		for(TChildren::iterator it = children.begin(); it != children.end(); it++)
		{
			if((*it)->StateIcon() != 0)
				return FALSE;
		}

		return TRUE;
	}

	BOOL AllSameState() const
	{
		unsigned int uState = 0;
		TChildren children = Children();
		for(TChildren::iterator it = children.begin(); it != children.end(); it++)
		{
			if((*it)->StateIcon() != 0 && (*it)->StateIcon() != IDI_CHECK_DISABLED && (*it)->StateIcon() != uState)
			{
				if(uState == 0)
					uState = (*it)->StateIcon();
				else
					return FALSE;
			}
		}

		return TRUE;
	}
};

template<class CHILD_TYPE>
void CChildEnumerator<CHILD_TYPE>::Update()
{
	bool bInsert = false;


	// Nodes of the children should be in the tree
	TGraphNodeSet data_items;
  
  /* 
  To solve Bug 72323, we need the grandchildren nodes to determine whether we can select a node. Therefore,
  we always create the observers that don't exist yet, because they're not being displayed.
  It should be okay to do this here; alternatively, we could create a 'force' parameter, but then we need
  to adjust more classes, and instances where Update() is called.

	if((Parent() == 0) || Parent()->IsExpanded() ||
		(ObservedChildren().size() > 0))
  {
  */
		// Nodes of the children now in the tree
		TGraphNodeSet displayed_items = ObservedChildren();
    for(size_t i = 0; i < ObservedItem().childSize(); i++) {
			CHILD_TYPE* pChild = dynamic_cast<CHILD_TYPE*>(&ObservedItem().childAt(i));
			if((pChild) && OnFilter(*pChild))
			{
				data_items.insert(pChild);
				// Do we have to diplay it?
				if(displayed_items.find(pChild) == displayed_items.end())
				{
					bInsert = true;
					m_stGenerated.insert(InsertChild(*pChild));
				}
			}
		}
		for(size_t i = 0; i < ObservedItem().referenceSize(); i++) {
			CHILD_TYPE* pChild = dynamic_cast<CHILD_TYPE*>(&ObservedItem().referenceAt(i));
			if((pChild) && OnFilter(*pChild))
			{
				data_items.insert(pChild);
				// Do we have to diplay it?
				if(displayed_items.find(pChild) == displayed_items.end())
				{
					bInsert = true;
					m_stGenerated.insert(InsertChild(*pChild));
				}
			}
		}
  /*
	}
  */

	// Delete non-existing nodes ...
	std::set<CTreeNode*> stGenerated = m_stGenerated;
	for(std::set<CTreeNode*>::iterator it_gen = stGenerated.begin(); it_gen != stGenerated.end(); it_gen++)
	{
		if(data_items.find(&(*it_gen)->ObservedItem()) == data_items.end())
			delete *it_gen;
	}

	if(bInsert)
		SortChildren();

	// We update the base
	CTreeNode::Update();
}

template<class CHILD_TYPE>
void CChildEnumerator<CHILD_TYPE>::OnChildDeleted(CTreeNode &child)
{
	m_stGenerated.erase(&child);
}

template<class CHILD_TYPE>
BOOL CChildEnumerator<CHILD_TYPE>::OnFilter(const CHILD_TYPE& t) const
{
	return TRUE;
}
		
template<class CHILD_TYPE>
CChildEnumerator<CHILD_TYPE>::CChildEnumerator(IGraphTreeObject &tree_object, HTREEITEM hInsertAfter)
: CTreeNode(tree_object, hInsertAfter)
{
}

template<class CHILD_TYPE>
CChildEnumerator<CHILD_TYPE>::CChildEnumerator(CTreeCtrl &ctrl,
											   HTREEITEM hParent,
											   HTREEITEM hInsertAfter)
: CTreeNode(ctrl, hParent, hInsertAfter)
{
}

template<class CHILD_TYPE>
void CChildEnumerator<CHILD_TYPE>::OnChildModified(CTreeNode &child)
{ SortChildren(); }

#endif // !defined(AFX_CHILDENUMERATOR__INCLUDED_)