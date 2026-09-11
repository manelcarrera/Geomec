#ifndef _ElementWrapper_h_
#define _ElementWrapper_h_

#include <QSharedPointer>

namespace geo
{

class IElement;

} // namespace geo

namespace WellPath
{

class CElementWrapper
{
  public:
  CElementWrapper(const geo::IElement& element);

  const geo::IElement& element() const;

  private:
  CElementWrapper(const CElementWrapper& rhs);
  CElementWrapper& operator = (CElementWrapper rhs);

  const geo::IElement& m_element;
};

typedef QSharedPointer <CElementWrapper> TElementWrapper;

} // namespace WellPath

#endif  // _ElementWrapper_h_
