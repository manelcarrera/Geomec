#ifndef _LoadPropertyDeltaFluidPressureFrac_h_
#define _LoadPropertyDeltaFluidPressureFrac_h_

#include "LoadPropertyPressureBase.h"

namespace GeomecRGI
{

class CLoadPropertyDeltaFluidPressureFrac : public CLoadPropertyPressureBase
{
  public:
    CLoadPropertyDeltaFluidPressureFrac(const RGProperty& rgProperty,
      RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CLoadPropertyDeltaFluidPressureFrac();

  protected:
    virtual bool addDelta(std::vector <geo::CValue>& vcNodalValues,
      const QString& strPropertyName, const CFormationBase* pFormation,
      const geo::IElement& elm, int nNod) const;
    virtual IValueDomainScalar::TValueVec addDeltaSpecific(
      const CDepletionStage& prevstage, const CFormationBase* pFormation,
      const geo::IElement& elm, int nNod) const;

  private:
    CLoadPropertyDeltaFluidPressureFrac(const CLoadPropertyDeltaFluidPressureFrac& rhs);
    CLoadPropertyDeltaFluidPressureFrac& operator = (
      const CLoadPropertyDeltaFluidPressureFrac& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyDeltaFluidPressureFrac_h_
