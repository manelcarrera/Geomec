#include "WellPathSections.h"
#include "ElementFaces.h"

namespace WellPath {

CWellPathSections::CWellPathSections(const CElementFaces &elementFaces)
    : m_wellPathSections(createWellPathSections(elementFaces)) {}

const TWellPathSections &CWellPathSections::wellPathSections() const { return m_wellPathSections; }

// private

// static

TWellPathSections CWellPathSections::createWellPathSections(const CElementFaces &elementFaces) {
  TWellPathSections wellPathSections;

  while (true) {
    TWellPathSection wellPathSection;
    TElementFaces elementFacesInSection;

    for (TElementFaces::const_iterator elementFace = elementFaces.elementFaces().begin();
         elementFace != elementFaces.elementFaces().end(); ++elementFace) {
      if (((*elementFace)->referenceCount() == INITIAL_REFERENCE_COUNT) &&
          ((*elementFace)->otherElementFace()->referenceCount() >= INITIAL_REFERENCE_COUNT) && (*elementFace)->top()) {
        elementFacesInSection.insert(*elementFace);
        wellPathSection.push_back((*elementFace)->element());
      }
    }

    if (!wellPathSection.empty()) {
      wellPathSections.push_back(wellPathSection);

      for (TElementFaces::const_iterator elementFace = elementFacesInSection.begin();
           elementFace != elementFacesInSection.end(); ++elementFace) {
        (*elementFace)->decrementReferenceCount();
        (*elementFace)->otherElementFace()->decrementReferenceCount();
      }
    } else {
      break;
    }
  }

  return wellPathSections;
}

} // namespace WellPath
