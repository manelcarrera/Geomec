#ifndef _WellZoomInMesh_Delegate_h_
#define _WellZoomInMesh_Delegate_h_

class CWellZoomInMesh;

#include "IWellMesh_Delegate.h"

class CWellZoomInMesh_Delegate : public IWellMesh_Delegate
{
public:
  CWellZoomInMesh_Delegate(CWellZoomInMesh* wellZoomInMesh);

  virtual bool Attributes();

  virtual bool IsMesh() const;

private:
  CWellZoomInMesh_Delegate(const CWellZoomInMesh_Delegate& rhs);
  CWellZoomInMesh_Delegate& operator = (const CWellZoomInMesh_Delegate& rhs);

  CWellZoomInMesh* m_wellZoomInMesh;

  REGISTER_DELEGATE(CWellZoomInMesh, CWellZoomInMesh_Delegate);
};

#endif  // _WellZoomInMesh_Delegate_h_
