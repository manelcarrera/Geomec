#include "stdafx.h"

#include "IMaterialBase_Delegate.h"
#include "IMaterialBase.h"

IMaterialBase_Delegate::IMaterialBase_Delegate(IMaterialBase* materialBase)
: CStorageNode_Delegate(materialBase)
, m_materialBase(materialBase)
{
}
