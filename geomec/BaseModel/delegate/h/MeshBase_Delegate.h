#ifndef _MeshBase_Delegate_h_
#define _MeshBase_Delegate_h_

class CMeshBase;

#include "ElementSet_Delegate.h"

class CMeshBase_Delegate : public IElementSet_Delegate
{
public:
  CMeshBase_Delegate(CMeshBase* meshBase);

  virtual bool Attributes();

  virtual bool IsMesh() const;

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);
  void CreatePointSet();

  virtual bool CanDestroy() const;

private:
  CMeshBase_Delegate(const CMeshBase_Delegate& rhs);
  CMeshBase_Delegate& operator = (const CMeshBase_Delegate& rhs);

  CMeshBase* m_meshBase;

  REGISTER_DELEGATE(CMeshBase, CMeshBase_Delegate);
};

#endif  // _MeshBase_Delegate_h_
