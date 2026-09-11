#ifndef _GraphNode_Delegate_h_
#define _GraphNode_Delegate_h_

class CContextMenuInvoker;

#include "Delegate.h"

class CGraphNode;
class CTreeNode;

class CGraphNode_Delegate : public CDelegate {
public:
  CGraphNode_Delegate(CGraphNode *graphNode);
  CGraphNode_Delegate(const QString &strInstanceName);

  virtual bool Attributes();

  CTreeNode *TreeNode();
  void TreeNode(CTreeNode *treeNode);

  virtual void AppendToMenu(CMenu &menu);
  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  virtual bool CanEdit() const;
  virtual bool Edit();

  virtual bool OnCanPaste() const;
  virtual void OnPaste();
  virtual void OnCopy();

  virtual bool CanDestroy() const;
  virtual bool Destroy();

  virtual void OnSelect(CGraphNode *previousSelected);

  CGraphNode *getGraphNode() const { return m_graphNode; }

private:
  CGraphNode_Delegate(const CGraphNode_Delegate &rhs);
  CGraphNode_Delegate &operator=(const CGraphNode_Delegate &rhs);

  CGraphNode *m_graphNode;
  CTreeNode *m_treeNode;

  REGISTER_DELEGATE(CGraphNode, CGraphNode_Delegate);
};

typedef CSingleCommandTemplate<CGraphNode_Delegate> TGraphNode_DelegateCommand;

#endif // _GraphNode_Delegate_h_