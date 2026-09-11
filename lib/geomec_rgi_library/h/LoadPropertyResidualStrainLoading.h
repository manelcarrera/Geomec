#ifndef _LoadPropertyResidualStrainLoading_h_
#define _LoadPropertyResidualStrainLoading_h_

#include "BuildTensor.h"
#include "ITensorGroup.h"
#include "LoadPropertyBase.h"

namespace GeomecRGI {

class CLoadPropertyResidualStrainLoading : public CLoadPropertyBase {
public:
  CLoadPropertyResidualStrainLoading(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase,
                                     CRockMechProcessor &rmp,
                                     const ITensorGroup::CComponentComposite::TENSOR_COMPONENT &tensorComponent);
  virtual ~CLoadPropertyResidualStrainLoading();

  virtual bool loadProperty();

private:
  CLoadPropertyResidualStrainLoading(const CLoadPropertyResidualStrainLoading &rhs);
  CLoadPropertyResidualStrainLoading &operator=(const CLoadPropertyResidualStrainLoading &rhs);

  CBuildTensor m_buildTensor;
  const ITensorGroup::CComponentComposite::TENSOR_COMPONENT m_tensorComponent;
};

} // namespace GeomecRGI

#endif // _LoadPropertyResidualStrainLoading_h_
