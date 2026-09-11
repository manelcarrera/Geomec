#include "GetSetInfo.h"
#include "ElementSet.h"

namespace cora {

CGetSetInfo::CGetSetInfo() {}

bool CGetSetInfo::isElementSet(const IPointSet &pointSet) const {
  try {
    dynamic_cast<const CElementSet &>(pointSet);

    return true;
  }

  catch (const std::bad_cast &) {
  }

  return false;
}

} // namespace cora
