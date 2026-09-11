#include "MapSingleValue2ElementValues.h"
#include "FailureTypeParameterBase.h"
#include "HorizonBase.h"
#include "ElementGroup.h"
#include "InterfaceElement.h"
#include "ifaceelmmat.h"
#include "SummaryResultFile.h"
#include "FaultParametersNode.h"

namespace cora
{

CMapSingleValue2ElementValues::CMapSingleValue2ElementValues()
: ICreateElementValueSet()
{
}

CMapSingleValue2ElementValues::~CMapSingleValue2ElementValues()
{
}

namespace
{

const QString FAULT = "fault";
const QString VALUE_TYPE_NOT_PRESENT =
  "value type %1 not present in the material linked to object '%2:%3'";

} // anonymous namespace

void CMapSingleValue2ElementValues::createElementValueSet(CModelBase* modelBase,
  const CFailureTypeParameterBase* failureTypeParameterBase)
{
  CElementSet* elementSet =
    failureTypeParameterBase->m_actualObject->getElementSet();
  int set = elementSet->AddElementValueSet();
  CElementValueSet& elementValueSet = elementSet->ElementValueSet(set);
  unsigned int valueTypeID =
    failureTypeParameterBase->m_actualParameter->valueTypeID();
  CHorizonBase* horizonBase = dynamic_cast <CHorizonBase*> (
    failureTypeParameterBase->m_actualParameter->object());
  const geo::CElementGroup* elementGroup =
    horizonBase->InterfaceElementGroup();
  const CDepletionStage& depletionStage = modelBase->DepletionStageEntry().
    StageByIndex(failureTypeParameterBase->m_actualParameter->depletionStage());

  for (int element = 0; element < elementGroup->ElementSize(); ++element)
  {
    const geo::CInterfaceElement& interfaceElement =
      dynamic_cast <const geo::CInterfaceElement&> (
        elementGroup->Element(element));
    const CInterfaceElementMaterial& interfaceElementMaterial =
      dynamic_cast <const CInterfaceElementMaterial&> (
        horizonBase->InterfaceMaterial(interfaceElement, depletionStage));
    int nodes = interfaceElement.NrOfNodes();
    std::vector <double> newValues(nodes / 2);
    double newValue = 0;

    if (interfaceElementMaterial.ValidParameterValue(valueTypeID))
    {
      double value =
        interfaceElementMaterial.ParameterValue(valueTypeID);

      newValue =
        failureTypeParameterBase->m_parameterModifier->modify(value).Value();
    }
    else
    {
      failureTypeParameterBase->m_summaryResultFile.addAdditionalInformation(
        QString(VALUE_TYPE_NOT_PRESENT).arg("").arg(FAULT).
        arg(failureTypeParameterBase->m_object));
    }

    std::fill(newValues.begin(), newValues.end(), newValue);
    elementValueSet.PushBack(newValues);
  }

  failureTypeParameterBase->createAndLinkValueType(modelBase, elementValueSet,
    horizonBase->FaultParameters(depletionStage), *elementSet, valueTypeID);
}

} // namespace cora
