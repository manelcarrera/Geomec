#ifndef _SavePropertyTotalStress_h_
#define _SavePropertyTotalStress_h_

#include "SavePropertyTensor.h"

namespace GeomecRGI {

class CSavePropertyTotalStress : public CSavePropertyTensor {
public:
  CSavePropertyTotalStress(const RGProperty &rgProperty,
                           const ITensorGroup::CComponentComposite::TENSOR_COMPONENT &tensorComponent);
  virtual ~CSavePropertyTotalStress();

  virtual bool saveProperty(RGInterface &rgi, CModelBase &modelBase, const CRockMechProcessor &rmp);

private:
  CSavePropertyTotalStress(const CSavePropertyTotalStress &rhs);
  CSavePropertyTotalStress &operator=(const CSavePropertyTotalStress &rhs);
};

} // namespace GeomecRGI

#endif // _SavePropertyTotalStress_h_
