#include "stdafx.h"

#include "LasPointSet.h"
#include "LasPointSet_Delegate.h"

CLasPointSet_Delegate::CLasPointSet_Delegate(CLasPointSet *lasPointSet)
    : CPointSet_Delegate(lasPointSet), m_lasPointSet(lasPointSet) {}
