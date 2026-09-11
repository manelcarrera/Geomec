#include "FailureTypeParameterFault.h"
#include "GetModelInfo.h"

namespace cora {

namespace {

const QString FAULT = "fault";

} // anonymous namespace

CFailureTypeParameterFault::CFailureTypeParameterFault(CSummaryResultFile &summaryResultFile, const std::string &object,
                                                       const std::string &option, const std::string &parameter,
                                                       std::vector<double> &value, const CGetModelInfo &modelInfo)
    : CFailureTypeParameterBase(summaryResultFile, object, option, parameter, value,
                                modelInfo.getFaultInfo().getObjects(), FAULT),
      m_createElementValueSet(selectElementValueSetCreator(m_actualParameter)) {}

CFailureTypeParameterFault::~CFailureTypeParameterFault() {}

void CFailureTypeParameterFault::modify(CModelBase *modelBase) {
  if (m_actualParameter) {
    m_createElementValueSet->createElementValueSet(modelBase, this);
  }
}

// private

// static

TCreateElementValueSet CFailureTypeParameterFault::selectElementValueSetCreator(TParameter m_actualParameter) {
  TCreateElementValueSet elementValueSetCreator;

  if (m_actualParameter != 0) {
    switch (m_actualParameter->valueTypeID()) {
    case IDT_VALUETYPE_COHESION:
    case IDT_VALUETYPE_FRICTION_ANGLE:
      elementValueSetCreator =
          TCreateElementValueSet(new CCreateElementValueSet(CCreateElementValueSet::mapSingleValue2ElementValues));
      break;
    case IDT_VALUETYPE_PRESSURE:
      elementValueSetCreator =
          TCreateElementValueSet(new CCreateElementValueSet(CCreateElementValueSet::mapInterfaceElement2ElementValues));
      break;
    default:
      assert(false);
      break;
    }
  }

  return elementValueSetCreator;
}

} // namespace cora
