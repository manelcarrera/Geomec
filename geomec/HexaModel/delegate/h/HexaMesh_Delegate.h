#ifndef _HexaMesh_Delegate_h_
#define _HexaMesh_Delegate_h_

class CHexaMesh;

#include "MeshBase_Delegate.h"

class CHexaMesh_Delegate : public CMeshBase_Delegate {
public:
  CHexaMesh_Delegate(CHexaMesh *hexaMesh);

  virtual bool IsMesh() const;

private:
  CHexaMesh_Delegate(const CHexaMesh_Delegate &rhs);
  CHexaMesh_Delegate &operator=(const CHexaMesh_Delegate &rhs);

  CHexaMesh *m_hexaMesh;

  REGISTER_DELEGATE(CHexaMesh, CHexaMesh_Delegate);
};

#endif // _HexaMesh_Delegate_h_
