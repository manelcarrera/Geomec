#include <limits>

#include "Pre.2.0.ResponseTypeFailureAreaMin.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CPre20ResponseTypeFailureAreaMin::CPre20ResponseTypeFailureAreaMin(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function)
: CPre20ResponseTypeBaseWithValue(summaryResultFile, function)
{
}

CPre20ResponseTypeFailureAreaMin::~CPre20ResponseTypeFailureAreaMin()
{
}

double CPre20ResponseTypeFailureAreaMin::calculate(const TObject& object,
  const TFailureMode& failureMode)
{
  const geo::IElement* element = object->getFirstElement();
  double failureArea = UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  if (element != 0)
  {
    failureArea = 0;

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
  }

  return failureArea;
}

} // namespace cora
