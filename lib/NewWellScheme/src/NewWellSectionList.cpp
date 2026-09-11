#include "NewWellSectionList.h"
#include "NewWellSection.h"

CNewWellSectionList::CNewWellSectionList(const std::list<INewWellSection *> &newWellSectionList)
    : m_newWellSectionList(newWellSectionList) {}

std::list<INewWellSection *> CNewWellSectionList::getSections(const CNewWellPoint &newWellPoint, bool includeEdge) {
  std::list<INewWellSection *> newWellSectionList;
  std::list<INewWellSection *>::const_iterator newWellSection;

  for (newWellSection = m_newWellSectionList.begin(); newWellSection != m_newWellSectionList.end(); ++newWellSection) {
    if ((*newWellSection)->Contains(newWellPoint, includeEdge)) {
      newWellSectionList.push_back(*newWellSection);
    }
  }

  return newWellSectionList;
}
