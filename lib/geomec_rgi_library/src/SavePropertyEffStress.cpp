
#include "SavePropertyEffStress.h"
#include "ModelBase.h"
#include "ResultTree.h"

namespace GeomecRGI
{

CSavePropertyEffStress::CSavePropertyEffStress(const RGProperty& rgProperty,
  const ITensorGroup::CComponentComposite::TENSOR_COMPONENT& tensorComponent)
: CSavePropertyTensor(rgProperty, tensorComponent)
{
}

CSavePropertyEffStress::~CSavePropertyEffStress()
{
}

bool CSavePropertyEffStress::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& rmp)
{
  return CSavePropertyTensor::saveProperty(
  modelBase.ResultTree().EffectiveStress(), rgi, modelBase, rmp, 1e6);
}

} // namespace GeomecRGI
