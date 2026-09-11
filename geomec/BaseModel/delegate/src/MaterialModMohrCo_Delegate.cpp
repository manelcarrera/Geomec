#include "stdafx.h"

#include "MaterialModMohrCo.h"
#include "MaterialModMohrCo_Delegate.h"

CMaterialModMohrCo_Delegate::CMaterialModMohrCo_Delegate(CMaterialModMohrCo *materialModMohrCo)
    : IMaterialRock_Delegate(materialModMohrCo), m_materialModMohrCo(materialModMohrCo) {}
