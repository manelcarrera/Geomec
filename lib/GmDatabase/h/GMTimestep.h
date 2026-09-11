// GMTimestep.h: interface for the CTimestep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMTIMESTEP_H__FD48DECB_667D_4EF9_8E00_D09D3DD77E62__INCLUDED_)
#define AFX_GMTIMESTEP_H__FD48DECB_667D_4EF9_8E00_D09D3DD77E62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

#include "GM3CheckedValue.h"

namespace gm { 

class CTimestep  
{
public:
	enum eIterationScheme { CONSTANT = 0, LINEAR, REGULAR, MODIFIED };
	enum eConvergenceType { FORCE = 0, DISPLACEMENT, ENERGY };
protected:
	eIterationScheme m_iteration_scheme;
	eConvergenceType m_convergence_type;
	double m_dConvNormVal;
	int m_nLoadSteps;
	int m_nMaxIterStep;
	TCheckedInt m_nYear;
	QString m_strName;
public:
	CTimestep();
	CTimestep(const QString& strName);
	CTimestep(const QString& strName, const int nYear);

	const QString& Name() const;
	const TCheckedInt& Year() const;
	eIterationScheme IterationScheme() const;
	eConvergenceType ConvergenceType() const;
	double ConvergenceNormValue() const;
	int LoadSteps() const;
	int MaxIterationSteps() const;
};

}

#endif // !defined(AFX_GMTIMESTEP_H__FD48DECB_667D_4EF9_8E00_D09D3DD77E62__INCLUDED_)
