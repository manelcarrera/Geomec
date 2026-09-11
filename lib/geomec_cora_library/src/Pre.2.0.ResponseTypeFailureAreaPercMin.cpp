#include <limits>

#include "Pre.2.0.ResponseTypeFailureAreaPercMin.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CPre20ResponseTypeFailureAreaPercMin::CPre20ResponseTypeFailureAreaPercMin(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function)
: CPre20ResponseTypeBaseWithValue(summaryResultFile, function)
{
}

CPre20ResponseTypeFailureAreaPercMin::~CPre20ResponseTypeFailureAreaPercMin()
{
}

double CPre20ResponseTypeFailureAreaPercMin::calculate(const TObject& object,
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
      double min = std::numeric_limits <double> ::max();

      verifyResponseType(valueVec);

      min = std::min(min, CUtilities4ValueVector::calculateAverage(valueVec));

      if (min <= m_value)
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
