/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// MF.cpp: implementation of the MF class.
//
//////////////////////////////////////////////////////////////////////
#include "MF.h"
#include "IFace.h"
#include "IInterfaceElement.h"
#include "dimple.h"
#include "mfStressTensor.h"
#include <cassert>
#include <cmath>

#define EPS_PORE 1E-2
#define NrE 2.71828182845904523536028747135266
#define MECFUNCEPS 10E-6

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MF::MF() {}

// The Gammas are calculated using the total stress
// GAMMAS ***************************************************************************************
geo::CValue MF::GammaVertical(const CStressTensor &init_stress, const CStressTensor &current_stress,
                              const double &press_init, const double &press_current, STRESS_TYPE stress_type) {
  if (fabs(press_current - press_init) < EPS_PORE)
    return geo::CValue();

  switch (stress_type) {
  case TOTAL:
    return (current_stress.ZZ() - init_stress.ZZ()) / (press_current - press_init);
  case EFFECTIVE: {
    CStressTensor init_total = TotalStress(init_stress, press_init);
    CStressTensor current_total = TotalStress(current_stress, press_current);
    return (current_total.ZZ() - init_total.ZZ()) / (press_current - press_init);
  }
  default:
    assert(false);
  }

  return geo::CValue();
}

geo::CValue MF::GammaMax(const CStressTensor &init_stress, const CStressTensor &current_stress,
                         const double &press_init, const double &press_current, STRESS_TYPE stress_type) {
  if (fabs(press_current - press_init) < EPS_PORE)
    return geo::CValue();

  switch (stress_type) {
  case TOTAL:
    return (current_stress.PrincMaxLength() - init_stress.PrincMaxLength()) / (press_current - press_init);
  case EFFECTIVE: {
    CStressTensor init_total = TotalStress(init_stress, press_init);
    CStressTensor current_total = TotalStress(current_stress, press_current);
    return (current_total.PrincMaxLength() - init_total.PrincMaxLength()) / (press_current - press_init);
  }
  default:
    assert(false);
  }

  return geo::CValue();
}

geo::CValue MF::GammaMed(const CStressTensor &init_stress, const CStressTensor &current_stress,
                         const double &press_init, const double &press_current, STRESS_TYPE stress_type) {
  if (fabs(press_current - press_init) < EPS_PORE)
    return geo::CValue();

  switch (stress_type) {
  case TOTAL:
    return (current_stress.PrincIntermLength() - init_stress.PrincIntermLength()) / (press_current - press_init);
  case EFFECTIVE: {
    CStressTensor init_total = TotalStress(init_stress, press_init);
    CStressTensor current_total = TotalStress(current_stress, press_current);
    return (current_total.PrincIntermLength() - init_total.PrincIntermLength()) / (press_current - press_init);
  }
  default:
    assert(false);
  }
  return geo::CValue();
}

geo::CValue MF::GammaMin(const CStressTensor &init_stress, const CStressTensor &current_stress,
                         const double &press_init, const double &press_current, STRESS_TYPE stress_type) {
  if (fabs(press_current - press_init) < EPS_PORE)
    return geo::CValue();

  switch (stress_type) {
  case TOTAL:
    return (current_stress.PrincMinLength() - init_stress.PrincMinLength()) / (press_current - press_init);
  case EFFECTIVE: {
    CStressTensor init_total = TotalStress(init_stress, press_init);
    CStressTensor current_total = TotalStress(current_stress, press_current);
    return (current_total.PrincMinLength() - init_total.PrincMinLength()) / (press_current - press_init);
  }
  default:
    assert(false);
  }

  return geo::CValue();
}

geo::CValue MF::GammaVolume(const CStressTensor &init_stress, const CStressTensor &current_stress,
                            const double &press_init, const double &press_current, STRESS_TYPE stress_type) {
  if (fabs(press_current - press_init) < EPS_PORE)
    return geo::CValue();

  switch (stress_type) {
  case TOTAL:
    return (current_stress.AverageStress() - init_stress.AverageStress()) / (press_current - press_init);
  case EFFECTIVE: {
    CStressTensor init_total = TotalStress(init_stress, press_init);
    CStressTensor current_total = TotalStress(current_stress, press_current);
    return (current_total.AverageStress() - init_total.AverageStress()) / (press_current - press_init);
  }
  default:
    assert(false);
  }

  return geo::CValue();
}
// **********************************************************************************************

// Effective/Total stress conversions ***********************************************************
CStressTensor MF::TotalStress(const CStressTensor &eff_stress, const double &pore_press) {
  CStressTensor total = eff_stress;
  total.XX(eff_stress.XX() + pore_press);
  total.YY(eff_stress.YY() + pore_press);
  total.ZZ(eff_stress.ZZ() + pore_press);

  return total;
}

CStressTensor MF::EffectiveStress(const CStressTensor &total_stress, const double &pore_press) {
  CStressTensor effective = total_stress;
  effective.XX(total_stress.XX() - pore_press);
  effective.YY(total_stress.YY() - pore_press);
  effective.ZZ(total_stress.ZZ() - pore_press);

  return effective;
}
// **********************************************************************************************

// Shear capacity *******************************************************************************
geo::CValue MF::ShearCapacity(const CStressTensor &effective_stress, const double &friction_angle,
                              const double &cohesion, ANGLE_TYPE angle_type) {
  double friction = friction_angle;
  double sigverteff, sig1eff, sig2eff;
  double p, J2, q, alpha, k, Qyield;

  switch (angle_type) {
  case DEG:
    friction *= DEG_TO_RAD;
  case RAD:
    break;
  default:
    assert(false);
  }

  sigverteff = effective_stress.ZZ();
  sig1eff = effective_stress.YY();
  sig2eff = effective_stress.XX();

  p = (sig1eff + sig2eff + sigverteff) / 3.0;
  CStressTensor tensor(effective_stress);
  tensor.XX(tensor.XX() - p);
  tensor.YY(tensor.YY() - p);
  tensor.ZZ(tensor.ZZ() - p);

  J2 = tensor.InvarI2();
  if (J2 > 0.) {
    J2 = 0.;
  }
  assert(J2 <= 0);
  q = sqrt(-3.0 * J2);
  alpha = (6.0 * sin(friction)) / (3.0 - sin(friction));
  k = (6.0 * cohesion * cos(friction)) / (3.0 - sin(friction));
  Qyield = alpha * p + k;

  if (fabs(Qyield) < EPS)
    return geo::CValue();

  return geo::CValue(q / Qyield);
}
// **********************************************************************************************

// Fault results ********************************************************************************
geo::CValue MF::FaultNormalStress(const geo::IInterfaceElement &fault_elm, const geo::IVector &fault_stress) {
  const geo::IFace *pFace = dynamic_cast<const geo::IFace *>(&fault_elm.Front());
  if (pFace)
    return geo::CValue(pFace->Normal().DotProduct(fault_stress));

  const geo::ILine *pLine = dynamic_cast<const geo::ILine *>(&fault_elm.Front());
  assert(pLine);
  return geo::CValue(pLine->Dir().CrossProduct(geo::CVector::Zaxis).UnitVector().DotProduct(fault_stress));
}

geo::CValue MF::FaultShearCapacity(const geo::IVector &shear_stress, const double &effective_normal_stress,
                                   const double &friction_angle, const double &cohesion, ANGLE_TYPE angle_type) {
  double friction = friction_angle;

  switch (angle_type) {
  case DEG:
    friction *= DEG_TO_RAD;
  case RAD:
    break;
  default:
    assert(false);
  }

  double denom = cohesion + (effective_normal_stress * tan(friction));

  if (denom < 0.0)
    return geo::CValue(-1.0);

  return geo::CValue(shear_stress.Length() / denom);
}
// **********************************************************************************************

geo::CMatrix MF::GenerateHardeningCurve(const double &Cohesion, const double &FrictionAngle,
                                        const double &DilatationAngle, const double &HardeningGradient,
                                        const double &CompFractEnergy, const double &YoungsModulus,
                                        HARD_TYPE HardeningType, ANGLE_TYPE angle_type) {
  double A;
  double B;

  double SinFricAngle = 0, CosFricAngle = 0, SinDilAngle = 0;

  switch (angle_type) {
  case DEG:
    SinFricAngle = sin(DEG_TO_RAD * FrictionAngle);
    CosFricAngle = cos(DEG_TO_RAD * FrictionAngle);
    SinDilAngle = sin(DEG_TO_RAD * DilatationAngle);
    break;
  case RAD:
    SinFricAngle = sin(FrictionAngle);
    CosFricAngle = cos(FrictionAngle);
    SinDilAngle = sin(DilatationAngle);
    break;
  default:
    assert(false);
  }

  A = (1.0 - SinFricAngle) / (2.0 * CosFricAngle);
  B = (2.0 * sqrt(1 / 3.0 * (1 + pow(SinDilAngle, 2)))) / (1.0 - SinDilAngle);

  geo::CMatrix HardeningCurve;

  switch (HardeningType) {
  case LINEAR_HARD: {
    double CKHardeningGradient = (A / B) * HardeningGradient;

    HardeningCurve.ReSize(2, 2);

    HardeningCurve.Value(0, 0, 0.0);
    HardeningCurve.Value(0, 1, Cohesion);
    HardeningCurve.Value(1, 0, 1.0);
    HardeningCurve.Value(1, 1, Cohesion + CKHardeningGradient);
  } break;
  case LINEAR_SOFT: {
    double UltEqPlStrain1 = 2.0 * CompFractEnergy * A * B / Cohesion;

    HardeningCurve.ReSize(3, 2);

    HardeningCurve.Value(0, 0, 0.0);
    HardeningCurve.Value(0, 1, Cohesion);
    HardeningCurve.Value(1, 0, UltEqPlStrain1);
    HardeningCurve.Value(1, 1, 0.0);
    HardeningCurve.Value(2, 0, 1000.0 * UltEqPlStrain1);
    HardeningCurve.Value(2, 1, 0.0);
  } break;
  case PARABOLIC_HARD: {
    double EqPlStrMaxCoh = (4.0 * B * Cohesion) / (3.0 * A * YoungsModulus);
    double UltEqPlStrain2 =
        std::max(1.75 * EqPlStrMaxCoh, ((A * B * 3.0 * CompFractEnergy) / (2.0 * Cohesion)) - EqPlStrMaxCoh / 6.0);
    double DeltaEqPlStr = UltEqPlStrain2 - EqPlStrMaxCoh;

    HardeningCurve.ReSize(19, 2);

    HardeningCurve.Value(0, 0, 0.0);
    HardeningCurve.Value(0, 1, 1 / 3.0 * Cohesion);
    HardeningCurve.Value(1, 0, 0.2 * EqPlStrMaxCoh);
    HardeningCurve.Value(1, 1, 0.573 * Cohesion);
    HardeningCurve.Value(2, 0, 0.4 * EqPlStrMaxCoh);
    HardeningCurve.Value(2, 1, 0.760 * Cohesion);
    HardeningCurve.Value(3, 0, 0.6 * EqPlStrMaxCoh);
    HardeningCurve.Value(3, 1, 0.893 * Cohesion);
    HardeningCurve.Value(4, 0, 0.7 * EqPlStrMaxCoh);
    HardeningCurve.Value(4, 1, 0.940 * Cohesion);
    HardeningCurve.Value(5, 0, 0.8 * EqPlStrMaxCoh);
    HardeningCurve.Value(5, 1, 0.973 * Cohesion);
    HardeningCurve.Value(6, 0, 0.9 * EqPlStrMaxCoh);
    HardeningCurve.Value(6, 1, 0.993 * Cohesion);
    HardeningCurve.Value(7, 0, EqPlStrMaxCoh);
    HardeningCurve.Value(7, 1, Cohesion);
    HardeningCurve.Value(8, 0, EqPlStrMaxCoh + 0.05 * DeltaEqPlStr);
    HardeningCurve.Value(8, 1, 0.998 * Cohesion);
    HardeningCurve.Value(9, 0, EqPlStrMaxCoh + 0.10 * DeltaEqPlStr);
    HardeningCurve.Value(9, 1, 0.9978 * Cohesion);
    HardeningCurve.Value(10, 0, EqPlStrMaxCoh + 0.15 * DeltaEqPlStr);
    HardeningCurve.Value(10, 1, 0.990 * Cohesion);
    HardeningCurve.Value(11, 0, EqPlStrMaxCoh + 0.20 * DeltaEqPlStr);
    HardeningCurve.Value(11, 1, 0.960 * Cohesion);
    HardeningCurve.Value(12, 0, EqPlStrMaxCoh + 0.30 * DeltaEqPlStr);
    HardeningCurve.Value(12, 1, 0.910 * Cohesion);
    HardeningCurve.Value(13, 0, EqPlStrMaxCoh + 0.40 * DeltaEqPlStr);
    HardeningCurve.Value(13, 1, 0.840 * Cohesion);
    HardeningCurve.Value(14, 0, EqPlStrMaxCoh + 0.50 * DeltaEqPlStr);
    HardeningCurve.Value(14, 1, 0.750 * Cohesion);
    HardeningCurve.Value(15, 0, EqPlStrMaxCoh + 0.60 * DeltaEqPlStr);
    HardeningCurve.Value(15, 1, 0.640 * Cohesion);
    HardeningCurve.Value(16, 0, EqPlStrMaxCoh + 0.80 * DeltaEqPlStr);
    HardeningCurve.Value(16, 1, 0.360 * Cohesion);
    HardeningCurve.Value(17, 0, UltEqPlStrain2);
    HardeningCurve.Value(17, 1, 0.0);
    HardeningCurve.Value(18, 0, 1000.0 * UltEqPlStrain2);
    HardeningCurve.Value(18, 1, 0.0);
  } break;
  case NONE:
  default:
    assert(false);
    break;
  }

  return HardeningCurve;
}

double MF::CalcCompFluidPressure(const double &Depth, const double &FluidStressGradient,
                                 const double &FluidCompressibility, const double &FluidPressure) {
  double press;
  assert(FluidCompressibility >= 0.0);

  if (FluidCompressibility > MECFUNCEPS) {
    double sgs;
    double mincompress = 0.75;
    sgs = (1 + FluidCompressibility * FluidPressure) * FluidStressGradient; // stress gradient at surface
    double comp_density = 1 - FluidCompressibility * sgs * Depth;
    if (comp_density > mincompress)
      press = FluidPressure - log(comp_density) / FluidCompressibility;
    else
      press = FluidPressure - log(mincompress) / FluidCompressibility +
              sgs / mincompress * (Depth - (1 - mincompress) / (sgs * FluidCompressibility));
  } else
    press = FluidPressure + FluidStressGradient * Depth;

  return press;
}

double MF::CalcFluidStressGradient(const double &Depth, const double &OverPressure, const double &FluidCompressibility,
                                   const double &FluidPressure) {
  double StressGradient;
  assert(FluidCompressibility >= 0.0);

  if (FluidCompressibility > MECFUNCEPS) {
    double dTempGr1;
    double dTempGr2;
    double dExponent = -1 * FluidCompressibility * (FluidPressure - OverPressure);
    dTempGr1 = (1 - pow(NrE, dExponent)) / (FluidCompressibility * Depth);

    double dTemp = 1 - FluidCompressibility * dTempGr1 * Depth;
    if (dTemp > 0.75)
      dTempGr2 = dTempGr1;
    else
      dTempGr2 =
          0.75 * ((FluidPressure - OverPressure) / Depth) + (0.25 + 0.75 * log(0.75)) / (FluidCompressibility * Depth);

    StressGradient = dTempGr2 / (1 + FluidCompressibility * OverPressure);
  } else
    StressGradient = (FluidPressure - OverPressure) / Depth;

  return StressGradient;
}
