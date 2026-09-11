#ifndef _cora_ObjectFormation_h_
#define _cora_ObjectFormation_h_

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

class CObjectFormation : public CObjectBase
{
  public:
  CObjectFormation(CFormationBase* formationBase, CModelBase* modelBase);
  virtual ~CObjectFormation();

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
  CObjectFormation(const CObjectFormation& rhs);
  CObjectFormation& operator = (CObjectFormation rhs);

  static TParameters getParameters(CFormationBase* formationBase,
      CModelBase* modelBase);
  static CElementSet* createElementSet(bool& owner, CModelBase* modelBase,
      CFormationBase* formationBase);
  static CElementSet* elementSetExists(CModelBase* modelBase,
      CFormationBase* formationBase);

  CFormationBase* m_formationBase;
  TParameters m_parameters;
  bool m_owner;
  CElementSet* m_elementSet;

  typedef std::pair <int, int> TSequenceState;

  TSequenceState m_sequenceState;
};

} // namespace cora

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif  // _cora_ObjectFormation_h_
