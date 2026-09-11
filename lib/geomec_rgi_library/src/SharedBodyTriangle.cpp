#include <typeinfo>

#include "BodyGroup.h"
#include "SharedBodyTriangle.h"

namespace {

geo::CBodyGroup &cast2BodyGroup(geo::IElementSet &elementSet) {
  // The 'elementSet' argument must be of type 'geo::CBodyGroup'!

  try {
    return dynamic_cast<geo::CBodyGroup &>(elementSet);
  }

  catch (const std::bad_cast &) {
    throw; // Perhaps a more meaningful message?
  }

  catch (...) {
    throw; // Perhaps a more meaningful message?
  }
}

} // anonymous namespace

namespace GeomecRGI {

CSharedBodyTriangle::CSharedBodyTriangle(geo::IElementSet &elementSet, const geo::CBodyTriangle *bodyTriangle,
                                         size_t count)
    : m_bodyGroup(cast2BodyGroup(elementSet)), m_bodyTriangle(bodyTriangle), m_count(count) {}

geo::CBodyGroup &CSharedBodyTriangle::getBodyGroup() const { return m_bodyGroup; }

const geo::CBodyTriangle *CSharedBodyTriangle::getBodyTriangle() const { return m_bodyTriangle; }

size_t CSharedBodyTriangle::getCount() const { return m_count; }

void CSharedBodyTriangle::setCount(size_t count) { m_count = count; }

} // namespace GeomecRGI
