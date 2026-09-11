#include "ObjectWell.h"

namespace cora {

CObjectWell::CObjectWell(CNewWellPath *newWellPath)
    : CObjectBase(), m_newWellPath(newWellPath), m_parameters(), m_sequenceState(0) {}

CObjectWell::~CObjectWell() {}

const QString &CObjectWell::name() const { return m_newWellPath->Name(); }

const TParameters &CObjectWell::getParameters() const { return m_parameters; }

namespace {

const QString PREFIX = "well:";

} // anonymous namespace

const QString &CObjectWell::prefix() const { return PREFIX; }

const COpenGLNode *CObjectWell::object() const { return m_newWellPath; }

namespace {

const QString TYPE_NAME = "Well";

} // anonymous namespace

const QString &CObjectWell::typeName() const { return TYPE_NAME; }

const geo::IElement *CObjectWell::getFirstElement() {
  m_sequenceState = 0;

  return getNextElement();
}

const geo::IElement *CObjectWell::getNextElement() {
  if (m_sequenceState < m_newWellPath->PolyLine().LineSize()) {
    return &(m_newWellPath->PolyLine().Line(m_sequenceState++));
  }

  return 0;
}

std::ostream &CObjectWell::operator()(std::ostream &os) const {
  os << PREFIX.toStdString() << m_newWellPath->Name().toStdString() << std::endl;

  return os;
}

} // namespace cora
