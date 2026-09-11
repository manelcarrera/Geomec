#include "stdafx.h"

#include "MaterialDualCapLinEla.h"
#include "MaterialDualCapLinEla_Delegate.h"

CMaterialDualCapLinEla_Delegate::CMaterialDualCapLinEla_Delegate(CMaterialDualCapLinEla *materialDualCapLinEla)
    : IMaterialRock_Delegate(materialDualCapLinEla), m_materialDualCapLinEla(materialDualCapLinEla) {}
