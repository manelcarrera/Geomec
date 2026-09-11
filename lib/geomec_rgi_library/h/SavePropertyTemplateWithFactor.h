#ifndef _SavePropertyTemplateWithFactor_h_
#define _SavePropertyTemplateWithFactor_h_

#include "SavePropertyTemplate.h"
#include "RGUtils.h"

namespace GeomecRGI
{

template <unsigned int VALUETYPE>
  class CSavePropertyTemplateWithFactor :
    public CSavePropertyTemplate <VALUETYPE>
{
  public:
    CSavePropertyTemplateWithFactor(const RGProperty& rgProperty,
      double factor);
    virtual ~CSavePropertyTemplateWithFactor();

    virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
    CSavePropertyTemplateWithFactor(const CSavePropertyTemplateWithFactor& rhs);
    CSavePropertyTemplateWithFactor& operator = (
      const CSavePropertyTemplateWithFactor& rhs);

    double m_factor;
};

template <unsigned int VALUETYPE>
  CSavePropertyTemplateWithFactor <VALUETYPE> ::CSavePropertyTemplateWithFactor(
    const RGProperty& rgProperty, double factor)
  : CSavePropertyTemplate <VALUETYPE> (rgProperty)
  , m_factor(factor)
{
}

template <unsigned int VALUETYPE>
  CSavePropertyTemplateWithFactor <VALUETYPE> ::
    ~CSavePropertyTemplateWithFactor()
{
}

struct CMultiply
{
  CMultiply(double factor)
  : m_factor(factor)
  {
  }

  void operator () (double& rhs) const
  {
    if (!RGUtils::isNull(rhs))
      rhs *= m_factor;
  }

  double m_factor;
};

template <unsigned int VALUETYPE>
  bool CSavePropertyTemplateWithFactor <VALUETYPE> ::saveProperty(
    RGInterface& rgi, CModelBase& modelBase, const CRockMechProcessor& rmp)
{
  std::vector <double> values(modelBase.Mesh().Mesh().ElementSize());

  CSavePropertyTemplate <VALUETYPE> ::saveProperty(rgi, modelBase, values, rmp);

  CMultiply multiplier(m_factor);

  std::for_each(values.begin(), values.end(), multiplier);

  rgi.saveProperty(this->m_RGProperty, values);

  return true;
}

} // namespace GeomecRGI

#endif  // _SavePropertyTemplateWithFactor_h_
