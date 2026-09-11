#include <limits>

#include "ResponseTypeMax.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CResponseTypeMax::CResponseTypeMax(CSummaryResultFile& summaryResultFile,
  const std::vector <QString>& function)
: CResponseTypeBase(summaryResultFile, function)
{
}

CResponseTypeMax::~CResponseTypeMax()
{
}

double CResponseTypeMax::calculate(const TObject& object,
  const TFailureMode& failureMode)
{
  const geo::IElement* element = object->getFirstElement();
  double max = UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  if (element != 0)
  {
  max = -std::numeric_limits <double> ::max();

  while (element != 0)
  {
      const IValueDomainScalar::TValueVec valueVec =
    failureMode->getResultComponent()->ValueElement(*element);

      verifyResponseType(valueVec);

      max = std::max(max, CUtilities4ValueVector::calculateAverage(valueVec));
      element = object->getNextElement();
  }
  }

  return max;
}

} // namespace cora
