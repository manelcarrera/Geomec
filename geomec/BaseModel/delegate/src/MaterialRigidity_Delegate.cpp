#include "stdafx.h"

#include "MaterialRigidity.h"
#include "MaterialRigidity_Delegate.h"

CMaterialRigidity_Delegate::CMaterialRigidity_Delegate(CMaterialRigidity *materialRigidity)
    : IMaterialRock_Delegate(materialRigidity), m_materialRigidity(materialRigidity) {}
