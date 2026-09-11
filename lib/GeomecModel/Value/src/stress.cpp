// stress.cpp: implementation of the stress class.
//
//////////////////////////////////////////////////////////////////////
#include "stress.h"
#include "resourceIDS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStressGradient::CStressGradient(const double &value, const UNIT unit)
: CPressureGradientQuantity(value, unit)
{
}

unsigned int CStressGradient::QuantityNameId() const
{
	return IDS_STRESS_GRAD_QUANTITY ;
}

CSingleStress::CSingleStress(const double &value, const UNIT unit)
: CSinglePressure(value, unit)
{
}

unsigned int CSingleStress::QuantityNameId() const
{
	return IDS_STRESS_QUANTITY;
}

