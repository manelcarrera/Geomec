#include "stdafx.h"

#include "MeshRegionBase.h"
#include "MeshRegionBase_Delegate.h"

CMeshRegionBase_Delegate::CMeshRegionBase_Delegate(CMeshRegionBase *meshRegionBase)
    : CColorNode_Delegate(meshRegionBase), m_meshRegionBase(meshRegionBase) {}
