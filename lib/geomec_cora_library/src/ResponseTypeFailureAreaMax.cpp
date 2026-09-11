#include <limits>

#include "ResponseTypeFailureAreaMax.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CResponseTypeFailureAreaMax::CResponseTypeFailureAreaMax(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function)
: CResponseTypeBaseWithValue(summaryResultFile, function)
{
}

CResponseTypeFailureAreaMax::~CResponseTypeFailureAreaMax()
{
}

double CResponseTypeFailureAreaMax::calculate(const TObject& object,
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

      if ((m_tooHigh && (max > m_value)) || (m_tooLow && (max < m_value)))
      {
        failureArea += element->Size();
      }

      element = object->getNextElement();
    }
  }

  return failureArea;
}

} // namespace cora
