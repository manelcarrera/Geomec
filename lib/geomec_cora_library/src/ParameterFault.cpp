#include "ParameterFault.h"

#ifdef _WIN32
#define MAKESTRING2(str) #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable : 4800)
#endif

#include "HorizonBase.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace cora {

CParameterFault::CParameterFault(const QString &name, const geo::CValue &minimum, const geo::CValue &maximum,
                                 double mean, unsigned int valueTypeID, int depletionStage, CHorizonBase *horizonBase)
    : CParameterLimit(), m_name(name), m_minimum(minimum), m_maximum(maximum), m_mean(mean), m_valueTypeID(valueTypeID),
      m_depletionStage(depletionStage), m_horizonBase(horizonBase) {}

CParameterFault::~CParameterFault() {}

int CParameterFault::depletionStage() const { return m_depletionStage; }

unsigned int CParameterFault::valueTypeID() const { return m_valueTypeID; }

const QString &CParameterFault::name() const { return m_name; }

CGraphNode *CParameterFault::object() const { return m_horizonBase; }

double CParameterFault::min() const {
  if (m_minimum.Valid()) {
    return m_minimum.Value();
  } else {
    return NAN_DOUBLE;
  }
}

double CParameterFault::max() const {
  if (m_maximum.Valid()) {
    return m_maximum.Value();
  } else {
    return NAN_DOUBLE;
  }
}

double CParameterFault::mean() const { return m_mean; }

std::ostream &CParameterFault::operator()(std::ostream &os) const {
  os << "parameter:" << m_name.toStdString() << "," << m_minimum.Representation() << "," << m_maximum.Representation()
     << "," << m_mean << std::endl;

  return os;
}

} // namespace cora
