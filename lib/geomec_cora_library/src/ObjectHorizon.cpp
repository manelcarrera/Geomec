#include "ObjectHorizon.h"
#include "3DHorizon.h"

namespace cora
{

CObjectHorizon::CObjectHorizon(CHorizonBase* horizonBase)
: CObjectBase()
, m_3DHorizon(dynamic_cast <C3DHorizon*> (horizonBase))
, m_parameters()
, m_sequenceState(0)
{
}

CObjectHorizon::~CObjectHorizon()
{
}

const QString& CObjectHorizon::name() const
{
  return m_3DHorizon->Name();
}

const TParameters& CObjectHorizon::getParameters() const
{
  return m_parameters;
}

namespace
{

// TODO SURFACE-HORIZON
// replace "surface:" with "horizon:"

const QString PREFIX = "surface:";

} // anonymous namespace

const QString& CObjectHorizon::prefix() const
{
  return PREFIX;
}

const COpenGLNode* CObjectHorizon::object() const
{
  return m_3DHorizon;
}

namespace
{

const QString TYPE_NAME = "Surface";

} // anonymous namespace

const QString& CObjectHorizon::typeName() const
{
  return TYPE_NAME;
}

const geo::IElement* CObjectHorizon::getFirstElement()
{
  m_sequenceState = 0;

  return getNextElement();
}

const geo::IElement* CObjectHorizon::getNextElement()
{
  if (m_sequenceState < m_3DHorizon->BodyFaceSize())
  {
  return &(m_3DHorizon->BodyFace(m_sequenceState++));
  }

  return 0;
}

std::ostream& CObjectHorizon::operator () (std::ostream& os) const
{
  os << PREFIX.toStdString() << m_3DHorizon->Name().toStdString() <<
  std::endl;

  return os;
}

} // namespace cora
