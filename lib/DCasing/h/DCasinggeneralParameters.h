// DCasinggeneralParameters.h: interface for the CDCasingGeneralParameters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCASINGGENERALPARAMETERS_H__A76A319F_70B0_42F6_B0AC_F2BE48E18BEB__INCLUDED_)
#define AFX_DCASINGGENERALPARAMETERS_H__A76A319F_70B0_42F6_B0AC_F2BE48E18BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuantityInclude.h"
#include <qobject.h>
#include <qstringlist.h>

#include "DCasingDefines.h"

class CDCasingGeneralParameters : public QObject {

  Q_OBJECT;

public:
  // construction/destruction
  CDCasingGeneralParameters();
  virtual ~CDCasingGeneralParameters();

  // interface methods
  inline CLengthQuantity &OuterDiameter() { return m_OuterDiameter; }
  inline CLengthQuantity &WallThickness() { return m_WallThickness; }
  inline CLengthQuantity &DriftingDiameter() { return m_DriftingDiameter; }
  inline CLengthQuantity &RequiredDrifting() { return m_RequiredDrifting; }
  inline CSinglePressure &CementYoungsModulus() { return m_CementYoungsModulus; }
  inline CNoneQuantity &CementPoissonRatio() { return m_CementPoissonRatio; }
  inline CLengthQuantity &ReferenceDepth() { return m_ReferenceDepth; }
  inline CSinglePressure &ReferencePressure() { return m_ReferencePressure; }
  inline CSinglePressure &PressureGradient() { return m_PressureGradient; }

  // advanced parameters
  inline CNoneQuantity &PipeLengthOD() { return m_PipeLengthOD; }
  inline CLengthQuantity &JointSpacing() { return m_JointSpacing; }
  inline CNoneQuantity &ModelOutDiamBoreDiamRatio() { return m_ModelOutDiamBoreDiamRatio; }
  inline CNoneQuantity &MaxAxialCompStrain() { return m_MaxAxialCompStrain; }

  // Material parameters
  inline CNoneQuantity &SteelYieldStrain() { return m_SteelYieldStrain; }
  inline CNoneQuantity &SteelPoissonsRatio() { return m_SteelPoissonsRatio; }
  inline CNoneQuantity &LinearCoeffStressStrain() { return m_LinearCoeffStressStrain; }
  inline CNoneQuantity &PowerCoeffStressStrain() { return m_PowerCoeffStressStrain; }
  inline CNoneQuantity &HardeningCoeffStressStrain() { return m_HardeningCoeffStressStrain; }
  inline CSinglePressure &SteelPipeYoungsModulus() { return m_SteelPipeYoungsModulus; }
  inline CNoneQuantity &PipeFrictionCoeff() { return m_PipeFrictionCoeff; }

  unsigned long NumberOfLoadSteps();
  void NumberOfLoadSteps(unsigned long Value);

  QString ModelLateralBoundary();
  void ModelLateralBoundary(const QString &Value);

  bool NoFormationCementOnly();
  void NoFormationCementOnly(bool Value);

  QString DeformationTheory();
  void DeformationTheory(const QString &Value);

  unsigned long MaximumWaveNumber();
  void MaximumWaveNumber(unsigned long Value);

  unsigned long MaxAxialHalfWaves();
  void MaxAxialHalfWaves(unsigned long Value);

  // see if all parameters are filled in
  // return list of undefined parameters
  bool CheckParams(QStringList &strCheck) const;

private slots:

  void slotOnValidate(const double &new_value, bool &accept, CDoubleQuantity::UNIT unit,
                      const CDoubleQuantity &Quantity, QString &Description);

private:
  // general parameters
  CLengthQuantity m_OuterDiameter;
  CLengthQuantity m_WallThickness;
  CLengthQuantity m_DriftingDiameter;
  CLengthQuantity m_RequiredDrifting;
  CNoneQuantity m_PipeFrictionCoeff;
  CSinglePressure m_CementYoungsModulus;
  CNoneQuantity m_CementPoissonRatio;
  CLengthQuantity m_ReferenceDepth;
  CSinglePressure m_ReferencePressure;
  CSinglePressure m_PressureGradient;

  // advanced parameters
  CNoneQuantity m_PipeLengthOD;
  CLengthQuantity m_JointSpacing;
  CNoneQuantity m_ModelOutDiamBoreDiamRatio;
  CNoneQuantity m_MaxAxialCompStrain;
  unsigned long m_NumberOfLoadSteps;
  QString m_ModelLateralBoundary;
  bool m_NoFormationCementOnly;

  // material
  CNoneQuantity m_SteelYieldStrain;
  CNoneQuantity m_SteelPoissonsRatio;
  CNoneQuantity m_LinearCoeffStressStrain;
  CNoneQuantity m_PowerCoeffStressStrain;
  CNoneQuantity m_HardeningCoeffStressStrain;
  CSinglePressure m_SteelPipeYoungsModulus;

  // defaults
  QString m_DeformationTheory;
  unsigned long m_MaximumWaveNumber;
  unsigned long m_MaxAxialHalfWaves;

  // defaults general parameters
  static CLengthQuantity m_stOuterDiameter;
  static CLengthQuantity m_stWallThickness;
  static CLengthQuantity m_stDriftingDiameter;
  static CLengthQuantity m_stRequiredDrifting;
  static CNoneQuantity m_stPipeFrictionCoeff;
  static CSinglePressure m_stCementYoungsModulus;
  static CNoneQuantity m_stCementPoissonRatio;
  static CLengthQuantity m_stReferenceDepth;
  static CSinglePressure m_stReferencePressure;
  static CSinglePressure m_stPressureGradient;

  // deafults advanced parameters
  static CNoneQuantity m_stPipeLengthOD;
  static CLengthQuantity m_stJointSpacing;
  static CNoneQuantity m_stModelOutDiamBoreDiamRatio;
  static CNoneQuantity m_stMaxAxialCompStrain;
  static unsigned long m_stNumberOfLoadSteps;
  static QString m_stModelLateralBoundary;
  static bool m_stNoFormationCementOnly;

  // material
  static CNoneQuantity m_stSteelYieldStrain;
  static CNoneQuantity m_stSteelPoissonsRatio;
  static CNoneQuantity m_stLinearCoeffStressStrain;
  static CNoneQuantity m_stPowerCoeffStressStrain;
  static CNoneQuantity m_stHardeningCoeffStressStrain;
  static CSinglePressure m_stSteelPipeYoungsModulus;

  static QString m_stDeformationTheory;
  static unsigned long m_stMaximumWaveNumber;
  static unsigned long m_stMaxAxialHalfWaves;
};

#endif // !defined(AFX_DCASINGGENERALPARAMETERS_H__A76A319F_70B0_42F6_B0AC_F2BE48E18BEB__INCLUDED_)
