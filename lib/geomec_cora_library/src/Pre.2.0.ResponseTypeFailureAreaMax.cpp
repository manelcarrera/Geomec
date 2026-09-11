#include <limits>

#include "Pre.2.0.ResponseTypeFailureAreaMax.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CPre20ResponseTypeFailureAreaMax::CPre20ResponseTypeFailureAreaMax(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function)
: CPre20ResponseTypeBaseWithValue(summaryResultFile, function)
{
}

CPre20ResponseTypeFailureAreaMax::~CPre20ResponseTypeFailureAreaMax()
{
}

double CPre20ResponseTypeFailureAreaMax::calculate(const TObject& object,
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
      double max = -std::numeric_limits <double> ::max();

      verifyResponseType(valueVec);

      max = std::max(max, CUtilities4ValueVector::calculateAverage(valueVec));

      if (max >= m_value)
      {
    failureArea += element->Size();
      }

      element = object->getNextElement();
  }
  }

  return failureArea;
}

} // namespace cora
