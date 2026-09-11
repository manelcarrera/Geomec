#include "ElementFaces.h"
#include "FormationBase.h"

namespace WellPath
{

CElementFaces::CElementFaces(std::vector <const CFormationBase*> formations)
: m_elementFaces(createElementFaces(formations))
{
}

const TElementFaces& CElementFaces::elementFaces() const
{
  return m_elementFaces;
}

// private

// static

TElementFaces CElementFaces::createElementFaces(
  std::vector <const CFormationBase*> formations)
{
  TElementFaces elementFaces;

  for (size_t f = 0; f < formations.size(); ++f)
  {
  for (int s = 0; s < formations[f]->ElementSetSize(); ++s)
  {
      for (int e = 0;
    e < formations[f]->ElementSet(s).ElementSet().ElementSize(); ++e)
      {
    createElementFace(elementFaces,
          formations[f]->ElementSet(s).ElementSet().Element(e));
      }
  }
  }

  return elementFaces;
}

namespace
{

std::pair <TElementFaces::iterator, bool> localCreateElementFace(
  TElementFaces& elementFaces, const geo::IElement& element, size_t index0,
  size_t index1, size_t index2, size_t index3, bool top)
{
  QSharedPointer <CElementFace> elementFace(new CElementFace(
  element.PointIndex(index0), element.PointIndex(index1),
  element.PointIndex(index2), element.PointIndex(index3), element,
  INITIAL_REFERENCE_COUNT, top));
  std::pair <TElementFaces::iterator, bool> inserted =
  elementFaces.insert(elementFace);

  if (!inserted.second)
  {
  (*inserted.first)->incrementReferenceCount();

  if (top)
  {
      (*inserted.first)->top(top);
      (*inserted.first)->element(elementFace->element());
  }
  }

  return inserted;
}

} // anonymous namespace

void CElementFaces::createElementFace(TElementFaces& elementFaces,
  const geo::IElement& element)
{
  std::pair <TElementFaces::iterator, bool> top =
  localCreateElementFace(elementFaces, element, 0, 1, 2, 3, true);
  std::pair <TElementFaces::iterator, bool> bottom =
  localCreateElementFace(elementFaces, element, 4, 5, 6, 7, false);

  // link

  (*top.first)->otherElementFace(*bottom.first);
  (*bottom.first)->otherElementFace(*top.first);
}

} // namespace WellPath
