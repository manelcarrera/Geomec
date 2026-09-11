#ifndef _NONMESHEDSURFACEPARAMETERSNODE_DELEGATE_H_
#define _NONMESHEDSURFACEPARAMETERSNODE_DELEGATE_H_

#include "StorageNode_Delegate.h"

class CNonMeshedSurfaceParametersNode;

class CNonMeshedSurfaceParametersNode_Delegate : public CStorageNode_Delegate
{
public:
  CNonMeshedSurfaceParametersNode_Delegate(CNonMeshedSurfaceParametersNode* node);

  virtual bool Attributes();

private:
  CNonMeshedSurfaceParametersNode_Delegate(const CNonMeshedSurfaceParametersNode_Delegate& rhs);
  CNonMeshedSurfaceParametersNode_Delegate& operator=(const CNonMeshedSurfaceParametersNode_Delegate& rhs);

  CNonMeshedSurfaceParametersNode* m_node;

  REGISTER_DELEGATE(CNonMeshedSurfaceParametersNode, CNonMeshedSurfaceParametersNode_Delegate);
};

#endif // _NONMESHEDSURFACEPARAMETERSNODE_DELEGATE_H_
