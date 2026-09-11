#ifndef _WellCasingMesh_Delegate_h_
#define _WellCasingMesh_Delegate_h_

class CWellCasingMesh;

#include "IWellMesh_Delegate.h"

class CWellCasingMesh_Delegate : public IWellMesh_Delegate
{
public:
  CWellCasingMesh_Delegate(CWellCasingMesh* wellCasingMesh);

  virtual bool Attributes();

  virtual bool IsMesh() const;

private:
  CWellCasingMesh_Delegate(const CWellCasingMesh_Delegate& rhs);
  CWellCasingMesh_Delegate& operator = (const CWellCasingMesh_Delegate& rhs);

  CWellCasingMesh* m_wellCasingMesh;

  REGISTER_DELEGATE(CWellCasingMesh, CWellCasingMesh_Delegate);
};

#endif  // _WellCasingMesh_Delegate_h_
