#include "FailureTypeParameterFormation4PressureChange.h"
#include "FormationBase.h"
#include "Pressure.h"

namespace cora
{

CFailureTypeParameterFormation4PressureChange::
  CFailureTypeParameterFormation4PressureChange(
  CSummaryResultFile& summaryResultFile, const std::string& object,
  const std::string& option, const std::string& parameter,
  std::vector <double>& value, const CGetModelInfo& modelInfo)
: CFailureTypeParameterFormation(summaryResultFile, object, option, parameter,
  value, modelInfo)
{
}

CFailureTypeParameterFormation4PressureChange::
  ~CFailureTypeParameterFormation4PressureChange()
{
}

void CFailureTypeParameterFormation4PressureChange::modify(
  CModelBase* modelBase)
{
  if (m_actualParameter)
  {
  CElementSet* elementSet = m_actualObject->getElementSet();
  int set = elementSet->AddElementValueSet();
  CElementValueSet& elementValueSet = elementSet->ElementValueSet(set);
  CFormationBase* formationBase = dynamic_cast <CFormationBase*> (
      m_actualParameter->object());
  const CPressure& initialPressure =
      formationBase->Pressure(modelBase->InitialDepletionStage());
  CPressure& pressure = formationBase->Pressure(modelBase->
      DepletionStageEntry().StageByIndex(m_actualParameter->depletionStage()));

  for (int s = 0; s < formationBase->ElementSetSize(); ++s)
  {
      for (int e = 0;
    e < formationBase->ElementSet(s).ElementSet().ElementSize(); ++e)
      {
    const geo::IElement& element =
          formationBase->ElementSet(s).ElementSet().Element(e);
    IValueDomainScalar::TValueVec newValues(element.NrOfNodes());
    const IValueDomainScalar::TValueVec initialPressureValues =
          initialPressure.Component().ScalarData().ValueElement(element);
    IValueDomainScalar::TValueVec pressureValues =
          pressure.Component().ScalarData().ValueElement(element);

    for (size_t n = 0; n < newValues.size(); ++n)
    {
          newValues[n] = m_parameterModifier->
      modify(pressureValues[n] - initialPressureValues[n]).Value();
    }

    elementValueSet.PushBack(newValues);
      }
  }

  unsigned int valueTypeID = m_actualParameter->valueTypeID();

  createAndLinkValueType(modelBase, elementValueSet,
      pressure, *elementSet, valueTypeID);
  }
}

} // namespace cora
