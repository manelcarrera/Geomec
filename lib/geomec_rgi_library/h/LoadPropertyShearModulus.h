#ifndef _LoadPropertyShearModulus_h_
#define _LoadPropertyShearModulus_h_

#include "LoadPropertyTemplate.h"

namespace GeomecRGI
{

class CLoadPropertyShearModulus :
  public CLoadPropertyTemplate <IDT_VALUETYPE_SHEARMODULUS>
{
  public:
    using CLoadPropertyBase::loadProperty;

    CLoadPropertyShearModulus(const RGProperty& rgProperty, RGInterface& rgi,
      CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CLoadPropertyShearModulus();

    bool canCalculatePropertyValues() const;
    std::vector <std::vector <geo::CValue> > calculatePropertyValues() const;

    virtual bool enforceUniqueness(unsigned int uValueType) const;

  private:
    CLoadPropertyShearModulus(const CLoadPropertyShearModulus& rhs);
    CLoadPropertyShearModulus& operator = (
      const CLoadPropertyShearModulus& rhs);

    virtual void convertValue(geo::CValue& dValue) const;

    void retrieveValueTypes(CValueType** normalYoungsModulus,
      CValueType** transverseYoungsModulus, CValueType** normalPoissonsRatio,
      CValueType** transversePoissonsRatio) const;

    CValueType* m_NormalYoungsModulus;
    CValueType* m_TransverseYoungsModulus;
    CValueType* m_NormalPoissonsRatio;
    CValueType* m_TransversePoissonsRatio;
};

} // namespace GeomecRGI

#endif  // _LoadPropertyShearModulus_h_
