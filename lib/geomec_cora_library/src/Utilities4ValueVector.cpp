#include <numeric>

#include "Utilities4ValueVector.h"
#include "Value.h"
#include "UndefinedOrInvalidValueVector.h"

namespace cora
{

// static

bool CUtilities4ValueVector::isValueVectorValid(
  const IValueDomainScalar::TValueVec& valueVector)
{
  for (IValueDomainScalar::TValueVec::const_iterator value =
    valueVector.begin(); value != valueVector.end(); ++value)
  {
    if (!(*value).Valid())
    {
      return false;
    }
  }

  return true;
}

namespace
{

double add(const geo::CValue& lhs, const geo::CValue& rhs)
{
  return lhs.Value() + rhs.Value();
}

} // anonymous namespace

double CUtilities4ValueVector::calculateAverage(
  const IValueDomainScalar::TValueVec& valueVector)
{
  double sum =
    std::accumulate(valueVector.begin(), valueVector.end(), 0.0, add);

  return sum / valueVector.size();
}

void CUtilities4ValueVector::verifyValueVector(const QString& prefix,
  const IValueDomainScalar::TValueVec& valueVector)
{
  if (!CUtilities4ValueVector::isValueVectorValid(valueVector))
  {
    throw CUndefinedOrInvalidValueVector(prefix);
  }
}

} // namespace cora
