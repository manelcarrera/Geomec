#include "stdafx.h"

#include "ITensorGroup_Delegate.h"
#include "ITensorGroup.h"

ITensorGroup_Delegate::CComponentComposite_Delegate::
  CComponentComposite_Delegate(
  ITensorGroup::CComponentComposite* componentComposite)
: IResult_Delegate(componentComposite)
, m_componentComposite(componentComposite)
{
}

ITensorGroup_Delegate::CVectorComposite_Delegate::CVectorComposite_Delegate(
  ITensorGroup::CVectorComposite* vectorComposite)
: IVectorResult_Delegate(vectorComposite)
, m_vectorComposite(vectorComposite)
{
}

ITensorGroup_Delegate::ITensorGroup_Delegate(ITensorGroup* tensorGroup)
: CResultGroup_Delegate(tensorGroup)
, m_tensorGroup(tensorGroup)
{
}
