#ifndef _LoadPropertyBodyIndex_h_
#define _LoadPropertyBodyIndex_h_

#include "LoadPropertyBase.h"

namespace GeomecRGI
{

class CLoadPropertyBodyIndex : public CLoadPropertyBase
{
  public:
    CLoadPropertyBodyIndex(const RGProperty& rgProperty, RGInterface& rgi,
      CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CLoadPropertyBodyIndex();

    virtual bool loadProperty();

  private:
    CLoadPropertyBodyIndex(const CLoadPropertyBodyIndex& rhs);
    CLoadPropertyBodyIndex& operator = (const CLoadPropertyBodyIndex& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyBodyIndex_h_
