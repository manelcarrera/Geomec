#include "stdafx.h"

#include "MaterialUndrained.h"
#include "MaterialUndrained_Delegate.h"

CMaterialUndrained_Delegate::CMaterialUndrained_Delegate(CMaterialUndrained *materialUndrained)
    : IMaterialRock_Delegate(materialUndrained), m_materialUndrained(materialUndrained) {}
