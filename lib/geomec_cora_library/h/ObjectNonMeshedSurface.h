#ifndef _cora_ObjectNonMeshedSurface_h_
#define _cora_ObjectNonMeshedSurface_h_

#include "ObjectBase.h"

#ifdef _WIN32
#define MAKESTRING2(str) #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable : 4800)
#endif

namespace cora {

class CObjectNonMeshedSurface : public CObjectBase {
public:
  CObjectNonMeshedSurface(CModelBase *modelBase, CNonMeshedSurface *nonMeshedSurface);
  virtual ~CObjectNonMeshedSurface();

  virtual const QString &name() const;
  virtual const TParameters &getParameters() const;
  virtual const QString &prefix() const;
  virtual const COpenGLNode *object() const;
  virtual const QString &typeName() const;

  virtual const geo::IElement *getFirstElement();
  virtual const geo::IElement *getNextElement();

  virtual std::ostream &operator()(std::ostream &os) const;

private:
  CObjectNonMeshedSurface(const CObjectNonMeshedSurface &rhs);
  CObjectNonMeshedSurface &operator=(CObjectNonMeshedSurface rhs);

  static TParameters getParameters(CModelBase *modelBase, CNonMeshedSurface *nonMeshedSurface);
  static void getParameters(TParameters &nonMeshedParameters, CModelBase *modelBase,
                            const CDepletionStage &depletionStage, CNonMeshedSurface *nonMeshedSurface);
  static void getParameters(TParameters &nonMeshedParameters, CModelBase *modelBase,
                            const CDepletionStage &depletionStage, CNonMeshedSurface *nonMeshedSurface,
                            unsigned int valueTypeID, unsigned int valueTypeName);

  CNonMeshedSurface *m_nonMeshedSurface;
  TParameters m_parameters;
  bool m_owner;
  int m_sequenceState;
};

} // namespace cora

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif // _cora_ObjectNonMeshedSurface_h_
