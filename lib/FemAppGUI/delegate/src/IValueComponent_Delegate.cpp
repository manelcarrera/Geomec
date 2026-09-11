#include "stdafx.h"

#include "IValueComponent_Delegate.h"
#include "ivaluecomponent.h"

IValueComponent_Delegate::IValueComponent_Delegate(
  IValueComponentBase* valueComponent)
: CStorageNode_Delegate(valueComponent)
, m_valueComponent(valueComponent)
{
}
