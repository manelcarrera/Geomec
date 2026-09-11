#ifndef _cora_Utilities4ValueVector_h_
#define _cora_Utilities4ValueVector_h_

#include <QString>

#include "IValueDomain.h"

namespace cora
{

class CUtilities4ValueVector
{
  public:
    static bool isValueVectorValid(
      const IValueDomainScalar::TValueVec& valueVector);
    static double calculateAverage(
      const IValueDomainScalar::TValueVec& valueVector);

    template <typename T>
      static double calculateAverage(const std::vector <T>& values);

    static void verifyValueVector(const QString& prefix,
      const IValueDomainScalar::TValueVec& valueVector);

  private:
    CUtilities4ValueVector(const CUtilities4ValueVector& rhs);
    CUtilities4ValueVector& operator = (const CUtilities4ValueVector& rhs);
};

template <typename T>
  double CUtilities4ValueVector::calculateAverage(const std::vector <T>& values)
{
  IValueDomainScalar::TValueVec valueVector;

  valueVector.assign(values.begin(), values.end());

  return calculateAverage(valueVector);
}

} // namespace cora

#endif  // _cora_Utilities4ValueVector_h_
