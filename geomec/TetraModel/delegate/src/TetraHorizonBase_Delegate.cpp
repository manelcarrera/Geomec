#include "stdafx.h"

#include "TetraHorizonBase.h"
#include "TetraHorizonBase_Delegate.h"

CTetraHorizonBase_Delegate::CTetraHorizonBase_Delegate(CTetraHorizonBase *tetraHorizonBase)
    : C3DHorizon_Delegate(tetraHorizonBase), m_tetraHorizonBase(tetraHorizonBase) {}
