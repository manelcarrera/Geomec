#include "MapInterfaceElement2ElementValues.h"
#include "FailureTypeParameterBase.h"
#include "HorizonBase.h"
#include "ElementGroup.h"
#include "InterfaceElement.h"
#include "SummaryResultFile.h"
#include "FaultParametersNode.h"
#include "FaultPressure.h"

namespace cora
{

CMapInterfaceElement2ElementValues::CMapInterfaceElement2ElementValues()
: ICreateElementValueSet()
{
}

CMapInterfaceElement2ElementValues::~CMapInterfaceElement2ElementValues()
{
}

void CMapInterfaceElement2ElementValues::createElementValueSet(CModelBase* modelBase,
  const CFailureTypeParameterBase* failureTypeParameterBase)
{
  CElementSet* elementSet =
    failureTypeParameterBase->m_actualObject->getElementSet();
  int set = elementSet->AddElementValueSet();
  CElementValueSet& elementValueSet = elementSet->ElementValueSet(set);
  CHorizonBase* horizonBase = dynamic_cast <CHorizonBase*> (
    failureTypeParameterBase->m_actualParameter->object());
  const geo::CElementGroup* elementGroup =
    horizonBase->InterfaceElementGroup();
  CFaultPressure& faultPressure = horizonBase->Pressure(modelBase->
    DepletionStageEntry().StageByIndex(
      failureTypeParameterBase->m_actualParameter->depletionStage()));

  for (int element = 0; element < elementGroup->ElementSize(); ++element)
  {
    const geo::CInterfaceElement& interfaceElement =
      dynamic_cast <const geo::CInterfaceElement&> (
        elementGroup->Element(element));
    IValueDomainScalar::TValueVec newValues(interfaceElement.NrOfNodes() / 2);
    IValueDomainScalar::TValueVec pressureValues =
      faultPressure.Component().ScalarData().ValueElement(interfaceElement);

    for (int v = 0; v < newValues.size(); ++v)
    {
      int n = interfaceElement.BodyFaceNode(v);

      newValues[n] = failureTypeParameterBase->m_parameterModifier->
        modify(pressureValues[v]).Value();
    }

    elementValueSet.PushBack(newValues);
  }

  unsigned int valueTypeID =
    failureTypeParameterBase->m_actualParameter->valueTypeID();

  failureTypeParameterBase->createAndLinkValueType(modelBase, elementValueSet,
    faultPressure, *elementSet, valueTypeID);
}

} // namespace cora
