#ifndef _SavePropertyVolumetricStrain_h_
#define _SavePropertyVolumetricStrain_h_

#include "SavePropertyBase.h"

namespace GeomecRGI {

class CSavePropertyVolumetricStrain : public CSavePropertyBase {
public:
  CSavePropertyVolumetricStrain(const RGProperty &rgProperty);
  virtual ~CSavePropertyVolumetricStrain();

  virtual bool saveProperty(RGInterface &rgi, CModelBase &modelBase, const CRockMechProcessor &rmp);

private:
  CSavePropertyVolumetricStrain(const CSavePropertyVolumetricStrain &rhs);
  CSavePropertyVolumetricStrain &operator=(const CSavePropertyVolumetricStrain &rhs);
};

} // namespace GeomecRGI

#endif // _SavePropertyVolumetricStrain_h_
