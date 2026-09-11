#include "stdafx.h"

#include "StaborResult_Delegate.h"
#include "StaborResult.h"

CStaborResult_Delegate::CStaborResult_Delegate(CStaborResult* staborResult)
: IResult_Delegate(staborResult)
, m_staborResult(staborResult)
{
}
