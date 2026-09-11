#include <limits>

#include "ResponseTypeFailureAreaPercMin.h"
#include "Result.h"
#include "Utilities4ValueVector.h"

namespace cora {

CResponseTypeFailureAreaPercMin::CResponseTypeFailureAreaPercMin(CSummaryResultFile &summaryResultFile,
                                                                 const std::vector<QString> &function)
    : CResponseTypeBaseWithValue(summaryResultFile, function) {}

CResponseTypeFailureAreaPercMin::~CResponseTypeFailureAreaPercMin() {}

double CResponseTypeFailureAreaPercMin::calculate(const TObject &object, const TFailureMode &failureMode) {
  const geo::IElement *element = object->getFirstElement();
  double failureAreaPercentage = UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  if (element != 0) {
    double failureArea = 0;
    double totalSize = calculateTotalSize(object, element);

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

    failureAreaPercentage = failureArea / totalSize;
  }

  return failureAreaPercentage;
}

} // namespace cora
