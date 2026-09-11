#include "Object.h"
#include "ObjectFault.h"
#include "ObjectFormation.h"
#include "ObjectSurface.h"
#include "ObjectNonMeshedSurface.h"
#include "ObjectHorizon.h"
#include "ObjectWell.h"

namespace cora
{

CObject::CObject(const TFaultObject&, CModelBase* modelBase,
  CHorizonBase* horizonBase)
: m_objectBase(TObjectBase(new CObjectFault(modelBase, horizonBase)))
, m_objectType(FAULT)
{
}

CObject::CObject(const TFormationObject&, CFormationBase* formationBase,
  CModelBase* modelBase)
: m_objectBase(TObjectBase(new CObjectFormation(formationBase, modelBase)))
, m_objectType(FORMATION)
{
}

CObject::CObject(const TSurfaceObject&, CSurfaceBase* surfaceBase)
: m_objectBase(TObjectBase(new CObjectSurface(surfaceBase)))
, m_objectType(SURFACE)
{
}

CObject::CObject(const TNonMeshedSurfaceObject&, CModelBase* modelBase,
  CNonMeshedSurface* nonMeshedSurface)
: m_objectBase(TObjectBase(
  new CObjectNonMeshedSurface(modelBase, nonMeshedSurface)))
, m_objectType(NON_MESHED_SURFACE)
{
}

CObject::CObject(const THorizonObject&, CHorizonBase* horizonBase)
: m_objectBase(TObjectBase(new CObjectHorizon(horizonBase)))
, m_objectType(HORIZON)
{
}

CObject::CObject(const TWellObject&, CNewWellPath* newWellPath)
: m_objectBase(TObjectBase(new CObjectWell(newWellPath)))
, m_objectType(WELL)
{
}

const QString& CObject::name() const
{
  return m_objectBase->name();
}

const TParameters& CObject::getParameters() const
{
  return m_objectBase->getParameters();
}

const QString& CObject::prefix() const
{
  return m_objectBase->prefix();
}

const COpenGLNode* CObject::object() const
{
  return m_objectBase->object();
}

const QString& CObject::typeName() const
{
  return m_objectBase->typeName();
}

const geo::IElement* CObject::getFirstElement()
{
  return m_objectBase->getFirstElement();
}

const geo::IElement* CObject::getNextElement()
{
  return m_objectBase->getNextElement();
}

std::ostream& CObject::operator () (std::ostream& os) const
{
  return m_objectBase->operator () (os);
}

CElementSet* CObject::getElementSet() const
{
  return m_objectBase->getElementSet();
}

CObject::TObjectType CObject::getObjectType() const
{
  return m_objectType;
}

// static

CObject::TFaultObject CObject::faultObject;
CObject::TFormationObject CObject::formationObject;
CObject::TSurfaceObject CObject::surfaceObject;
CObject::TNonMeshedSurfaceObject CObject::nonMeshedSurfaceObject;
CObject::THorizonObject CObject::horizonObject;
CObject::TWellObject CObject::wellObject;

} // namespace cora

// global

std::ostream& operator << (std::ostream& os, const cora::CObject& rhs)
{
  return rhs(os);
}

std::ostream& operator << (std::ostream& os, const cora::TObject& rhs)
{
  os << *rhs;

  return os;
}

std::ostream& operator << (std::ostream& os, const cora::TObjects& rhs)
{
  os << rhs.size() << std::endl;

  for (cora::TObjects::const_iterator object = rhs.begin(); object != rhs.end();
  ++object)
  {
  os << *object;
  }

  return os;
}
