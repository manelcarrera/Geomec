/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_MF__INCLUDED_)
#define AFX_MF__INCLUDED_

#include "Matrix.h"
#include "Value.h"

#include "TensorExports.h"

class CStressTensor;
namespace geo {
class IFace;
class IElement;
class IInterfaceElement;
} // namespace geo

class TENSOR_EXPORT MF {
public:
  enum STRESS_TYPE { TOTAL = 0, EFFECTIVE };
  enum ANGLE_TYPE { DEG = 0, RAD };
  enum HARD_TYPE { LINEAR_HARD = 0, LINEAR_SOFT, PARABOLIC_HARD, NONE };

  // Private constructor, class should function as a namespace.
  // Not allowed to instantiate an object of this kind.
private:
  MF();

public:
  static geo::CValue GammaVertical(const CStressTensor &init_stress, const CStressTensor &current_stress,
                                   const double &press_init, const double &press_current, STRESS_TYPE stress_type);
  static geo::CValue GammaMax(const CStressTensor &init_stress, const CStressTensor &current_stress,
                              const double &press_init, const double &press_current, STRESS_TYPE stress_type);
  static geo::CValue GammaMed(const CStressTensor &init_stress, const CStressTensor &current_stress,
                              const double &press_init, const double &press_current, STRESS_TYPE stress_type);
  static geo::CValue GammaMin(const CStressTensor &init_stress, const CStressTensor &current_stress,
                              const double &press_init, const double &press_current, STRESS_TYPE stress_type);
  static geo::CValue GammaVolume(const CStressTensor &init_stress, const CStressTensor &current_stress,
                                 const double &press_init, const double &press_current, STRESS_TYPE stress_type);

  static CStressTensor TotalStress(const CStressTensor &eff_stress, const double &pore_press);
  static CStressTensor EffectiveStress(const CStressTensor &total_stress, const double &pore_press);

  static geo::CValue ShearCapacity(const CStressTensor &effective_stress, const double &friction_angle,
                                   const double &cohesion, ANGLE_TYPE angle_type);

  static geo::CMatrix GenerateHardeningCurve(const double &Cohesion, const double &FrictionAngle,
                                             const double &DilatationAngle, const double &HardeningGradient,
                                             const double &CompFractEnergy, const double &YoungsModulus,
                                             HARD_TYPE HardeningType, ANGLE_TYPE angle_type);
  static double CalcCompFluidPressure(const double &DepthAnalysisPoint, const double &FluidStressGradient,
                                      const double &FluidCompressibility, const double &FluidPressure);
  static double CalcFluidStressGradient(const double &Depth, const double &OverPressure,
                                        const double &FluidCompressibility, const double &FluidPressure);

  static geo::CValue FaultNormalStress(const geo::IInterfaceElement &fault_elm, const geo::IVector &fault_stress);
  static geo::CValue FaultShearCapacity(const geo::IVector &shear_stress, const double &effective_normal_stress,
                                        const double &friction_angle, const double &cohesion, ANGLE_TYPE angle_type);
};

#endif // !defined(AFX_MF__INCLUDED_)
