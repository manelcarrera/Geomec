#ifndef _ElementFace_h_
#define _ElementFace_h_

#include <set>

#include <QSharedPointer>

namespace geo
{

class IElement;

} // namespace geo

#include "ElementWrapper.h"

namespace WellPath
{

class CElementFace
{
  public:
    typedef std::set <int> TIndices;

    CElementFace(int index0, int index1, int index2, int index3,
      const geo::IElement& element, size_t referenceCount, bool top);

    void decrementReferenceCount();
    void incrementReferenceCount();

    TIndices faceIndices() const;
    size_t referenceCount() const;

    const TElementWrapper& element() const;
    void element(const TElementWrapper& element);

    bool top() const;
    void top(bool top);

    const QSharedPointer <CElementFace>& otherElementFace() const;
    void otherElementFace(
      const QSharedPointer <CElementFace>& otherElementFace);

  private:
    CElementFace(const CElementFace& rhs);
    CElementFace& operator = (CElementFace rhs);

    static TIndices createFaceIndices(int index0, int index1, int index2,
      int index3);

    TIndices m_faceIndices;
    TElementWrapper m_element;
    size_t m_referenceCount;
    bool m_top;
    QSharedPointer <CElementFace> m_otherElementFace; // top or bottom
};

} // namespace WellPath

#endif  // _ElementFace_h_
