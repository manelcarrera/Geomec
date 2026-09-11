#ifndef _SharedBodyTriangle_h_
#define _SharedBodyTriangle_h_

#include <map>

#include <QSharedPointer>

#include "BodyTriangle.h"

namespace GeomecRGI {

// QSharedPointer does not have a function to retrieve the reference count,
// at least I did not find one. Hence these types to register the number of
// references.

class CSharedBodyTriangle {
public:
  CSharedBodyTriangle(geo::IElementSet &elementSet, const geo::CBodyTriangle *bodyTriangle, size_t count);

  geo::CBodyGroup &getBodyGroup() const;
  const geo::CBodyTriangle *getBodyTriangle() const;

  size_t getCount() const;
  void setCount(size_t count);

private:
  CSharedBodyTriangle(const CSharedBodyTriangle &rhs);
  CSharedBodyTriangle &operator=(const CSharedBodyTriangle &rhs);

  geo::CBodyGroup &m_bodyGroup;
  const geo::CBodyTriangle *m_bodyTriangle;
  size_t m_count;
};

typedef QSharedPointer<CSharedBodyTriangle> TSharedBodyTriangle;
typedef std::map<QString, TSharedBodyTriangle> TSharedBodyTriangleMap;

} // namespace GeomecRGI

#endif // _SharedBodyTriangle_h_
