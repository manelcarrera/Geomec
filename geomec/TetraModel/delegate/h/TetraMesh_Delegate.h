#ifndef _TetraMesh_Delegate_h_
#define _TetraMesh_Delegate_h_

class CTetraMesh;

#include "MeshBase_Delegate.h"

class CTetraMesh_Delegate : public CMeshBase_Delegate
{
public:
  CTetraMesh_Delegate(CTetraMesh* tetraMesh);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool IsMesh() const;

private:
  CTetraMesh_Delegate(const CTetraMesh_Delegate& rhs);
  CTetraMesh_Delegate& operator = (const CTetraMesh_Delegate& rhs);

  CTetraMesh* m_tetraMesh;

  REGISTER_DELEGATE(CTetraMesh, CTetraMesh_Delegate);
};

#endif  // _TetraMesh_Delegate_h_
