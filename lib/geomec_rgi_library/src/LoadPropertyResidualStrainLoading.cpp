
#include "LoadPropertyResidualStrainLoading.h"

namespace GeomecRGI {

CLoadPropertyResidualStrainLoading::CLoadPropertyResidualStrainLoading(
    const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase, CRockMechProcessor &rmp,
    const ITensorGroup::CComponentComposite::TENSOR_COMPONENT &tensorComponent)
    : CLoadPropertyBase(rgProperty, rgi, modelBase, rmp), m_buildTensor(), m_tensorComponent(tensorComponent) {}

CLoadPropertyResidualStrainLoading::~CLoadPropertyResidualStrainLoading() {}

bool CLoadPropertyResidualStrainLoading::loadProperty() {
  return m_buildTensor.loadProperty(m_RGI, m_ModelBase, m_RMP, m_RGProperty, m_tensorComponent);
}

} // namespace GeomecRGI
