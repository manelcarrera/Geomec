#ifndef _BuildTensor_h_
#define _BuildTensor_h_

class RGInterface;
class RGProperty;

namespace GeomecRGI
{

class CRockMechProcessor;

} // namespace GeomecRGI

#include "ITensorGroup.h"

namespace GeomecRGI
{

class CBuildTensor
{
  public:
    class CBuildTensorImpl;

    CBuildTensor();
    ~CBuildTensor();

    bool loadProperty(RGInterface& rgi, CModelBase& modelBase,
      CRockMechProcessor& rmp, const RGProperty& rgProperty,
      const ITensorGroup::CComponentComposite::TENSOR_COMPONENT&
        tensorComponent);

    bool isTensorComplete() const;

  private:
    CBuildTensor(const CBuildTensor& rhs);
    CBuildTensor& operator = (const CBuildTensor& rhs);

#if 0
    // prevent heap allocation
    void* operator new (size_t);
    void* operator new [] (size_t);

    void operator delete (void*);
    void operator delete [] (void*);
#endif
    QSharedPointer <CBuildTensorImpl> m_Tensor;
};

} // namespace GeomecRGI

#endif  // _BuildTensor_h_
