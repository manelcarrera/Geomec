#include "stdafx.h"

#include "LasPointSet_Delegate.h"
#include "LasPointSet.h"

CLasPointSet_Delegate::CLasPointSet_Delegate(CLasPointSet* lasPointSet)
: CPointSet_Delegate(lasPointSet)
, m_lasPointSet(lasPointSet)
{
}
