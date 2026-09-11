// DCasingGeneralParameters.cpp: implementation of the CDCasingGeneralParameters class.
//
//////////////////////////////////////////////////////////////////////

#include <cfloat>

#include "DCasinggeneralParameters.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// defaults
CLengthQuantity CDCasingGeneralParameters::m_stOuterDiameter(0.1778, 0, DBL_MAX, CDoubleQuantity::SI_UNIT);
CLengthQuantity CDCasingGeneralParameters::m_stWallThickness(0.00805, 0, DBL_MAX, CDoubleQuantity::SI_UNIT);
CLengthQuantity CDCasingGeneralParameters::m_stDriftingDiameter(0.15, 0, DBL_MAX, CDoubleQuantity::SI_UNIT);
CLengthQuantity CDCasingGeneralParameters::m_stRequiredDrifting(0.12, 0, DBL_MAX, CDoubleQuantity::SI_UNIT);
CNoneQuantity CDCasingGeneralParameters::m_stPipeFrictionCoeff(0.1);
CSinglePressure CDCasingGeneralParameters::m_stCementYoungsModulus(20000, 0, DBL_MAX, CDoubleQuantity::SI_UNIT);
CNoneQuantity CDCasingGeneralParameters::m_stCementPoissonRatio(0.2, 0, 0.5, false, false,
                                                                CDoubleQuantity::SI_UNIT); // wjrx mantis 2327
CLengthQuantity CDCasingGeneralParameters::m_stReferenceDepth(0, -DBL_MAX, DBL_MAX, CDoubleQuantity::SI_UNIT);
CSinglePressure CDCasingGeneralParameters::m_stReferencePressure(0, -DBL_MAX, DBL_MAX, CDoubleQuantity::SI_UNIT);
CSinglePressure CDCasingGeneralParameters::m_stPressureGradient(0.016, 0, DBL_MAX, CDoubleQuantity::SI_UNIT);
// CLengthQuantity CDCasingGeneralParameters::m_stPipeLength(1.1778,1,);

CNoneQuantity CDCasingGeneralParameters::m_stPipeLengthOD(10, 1, DBL_MAX, CDoubleQuantity::SI_UNIT);
CLengthQuantity CDCasingGeneralParameters::m_stJointSpacing(10.16, 0, DBL_MAX, CDoubleQuantity::SI_UNIT);
CNoneQuantity CDCasingGeneralParameters::m_stModelOutDiamBoreDiamRatio(1000, 1, DBL_MAX, CDoubleQuantity::SI_UNIT);
CNoneQuantity CDCasingGeneralParameters::m_stMaxAxialCompStrain(0.15, 0, DBL_MAX, CDoubleQuantity::SI_UNIT);
unsigned long CDCasingGeneralParameters::m_stNumberOfLoadSteps(100);
QString CDCasingGeneralParameters::m_stModelLateralBoundary("FIXED");
bool CDCasingGeneralParameters::m_stNoFormationCementOnly(false);
CNoneQuantity CDCasingGeneralParameters::m_stSteelYieldStrain(0.00329);
CNoneQuantity CDCasingGeneralParameters::m_stSteelPoissonsRatio(0.3);
CNoneQuantity CDCasingGeneralParameters::m_stLinearCoeffStressStrain(1.0);
CNoneQuantity CDCasingGeneralParameters::m_stPowerCoeffStressStrain(.35);
CNoneQuantity CDCasingGeneralParameters::m_stHardeningCoeffStressStrain(.06);
CSinglePressure CDCasingGeneralParameters::m_stSteelPipeYoungsModulus(210000);
QString CDCasingGeneralParameters::m_stDeformationTheory("DEFOR");
unsigned long CDCasingGeneralParameters::m_stMaximumWaveNumber(2);
unsigned long CDCasingGeneralParameters::m_stMaxAxialHalfWaves(200);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCasingGeneralParameters::CDCasingGeneralParameters()
    : m_OuterDiameter(m_stOuterDiameter), m_WallThickness(m_stWallThickness), m_DriftingDiameter(m_stDriftingDiameter),
      m_RequiredDrifting(m_stRequiredDrifting), m_PipeFrictionCoeff(m_stPipeFrictionCoeff),
      m_CementYoungsModulus(m_stCementYoungsModulus), m_CementPoissonRatio(m_stCementPoissonRatio),
      m_ReferenceDepth(m_stReferenceDepth), m_ReferencePressure(m_stReferencePressure),
      m_PressureGradient(m_stPressureGradient), m_PipeLengthOD(m_stPipeLengthOD), m_JointSpacing(m_stJointSpacing),
      m_ModelOutDiamBoreDiamRatio(m_stModelOutDiamBoreDiamRatio), m_MaxAxialCompStrain(m_stMaxAxialCompStrain),
      m_NumberOfLoadSteps(m_stNumberOfLoadSteps), m_ModelLateralBoundary(m_stModelLateralBoundary),
      m_NoFormationCementOnly(m_stNoFormationCementOnly), m_SteelYieldStrain(m_stSteelYieldStrain),
      m_SteelPoissonsRatio(m_stSteelPoissonsRatio), m_LinearCoeffStressStrain(m_stLinearCoeffStressStrain),
      m_PowerCoeffStressStrain(m_stPowerCoeffStressStrain),
      m_HardeningCoeffStressStrain(m_stHardeningCoeffStressStrain),
      m_SteelPipeYoungsModulus(m_stSteelPipeYoungsModulus), m_DeformationTheory(m_stDeformationTheory),
      m_MaximumWaveNumber(m_stMaximumWaveNumber), m_MaxAxialHalfWaves(m_stMaxAxialHalfWaves) {
  QObject::connect(
      &m_OuterDiameter,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_WallThickness,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_DriftingDiameter,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_RequiredDrifting,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_CementYoungsModulus,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_ReferenceDepth,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_ReferencePressure,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_PressureGradient,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_PipeLengthOD,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_JointSpacing,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_SteelYieldStrain,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_SteelPoissonsRatio,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_LinearCoeffStressStrain,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_PowerCoeffStressStrain,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_HardeningCoeffStressStrain,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
  QObject::connect(
      &m_SteelPipeYoungsModulus,
      SIGNAL(OnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)), this,
      SLOT(slotOnValidate(const double &, bool &, CDoubleQuantity::UNIT, const CDoubleQuantity &, QString &)));
}

//////////////////////////////////////////////////////////////////////

CDCasingGeneralParameters::~CDCasingGeneralParameters() {}

//////////////////////////////////////////////////////////////////////

unsigned long CDCasingGeneralParameters::NumberOfLoadSteps() { return m_NumberOfLoadSteps; }

//////////////////////////////////////////////////////////////////////

void CDCasingGeneralParameters::NumberOfLoadSteps(unsigned long Value) {
  if (Value <= 1000)
    m_NumberOfLoadSteps = Value;
}

//////////////////////////////////////////////////////////////////////

QString CDCasingGeneralParameters::ModelLateralBoundary() { return m_ModelLateralBoundary; }

//////////////////////////////////////////////////////////////////////

void CDCasingGeneralParameters::ModelLateralBoundary(const QString &Value) {
  if (Value == "FIXED" || Value == "FREE")
    m_ModelLateralBoundary = Value;
}
//////////////////////////////////////////////////////////////////////

bool CDCasingGeneralParameters::NoFormationCementOnly() { return m_NoFormationCementOnly; }
//////////////////////////////////////////////////////////////////////

void CDCasingGeneralParameters::NoFormationCementOnly(bool Value) { m_NoFormationCementOnly = Value; }

//////////////////////////////////////////////////////////////////////

QString CDCasingGeneralParameters::DeformationTheory() { return m_DeformationTheory; }

//////////////////////////////////////////////////////////////////////

void CDCasingGeneralParameters::DeformationTheory(const QString &Value) {
  if (Value == "FLOW" || Value == "DEFOR")
    m_DeformationTheory = Value;
}

//////////////////////////////////////////////////////////////////////

unsigned long CDCasingGeneralParameters::MaximumWaveNumber() { return m_MaximumWaveNumber; }

//////////////////////////////////////////////////////////////////////

void CDCasingGeneralParameters::MaximumWaveNumber(unsigned long Value) { m_MaximumWaveNumber = Value; }

//////////////////////////////////////////////////////////////////////

unsigned long CDCasingGeneralParameters::MaxAxialHalfWaves() { return m_MaxAxialHalfWaves; }

//////////////////////////////////////////////////////////////////////

void CDCasingGeneralParameters::MaxAxialHalfWaves(unsigned long Value) { m_MaxAxialHalfWaves = Value; }

//////////////////////////////////////////////////////////////////////

void CDCasingGeneralParameters::slotOnValidate(const double &new_value, bool &accept, CDoubleQuantity::UNIT unit,
                                               const CDoubleQuantity &Quantity, QString &Description) {
  if (&m_OuterDiameter == &Quantity) {
    // check the diameter
    if (new_value < 0) {
      Description = "(Outer diameter) Invalid value : value < 0";
      accept = false;
      return;
    }

    // compare the radius with the wallthickness
    if (!m_WallThickness.Undefined()) {
      if (new_value / 2 < m_WallThickness.Value(unit)) {
        Description = "(Outer diameter) Invalid value : Radius is smaller then wallthickness";
        accept = false;
        return;
      }
    }

    accept = true;
    return;
  }

  if (&m_WallThickness == &Quantity) {
    if (new_value < 0) {
      Description = "(Wall thickness) Invalid value : value < 0";
      accept = false;
      return;
    }

    // compare the radius with the wallthickness
    if (!m_OuterDiameter.Undefined()) {
      if (new_value > m_OuterDiameter.Value(unit) / 2) {
        Description = "(Wall thickness) Invalid value : Radius is smaller then wallthickness";
        accept = false;
        return;
      }
    }

    accept = true;
    return;
  }

  if (&m_DriftingDiameter == &Quantity) {
    // minumimum
    if (!m_RequiredDrifting.Undefined()) {
      if (new_value < m_RequiredDrifting.Value(unit)) {
        Description = "(Nominal drifting Diameter) Invalid value : value < Minimum required drifting diameter";

        accept = false;
        return;
      }
    }

    // maximum
    // compare the radius with the wallthickness
    if (!m_OuterDiameter.Undefined() && !m_WallThickness.Undefined()) {
      if (new_value > m_OuterDiameter.Value(unit) - 2 * m_WallThickness.Value(unit)) {
        Description = "(Drifting Diameter) Invalid value : Diameter is larger then Outerdiameter - 2xWall thickness";
        accept = false;
        return;
      }
    }

    accept = true;
    return;
  }

  if (&m_RequiredDrifting == &Quantity) {
    // minumimum
    if (!m_DriftingDiameter.Undefined()) {
      // set the new limits
      if (new_value > m_DriftingDiameter.Value(unit)) {
        Description = "(Nominal drifting Diameter) Invalid value : value < Minimum required drifting diameter";
        accept = false;
        return;
      }
    }

    // maximum
    // compare the radius with the wallthickness
    if (!m_OuterDiameter.Undefined() && !m_WallThickness.Undefined()) {
      if (new_value > m_OuterDiameter.Value(unit) - 2 * m_WallThickness.Value(unit)) {
        Description = "(Drifting Diameter) Invalid value : Diameter is larger then Outerdiameter - 2xWall thickness";
        accept = false;
        return;
      }
    }

    accept = true;
    return;
  }

  if (&m_JointSpacing == &Quantity) {
    // minumimum
    if (!m_OuterDiameter.Undefined() && !m_PipeLengthOD.Undefined()) {
      // set the new limits
      if (new_value < (m_OuterDiameter.Value(unit) * m_PipeLengthOD.Value())) {
        Description =
            "(Pipe Joint length) Invalid value : value < Outerdiameter x modelled over outer borehole diameter ratio";
        accept = false;
        return;
      }
    }

    accept = true;
    return;
  }

  accept = true;
}

// see if all parameters are filled in
// return list of undefined parameters
bool CDCasingGeneralParameters::CheckParams(QStringList &strCheck) const {
  bool ret = true;
  if (m_OuterDiameter.Undefined()) {
    strCheck.append("Outer diameter not defined");
    ret = false;
  }

  if (m_WallThickness.Undefined()) {
    strCheck.append("Wall thickness not defined");
    ret = false;
  }

  if (m_DriftingDiameter.Undefined()) {
    strCheck.append("Drifting diameter not defined");
    ret = false;
  }

  if (m_RequiredDrifting.Undefined()) {
    strCheck.append("required drifting not defined");
    ret = false;
  }

  if (m_PipeFrictionCoeff.Undefined()) {
    strCheck.append("Pipe friction coefficient not defined");
    ret = false;
  }

  if (m_CementYoungsModulus.Undefined()) {
    strCheck.append("Cement Youngs modulus not defined");
    ret = false;
  }

  if (m_CementPoissonRatio.Undefined()) {
    strCheck.append("Poisson ratio not defined");
    ret = false;
  }

  if (m_ReferenceDepth.Undefined()) {
    strCheck.append("Reference depth not defined");
    ret = false;
  }

  if (m_ReferencePressure.Undefined()) {
    strCheck.append("Reference pressure not defined");
    ret = false;
  }

  if (m_PressureGradient.Undefined()) {
    strCheck.append("Reference pressure gradient not defined");
    ret = false;
  }

  if (m_PipeLengthOD.Undefined()) {
    strCheck.append("Pipe length OD not defined");
    ret = false;
  }

  if (m_JointSpacing.Undefined()) {
    strCheck.append("Joint spacing not defined");
    ret = false;
  }

  if (m_ModelOutDiamBoreDiamRatio.Undefined()) {
    strCheck.append("Model outer diamater ratio not defined");
    ret = false;
  }

  if (m_MaxAxialCompStrain.Undefined()) {
    strCheck.append("Max. axial compaction strain not defined");
    ret = false;
  }

  if (m_NumberOfLoadSteps <= 0) {
    strCheck.append("No loadsteps defined");
    ret = false;
  }

  if (m_SteelYieldStrain.Undefined()) {
    strCheck.append("Steel yield strain not defined");
    ret = false;
  }

  if (m_SteelPoissonsRatio.Undefined()) {
    strCheck.append("Steel poisson ratio not defined");
    ret = false;
  }

  if (m_LinearCoeffStressStrain.Undefined()) {
    strCheck.append("Linear coefficient stress strain not defined");
    ret = false;
  }

  if (m_HardeningCoeffStressStrain.Undefined()) {
    strCheck.append("Hardening coefficient stress strain not defined");
    ret = false;
  }

  if (m_SteelPipeYoungsModulus.Undefined()) {
    strCheck.append("Steel pipe youngs modulus not defined");
    ret = false;
  }

  return ret;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
