#include "ParameterNonMeshedSurface.h"

#ifdef _WIN32
#define MAKESTRING2(str) #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable : 4800)
#endif

#include "GeoSurface.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace cora {

CParameterNonMeshedSurface::CParameterNonMeshedSurface(const QString &name, const geo::CValue &minimum,
                                                       const geo::CValue &maximum, double mean,
                                                       unsigned int valueTypeID, int depletionStage,
                                                       CSurfaceBase *surfaceBase)
    : CParameterLimit(), m_name(name), m_minimum(minimum), m_maximum(maximum), m_mean(mean), m_valueTypeID(valueTypeID),
      m_depletionStage(depletionStage), m_surfaceBase(surfaceBase) {}

CParameterNonMeshedSurface::~CParameterNonMeshedSurface() {}

int CParameterNonMeshedSurface::depletionStage() const { return m_depletionStage; }

unsigned int CParameterNonMeshedSurface::valueTypeID() const { return m_valueTypeID; }

const QString &CParameterNonMeshedSurface::name() const { return m_name; }

CGraphNode *CParameterNonMeshedSurface::object() const { return m_surfaceBase; }

double CParameterNonMeshedSurface::min() const {
  if (m_minimum.Valid()) {
    return m_minimum.Value();
  } else {
    return NAN_DOUBLE;
  }
}

double CParameterNonMeshedSurface::max() const {
  if (m_maximum.Valid()) {
    return m_maximum.Value();
  } else {
    return NAN_DOUBLE;
  }
}

double CParameterNonMeshedSurface::mean() const { return m_mean; }

std::ostream &CParameterNonMeshedSurface::operator()(std::ostream &os) const {
  os << "parameter:" << m_name.toStdString() << "," << m_minimum.Representation() << "," << m_maximum.Representation()
     << "," << m_mean << std::endl;

  return os;
}

} // namespace cora
