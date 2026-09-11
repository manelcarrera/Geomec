#ifndef _SavePropertyBridge_h_
#define _SavePropertyBridge_h_

class RGProperty;
class RGInterface;

#ifdef _WIN32
#define MAKESTRING2(str) #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4702 (unreachable code in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include <QSharedPointer>

#ifdef _WIN32
#pragma warning(pop)
#endif

#include "SavePropertyBase.h"

namespace GeomecRGI {

class CSavePropertyBridge {
public:
  CSavePropertyBridge(const RGProperty &rgProperty);
  ~CSavePropertyBridge();

  bool saveProperty(RGInterface &rgi, CModelBase &modelBase, const CRockMechProcessor &rmp);

private:
  CSavePropertyBridge(const CSavePropertyBridge &rhs);
  CSavePropertyBridge &operator=(const CSavePropertyBridge &rhs);

  QSharedPointer<CSavePropertyBase> m_SavePropertyBase;
};

} // namespace GeomecRGI

#endif // _SavePropertyBridge_h_
