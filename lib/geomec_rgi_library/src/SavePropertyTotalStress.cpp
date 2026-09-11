
#include "SavePropertyTotalStress.h"
#include "ModelBase.h"
#include "ResultTree.h"

namespace GeomecRGI
{

CSavePropertyTotalStress::CSavePropertyTotalStress(const RGProperty& rgProperty,
  const ITensorGroup::CComponentComposite::TENSOR_COMPONENT& tensorComponent)
: CSavePropertyTensor(rgProperty, tensorComponent)
{
}

CSavePropertyTotalStress::~CSavePropertyTotalStress()
{
}

bool CSavePropertyTotalStress::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& rmp)
{
  return CSavePropertyTensor::saveProperty(
    modelBase.ResultTree().TotalStress(), rgi, modelBase, rmp, 1e6);
}

} // namespace GeomecRGI
