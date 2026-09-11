#include "FailureMode.h"

namespace cora {

CFailureMode::CFailureMode(const QString &resultExportLabel, const IResultComponent *resultComponent)
    : m_resultExportLabel(resultExportLabel), m_resultComponent(resultComponent), m_object(), m_failureModeLabel() {}

const QString &CFailureMode::getResultExportLabel() const { return m_resultExportLabel; }

const IResultComponent *CFailureMode::getResultComponent() const { return m_resultComponent; }

void CFailureMode::setObject(const TObject &object) { m_object = object; }

void CFailureMode::setFailureModeLabel(const QString &failureModeLabel) { m_failureModeLabel = failureModeLabel; }

const TObject &CFailureMode::getObject() const { return m_object; }

const QString &CFailureMode::getFailureModeLabel() const { return m_failureModeLabel; }

int CFailureMode::getFailureModeType() const {
  const QString type = getFailureModeLabel();

  if (type.startsWith("FaultShearCapacity"))
    return 0;
  else if (type.startsWith("FaultDisplacementL"))
    return 1;
  else if (type.startsWith("ShearCap"))
    return 2;
  else if (type.startsWith("EPStress1L"))
    return 3;
  else if (type.startsWith("EPStress3L"))
    return 4;
  else if (type.startsWith("PStrain1L"))
    return 5;
  else if (type.startsWith("PStrain3L"))
    return 6;
  else if (type.startsWith("StrainInvVolumetric"))
    return 7;
  else if (type.startsWith("PStrain1PlasticL"))
    return 8;
  else if (type.startsWith("PStrain3PlasticL"))
    return 9;
  else if (type.startsWith("CompactionCapacity"))
    return 10;
  else if (type.startsWith("WPAxialStrain"))
    return 11;
  else if (type.startsWith("DisplacementV"))
    return 12;
  else
    return 13;
}

} // namespace cora
