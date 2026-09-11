#ifndef _LoadPropertyTemplateWithFactor_h_
#define _LoadPropertyTemplateWithFactor_h_

#include "LoadPropertyTemplate.h"

namespace GeomecRGI
{

template <unsigned int VALUETYPE>
  class CLoadPropertyTemplateWithFactor :
    public CLoadPropertyTemplate <VALUETYPE>
{
  public:
    CLoadPropertyTemplateWithFactor(const RGProperty& rgProperty,
      RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp,
      double factor);
    virtual ~CLoadPropertyTemplateWithFactor();

  protected:
    virtual void convertValue(geo::CValue& dValue) const;

  private:
    CLoadPropertyTemplateWithFactor(const CLoadPropertyTemplateWithFactor& rhs);
    CLoadPropertyTemplateWithFactor& operator = (
      const CLoadPropertyTemplateWithFactor& rhs);

    double m_factor;
};

template <unsigned int VALUETYPE>
  CLoadPropertyTemplateWithFactor <VALUETYPE> ::CLoadPropertyTemplateWithFactor(
    const RGProperty& rgProperty, RGInterface& rgi, CModelBase& modelBase,
    CRockMechProcessor& rmp, double factor)
  : CLoadPropertyTemplate <VALUETYPE> (rgProperty, rgi, modelBase, rmp)
  , m_factor(factor)
{
}

template <unsigned int VALUETYPE>
  CLoadPropertyTemplateWithFactor <VALUETYPE> ::
    ~CLoadPropertyTemplateWithFactor()
{
}

// protected

template <unsigned int VALUETYPE>
  void CLoadPropertyTemplateWithFactor <VALUETYPE> ::convertValue(
    geo::CValue& dValue) const
{
  if (dValue.Valid())
  {
    dValue.Value(dValue.Value() * 1e-6);  // from Pa to MPa
  }
}

} // namespace GeomecRGI

#endif  // _LoadPropertyTemplateWithFactor_h_
