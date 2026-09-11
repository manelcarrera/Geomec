 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_MECHANICALTENSOR_H__INCLUDED_)
#define AFX_MECHANICALTENSOR_H__INCLUDED_

#include "mfTensor.h"

#include "TensorExports.h"

class TENSOR_EXPORT CMechanicalTensor : public CTensor
{
public:
	CMechanicalTensor();
	CMechanicalTensor(const double &xx, const double &yy, const double &zz,
					  const double &xy, const double &yz, const double &xz);
	CMechanicalTensor(const ITensor &rhs);
	virtual ~CMechanicalTensor();

	double PrincMaxNorth() const;
	double PrincMaxEast() const;
	double PrincMaxDepth() const;
	double PrincMaxLength() const;

	double PrincIntermNorth() const;
	double PrincIntermEast() const;
	double PrincIntermDepth() const;
	double PrincIntermLength() const;

	double PrincMinNorth() const;
	double PrincMinEast() const;
	double PrincMinDepth() const;
	double PrincMinLength() const;

	double InvarI1() const;
	double InvarI2() const;
	double InvarI3() const;
	virtual double InvarVonMis() const = 0;

	// deviatoric tensor, must be deleted by user
	virtual const CMechanicalTensor *DeviatoricTensor() const = 0;

	double AzimuthMaxHorizontal() const;
	double MaxHorValue() const;
	double MinHorValue() const;


};

#endif // !defined(AFX_MECHANICALTENSOR_H__INCLUDED_)
