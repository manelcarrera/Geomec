#ifndef _cora_ICreateElementValueSet_h_
#define _cora_ICreateElementValueSet_h_

#include "safeQSharedPointer.h"

class CModelBase;

namespace cora {

class CFailureTypeParameterBase;

} // namespace cora

namespace cora {

class ICreateElementValueSet {
public:
  ICreateElementValueSet();
  virtual ~ICreateElementValueSet() = 0;

  virtual void createElementValueSet(CModelBase *modelBase,
                                     const CFailureTypeParameterBase *failureTypeParameterBase) = 0;

private:
  ICreateElementValueSet(const ICreateElementValueSet &rhs);
  ICreateElementValueSet &operator=(const ICreateElementValueSet &rhs);
};

typedef QSharedPointer<ICreateElementValueSet> TCreateElementValueSetBase;

} // namespace cora

#endif // _cora_ICreateElementValueSet_h_
