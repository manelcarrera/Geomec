#include "stdafx.h"

#include "StaborResult.h"
#include "StaborResult_Delegate.h"

CStaborResult_Delegate::CStaborResult_Delegate(CStaborResult *staborResult)
    : IResult_Delegate(staborResult), m_staborResult(staborResult) {}
