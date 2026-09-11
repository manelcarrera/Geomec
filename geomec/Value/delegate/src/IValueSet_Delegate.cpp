#include "stdafx.h"

#include "GeomecIValueSet.h"
#include "IValueSet_Delegate.h"

IValueSet_Delegate::IValueSet_Delegate(IValueSet *valueSet) : CStorageNode_Delegate(valueSet), m_valueSet(valueSet) {}
