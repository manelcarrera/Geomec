#ifndef _cora_Object_h_
#define _cora_Object_h_

#ifdef _WIN32
#define MAKESTRING2(str) #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable : 4800)
#endif

class CModelBase;
class CNonMeshedSurface;
class CNewWellPath;
class COpenGLNode;

namespace geo {

class IElement;

} // namespace geo

class CElementSet;

#include "ObjectBase.h"

namespace cora {

class CObject {
public:
  enum TObjectType { FAULT = 1, FORMATION, SURFACE, NON_MESHED_SURFACE, HORIZON, WELL };

  static struct TFaultObject {
  } faultObject;
  static struct TFormationObject {
  } formationObject;
  static struct TSurfaceObject {
  } surfaceObject;
  static struct TNonMeshedSurfaceObject {
  } nonMeshedSurfaceObject;
  static struct THorizonObject {
  } horizonObject;
  static struct TWellObject {
  } wellObject;

  CObject(const TFaultObject &, CModelBase *modelBase, CHorizonBase *horizonBase);
  CObject(const TFormationObject &, CFormationBase *formationBase, CModelBase *modelBase);
  CObject(const TSurfaceObject &, CSurfaceBase *surfaceBase);
  CObject(const TNonMeshedSurfaceObject &, CModelBase *modelBase, CNonMeshedSurface *nonMeshedSurface);
  CObject(const THorizonObject &, CHorizonBase *horizonBase);
  CObject(const TWellObject &, CNewWellPath *newWellPath);

  const QString &name() const;
  const TParameters &getParameters() const;
  const QString &prefix() const;
  const COpenGLNode *object() const;
  const QString &typeName() const;

  const geo::IElement *getFirstElement();
  const geo::IElement *getNextElement();

  std::ostream &operator()(std::ostream &os) const;

  CElementSet *getElementSet() const;

  TObjectType getObjectType() const;

private:
  CObject(const CObject &rhs);
  CObject &operator=(CObject rhs);

  TObjectBase m_objectBase;
  TObjectType m_objectType;
};

typedef QSharedPointer<CObject> TObject;
typedef std::vector<TObject> TObjects;

} // namespace cora

std::ostream &operator<<(std::ostream &os, const cora::CObject &rhs);
std::ostream &operator<<(std::ostream &os, const cora::TObject &rhs);
std::ostream &operator<<(std::ostream &os, const cora::TObjects &rhs);

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif // _cora_Object_h_
