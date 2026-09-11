#ifndef _SavePropertyEffStress_h_
#define _SavePropertyEffStress_h_

#include "SavePropertyTensor.h"

namespace GeomecRGI
{

class CSavePropertyEffStress : public CSavePropertyTensor
{
  public:
  CSavePropertyEffStress(const RGProperty& rgProperty,
      const ITensorGroup::CComponentComposite::TENSOR_COMPONENT&
    tensorComponent);
  virtual ~CSavePropertyEffStress();

  virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
  CSavePropertyEffStress(const CSavePropertyEffStress& rhs);
  CSavePropertyEffStress& operator = (const CSavePropertyEffStress& rhs);
};

} // namespace GeomecRGI

#endif  // _SavePropertyEffStress_h_
