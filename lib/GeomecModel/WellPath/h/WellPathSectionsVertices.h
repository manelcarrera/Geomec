#ifndef _WellPathSectionsVertices_h_
#define _WellPathSectionsVertices_h_

#include <vector>

namespace WellPath
{

class CWellPathSections;

} // namespace WellPath

namespace geo
{

class IElement;

} // namespace geo

#include "ElementVertex.h"

namespace WellPath
{

typedef TElementVertices TWellPathSectionVertices;
typedef std::vector <TWellPathSectionVertices> TWellPathSectionsVertices;

class CWellPathSectionsVertices
{
  public:
  CWellPathSectionsVertices(const CWellPathSections& wellPathSections);

  TWellPathSectionsVertices& wellPathSectionsVertices();
  const TWellPathSectionsVertices& wellPathSectionsVertices() const;

  private:
  CWellPathSectionsVertices(const CWellPathSectionsVertices& rhs);
  CWellPathSectionsVertices& operator = (const CWellPathSectionsVertices& rhs);

  static TWellPathSectionsVertices createElementVertices(
      const CWellPathSections& wellPathSections);
  static void createElementVertex(
      TWellPathSectionVertices& wellPathSectionVertices,
      const geo::IElement& element);

  TWellPathSectionsVertices m_wellPathSectionsVertices;
};

} // namespace WellPath

#endif  // _WellPathSectionsVertices_h_
