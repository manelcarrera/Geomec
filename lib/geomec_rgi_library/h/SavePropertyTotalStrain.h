#ifndef _SavePropertyTotalStrain_h_
#define _SavePropertyTotalStrain_h_

#include "SavePropertyTensor.h"

namespace GeomecRGI
{

class CSavePropertyTotalStrain : public CSavePropertyTensor
{
  public:
    CSavePropertyTotalStrain(const RGProperty& rgProperty,
      const ITensorGroup::CComponentComposite::TENSOR_COMPONENT&
        tensorComponent);
    virtual ~CSavePropertyTotalStrain();

    virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
    CSavePropertyTotalStrain(const CSavePropertyTotalStrain& rhs);
    CSavePropertyTotalStrain& operator = (const CSavePropertyTotalStrain& rhs);
};

} // namespace GeomecRGI

#endif  // _SavePropertyTotalStrain_h_
