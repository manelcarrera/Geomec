#include "stdafx.h"

#include "IVectorResult_Delegate.h"
#include "IVectorResult.h"

IVectorResult_Delegate::CVectorComponent_Delegate::CVectorComponent_Delegate(
  IVectorResult::CVectorComponent* vectorComponent)
: IResultComponent_Delegate(vectorComponent)
, m_vectorComponent(vectorComponent)
{
}

IVectorResult_Delegate::IVectorResult_Delegate(IVectorResult* vectorResult)
: IResult_Delegate(vectorResult)
, m_vectorResult(vectorResult)
{
}

CVectorResult_Delegate::CVectorResult_Delegate(CVectorResult* vectorResult)
: IVectorResult_Delegate(vectorResult)
, m_vectorResult(vectorResult)
{
}
