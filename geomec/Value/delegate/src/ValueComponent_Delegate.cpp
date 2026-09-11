#include "stdafx.h"

#include "ValueComponent_Delegate.h"

CValueComponent_Delegate::CValueComponent_Delegate(
  CValueComponent* valueComponent)
: IValueComponent_Delegate(valueComponent)
, m_valueComponent(valueComponent)
{
}

bool CValueComponent_Delegate::Attributes()
{
  return false;
}
