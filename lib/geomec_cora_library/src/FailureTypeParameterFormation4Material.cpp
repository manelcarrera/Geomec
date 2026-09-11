#include "FailureTypeParameterFormation4Material.h"
#include "FormationBase.h"

namespace cora
{

CFailureTypeParameterFormation4Material::CFailureTypeParameterFormation4Material(
  CSummaryResultFile& summaryResultFile, const std::string& object,
  const std::string& option, const std::string& parameter,
  std::vector <double>& value, const CGetModelInfo& modelInfo)
: CFailureTypeParameterFormation(summaryResultFile, object, option, parameter, value,
  modelInfo)
{
}

CFailureTypeParameterFormation4Material::~CFailureTypeParameterFormation4Material()
{
}

namespace
{

const QString VALUE_TYPE_NOT_PRESENT =
  "value type %1 not present in the material linked to object '%2:%3'";

} // anonymous namespace

void CFailureTypeParameterFormation4Material::modify(CModelBase* modelBase)
{
  if (m_actualParameter)
  {
  int set = modelBase->Mesh().AddElementValueSet();
  CElementValueSet& elementValueSet = modelBase->Mesh().ElementValueSet(set);
  unsigned int valueTypeID = m_actualParameter->valueTypeID();
  CFormationBase* formationBase = dynamic_cast <CFormationBase*> (
      m_actualParameter->object());

  for (int e = 0; e < modelBase->Mesh().Mesh().ElementSize(); ++e)
  {
      const geo::IElement& element = modelBase->Mesh().Mesh().Element(e);
      const CFFMaterial& cffMaterial = formationBase->Material(
    modelBase->DepletionStageEntry().StageByIndex(
          m_actualParameter->depletionStage())).Material(element);
      int nodes = element.NrOfNodes();
      std::vector <double> newValues(nodes);
      double newValue = 0;

      if (cffMaterial.IsParameter(valueTypeID))
      {
    double value =
          cffMaterial.ParameterValue(valueTypeID);

    newValue = m_parameterModifier->modify(value).Value();
      }
      else
      {
    m_summaryResultFile.addAdditionalInformation(
          QString(VALUE_TYPE_NOT_PRESENT).arg(valueTypeID).arg(FORMATION).
      arg(m_object));
      }

      std::fill(newValues.begin(), newValues.end(), newValue);
      elementValueSet.PushBack(newValues);
  }

  createAndLinkValueType(modelBase, elementValueSet,
      formationBase->ActiveMaterial(), modelBase->Mesh(), valueTypeID);
  }
}

} // namespace cora
