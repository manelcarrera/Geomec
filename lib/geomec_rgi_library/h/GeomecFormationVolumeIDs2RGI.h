#ifndef _GeomecFormationVolumeIDs2RGI_h_
#define _GeomecFormationVolumeIDs2RGI_h_

class RGInterface;
class CModelBase;
class CGraphNode;

#include <vector>
#include <set>

namespace GeomecRGI
{

class CGeomecFormationVolumeIDs2RGI
{
  public:
  CGeomecFormationVolumeIDs2RGI(RGInterface& rgi,
      const CModelBase& modelBase);
  ~CGeomecFormationVolumeIDs2RGI();

  std::vector <int> getFormationVolumeIDs() const;

  private:
  CGeomecFormationVolumeIDs2RGI(const CGeomecFormationVolumeIDs2RGI& rhs);
  CGeomecFormationVolumeIDs2RGI& operator = (
      const CGeomecFormationVolumeIDs2RGI& rhs);

  const CModelBase& m_modelBase;
  std::vector <CGraphNode*> m_formationVolumes;
  std::vector <int> m_GeomecFormationVolumeIDs;
  std::set <int> m_distinctGeomecFormationVolumeIDs;
};

} // namespace GeomecRGI

#endif  // _GeomecFormationVolumeIDs2RGI_h_
