#include "stdafx.h"

#include "IMaterialRock.h"
#include "IMaterialRock_Delegate.h"

IMaterialRock_Delegate::IMaterialRock_Delegate(IMaterialRock *materialRock)
    : IMaterial_Delegate(materialRock), m_materialRock(materialRock) {}
