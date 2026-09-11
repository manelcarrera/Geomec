#include "UndefinedOrInvalidValueVector.h"

namespace cora
{

namespace
{

const QString INVALID_VALUE_VECTOR = "%1 contains an invalid value";

} // anonymous namespace

CUndefinedOrInvalidValueVector::CUndefinedOrInvalidValueVector(
  const QString& prefix)
: std::runtime_error(QString(INVALID_VALUE_VECTOR).arg(prefix).toStdString())
{
}

} // namespace cora
