#ifndef _LoadPropertyDeltaFluidPressure_h_
#define _LoadPropertyDeltaFluidPressure_h_

#include "LoadPropertyPressureBase.h"

namespace GeomecRGI
{

class CLoadPropertyDeltaFluidPressure : public CLoadPropertyPressureBase
{
  public:
  CLoadPropertyDeltaFluidPressure(const RGProperty& rgProperty,
      RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp);
  virtual ~CLoadPropertyDeltaFluidPressure();

  protected:
  virtual unsigned int OverruleValueType(unsigned int uValueType) const;

  virtual bool addDelta(std::vector <geo::CValue>& vcNodalValues,
      const QString& strPropertyName, const CFormationBase* pFormation,
      const geo::IElement& elm, int nNod) const;
  virtual IValueDomainScalar::TValueVec addDeltaSpecific(
      const CDepletionStage& prevstage, const CFormationBase* pFormation,
      const geo::IElement& elm, int nNod) const;

  private:
  CLoadPropertyDeltaFluidPressure(const CLoadPropertyDeltaFluidPressure& rhs);
  CLoadPropertyDeltaFluidPressure& operator = (
      const CLoadPropertyDeltaFluidPressure& rhs);

  mutable bool m_useFractureMatrixPressure;
};

} // namespace GeomecRGI

#endif  // _LoadPropertyDeltaFluidPressure_h_
