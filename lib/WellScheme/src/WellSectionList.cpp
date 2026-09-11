// WellSectionList.cpp: implementation of the CWellSectionList class.
//
//////////////////////////////////////////////////////////////////////
#include "WellSectionList.h"
#include "IWellSection.h"
#include "WellDefinitionPointList.h"
#include "WellPoint.h"
#include "WellPointList.h"
#include "wellpathbase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well {

CWellSectionList::CWellSectionList(const CWellPathBase &WellPath, bool bAutoDelete)
    : CWellSectionListBase(WellPath, bAutoDelete) {}

CWellSectionList::~CWellSectionList() {}

CWellSectionList &CWellSectionList::operator=(const CWellSectionList &rhs) {
  CWellSectionListBase::operator=(rhs);
  return *this;
}

CWellSectionList::CWellSectionList(const CWellSectionList &rhs) : CWellSectionListBase(rhs) {}

int CWellSectionList::AddSection(IWellSection *section) {

  assert(&WellPath() == &section->WellPath());

  if (CWellSectionListBase::Exist(section)) {
    return -1;
  }

  Iterator it;
  for (it = begin(); it != end(); ++it) {
    int comp = compareItems(*it, section);
    if (comp == 0)
      return -1;
    if (comp > 0)
      break;
  }
  m_lstSections.insert(it, section);
  return m_lstSections.indexOf(section);
}

IWellSection *CWellSectionList::First() const {
  if (m_lstSections.empty())
    return 0;

  return m_lstSections.first();
}

IWellSection *CWellSectionList::Last() const {
  if (m_lstSections.empty())
    return 0;

  return m_lstSections.last();
}

void CWellSectionList::RemoveSection(int index) { m_lstSections.removeAt(index); }

void CWellSectionList::RemoveAllSections() { clear(); }

} // namespace well
