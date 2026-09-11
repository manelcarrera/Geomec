#include "ValueVectorContainsInvalidValues.h"

namespace cora
{

namespace
{

const QString INVALID_VALUE_VECTOR =
  "a value vector for value type '%1' contains invalid values";

} // anonymous namespace

CValueVectorContainsInvalidValues::CValueVectorContainsInvalidValues(
  const QString& valueType)
: std::runtime_error(QString(INVALID_VALUE_VECTOR).arg(valueType).toStdString())
{
}

} // namespace cora
