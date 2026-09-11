#ifndef _MeshRegionBase_Delegate_h_
#define _MeshRegionBase_Delegate_h_

class CMeshRegionBase;

#include "ColorNode_Delegate.h"

class CMeshRegionBase_Delegate : public CColorNode_Delegate
{
public:
  CMeshRegionBase_Delegate(CMeshRegionBase* meshRegionBase);

private:
  CMeshRegionBase_Delegate(const CMeshRegionBase_Delegate& rhs);
  CMeshRegionBase_Delegate& operator = (const CMeshRegionBase_Delegate& rhs);

  CMeshRegionBase* m_meshRegionBase;

  REGISTER_DELEGATE(CMeshRegionBase, CMeshRegionBase_Delegate);
};

#endif  // _MeshRegionBase_Delegate_h_
