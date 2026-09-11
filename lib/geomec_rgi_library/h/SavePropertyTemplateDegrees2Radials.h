#ifndef _SavePropertyTemplateDegrees2Radials_h_
#define _SavePropertyTemplateDegrees2Radials_h_

#include "SavePropertyTemplate.h"
#include "RGUtils.h"

namespace GeomecRGI
{

template <unsigned int VALUETYPE>
  class CSavePropertyTemplateDegrees2Radials :
    public CSavePropertyTemplate <VALUETYPE>
{
  public:
    CSavePropertyTemplateDegrees2Radials(const RGProperty& rgProperty);
    virtual ~CSavePropertyTemplateDegrees2Radials();

    virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
    CSavePropertyTemplateDegrees2Radials(
      const CSavePropertyTemplateDegrees2Radials& rhs);
    CSavePropertyTemplateDegrees2Radials& operator = (
      const CSavePropertyTemplateDegrees2Radials& rhs);
};

template <unsigned int VALUETYPE>
  CSavePropertyTemplateDegrees2Radials <VALUETYPE> ::
    CSavePropertyTemplateDegrees2Radials(const RGProperty& rgProperty)
  : CSavePropertyTemplate <VALUETYPE> (rgProperty)
{
}

template <unsigned int VALUETYPE>
  CSavePropertyTemplateDegrees2Radials <VALUETYPE> ::
    ~CSavePropertyTemplateDegrees2Radials()
{
}

struct CDegrees2Radials
{
  CDegrees2Radials()
  {
  }

  void operator () (double& rhs) const
  {
    if (!RGUtils::isNull(rhs))
    {
      rhs = DEG2RAD(rhs);
    }
  }
};

template <unsigned int VALUETYPE>
  bool CSavePropertyTemplateDegrees2Radials <VALUETYPE> ::saveProperty(
    RGInterface& rgi, CModelBase& modelBase, const CRockMechProcessor& rmp)
{
  std::vector <double> values(modelBase.Mesh().Mesh().ElementSize());

  CSavePropertyTemplate <VALUETYPE> ::saveProperty(rgi, modelBase, values, rmp);

  CDegrees2Radials degrees2Radials;

  std::for_each(values.begin(), values.end(), degrees2Radials);

  rgi.saveProperty(this->m_RGProperty, values);

  return true;
}

} // namespace GeomecRGI

#endif  // _SavePropertyTemplateDegrees2Radials_h_
