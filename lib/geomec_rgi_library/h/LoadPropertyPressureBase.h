#ifndef _LoadPropertyPressureBase_h_
#define _LoadPropertyPressureBase_h_

#include "LoadPropertyTemplate.h"

namespace GeomecRGI
{

class CLoadPropertyPressureBase :
  public CLoadPropertyTemplate <IDT_VALUETYPE_PRESSURE>
{
  public:
    CLoadPropertyPressureBase(const RGProperty& rgProperty,
      RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CLoadPropertyPressureBase();

  protected:
    virtual void linkValueTypes(CFormationBase& formation, CValueType* pVT,
      const CDepletionStage& stage, bool fluidPressureFracDetected) const;
    virtual void convertValue(geo::CValue& dValue) const;

  private:
    CLoadPropertyPressureBase(const CLoadPropertyPressureBase& rhs);
    CLoadPropertyPressureBase& operator = (
      const CLoadPropertyPressureBase& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyPressureBase_h_
