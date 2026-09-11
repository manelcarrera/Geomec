#include "stdafx.h"

#include "MaterialCreep_Delegate.h"
#include "MaterialCreep.h"

CMaterialCreep_Delegate::CMaterialCreep_Delegate(CMaterialCreep* materialCreep)
: IMaterialRock_Delegate(materialCreep)
, m_materialCreep(materialCreep)
{
}
