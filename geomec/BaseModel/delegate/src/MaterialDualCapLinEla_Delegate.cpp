#include "stdafx.h"

#include "MaterialDualCapLinEla_Delegate.h"
#include "MaterialDualCapLinEla.h"

CMaterialDualCapLinEla_Delegate::CMaterialDualCapLinEla_Delegate(
  CMaterialDualCapLinEla* materialDualCapLinEla)
: IMaterialRock_Delegate(materialDualCapLinEla)
, m_materialDualCapLinEla(materialDualCapLinEla)
{
}
