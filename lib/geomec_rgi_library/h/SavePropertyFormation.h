#ifndef _SavePropertyFormation_h_
#define _SavePropertyFormation_h_

#include "SavePropertyBase.h"

namespace GeomecRGI
{

class CSavePropertyFormation : public CSavePropertyBase
{
  public:
    CSavePropertyFormation(const RGProperty& rgProperty);
    virtual ~CSavePropertyFormation();

    virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
    CSavePropertyFormation(const CSavePropertyFormation& rhs);
    CSavePropertyFormation& operator = (
      const CSavePropertyFormation& rhs);
};

} // namespace GeomecRGI

#endif  // _SavePropertyFormation_h_
