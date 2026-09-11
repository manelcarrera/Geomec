
#include "SavePropertyTotalStrain.h"
#include "ModelBase.h"
#include "ResultTree.h"

namespace GeomecRGI {

CSavePropertyTotalStrain::CSavePropertyTotalStrain(
    const RGProperty &rgProperty, const ITensorGroup::CComponentComposite::TENSOR_COMPONENT &tensorComponent)
    : CSavePropertyTensor(rgProperty, tensorComponent) {}

CSavePropertyTotalStrain::~CSavePropertyTotalStrain() {}

bool CSavePropertyTotalStrain::saveProperty(RGInterface &rgi, CModelBase &modelBase, const CRockMechProcessor &rmp) {
  return CSavePropertyTensor::saveProperty(modelBase.ResultTree().TotalStrain(), rgi, modelBase, rmp, 1.0);
}

} // namespace GeomecRGI
