#ifndef _GeomecSurfaces2RGI_h_
#define _GeomecSurfaces2RGI_h_

namespace GeomecRGI
{

class CRockMechProcessor;

} // namespace GeomecRGI

class RGInterface;
class CModelBase;
class C3DHorizon;
class CHexaHorizon;
class CFormationPlane;

namespace geo
{

class ISurface;

} // namespace geo

#include "RGSurface.h"

namespace GeomecRGI
{

class CGeomecSurfaces2RGI
{
  public:
  CGeomecSurfaces2RGI(RGInterface& rgi, const CRockMechProcessor& rmp,
      const CModelBase& modelBase);
  ~CGeomecSurfaces2RGI();

  void saveSurfaces();

  private:
  CGeomecSurfaces2RGI(const CGeomecSurfaces2RGI& rhs);
  CGeomecSurfaces2RGI& operator = (const CGeomecSurfaces2RGI& rhs);

  std::vector <int> retrieveSurfaceNodeIndices(const geo::ISurface& surface)
      const;

  void saveFaults();
  void saveHorizon(const C3DHorizon* horizonBase,
      RGSurface::Type type, RGSurface::Attribute attribute);
  void saveHorizon(const CHexaHorizon *horizon, const CFormationPlane *plane, RGSurface::Type type, RGSurface::Attribute attribute);
  void saveHorizons();
  void saveSideSurfaces();

  RGInterface& m_rgi;
  const CRockMechProcessor& m_rmp;
  const CModelBase& m_modelBase;
};

} // namespace GeomecRGI

#endif  // _GeomecSurfaces2RGI_h_
