#ifndef _LoadPropertyDeltaTemperature_h_
#define _LoadPropertyDeltaTemperature_h_

#include "LoadPropertyTemperatureBase.h"

namespace GeomecRGI
{

class CLoadPropertyDeltaTemperature : public CLoadPropertyTemperatureBase
{
  public:
    CLoadPropertyDeltaTemperature(const RGProperty& rgProperty,
      RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CLoadPropertyDeltaTemperature();

  protected:
    virtual void convertValue(geo::CValue& dValue) const;
    virtual bool addDelta(std::vector <geo::CValue>& vcNodalValues,
      const QString& strPropertyName, const CFormationBase* pFormation,
      const geo::IElement& elm, int nNod) const;
    virtual IValueDomainScalar::TValueVec addDeltaSpecific(
      const CDepletionStage& prevstage, const CFormationBase* pFormation,
      const geo::IElement& elm, int nNod) const;

  private:
    CLoadPropertyDeltaTemperature(const CLoadPropertyDeltaTemperature& rhs);
    CLoadPropertyDeltaTemperature& operator = (
      const CLoadPropertyDeltaTemperature& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyDeltaTemperature_h_
