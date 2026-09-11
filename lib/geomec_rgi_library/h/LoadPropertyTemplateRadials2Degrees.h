#ifndef _LoadPropertyTemplateRadials2Degrees_h_
#define _LoadPropertyTemplateRadials2Degrees_h_

#include "LoadPropertyTemplate.h"

namespace GeomecRGI
{

template <unsigned int VALUETYPE>
  class CLoadPropertyTemplateRadials2Degrees :
  public CLoadPropertyTemplate <VALUETYPE>
{
  public:
  CLoadPropertyTemplateRadials2Degrees(const RGProperty& rgProperty,
      RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp);
  virtual ~CLoadPropertyTemplateRadials2Degrees();

  protected:
  virtual void convertValue(geo::CValue& dValue) const;

  private:
  CLoadPropertyTemplateRadials2Degrees(
      const CLoadPropertyTemplateRadials2Degrees& rhs);
  CLoadPropertyTemplateRadials2Degrees& operator = (
      const CLoadPropertyTemplateRadials2Degrees& rhs);
};

template <unsigned int VALUETYPE>
  CLoadPropertyTemplateRadials2Degrees <VALUETYPE> ::
  CLoadPropertyTemplateRadials2Degrees(const RGProperty& rgProperty,
  RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp)
  : CLoadPropertyTemplate <VALUETYPE> (rgProperty, rgi, modelBase, rmp)
{
}

template <unsigned int VALUETYPE>
  CLoadPropertyTemplateRadials2Degrees <VALUETYPE> ::
  ~CLoadPropertyTemplateRadials2Degrees()
{
}

// protected

template <unsigned int VALUETYPE>
  void CLoadPropertyTemplateRadials2Degrees <VALUETYPE> ::convertValue(
  geo::CValue& dValue) const
{
  if (dValue.Valid())
  {
  dValue.Value(RAD2DEG(dValue.Value()));
  }
}

} // namespace GeomecRGI

#endif  // _LoadPropertyTemplateRadials2Degrees_h_
