#include "stdafx.h"

#include "MaterialMohrCo.h"
#include "MaterialMohrCo_Delegate.h"
#include "materialmccohesionhard1.h"
#include "materialmccohesionhard2.h"
#include "materialmccohesionhard3.h"
#include "materialmcfrictionhard1.h"
#include "materialmcfrictionhard2.h"
#include "materialmcfrictionhard3.h"

CMaterialMohrCo_Delegate::CMaterialMohrCo_Delegate(CMaterialMohrCo *materialMohrCo)
    : IMaterialRock_Delegate(materialMohrCo), m_materialMohrCo(materialMohrCo) {}

CMaterialMCCohesionHard1_Delegate::CMaterialMCCohesionHard1_Delegate(CMaterialMCCohesionHard1 *materialMCCohesionHard1)
    : CMaterialMohrCo_Delegate(materialMCCohesionHard1), m_materialMCCohesionHard1(materialMCCohesionHard1) {}

CMaterialMCCohesionHard2_Delegate::CMaterialMCCohesionHard2_Delegate(CMaterialMCCohesionHard2 *materialMCCohesionHard2)
    : CMaterialMohrCo_Delegate(materialMCCohesionHard2), m_materialMCCohesionHard2(materialMCCohesionHard2) {}

CMaterialMCCohesionHard3_Delegate::CMaterialMCCohesionHard3_Delegate(CMaterialMCCohesionHard3 *materialMCCohesionHard3)
    : CMaterialMohrCo_Delegate(materialMCCohesionHard3), m_materialMCCohesionHard3(materialMCCohesionHard3) {}

CMaterialMCFrictionHard1_Delegate::CMaterialMCFrictionHard1_Delegate(CMaterialMCFrictionHard1 *materialMCFrictionHard1)
    : CMaterialMohrCo_Delegate(materialMCFrictionHard1), m_materialMCFrictionHard1(materialMCFrictionHard1) {}

CMaterialMCFrictionHard2_Delegate::CMaterialMCFrictionHard2_Delegate(CMaterialMCFrictionHard2 *materialMCFrictionHard2)
    : CMaterialMohrCo_Delegate(materialMCFrictionHard2), m_materialMCFrictionHard2(materialMCFrictionHard2) {}

CMaterialMCFrictionHard3_Delegate::CMaterialMCFrictionHard3_Delegate(CMaterialMCFrictionHard3 *materialMCFrictionHard3)
    : CMaterialMohrCo_Delegate(materialMCFrictionHard3), m_materialMCFrictionHard3(materialMCFrictionHard3) {}
