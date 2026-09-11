#include <limits>

#include "Pre.2.0.ResponseTypeFailureAreaPercMax.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CPre20ResponseTypeFailureAreaPercMax::CPre20ResponseTypeFailureAreaPercMax(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function)
: CPre20ResponseTypeBaseWithValue(summaryResultFile, function)
{
}

CPre20ResponseTypeFailureAreaPercMax::~CPre20ResponseTypeFailureAreaPercMax()
{
}

double CPre20ResponseTypeFailureAreaPercMax::calculate(const TObject& object,
  const TFailureMode& failureMode)
{
  const geo::IElement* element = object->getFirstElement();
  double failureAreaPercentage = UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  if (element != 0)
  {
  double failureArea = 0;
  double totalSize = calculateTotalSize(object, element);

  while (element != 0)
  {
      const IValueDomainScalar::TValueVec valueVec =
    failureMode->getResultComponent()->ValueElement(*element);
      double max = -std::numeric_limits <double> ::max();

      verifyResponseType(valueVec);

      max = std::max(max, CUtilities4ValueVector::calculateAverage(valueVec));

      if (max >= m_value)
      {
    failureArea += element->Size();
      }

      element = object->getNextElement();
  }

  failureAreaPercentage = failureArea / totalSize;
  }

  return failureAreaPercentage;
}

} // namespace cora
