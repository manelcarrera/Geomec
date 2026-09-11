#ifndef _SavePropertyBodyIndex_h_
#define _SavePropertyBodyIndex_h_

#include "SavePropertyBase.h"

namespace GeomecRGI
{

class CSavePropertyBodyIndex : public CSavePropertyBase
{
  public:
    CSavePropertyBodyIndex(const RGProperty& rgProperty);
    virtual ~CSavePropertyBodyIndex();

    virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
    CSavePropertyBodyIndex(const CSavePropertyBodyIndex& rhs);
    CSavePropertyBodyIndex& operator = (
      const CSavePropertyBodyIndex& rhs);
};

} // namespace GeomecRGI

#endif  // _SavePropertyBodyIndex_h_
