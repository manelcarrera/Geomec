 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_STRESSTENSOR_H__INCLUDED_)
#define AFX_STRESSTENSOR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mfMechanicalTensor.h"

#include "TensorExports.h"

class TENSOR_EXPORT CStressTensor : public CMechanicalTensor 
{
public:
  CStressTensor();
  CStressTensor(const ITensor& tensor);
  CStressTensor(const double &xx, const double &yy, const double &zz,
                  const double &xy, const double &yz, const double &xz);
  virtual ~CStressTensor();

  double Tresca() const;
  double MeanStress() const;
  virtual double InvarVonMis() const;
  double AverageStress() const;

  virtual const CMechanicalTensor *DeviatoricTensor() const;
};

#endif // !defined(AFX_STRESSTENSOR_H__INCLUDED_)
