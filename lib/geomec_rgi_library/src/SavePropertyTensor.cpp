
#include "SavePropertyTensor.h"

namespace GeomecRGI
{

CSavePropertyTensor::CSavePropertyTensor(const RGProperty& rgProperty,
  const ITensorGroup::CComponentComposite::TENSOR_COMPONENT& tensorComponent)
: CSavePropertyBase(rgProperty)
, m_tensorComponent(tensorComponent)
{
}

CSavePropertyTensor::~CSavePropertyTensor()
{
}

} // namespace GeomecRGI
