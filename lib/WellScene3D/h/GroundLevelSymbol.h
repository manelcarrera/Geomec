// GroundLevelSymbol.h: interface for the CGroundLevelSymbol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GROUNDLEVELSYMBOL_H__18F70E08_FF92_4A50_AC2C_6AF16C94A4FB__INCLUDED_)
#define AFX_GROUNDLEVELSYMBOL_H__18F70E08_FF92_4A50_AC2C_6AF16C94A4FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellDrawSpecBase.h"

class CGroundLevelSymbol : public CWellDrawSpecBase  
{
friend class CWellSceneInterMed;
public:
	
	virtual ~CGroundLevelSymbol();
	virtual void	CreateScene();

private:
	CGroundLevelSymbol(CWellSceneInterMed&);
};

#endif // !defined(AFX_GROUNDLEVELSYMBOL_H__18F70E08_FF92_4A50_AC2C_6AF16C94A4FB__INCLUDED_)
