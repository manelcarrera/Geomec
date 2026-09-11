/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// StrainTensor.cpp: implementation of the CStrainTensor class.
//
//////////////////////////////////////////////////////////////////////

#include "mfStrainTensor.h"
#include "IObject.h"
#include "Matrix.h"
#include "dimple.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrainTensor::CStrainTensor() {}

CStrainTensor::CStrainTensor(const ITensor &tensor) : CMechanicalTensor(tensor) {}

CStrainTensor::~CStrainTensor() {}

// Make sure that the components you supply here are the scientific strain components!!
CStrainTensor::CStrainTensor(const double &xx, const double &yy, const double &zz, const double &xy, const double &yz,
                             const double &xz)
    : CMechanicalTensor(xx, yy, zz, xy, yz, xz) {}

double CStrainTensor::EngineerXY() const { return 2.0 * XY(); }

double CStrainTensor::EngineerYZ() const { return 2.0 * YZ(); }

double CStrainTensor::EngineerZX() const { return 2.0 * XZ(); }

double CStrainTensor::InvarVonMis() const {
  const CMechanicalTensor *pDev = DeviatoricTensor();
  double J2acc = pDev->InvarI2();
  double sqrd = -(4.0 / 3.0) * J2acc; // olivier

  if (fabs(sqrd) < EPS * EPS)
    sqrd = 0.0;

  delete pDev;
  return sqrt(sqrd);
}

double CStrainTensor::VolumetricStrain() const { return InvarI1(); }

const CMechanicalTensor *CStrainTensor::DeviatoricTensor() const {
  CStrainTensor *pDev = new CStrainTensor(*this);
  double dPres = (pDev->XX() + pDev->YY() + pDev->ZZ()) / 3.0;
  pDev->XX(pDev->XX() - dPres);
  pDev->YY(pDev->YY() - dPres);
  pDev->ZZ(pDev->ZZ() - dPres);

  return pDev;
}
