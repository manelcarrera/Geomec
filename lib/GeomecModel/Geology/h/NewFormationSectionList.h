#ifndef _NewFormationSectionList_h_
#define _NewFormationSectionList_h_

#include <list>

class CNewFormationSection;
class CNewWellPoint;

class CNewFormationSectionList {
public:
  CNewFormationSectionList(const std::list<CNewFormationSection> &newFormationSectionList);

  std::list<const CNewFormationSection *> getSections(const CNewWellPoint &newWellPoint, bool includeEdge = true);

private:
  CNewFormationSectionList(const CNewFormationSectionList &rhs);
  CNewFormationSectionList &operator=(const CNewFormationSectionList &rhs);

  const std::list<CNewFormationSection> &m_newFormationSectionList;
};

#endif // _NewFormationSectionList_h_
