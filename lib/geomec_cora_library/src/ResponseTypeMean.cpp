#include "ResponseTypeMean.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CResponseTypeMean::CResponseTypeMean(CSummaryResultFile& summaryResultFile,
  const std::vector <QString>& function)
: CResponseTypeBase(summaryResultFile, function)
{
}

CResponseTypeMean::~CResponseTypeMean()
{
}

double CResponseTypeMean::calculate(const TObject& object,
  const TFailureMode& failureMode)
{
  const geo::IElement* element = object->getFirstElement();
  double weightedMean = UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  if (element != 0)
  {
    double totalSize = calculateTotalSize(object, element);

    weightedMean = 0;

    while (element != 0)
    {
      const IValueDomainScalar::TValueVec valueVec =
        failureMode->getResultComponent()->ValueElement(*element);

      verifyResponseType(valueVec);

      double mean = CUtilities4ValueVector::calculateAverage(valueVec);

      weightedMean += mean * (element->Size() / totalSize);
      element = object->getNextElement();
    }
  }

  return weightedMean;
}

} // namespace cora
