#ifndef _LoadPropertyTemperatureBase_h_
#define _LoadPropertyTemperatureBase_h_

#include "LoadPropertyTemplate.h"

namespace GeomecRGI
{

class CLoadPropertyTemperatureBase :
  public CLoadPropertyTemplate <IDT_VALUETYPE_TEMPERATURE>
{
  public:
  CLoadPropertyTemperatureBase(const RGProperty& rgProperty, RGInterface& rgi,
      CModelBase& modelBase, CRockMechProcessor& rmp);
  virtual ~CLoadPropertyTemperatureBase();

  protected:
  virtual void linkValueTypes(CFormationBase& formation, CValueType* pVT,
      const CDepletionStage& stage, bool fluidPressureFracDetected) const;
  virtual void convertValue(geo::CValue& dValue) const;

  private:
  CLoadPropertyTemperatureBase(const CLoadPropertyTemperatureBase& rhs);
  CLoadPropertyTemperatureBase& operator = (
      const CLoadPropertyTemperatureBase& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyTemperatureBase_h_
