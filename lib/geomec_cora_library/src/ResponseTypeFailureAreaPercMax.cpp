#include <limits>

#include "ResponseTypeFailureAreaPercMax.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CResponseTypeFailureAreaPercMax::CResponseTypeFailureAreaPercMax(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function)
: CResponseTypeBaseWithValue(summaryResultFile, function)
{
}

CResponseTypeFailureAreaPercMax::~CResponseTypeFailureAreaPercMax()
{
}

double CResponseTypeFailureAreaPercMax::calculate(const TObject& object,
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

      if ((m_tooHigh && (max > m_value)) || (m_tooLow && (max < m_value)))
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
