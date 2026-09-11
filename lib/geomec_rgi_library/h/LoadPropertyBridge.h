#ifndef _LoadPropertyBridge_h_
#define _LoadPropertyBridge_h_

class RGProperty;
class RGInterface;

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4702 (unreachable code in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4702)
#endif

#include <QSharedPointer>

#ifdef _WIN32
#pragma warning(pop)
#endif

#include "LoadPropertyBase.h"

namespace GeomecRGI
{

class CLoadPropertyBridge
{
  public:
    CLoadPropertyBridge(const RGProperty& rgProperty, RGInterface& rgi,
      CModelBase& modelBase, CRockMechProcessor& rmp);
    ~CLoadPropertyBridge();

    bool loadProperty();

  private:
    CLoadPropertyBridge(const CLoadPropertyBridge& rhs);
    CLoadPropertyBridge& operator = (const CLoadPropertyBridge& rhs);

    QSharedPointer <CLoadPropertyBase> m_LoadPropertyBase;
};

} // namespace GeomecRGI

#endif  // _LoadPropertyBridge_h_
