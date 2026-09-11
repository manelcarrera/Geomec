#ifndef _WellPathSections_h_
#define _WellPathSections_h_

#include <vector>

namespace WellPath {

class CElementFaces;

} // namespace WellPath

#include "ElementWrapper.h"

namespace WellPath {

typedef std::vector<TElementWrapper> TWellPathSection;
typedef std::vector<TWellPathSection> TWellPathSections;

class CWellPathSections {
public:
  CWellPathSections(const CElementFaces &elementFaces);

  const TWellPathSections &wellPathSections() const;

private:
  CWellPathSections(const CWellPathSections &rhs);
  CWellPathSections &operator=(const CWellPathSections &rhs);

  static TWellPathSections createWellPathSections(const CElementFaces &elementFaces);

  TWellPathSections m_wellPathSections;
};

} // namespace WellPath

#endif // _WellPathSections_h_
