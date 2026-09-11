#ifndef _SavePropertyPermMultFrac_h_
#define _SavePropertyPermMultFrac_h_

class CFFMaterial;
class CMaterialFractureApertureBase;
class IScalarResultComponent;
class CDepletionStage;

namespace geo
{

  class IElement;

} // namespace geo

#include "SavePropertyBase.h"
#include "Vector.h"

namespace GeomecRGI
{

class CSavePropertyPermMultFrac : public CSavePropertyBase
{
  public:
  enum TDensityDirection
  {
      HIGH,         // Maps to propFractDens_1...
      INTERMEDIATE, // Maps to propFractDens_2...
      LOW           // Maps to propFractDens_3...
  };

  CSavePropertyPermMultFrac(const RGProperty& rgProperty,
      TDensityDirection densityDirection);
  virtual ~CSavePropertyPermMultFrac();

  virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
  CSavePropertyPermMultFrac(const CSavePropertyPermMultFrac& rhs);
  CSavePropertyPermMultFrac& operator = (
      const CSavePropertyPermMultFrac& rhs);

  std::pair <double, bool> getLength(
      const IScalarResultComponent* scalarResultComponent,
      const CModelBase& modelBase, const geo::IElement& element) const;
  const IScalarResultComponent* getScalarResultComponent(
      CModelBase& modelBase, const CDepletionStage& depletionStage) const;
  geo::CVector getVector(const CFFMaterial& cffMaterial,
      const CMaterialFractureApertureBase& materialFractureApertureBase) const;
  double getVectorComponent(const geo::CVector& vector, double length) const;

  TDensityDirection m_DensityDirection;
};

} // namespace GeomecRGI

#endif  // _SavePropertyPermMultFrac_h_
