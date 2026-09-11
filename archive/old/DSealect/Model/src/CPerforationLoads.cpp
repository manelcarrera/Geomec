// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CPerforationLoads.h"

//##ModelId=3C0DDDAA032D
CPerforationLoads::CPerforationLoads()
:
m_qCompletionPressureDiff(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qCompletionPressureDiff.Invalidate();
}

//From: Completion&Testing
//Label: completion pressure
//Descr: pressure in casing during perforation (compared 
//to pore pressure and checked against over/underbalanced 
//declaration.
//##ModelId=3C0DDDAA032E
CSinglePressure *CPerforationLoads::CompletionPressureDiff()
{
	return &m_qCompletionPressureDiff;
}

//##ModelId=3C3040A7033A
bool CPerforationLoads::CheckComplete() const
{
	if (m_qCompletionPressureDiff.Undefined())
	{
		return false;
	}
		
	return true;
}
