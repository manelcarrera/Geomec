#include <cassert>

#include "ElementFace.h"

namespace WellPath {

CElementFace::CElementFace(int index0, int index1, int index2, int index3, const geo::IElement &element,
                           size_t referenceCount, bool top)
    : m_faceIndices(createFaceIndices(index0, index1, index2, index3)),
      m_element(top ? TElementWrapper(new CElementWrapper(element)) : TElementWrapper(0)),
      m_referenceCount(referenceCount), m_top(top), m_otherElementFace() {}

void CElementFace::decrementReferenceCount() { --m_referenceCount; }

void CElementFace::incrementReferenceCount() { ++m_referenceCount; }

CElementFace::TIndices CElementFace::faceIndices() const { return m_faceIndices; }

size_t CElementFace::referenceCount() const { return m_referenceCount; }

const TElementWrapper &CElementFace::element() const { return m_element; }

void CElementFace::element(const TElementWrapper &element) { m_element = element; }

bool CElementFace::top() const { return m_top; }

void CElementFace::top(bool top) { m_top = (top ? top : m_top); }

const QSharedPointer<CElementFace> &CElementFace::otherElementFace() const { return m_otherElementFace; }

void CElementFace::otherElementFace(const QSharedPointer<CElementFace> &otherElementFace) {
  m_otherElementFace = otherElementFace;
}

// private

// static

CElementFace::TIndices CElementFace::createFaceIndices(int index0, int index1, int index2, int index3) {
  TIndices faceIndices;

  faceIndices.insert(index0);
  faceIndices.insert(index1);
  faceIndices.insert(index2);
  faceIndices.insert(index3);

  return faceIndices;
}

} // namespace WellPath
