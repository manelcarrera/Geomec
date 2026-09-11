#include "stdafx.h"

#include "3DFormation.h"
#include "3DFormation_Delegate.h"

C3DFormation_Delegate::C3DFormation_Delegate(C3DFormation *a3DFormation)
    : CFormationBase_Delegate(a3DFormation), m_3DFormation(a3DFormation) {}
