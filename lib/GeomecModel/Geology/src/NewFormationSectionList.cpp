#include "NewFormationSectionList.h"
#include "NewWellPath.h"

CNewFormationSectionList::CNewFormationSectionList(const std::list<CNewFormationSection> &newFormationSectionList)
    : m_newFormationSectionList(newFormationSectionList) {}

std::list<const CNewFormationSection *> CNewFormationSectionList::getSections(const CNewWellPoint &newWellPoint,
                                                                              bool includeEdge) {
  std::list<const CNewFormationSection *> newFormationSectionList;
  std::list<CNewFormationSection>::const_iterator newFormationSection;

  for (newFormationSection = m_newFormationSectionList.begin(); newFormationSection != m_newFormationSectionList.end();
       ++newFormationSection) {
    if ((*newFormationSection).Contains(newWellPoint, includeEdge)) {
      newFormationSectionList.push_back(&*newFormationSection);
    }
  }

  return newFormationSectionList;
}
