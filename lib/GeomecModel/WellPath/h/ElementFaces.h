#ifndef _ElementFaces_h_
#define _ElementFaces_h_

#include <vector>

#include <QSharedPointer>

class CFormationBase;

#include "ElementFace.h"

namespace WellPath {

struct COrderByFaceIndices {
  bool operator()(const QSharedPointer<CElementFace> &lhs, const QSharedPointer<CElementFace> &rhs) {
    return (lhs->faceIndices() < rhs->faceIndices());
  }
};

typedef std::set<QSharedPointer<CElementFace>, COrderByFaceIndices> TElementFaces;

const size_t INITIAL_REFERENCE_COUNT = 1;

class CElementFaces {
public:
  CElementFaces(std::vector<const CFormationBase *> formations);

  const TElementFaces &elementFaces() const;

private:
  CElementFaces(const CElementFaces &rhs);
  CElementFaces &operator=(const CElementFaces &rhs);

  static TElementFaces createElementFaces(std::vector<const CFormationBase *> formations);
  static void createElementFace(TElementFaces &elementFaces, const geo::IElement &element);

  TElementFaces m_elementFaces;
};

} // namespace WellPath

#endif // _ElementFaces_h_
