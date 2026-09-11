// Formation.h: interface for the CFormation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATION_H__CBD5AC84_79C0_475C_9866_70492864643E__INCLUDED_)
#define AFX_FORMATION_H__CBD5AC84_79C0_475C_9866_70492864643E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GM3Pressure.h"
#include "GMFormation.h"

namespace gm 
{
class C3DDatabase;
class C3DModel;
class CHorizon;
class C3DFormation : public CFormation
{
	typedef std::vector<C3DPressure> TPressureVec;
	TPressureVec	m_vcPressure;
	long			m_lElement;
public:
	// Formation
	C3DFormation();
	virtual ~C3DFormation();

	int Element() const;
	virtual const CPressure& Pressure(const int nDepletionStage) const;

	void ReadFormation(C3DDatabase &db, 
					   const C3DModel& model,
					   const long lFormationKey,
					   IProgressBase &callback);
};

}

#endif // !defined(AFX_FORMATION_H__CBD5AC84_79C0_475C_9866_70492864643E__INCLUDED_)
