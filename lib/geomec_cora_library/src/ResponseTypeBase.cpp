#include <numeric>

#include "ResponseTypeBase.h"
#include "Utilities4ValueVector.h"

namespace cora {

const QString CResponseTypeBase::RESPONSE_TYPE_MAX = "Max";
const QString CResponseTypeBase::RESPONSE_TYPE_MIN = "Min";
const QString CResponseTypeBase::RESPONSE_TYPE_MEAN = "Mean";
const QString CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_MAX = "Failure area Max";
const QString CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_MIN = "Failure area Min";
const QString CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MAX = "Failure area % Max";
const QString CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MIN = "Failure area % Min";
const QString CResponseTypeBase::RESPONSE_TYPE_FN = "[Ff][0-9]{1,3}";

const QString CResponseTypeBase::RESPONSE_TYPE_IS_NOT_IMPLEMENTED = "response type '%1' is not implemented";

const double CResponseTypeBase::UNDEFINED_OR_INVALID_RESPONSE_TYPE = -999;

CResponseTypeBase::CResponseTypeBase(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function)
    : m_summaryResultFile(summaryResultFile), m_failureType(function[0]), m_responseType(function[3]) {}

CResponseTypeBase::~CResponseTypeBase() {}

const QString CResponseTypeBase::getFailureType() const { return m_failureType; }

const QString CResponseTypeBase::getResponseType() const { return m_responseType; }

// protected

namespace {

const QString RESPONSE_TYPE = "response type %1";

} // anonymous namespace

void CResponseTypeBase::verifyResponseType(const IValueDomainScalar::TValueVec &valueVector) const {
  const QString prefix = QString(RESPONSE_TYPE).arg(getResponseType());

  CUtilities4ValueVector::verifyValueVector(prefix, valueVector);
}

// static

namespace {

typedef std::map<QString, double> TTotalSizeCache;

static TTotalSizeCache totalSizeCache;

} // anonymous namespace

double CResponseTypeBase::calculateTotalSize(const TObject &object, const geo::IElement *element) {
  double totalSize = 0;
  QString key = object->prefix() + object->name();
  TTotalSizeCache::const_iterator totalSizeValue = totalSizeCache.find(key);

  if (totalSizeValue != totalSizeCache.end()) {
    totalSize = (*totalSizeValue).second;
  } else {
    while (element != 0) {
      totalSize += element->Size();
      element = object->getNextElement();
    }

    element = object->getFirstElement();

    totalSizeCache[key] = totalSize;
  }

  return totalSize;
}

} // namespace cora
