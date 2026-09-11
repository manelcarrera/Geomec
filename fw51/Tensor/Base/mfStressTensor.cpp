 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// StressTensor.cpp: implementation of the CStressTensor class.
//
//////////////////////////////////////////////////////////////////////
#include <cmath>
#include "dimple.h"
#include "IObject.h"
#include "Matrix.h"
#include "mfStressTensor.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStressTensor::CStressTensor()
{

}

CStressTensor::~CStressTensor()
{

}

CStressTensor::CStressTensor(const ITensor& tensor)
: CMechanicalTensor(tensor)
{
}

CStressTensor::CStressTensor(const double &xx, const double &yy, const double &zz,
							 const double &xy, const double &yz, const double &xz)
:CMechanicalTensor(xx, yy, zz, xy, yz, xz)
{
}

double CStressTensor::Tresca() const
{
	geo::CMatrix Values = GetPrincipalStr();
	return fabs((Values.Value(0, 0) - Values.Value(2, 2)) / 2);
}

double CStressTensor::MeanStress() const
{
	geo::CMatrix Values = GetPrincipalStr();
	return (Values.Value(0, 0) + Values.Value(2, 2)) / 2;
}

double CStressTensor::InvarVonMis() const
{
	const CMechanicalTensor *pDev = DeviatoricTensor();
	double J2acc = pDev->InvarI2();
	double sqrd = -3.0 * J2acc; //olivier

	if(fabs(sqrd) < EPS)
		sqrd = 0.0;

	delete pDev;
	return sqrt(sqrd);
}

double CStressTensor::AverageStress() const
{
	return (1.0 / 3.0) * InvarI1();
}

const CMechanicalTensor *CStressTensor::DeviatoricTensor() const
{
	CStressTensor *pDev = new CStressTensor(*this);
	double dPres = (pDev->XX() + pDev->YY() + pDev->ZZ()) / 3;
	pDev->XX(pDev->XX() - dPres);
	pDev->YY(pDev->YY() - dPres);
	pDev->ZZ(pDev->ZZ() - dPres);

	return pDev;
}

