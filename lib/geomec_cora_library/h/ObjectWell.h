#ifndef _cora_ObjectWell_h_
#define _cora_ObjectWell_h_

#include "ObjectBase.h"
#include "NewWellPath.h"

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4800)
#endif

namespace cora
{

class CObjectWell : public CObjectBase
{
  public:
    CObjectWell(CNewWellPath* newWellPath);
    virtual ~CObjectWell();

    virtual const QString& name() const;
    virtual const TParameters& getParameters() const;
    virtual const QString& prefix() const;
    virtual const COpenGLNode* object() const;
    virtual const QString& typeName() const;

    virtual const geo::IElement* getFirstElement();
    virtual const geo::IElement* getNextElement();

    virtual std::ostream& operator () (std::ostream& os) const;

  private:
    CObjectWell(const CObjectWell& rhs);
    CObjectWell& operator = (CObjectWell rhs);

    CNewWellPath* m_newWellPath;
    TParameters m_parameters;
    int m_sequenceState;
};

} // namespace cora

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif  // _cora_ObjectWell_h_
