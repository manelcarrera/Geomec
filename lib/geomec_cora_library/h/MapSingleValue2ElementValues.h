#ifndef _cora_MapSingleValue2ElementValues_h_
#define _cora_MapSingleValue2ElementValues_h_

class CModelBase;

namespace cora
{

class CFailureTypeParameterBase;

} // namespace cora

#include "ICreateElementValueSet.h"

namespace cora
{

class CMapSingleValue2ElementValues : public ICreateElementValueSet
{
  public:
    CMapSingleValue2ElementValues();
    virtual ~CMapSingleValue2ElementValues();

    virtual void createElementValueSet(CModelBase* modelBase,
      const CFailureTypeParameterBase* failureTypeParameterBase);

  private:
    CMapSingleValue2ElementValues(const CMapSingleValue2ElementValues& rhs);
    CMapSingleValue2ElementValues& operator = (
      const CMapSingleValue2ElementValues& rhs);
};

} // namespace cora

#endif  // _cora_MapSingleValue2ElementValues_h_
