#ifndef _CalculateShearModulus_h_
#define _CalculateShearModulus_h_

#include "LoadPropertyBase.h"
#include "RGProperty.h"
#include "LoadPropertyShearModulus.h"
#include "RockMechProcessor.h"

namespace GeomecRGI
{

class CCalculateShearModulus : public CLoadPropertyBase
{
  public:
    CCalculateShearModulus(const RGProperty& rgProperty, RGInterface& rgi,
      CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CCalculateShearModulus();

    template <typename P, unsigned int VALUE_TYPE, unsigned int NORMAL,
      unsigned int TRANSVERSE> bool loadProperty();

    virtual bool enforceUniqueness(unsigned int uValueType) const;

  private:
    CCalculateShearModulus(const CCalculateShearModulus& rhs);
    CCalculateShearModulus& operator = (const CCalculateShearModulus& rhs);
};

template <typename P, unsigned int VALUE_TYPE, unsigned int NORMAL,
  unsigned int TRANSVERSE> bool CCalculateShearModulus::loadProperty()
{
  bool returnValue = CLoadPropertyBase::loadProperty(VALUE_TYPE);

  if (returnValue && isAnisotropic())
  {
    P normal(m_RGProperty, m_RGI, m_ModelBase, m_RMP);
    P transverse(m_RGProperty, m_RGI, m_ModelBase, m_RMP);

    returnValue = returnValue && normal.loadProperty(NORMAL) &&
      transverse.loadProperty(TRANSVERSE);

    RGProperty rgProperty(RGPropertyType::propElementShearModulus);
    CLoadPropertyShearModulus
      loadPropertyShearModulus(rgProperty, m_RGI, m_ModelBase, m_RMP);

    if (loadPropertyShearModulus.canCalculatePropertyValues())
    {
      std::vector <std::vector <geo::CValue> > vcValues =
        loadPropertyShearModulus.calculatePropertyValues();

      returnValue = returnValue &&
        loadPropertyShearModulus.loadProperty(vcValues,
          m_RMP.PropertyName(rgProperty), IDT_VALUETYPE_SHEARMODULUS);
    }
  }

  return returnValue;
}

} // namespace GeomecRGI

#endif  // _CalculateShearModulus_h_
