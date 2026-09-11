#include "ElementWrapper.h"

namespace WellPath {

CElementWrapper::CElementWrapper(const geo::IElement &element) : m_element(element) {}

const geo::IElement &CElementWrapper::element() const { return m_element; }

} // namespace WellPath
