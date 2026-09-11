#ifndef _cora_ObjectFault_h_
#define _cora_ObjectFault_h_

class CHorizonBase;
class CModelBase;
class CDepletionStage;

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

class CObjectFault : public CObjectBase
{
  public:
    CObjectFault(CModelBase* modelBase, CHorizonBase* horizonBase);
    virtual ~CObjectFault();

    virtual const QString& name() const;
    virtual const TParameters& getParameters() const;
    virtual const QString& prefix() const;
    virtual const COpenGLNode* object() const;
    virtual const QString& typeName() const;

    virtual const geo::IElement* getFirstElement();
    virtual const geo::IElement* getNextElement();

    virtual std::ostream& operator () (std::ostream& os) const;

    virtual CElementSet* getElementSet() const;

  private:
    CObjectFault(const CObjectFault& rhs);
    CObjectFault& operator = (CObjectFault rhs);

    static TParameters getParameters(CModelBase* modelBase,
      CHorizonBase* horizonBase);
    static CElementSet* createElementSet(bool& owner, CModelBase* modelBase,
      CHorizonBase* horizonBase);
    static CElementSet* elementSetExists(CModelBase* modelBase,
      CHorizonBase* horizonBase);

    const CHorizonBase* m_horizonBase;
    TParameters m_parameters;
    bool m_owner;
    CElementSet* m_elementSet;
    int m_sequenceState;
};

} // namespace cora

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif  // _cora_ObjectFault_h_
