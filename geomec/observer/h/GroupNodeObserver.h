#pragma once

#include "StateBranch_Delegate.h"
#include "UndefinedIconProvider.h"
#include "GroupNode.h"
#include "GroupNode_Delegate.h"
#include "GraphEntry.h"

template<class Node, class Observer>
class CGroupNodeObserver : public CStateBranch_Delegate<CGroupNode, CGroupNode_Delegate, Node, Observer, CUndefinedIconProvider, TRUE, TREE_DELETE_ITEM>
{
public:
  CGroupNodeObserver(CGroupNode& groupNode, CTreeNode& parent)
  : CStateBranch_Delegate(groupNode, parent, FALSE, TREE_DELETE_ITEM, TVI_ROOT, FALSE)
  {
  }

  virtual unsigned int Icon() const
  {
  const CGraphEntry *entry = static_cast<const CGroupNode&>(ObservedItem()).GraphEntry();
  return entry ? entry->IconId() : CStateBranch_Delegate::Icon();

  }

  virtual BOOL CanDelete()
  {
  return !static_cast<CGroupNode&>(ObservedItem()).ReadOnly();
  }

};

