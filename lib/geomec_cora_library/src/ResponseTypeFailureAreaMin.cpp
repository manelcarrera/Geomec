#include <limits>

#include "ResponseTypeFailureAreaMin.h"
#include "Result.h"
#include "Utilities4ValueVector.h"

namespace cora {

CResponseTypeFailureAreaMin::CResponseTypeFailureAreaMin(CSummaryResultFile &summaryResultFile,
                                                         const std::vector<QString> &function)
    : CResponseTypeBaseWithValue(summaryResultFile, function) {}

CResponseTypeFailureAreaMin::~CResponseTypeFailureAreaMin() {}

double CResponseTypeFailureAreaMin::calculate(const TObject &object, const TFailureMode &failureMode) {
  const geo::IElement *element = object->getFirstElement();
  double failureArea = UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  if (element != 0) {
    failureArea = 0;

    while (element != 0) {
      const IValueDomainScalar::TValueVec valueVec = failureMode->getResultComponent()->ValueElement(*element);
      double min = std::numeric_limits<double>::max();

      verifyResponseType(valueVec);

      min = std::min(min, CUtilities4ValueVector::calculateAverage(valueVec));

      if ((m_tooHigh && (min > m_value)) || (m_tooLow && (min < m_value))) {
        failureArea += element->Size();
      }

      element = object->getNextElement();
    }
  }

  return failureArea;
}

} // namespace cora
