#ifndef _IWellMesh_Delegate_h_
#define _IWellMesh_Delegate_h_

class IWellMesh;

#include "MeshBase_Delegate.h"

class IWellMesh_Delegate : public CMeshBase_Delegate
{
public:
  IWellMesh_Delegate(IWellMesh* wellMesh);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  IWellMesh_Delegate(const IWellMesh_Delegate& rhs);
  IWellMesh_Delegate& operator = (const IWellMesh_Delegate& rhs);

  IWellMesh* m_wellMesh;

  REGISTER_DELEGATE(IWellMesh, IWellMesh_Delegate);
};

#endif  // _3DHorizon_Delegate_h_
