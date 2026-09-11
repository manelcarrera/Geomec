#include "WellPathCenterPointSet.h"
#include "ModelBase.h"
#include "ElementFaces.h"
#include "WellPathSections.h"
#include "WellPathSectionsVertices.h"
#include "FormationBase.h"

namespace WellPath
{

CWellPathCenterPointSet::CWellPathCenterPointSet(
  const std::vector <const CFormationBase*>& formations, CModelBase& modelBase)
: CPointSet("", modelBase, 0,
    static_cast <IPointSet::DIMENSION> (modelBase.Dimension()))
{
  populatePointSet(formations, modelBase);
}

// private

namespace
{

TElementVertex
  retrieveCenterVertex(TWellPathSectionVertices& wellPathSectionVertices)
{
  while (wellPathSectionVertices.size() > 1)
  {
    std::vector <TElementVertex> elementVertices2Clean;

    for (TWellPathSectionVertices::const_iterator
      elementVertex = wellPathSectionVertices.begin();
      elementVertex != wellPathSectionVertices.end(); ++elementVertex)
    {
      size_t referenceCount = (*elementVertex)->otherElementVertices().size();

      if ((0 < referenceCount) && (referenceCount <= 2))
      {
        elementVertices2Clean.push_back(*elementVertex);
        wellPathSectionVertices.erase(elementVertex);
        elementVertex = wellPathSectionVertices.begin();
      }
    }

    for (std::vector <TElementVertex>::const_iterator elementVertex =
      elementVertices2Clean.begin(); elementVertex !=
      elementVertices2Clean.end(); ++elementVertex)
    {
      (*elementVertex)->removeLinkWithOtherElementVertices();
    }
  }

  return (*wellPathSectionVertices.begin());
}

} // anonymous namespace

void CWellPathCenterPointSet::populatePointSet(
  const std::vector <const CFormationBase*>& formations,
  CModelBase& /*modelBase*/)
{
  CElementFaces elementFaces(formations);
  CWellPathSections wellPathSections(elementFaces);
  CWellPathSectionsVertices wellPathSectionsVertices(wellPathSections);

  std::vector <TElementVertex> centerVertices;

  for (TWellPathSectionsVertices::iterator wellPathSectionVertices =
    wellPathSectionsVertices.wellPathSectionsVertices().begin();
    wellPathSectionVertices !=
      wellPathSectionsVertices.wellPathSectionsVertices().end();
    ++wellPathSectionVertices)
  {
    centerVertices.push_back(retrieveCenterVertex(*wellPathSectionVertices));
  }

  std::vector <TElementVertex>::const_iterator centerVertex =
    centerVertices.begin();

  {
    geo::CElementPoint elementPoint = (*centerVertex)->element().
      IndexingElementSet()->Point((*centerVertex)->vertexIndices()[0]);

    PushBack(elementPoint, std::vector <double> ());
  }

  for (; centerVertex != centerVertices.end(); ++centerVertex)
  {
    geo::CElementPoint elementPoint = (*centerVertex)->element().
      IndexingElementSet()->Point((*centerVertex)->vertexIndices()[1]);

    PushBack(elementPoint, std::vector <double> ());
  }
}

} // namespace WellPath
