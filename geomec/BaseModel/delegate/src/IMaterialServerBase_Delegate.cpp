#include "stdafx.h"

#include "IMaterialServerBase_Delegate.h"
#include "IMaterialServerBase.h"

IMaterialServerBase_Delegate::IMaterialServerBase_Delegate(
  IMaterialServerBase* materialServerBase)
: CStorageNode_Delegate(materialServerBase)
, m_materialServerBase(materialServerBase)
{
}
