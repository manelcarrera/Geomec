#include "ParameterFormation.h"

#ifdef _WIN32
#define MAKESTRING2(str) #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable : 4800)
#endif

#include "FormationBase.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace cora {

CParameterFormation::CParameterFormation(const QString &name, const geo::CValue &minimum, const geo::CValue &maximum,
                                         double mean, CFormationBase *formationBase, int depletionStage)
    : CParameterLimit(), m_name(name), m_minimum(minimum), m_maximum(maximum), m_mean(mean),
      m_formationBase(formationBase), m_depletionStage(depletionStage) {}

CParameterFormation::~CParameterFormation() {}

int CParameterFormation::depletionStage() const { return m_depletionStage; }

const QString &CParameterFormation::name() const { return m_name; }

CGraphNode *CParameterFormation::object() const { return m_formationBase; }

double CParameterFormation::min() const {
  if (m_minimum.Valid()) {
    return m_minimum.Value();
  } else {
    return NAN_DOUBLE;
  }
}

double CParameterFormation::max() const {
  if (m_maximum.Valid()) {
    return m_maximum.Value();
  } else {
    return NAN_DOUBLE;
  }
}

double CParameterFormation::mean() const { return m_mean; }

std::ostream &CParameterFormation::operator()(std::ostream &os) const {
  os << "parameter:" << m_name.toStdString() << "," << m_minimum.Representation() << "," << m_maximum.Representation()
     << "," << m_mean << std::endl;

  return os;
}

} // namespace cora
