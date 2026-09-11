#ifndef _SavePropertyFracDens_h_
#define _SavePropertyFracDens_h_

class CFFMaterial;
class CMaterialFractureApertureBase;

#include "SavePropertyBase.h"
#include "Vector.h"

namespace GeomecRGI
{

class CSavePropertyFracDens : public CSavePropertyBase
{
  public:
  enum TDensityDirection
  {
      HIGH,         // Maps to propFractDens_1...
      INTERMEDIATE, // Maps to propFractDens_2...
      LOW           // Maps to propFractDens_3...
  };

  enum TVectorDirection
  {
      X,
      Y,
      Z
  };

  CSavePropertyFracDens(const RGProperty& rgProperty,
      TDensityDirection densityDirection, TVectorDirection vectorDirection);
  virtual ~CSavePropertyFracDens();

  virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
  CSavePropertyFracDens(const CSavePropertyFracDens& rhs);
  CSavePropertyFracDens& operator = (
      const CSavePropertyFracDens& rhs);

  double getLength(const CFFMaterial& cffMaterial) const;
  geo::CVector getVector(const CFFMaterial& cffMaterial,
      const CMaterialFractureApertureBase& materialFractureApertureBase) const;
  double getVectorComponent(const geo::CVector& vector, double length) const;

  TDensityDirection m_DensityDirection;
  TVectorDirection m_VectorDirection;
};

} // namespace GeomecRGI

#endif  // _SavePropertyFracDens_h_
