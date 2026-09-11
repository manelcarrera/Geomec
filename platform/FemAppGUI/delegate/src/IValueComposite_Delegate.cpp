#include "stdafx.h"

#include "IValueComposite_Delegate.h"
#include "ivaluecomposite.h"

IValueComposite_Delegate::IValueComposite_Delegate(IValueComposite *valueComposite)
    : CStorageNode_Delegate(valueComposite), m_valueComposite(valueComposite) {}
