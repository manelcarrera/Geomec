#ifndef _cora_IValueTypeWrapper_h_
#define _cora_IValueTypeWrapper_h_

#include <vector>

#include "safeQSharedPointer.h"

namespace geo
{

class IElement;

} // namespace geo

#include "SingleQuantity.h"

namespace cora
{

class IValueTypeWrapper
{
  public:
  IValueTypeWrapper();
  virtual ~IValueTypeWrapper() = 0;

  virtual bool hasDistributedValueType() const = 0;
  virtual const std::vector <double> getDistributedValueType(
      const geo::IElement& element, const CQuantity::UNIT unit) const = 0;
  virtual const CDoubleQuantity& getDistributedValueTypeQuantity() const = 0;
  
  private:
  IValueTypeWrapper(const IValueTypeWrapper& rhs);
  IValueTypeWrapper& operator = (const IValueTypeWrapper& rhs);
};

typedef QSharedPointer <IValueTypeWrapper> TValueTypeWrapper;

} // namespace cora

#endif  // _cora_IValueTypeWrapper_h_
