#include "stdafx.h"

#include "IValueSet_Delegate.h"
#include "GeomecIValueSet.h"

IValueSet_Delegate::IValueSet_Delegate(IValueSet* valueSet)
: CStorageNode_Delegate(valueSet)
, m_valueSet(valueSet)
{
}
