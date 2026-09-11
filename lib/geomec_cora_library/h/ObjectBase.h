#ifndef _cora_ObjectBase_h_
#define _cora_ObjectBase_h_

#include "safeQSharedPointer.h"

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4800)
#endif

class COpenGLNode;

namespace geo
{

class IElement;

} // namespace geo

#include "Parameter.h"
#include "ElementSet.h"

namespace cora
{

class CObjectBase
{
  public:
    CObjectBase();
    virtual ~CObjectBase() = 0;

    virtual const QString& name() const = 0;
    virtual const TParameters& getParameters() const = 0;
    virtual const QString& prefix() const = 0;
    virtual const COpenGLNode* object() const = 0;
    virtual const QString& typeName() const = 0;

    virtual const geo::IElement* getFirstElement() = 0;
    virtual const geo::IElement* getNextElement() = 0;

    virtual std::ostream& operator () (std::ostream& os) const = 0;

    virtual CElementSet* getElementSet() const;

  private:
    CObjectBase(const CObjectBase& rhs);
    CObjectBase& operator = (const CObjectBase& rhs);
};

typedef QSharedPointer <CObjectBase> TObjectBase;

} // namespace cora

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif  // _cora_ObjectBase_h_
