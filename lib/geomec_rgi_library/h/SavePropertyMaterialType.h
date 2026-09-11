#ifndef _SavePropertyMaterialType_h_
#define _SavePropertyMaterialType_h_

#include "SavePropertyBase.h"

namespace GeomecRGI
{

class CSavePropertyMaterialType : public CSavePropertyBase
{
  public:
    CSavePropertyMaterialType(const RGProperty& rgProperty);
    virtual ~CSavePropertyMaterialType();

    virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
    CSavePropertyMaterialType(const CSavePropertyMaterialType& rhs);
    CSavePropertyMaterialType& operator = (
      const CSavePropertyMaterialType& rhs);
};

} // namespace GeomecRGI

#endif  // _SavePropertyMaterialType_h_
