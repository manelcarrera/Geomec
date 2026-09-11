#ifndef _cora_CreateElementValueSet_h_
#define _cora_CreateElementValueSet_h_

#include "ICreateElementValueSet.h"

namespace cora {

class CCreateElementValueSet {
public:
  static struct TMapSingleValue2ElementValues {
  } mapSingleValue2ElementValues;
  static struct TMapInterfaceElement2ElementValues {
  } mapInterfaceElement2ElementValues;

  CCreateElementValueSet(const TMapSingleValue2ElementValues &);
  CCreateElementValueSet(const TMapInterfaceElement2ElementValues &);

  void createElementValueSet(CModelBase *modelBase, const CFailureTypeParameterBase *failureTypeParameterBase);

private:
  CCreateElementValueSet(const CCreateElementValueSet &rhs);
  CCreateElementValueSet &operator=(CCreateElementValueSet rhs);

  TCreateElementValueSetBase m_createElementValueSet;
};

typedef QSharedPointer<CCreateElementValueSet> TCreateElementValueSet;

} // namespace cora

#endif // _cora_CreateElementValueSet_h_
