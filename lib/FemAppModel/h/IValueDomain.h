#ifndef _I_VALUE_DOMAIN_H_
#define _I_VALUE_DOMAIN_H_

namespace geo
{
  class CValue;
  class CVector;
}
class CTensor;

#include <vector>

template<class VALUE_TYPE>
class IValueDomain
{
public:
  typedef std::pair<VALUE_TYPE, VALUE_TYPE> TMinMax;
  typedef VALUE_TYPE TValue;
  typedef std::vector<VALUE_TYPE> TValueVec;
};

typedef IValueDomain<geo::CValue> IValueDomainScalar;
typedef IValueDomain<geo::CVector> IValueDomainVector;
typedef IValueDomain<CTensor> IValueDomainTensor;





#endif