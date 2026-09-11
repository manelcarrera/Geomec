#include "WellPathSectionsVertices.h"
#include "ElementFaces.h"
#include "IElement.h"
#include "WellPathSections.h"

namespace WellPath {

CWellPathSectionsVertices::CWellPathSectionsVertices(const CWellPathSections &wellPathSections)
    : m_wellPathSectionsVertices(createElementVertices(wellPathSections)) {}

TWellPathSectionsVertices &CWellPathSectionsVertices::wellPathSectionsVertices() { return m_wellPathSectionsVertices; }

const TWellPathSectionsVertices &CWellPathSectionsVertices::wellPathSectionsVertices() const {
  return m_wellPathSectionsVertices;
}

// private

// static

TWellPathSectionsVertices CWellPathSectionsVertices::createElementVertices(const CWellPathSections &wellPathSections) {
  TWellPathSectionsVertices wellPathSectionsVertices;

  for (TWellPathSections::const_iterator wellPathSection = wellPathSections.wellPathSections().begin();
       wellPathSection != wellPathSections.wellPathSections().end(); ++wellPathSection) {
    TWellPathSectionVertices wellPathSectionVertices;

    for (TWellPathSection::const_iterator element = (*wellPathSection).begin(); element != (*wellPathSection).end();
         ++element) {
      createElementVertex(wellPathSectionVertices, (*element)->element());
    }

    wellPathSectionsVertices.push_back(wellPathSectionVertices);
  }

  return wellPathSectionsVertices;
}

namespace {

std::pair<TWellPathSectionVertices::iterator, bool>
localCreateElementVertex(TWellPathSectionVertices &wellPathSectionVertices, const geo::IElement &element, size_t index0,
                         size_t index1) {
  QSharedPointer<CElementVertex> elementVertex(
      new CElementVertex(element, element.PointIndex(index0), element.PointIndex(index1)));
  std::pair<TWellPathSectionVertices::iterator, bool> inserted = wellPathSectionVertices.insert(elementVertex);

  return inserted;
}

const int INDEX_OF_OPPOSITE_POINT = 4;

} // anonymous namespace

void CWellPathSectionsVertices::createElementVertex(TWellPathSectionVertices &wellPathSectionVertices,
                                                    const geo::IElement &element) {
  std::pair<TWellPathSectionVertices::iterator, bool> elementVertex0 =
      localCreateElementVertex(wellPathSectionVertices, element, 0, 0 + INDEX_OF_OPPOSITE_POINT);
  std::pair<TWellPathSectionVertices::iterator, bool> elementVertex1 =
      localCreateElementVertex(wellPathSectionVertices, element, 1, 1 + INDEX_OF_OPPOSITE_POINT);
  std::pair<TWellPathSectionVertices::iterator, bool> elementVertex2 =
      localCreateElementVertex(wellPathSectionVertices, element, 2, 2 + INDEX_OF_OPPOSITE_POINT);
  std::pair<TWellPathSectionVertices::iterator, bool> elementVertex3 =
      localCreateElementVertex(wellPathSectionVertices, element, 3, 3 + INDEX_OF_OPPOSITE_POINT);

  // link

  (*elementVertex0.first)->otherElementVertex(*elementVertex1.first);
  (*elementVertex0.first)->otherElementVertex(*elementVertex3.first);

  (*elementVertex1.first)->otherElementVertex(*elementVertex0.first);
  (*elementVertex1.first)->otherElementVertex(*elementVertex2.first);

  (*elementVertex2.first)->otherElementVertex(*elementVertex1.first);
  (*elementVertex2.first)->otherElementVertex(*elementVertex3.first);

  (*elementVertex3.first)->otherElementVertex(*elementVertex0.first);
  (*elementVertex3.first)->otherElementVertex(*elementVertex2.first);
}

} // namespace WellPath
