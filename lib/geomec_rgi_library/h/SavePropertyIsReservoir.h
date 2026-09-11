#ifndef _SavePropertyIsReservoir_h_
#define _SavePropertyIsReservoir_h_

#include "SavePropertyBase.h"

namespace GeomecRGI
{

  class CSavePropertyIsReservoir : public CSavePropertyBase
  {
  public:
    CSavePropertyIsReservoir(const RGProperty& rgProperty);
    virtual ~CSavePropertyIsReservoir();

    virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
    CSavePropertyIsReservoir(const CSavePropertyIsReservoir& rhs);
    CSavePropertyIsReservoir& operator = (
      const CSavePropertyIsReservoir& rhs);
  };

} // namespace GeomecRGI

#endif  // _SavePropertyIsReservoir_h_
