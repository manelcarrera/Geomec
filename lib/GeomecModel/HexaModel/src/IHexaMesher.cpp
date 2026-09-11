#include "IHexaMesher.h"

#include "HexaBoundary.h"
#include "HexaEntryTypes.h"
#include "HexaMesh.h"
#include "HexaModel.h"
#include "RotatedSystem.h"
#include "hexameshregion.h"

bool IHexaMesher::CXYLess::operator()(const geo::IPoint &P1, const geo::IPoint &P2) const {
  if (P1.Y() - P2.Y() < -EPS)
    return true;

  if (P1.Y() - P2.Y() > EPS)
    return false;

  if (P1.X() - P2.X() < -EPS)
    return true;

  return false;
}

IHexaMesher::IHexaMesher(CHexaMesh &mesh) : m_hexamesh(mesh) {}

IHexaMesher::~IHexaMesher() {}

CHexaMesh &IHexaMesher::Mesh() { return m_hexamesh; }

CModelBase &IHexaMesher::Model() { return static_cast<CModelBase &>(Mesh().Model()); }

bool IHexaMesher::ValidRegions() {
  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry *)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();

  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end();
       ++itMeshRegion) {
    if ((*itMeshRegion)->Hull().NrOfPoints() < 4)
      delete *itMeshRegion;
  }

  const CModelBase &model = static_cast<const CModelBase &>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  stNode = pEntry->EntryNodes();
  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end();
       ++itMeshRegion) {
    geo::CPoint ptMeshRegionMin = rs.LocalMin((*itMeshRegion)->Hull());
    geo::CPoint ptMeshRegionMax = rs.LocalMax((*itMeshRegion)->Hull());

    if (dynamic_cast<CHexaSubMeshRegion *>(*itMeshRegion)) {
      // only when current meshregion is not the main mesh region.
      for (CHexaMeshRegionEntry::TNodeSet::iterator itOtherRegion = stNode.begin(); itOtherRegion != stNode.end();
           ++itOtherRegion) {
        if (*itOtherRegion == *itMeshRegion)
          continue;

        geo::CPoint ptRegionMin = rs.LocalMin((*itOtherRegion)->Hull());
        geo::CPoint ptRegionMax = rs.LocalMax((*itOtherRegion)->Hull());
        bool bInside = (ptRegionMin.X() > ptMeshRegionMin.X() && ptRegionMin.Y() > ptMeshRegionMin.Y() &&
                        ptRegionMax.X() < ptMeshRegionMax.X() && ptRegionMax.Y() < ptMeshRegionMax.Y());

        if (bInside) {
          // don't allow smaller gridsizes
          if ((*itOtherRegion)->GridSizeX() > (*itMeshRegion)->GridSizeX() ||
              (*itOtherRegion)->GridSizeY() > (*itMeshRegion)->GridSizeY()) {
            return false;
          }
        }
      }
    }
  }

  return true;
}

void IHexaMesher::CreateTyings() {
  IProgressBase progress;

  CreateTyings(progress);
}
