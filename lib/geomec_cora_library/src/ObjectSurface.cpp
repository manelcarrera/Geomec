#include "ObjectSurface.h"
#include "GeoSurface.h"

namespace cora {

CObjectSurface::CObjectSurface(CSurfaceBase *surfaceBase)
    : CObjectBase(), m_surfaceBase(surfaceBase), m_parameters(), m_sequenceState(0) {}

CObjectSurface::~CObjectSurface() {}

const QString &CObjectSurface::name() const { return m_surfaceBase->Name(); }

const TParameters &CObjectSurface::getParameters() const { return m_parameters; }

namespace {

const QString PREFIX = "surface:";

} // anonymous namespace

const QString &CObjectSurface::prefix() const { return PREFIX; }

const COpenGLNode *CObjectSurface::object() const { return m_surfaceBase; }

namespace {

const QString TYPE_NAME = "Surface";

} // anonymous namespace

const QString &CObjectSurface::typeName() const { return TYPE_NAME; }

const geo::IElement *CObjectSurface::getFirstElement() {
  m_sequenceState = 0;

  return getNextElement();
}

const geo::IElement *CObjectSurface::getNextElement() {
  if (m_sequenceState < m_surfaceBase->ElementSet().ElementSize()) {
    // m_surfaceBase->ElementSet().Element(...),
    // m_surfaceBase->Surface().Element(...) and
    // m_surfaceBase->Surface().Face(...) return the same element.

    return &(m_surfaceBase->ElementSet().Element(m_sequenceState++));
  }

  return 0;
}

std::ostream &CObjectSurface::operator()(std::ostream &os) const {
  os << PREFIX.toStdString() << m_surfaceBase->Name().toStdString() << std::endl;

  return os;
}

} // namespace cora
