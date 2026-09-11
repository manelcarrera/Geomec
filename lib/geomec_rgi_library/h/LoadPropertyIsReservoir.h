#ifndef _LoadPropertyIsReservoir_h_
#define _LoadPropertyIsReservoir_h_

#include "LoadPropertyBase.h"

namespace GeomecRGI {

class CLoadPropertyIsReservoir : public CLoadPropertyBase {
public:
  CLoadPropertyIsReservoir(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase,
                           CRockMechProcessor &rmp);
  virtual ~CLoadPropertyIsReservoir();

  virtual bool loadProperty();

private:
  CLoadPropertyIsReservoir(const CLoadPropertyIsReservoir &rhs);
  CLoadPropertyIsReservoir &operator=(const CLoadPropertyIsReservoir &rhs);
};

} // namespace GeomecRGI

#endif // _LoadPropertyIsReservoir_h_
