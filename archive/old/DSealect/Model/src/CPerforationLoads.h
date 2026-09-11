// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPERFORATIONLOADS_3B67A5F90032_INCLUDED
#define _INC_CPERFORATIONLOADS_3B67A5F90032_INCLUDED



#include "IComponent.h"


//##ModelId=3C0DDDAA030E
class CPerforationLoads 
: public IComponent
{
private:
	
	//##ModelId=3CAAA65602F2
	CSinglePressure m_qCompletionPressureDiff;

public:
	//##ModelId=3C3040A7033A
	bool CheckComplete() const;

	//##ModelId=3C0DDDAA032D
	CPerforationLoads();

	//From: Completion&Testing
	//Label: completion pressure
	//Descr: pressure in casing during perforation (compared 
	//to pore pressure and checked against over/underbalanced 
	//declaration.
	//##ModelId=3C0DDDAA032E
	CSinglePressure* CompletionPressureDiff();
	

};

#endif /* _INC_CPERFORATIONLOADS_3B67A5F90032_INCLUDED */
