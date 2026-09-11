#include <limits>

#include "ResponseTypefn.h"
#include "Utilities4ValueVector.h"
#include "Result.h"

namespace cora
{

CResponseTypefn::CResponseTypefn(CSummaryResultFile& summaryResultFile,
  const std::vector <QString>& function)
: CResponseTypeBase(summaryResultFile, function)
{
}

CResponseTypefn::~CResponseTypefn()
{
}

namespace
{

const double HUNDRED = 100.0;

double determineBoundaryValue(const std::map <double, double>& contribution,
  const QString& responseType, double totalSize)
{
  double boundary = (responseType.mid(1).toDouble() / HUNDRED) * totalSize;
  double size = 0;
  std::map <double, double>::const_iterator pair = contribution.begin();

  for (; pair != contribution.end(); ++pair)
  {
  size += (*pair).second;

  if (size > boundary)
  {
      return (*pair).first;
  }
  }

  assert(pair == contribution.end());
  assert(size == totalSize);

  return (*--pair).first;
}

} // anonymous namespace

double CResponseTypefn::calculate(const TObject& object,
  const TFailureMode& failureMode)
{
  const geo::IElement* element = object->getFirstElement();
  double p = UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  // The only time that the 'Defined()' function is necessary, why?

  if ((element != 0) && (failureMode->getResultComponent()->Defined()))
  {
  typedef std::map <double, double> TDoublesMap;

  TDoublesMap contribution;
  double totalSize = 0;

  while (element != 0)
  {
      const IValueDomainScalar::TValueVec valueVec =
    failureMode->getResultComponent()->ValueElement(*element);

      verifyResponseType(valueVec);

      double value = CUtilities4ValueVector::calculateAverage(valueVec);
      double valueTimesSize = value * element->Size();

      contribution.insert(TDoublesMap::value_type(value, valueTimesSize));
      totalSize += valueTimesSize;

      element = object->getNextElement();
  }

  p = determineBoundaryValue(contribution, getResponseType(), totalSize);
  }

  return p;
}

} // namespace cora
