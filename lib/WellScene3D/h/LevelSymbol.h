// WellLevelDrawSpec.h: interface for the CWellLevelDrawSpec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LEVEL_SYMBOL_INCLUDE)
#define LEVEL_SYMBOL_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellSymbolBase.h"

class QString;

class CLevelSymbol: public CWellSymbolBase
{
public:
	CLevelSymbol(const geo::CPoint& level,const QString& description,bool autoDelete =true);
};



#endif // !defined(LEVEL_SYMBOL_INCLUDE)
