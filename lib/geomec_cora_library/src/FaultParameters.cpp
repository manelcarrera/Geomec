
#include "FaultParameters.h"
#include "DepletionStage.h"

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4800)
#endif

#include "ModelBase.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

#include "BaseEntryTypes.h"
#include "BranchState.h"
#include "HorizonBase.h"
#include "ValueTypes.h"
#include "resourceIDS.h"
#include "GetValueTypeInfo.h"
#include "ObjectParametersNodeValue.h"
#include "FaultParametersNode.h"
#include "FaultPressure.h"
#include "getPressureParameters.h"

namespace cora
{

CFaultParameters::CFaultParameters()
{
}

// static

TParameters CFaultParameters::getParameters(CModelBase* modelBase,
  CHorizonBase* horizonBase)
{
  const CDepletionStageEntry* depletionStages =
    dynamic_cast <const CDepletionStageEntry*> (
      modelBase->GraphEntry(MD_BASE_DEPLETION_STAGE));
  TParameters faultParameters;

  for (CDepletionStageEntry::const_iterator
    depletionStage = depletionStages->begin();
    depletionStage != depletionStages->LastStage(); ++depletionStage)
  {
    if ((*depletionStage).IsPhaseStartStage())
    {
      getParameters(faultParameters, modelBase, *depletionStage, horizonBase);
    }
  }

  return faultParameters;
}

// private

// static

void CFaultParameters::getParameters(TParameters& faultParameters,
  CModelBase* modelBase, const CDepletionStage& depletionStage,
  CHorizonBase* horizonBase)
{
  // See CFaultParametersNode::CanConnectItem(...)!

  if (!modelBase->BranchState().IsFixedStage(depletionStage) &&
    horizonBase->hasFaultParameters(depletionStage))
  {
    getParameters(faultParameters, modelBase, depletionStage, horizonBase,
      IDT_VALUETYPE_COHESION, IDS_VALUENAME_COHESION);
    getParameters(faultParameters, modelBase, depletionStage, horizonBase,
      IDT_VALUETYPE_FRICTION_ANGLE, IDS_VALUENAME_FRICTION_ANGLE);
  }

  if (horizonBase->Slip())
  {
    getFaultParameters(faultParameters, modelBase, depletionStage, horizonBase,
      IDT_VALUETYPE_PRESSURE, IDS_VALUENAME_FAULT_PRESSURE);
  }
}

void CFaultParameters::getParameters(TParameters& faultParameters,
  CModelBase* modelBase, const CDepletionStage& depletionStage,
  CHorizonBase* horizonBase, unsigned int valueTypeID,
  unsigned int /*valueTypeName*/)
{
  const CFaultParametersNode& faultParametersNode =
    horizonBase->FaultParameters(depletionStage);
  CGetValueTypeInfo& getValueTypeInfo(CGetValueTypeInfo::instance(modelBase));
  CObjectParametersNodeValue <geo::CElementGroup, CFaultParametersNode>
    faultParametersNodeValue(horizonBase->InterfaceElementGroup(), valueTypeID,
      faultParametersNode);
  std::pair <geo::CValue, geo::CValue> range =
    faultParametersNodeValue.getRange();
  double mean = faultParametersNodeValue.getMean();

  faultParameters.push_back(TParameter(new CParameter(
    CParameter::faultParameter, QString("%1_D%2").
      arg(getValueTypeInfo.getImportTag(valueTypeID)).
      arg(depletionStage.Index()),
    range.first, range.second, mean, horizonBase, depletionStage.Index(),
    valueTypeID)));
}

namespace
{

const QString PRESSURE_TYPE = "a fault pressure";

} // anonymous namespace

void CFaultParameters::getFaultParameters(TParameters& faultParameters,
  CModelBase* modelBase, const CDepletionStage& depletionStage,
  CHorizonBase* horizonBase, unsigned int valueTypeID,
  unsigned int valueTypeName)
{
  getPressureParameters <CHorizonBase, geo::IElementSet,
    CParameter::TFaultParameter, CFaultPressure> (faultParameters,
      modelBase, depletionStage, horizonBase,
      *(horizonBase->InterfaceElementGroup()),
      CParameter::faultParameter, valueTypeID,
      valueTypeName, PRESSURE_TYPE);
}

} // namespace cora
