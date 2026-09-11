#include <limits>

#include "ResponseTypeMin.h"
#include "Result.h"
#include "Utilities4ValueVector.h"

namespace cora {

CResponseTypeMin::CResponseTypeMin(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function)
    : CResponseTypeBase(summaryResultFile, function) {}

CResponseTypeMin::~CResponseTypeMin() {}

double CResponseTypeMin::calculate(const TObject &object, const TFailureMode &failureMode) {
  const geo::IElement *element = object->getFirstElement();
  double min = UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  if (element != 0) {
    min = std::numeric_limits<double>::max();

    while (element != 0) {
      const IValueDomainScalar::TValueVec valueVec = failureMode->getResultComponent()->ValueElement(*element);

      verifyResponseType(valueVec);

      min = std::min(min, CUtilities4ValueVector::calculateAverage(valueVec));
      element = object->getNextElement();
    }
  }

  return min;
}

} // namespace cora
