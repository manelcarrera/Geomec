#ifndef _BoundaryInterfaceDef_Delegate_h_
#define _BoundaryInterfaceDef_Delegate_h_

class CBoundaryInterfaceDef;

#include "StorageNode_Delegate.h"

class CBoundaryInterfaceDef_Delegate : public CStorageNode_Delegate
{
public:
  CBoundaryInterfaceDef_Delegate(CBoundaryInterfaceDef* boundaryInterfaceDef);

  virtual bool Attributes();

private:
  CBoundaryInterfaceDef_Delegate(const CBoundaryInterfaceDef_Delegate& rhs);
  CBoundaryInterfaceDef_Delegate& operator = (
    const CBoundaryInterfaceDef_Delegate& rhs);

  CBoundaryInterfaceDef* m_boundaryInterfaceDef;

  REGISTER_DELEGATE(CBoundaryInterfaceDef, CBoundaryInterfaceDef_Delegate);
};

#endif  // _BoundaryInterfaceDef_Delegate_h_
