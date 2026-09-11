#include "Parameter.h"
#include "ParameterFault.h"
#include "ParameterFormation4Material.h"
#include "ParameterFormation4Pressure.h"
#include "ParameterFormation4PressureChange.h"
#include "ParameterHorizon.h"
#include "ParameterNonMeshedSurface.h"
#include "ParameterSurface.h"
#include "ParameterWell.h"

namespace cora {

CParameter::CParameter(const TFaultParameter &, const QString &name, const geo::CValue &minimum,
                       const geo::CValue &maximum, double mean, CHorizonBase *horizonBase, int depletionStage,
                       unsigned int valueTypeID)
    : m_objectBase(
          TParameterBase(new CParameterFault(name, minimum, maximum, mean, valueTypeID, depletionStage, horizonBase))) {
}

CParameter::CParameter(const TFormationParameter4Material &, const QString &name, const geo::CValue &minimum,
                       const geo::CValue &maximum, double mean, CFormationBase *formationBase, int depletionStage,
                       CLibraryMaterialParameter &libraryMaterialParameter)
    : m_objectBase(TParameterBase(new CParameterFormation4Material(name, minimum, maximum, mean, formationBase,
                                                                   depletionStage, libraryMaterialParameter))) {}

CParameter::CParameter(const TFormationParameter4Pressure &, const QString &name, const geo::CValue &minimum,
                       const geo::CValue &maximum, double mean, CFormationBase *formationBase, int depletionStage,
                       unsigned int valueTypeID)
    : m_objectBase(TParameterBase(new CParameterFormation4Pressure(name, minimum, maximum, mean, formationBase,
                                                                   depletionStage, valueTypeID))) {}

CParameter::CParameter(const TFormationParameter4PressureChange &, const QString &name, const geo::CValue &minimum,
                       const geo::CValue &maximum, double mean, CFormationBase *formationBase, int depletionStage,
                       unsigned int valueTypeID)
    : m_objectBase(TParameterBase(new CParameterFormation4PressureChange(name, minimum, maximum, mean, formationBase,
                                                                         depletionStage, valueTypeID))) {}

CParameter::CParameter(const THorizonParameter &) : m_objectBase() {}

CParameter::CParameter(const TSurfaceParameter &) : m_objectBase() {}

CParameter::CParameter(const TNonMeshedSurfaceParameter &, const QString &name, const geo::CValue &minimum,
                       const geo::CValue &maximum, double mean, unsigned int valueTypeID, int depletionStage,
                       CSurfaceBase *surfaceBase)
    : m_objectBase(TParameterBase(
          new CParameterNonMeshedSurface(name, minimum, maximum, mean, valueTypeID, depletionStage, surfaceBase))) {}

CParameter::CParameter(const TWellParameter &) : m_objectBase() {}

int CParameter::depletionStage() const { return m_objectBase->depletionStage(); }

unsigned int CParameter::valueTypeID() const { return m_objectBase->valueTypeID(); }

const QString &CParameter::name() const { return m_objectBase->name(); }

CGraphNode *CParameter::object() const { return m_objectBase->object(); }

double CParameter::min() const { return m_objectBase->min(); }

double CParameter::max() const { return m_objectBase->max(); }

double CParameter::mean() const { return m_objectBase->mean(); }

const QString CParameter::lowerLimit() const { return m_objectBase->lowerLimit(); }

const QString CParameter::upperLimit() const { return m_objectBase->upperLimit(); }

std::ostream &CParameter::operator()(std::ostream &os) const { return m_objectBase->operator()(os); }

// static

CParameter::TFaultParameter CParameter::faultParameter;
CParameter::TFormationParameter4Material CParameter::formationParameter4Material;
CParameter::TFormationParameter4Pressure CParameter::formationParameter4Pressure;
CParameter::TFormationParameter4PressureChange CParameter::formationParameter4PressureChange;
CParameter::THorizonParameter CParameter::horizonParameter;
CParameter::TSurfaceParameter CParameter::surfaceParameter;
CParameter::TNonMeshedSurfaceParameter CParameter::nonMeshedSurfaceParameter;
CParameter::TWellParameter CParameter::wellParameter;

} // namespace cora

// global

std::ostream &operator<<(std::ostream &os, const cora::CParameter &rhs) { return rhs(os); }

std::ostream &operator<<(std::ostream &os, const cora::TParameter &rhs) {
  os << *rhs;

  return os;
}

std::ostream &operator<<(std::ostream &os, const cora::TParameters &rhs) {
  os << rhs.size() << std::endl;

  for (cora::TParameters::const_iterator object = rhs.begin(); object != rhs.end(); ++object) {
    os << *object;
  }

  return os;
}
