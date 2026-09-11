#ifndef _MeshResultTree_Delegate_h_
#define _MeshResultTree_Delegate_h_

class CMeshResult;
class CMeshResultTree;

#include "IValueComposite_Delegate.h"
#include "GraphEntry_Delegate.h"

class CMeshResult_Delegate : public IValueComposite_Delegate
{
public:
  CMeshResult_Delegate(CMeshResult* meshResult);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  // Export functions

  bool CanExport() const;
  void Export();

private:
  CMeshResult_Delegate(const CMeshResult_Delegate& rhs);
  CMeshResult_Delegate& operator = (const CMeshResult_Delegate& rhs);

  CMeshResult* m_meshResult;

  REGISTER_DELEGATE(CMeshResult, CMeshResult_Delegate);
};

class CMeshResultTree_Delegate : public CGraphEntryTemp_Delegate <CMeshResult>
{
public:
  CMeshResultTree_Delegate(CMeshResultTree* meshResultTree);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanExport() const;
  void Export();

private:
  CMeshResultTree_Delegate(const CMeshResultTree_Delegate& rhs);
  CMeshResultTree_Delegate& operator = (const CMeshResultTree_Delegate& rhs);

  CMeshResultTree* m_meshResultTree;

  REGISTER_DELEGATE(CMeshResultTree, CMeshResultTree_Delegate);
};

#endif  // _MeshResultTree_Delegate_h_
