 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_STRAINTENSOR_H__INCLUDED_)
#define AFX_STRAINTENSOR_H__INCLUDED_

#include "mfMechanicalTensor.h"

#include "TensorExports.h"

class TENSOR_EXPORT CStrainTensor : public CMechanicalTensor
{
public:
	CStrainTensor();
	CStrainTensor(const ITensor& tensor);
	CStrainTensor(const double &xx, const double &yy, const double &zz,
                  const double &xy, const double &yz, const double &xz);
	virtual ~CStrainTensor();


	double EngineerXY() const;
	double EngineerYZ() const;
	double EngineerZX() const;
	virtual double InvarVonMis() const;
	double VolumetricStrain() const;
	virtual const CMechanicalTensor *DeviatoricTensor() const;
};

#endif // !defined(AFX_STRAINTENSOR_H__INCLUDED_)
