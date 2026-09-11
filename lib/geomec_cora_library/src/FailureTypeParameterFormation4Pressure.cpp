#include "FailureTypeParameterFormation4Pressure.h"
#include "FormationBase.h"
#include "Pressure.h"

namespace cora {

CFailureTypeParameterFormation4Pressure::CFailureTypeParameterFormation4Pressure(
    CSummaryResultFile &summaryResultFile, const std::string &object, const std::string &option,
    const std::string &parameter, std::vector<double> &value, const CGetModelInfo &modelInfo)
    : CFailureTypeParameterFormation(summaryResultFile, object, option, parameter, value, modelInfo) {}

CFailureTypeParameterFormation4Pressure::~CFailureTypeParameterFormation4Pressure() {}

void CFailureTypeParameterFormation4Pressure::modify(CModelBase *modelBase) {
  if (m_actualParameter) {
    CElementSet *elementSet = m_actualObject->getElementSet();
    int set = elementSet->AddElementValueSet();
    CElementValueSet &elementValueSet = elementSet->ElementValueSet(set);
    CFormationBase *formationBase = dynamic_cast<CFormationBase *>(m_actualParameter->object());
    CPressure &pressure =
        formationBase->Pressure(modelBase->DepletionStageEntry().StageByIndex(m_actualParameter->depletionStage()));

    for (int s = 0; s < formationBase->ElementSetSize(); ++s) {
      for (int e = 0; e < formationBase->ElementSet(s).ElementSet().ElementSize(); ++e) {
        const geo::IElement &element = formationBase->ElementSet(s).ElementSet().Element(e);
        IValueDomainScalar::TValueVec newValues(element.NrOfNodes());
        IValueDomainScalar::TValueVec pressureValues = pressure.Component().ScalarData().ValueElement(element);

        for (size_t n = 0; n < newValues.size(); ++n) {
          newValues[n] = m_parameterModifier->modify(pressureValues[n]).Value();
        }

        elementValueSet.PushBack(newValues);
      }
    }

    unsigned int valueTypeID = m_actualParameter->valueTypeID();

    createAndLinkValueType(modelBase, elementValueSet, pressure, *elementSet, valueTypeID);
  }
}

} // namespace cora
