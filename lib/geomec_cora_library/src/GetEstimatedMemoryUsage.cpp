#include <QString>

#include "GetEstimatedMemoryUsage.h"

namespace cora {

CGetEstimatedMemoryUsage::CGetEstimatedMemoryUsage() {}

namespace {

const QString MEMORY_USAGE = "1Gb";

} // anonymous namespaCE

std::ostream &CGetEstimatedMemoryUsage::operator()(std::ostream &os) const {
  os << MEMORY_USAGE.toStdString() << std::endl;

  return os;
}

} // namespace cora
