#ifndef _NewWellSectionList_h_
#define _NewWellSectionList_h_

#include <list>

class INewWellSection;
class CNewWellPoint;

class CNewWellSectionList {
public:
  CNewWellSectionList(const std::list<INewWellSection *> &newWellSectionList);

  std::list<INewWellSection *> getSections(const CNewWellPoint &newWellPoint, bool includeEdge = true);

private:
  CNewWellSectionList(const CNewWellSectionList &rhs);
  CNewWellSectionList &operator=(const CNewWellSectionList &rhs);

  const std::list<INewWellSection *> &m_newWellSectionList;
};

#endif // _NewWellSectionList_h_
