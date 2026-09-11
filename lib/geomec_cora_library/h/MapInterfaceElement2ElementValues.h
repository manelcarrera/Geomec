#ifndef _cora_MapInterfaceElement2ElementValues_h_
#define _cora_MapInterfaceElement2ElementValues_h_

class CModelBase;

namespace cora
{

class CFailureTypeParameterBase;

} // namespace cora

#include "ICreateElementValueSet.h"

namespace cora
{

class CMapInterfaceElement2ElementValues : public ICreateElementValueSet
{
  public:
    CMapInterfaceElement2ElementValues();
    virtual ~CMapInterfaceElement2ElementValues();

    virtual void createElementValueSet(CModelBase* modelBase,
      const CFailureTypeParameterBase* failureTypeParameterBase);

  private:
    CMapInterfaceElement2ElementValues(
      const CMapInterfaceElement2ElementValues& rhs);
    CMapInterfaceElement2ElementValues& operator = (
      const CMapInterfaceElement2ElementValues& rhs);
};

} // namespace cora

#endif  // _cora_MapInterfaceElement2ElementValues_h_
