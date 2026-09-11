#ifndef _cora_ObjectHorizon_h_
#define _cora_ObjectHorizon_h_

class C3DHorizon;

#include "ObjectBase.h"

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4800)
#endif

namespace cora
{

class CObjectHorizon : public CObjectBase
{
  public:
  CObjectHorizon(CHorizonBase* horizonBase);
  virtual ~CObjectHorizon();

  virtual const QString& name() const;
  virtual const TParameters& getParameters() const;
  virtual const QString& prefix() const;
  virtual const COpenGLNode* object() const;
  virtual const QString& typeName() const;

  virtual const geo::IElement* getFirstElement();
  virtual const geo::IElement* getNextElement();

  virtual std::ostream& operator () (std::ostream& os) const;

  private:
  CObjectHorizon(const CObjectHorizon& rhs);
  CObjectHorizon& operator = (CObjectHorizon rhs);

  C3DHorizon* m_3DHorizon;
  TParameters m_parameters;
  int m_sequenceState;
};

} // namespace cora

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif  // _cora_ObjectHorizon_h_
