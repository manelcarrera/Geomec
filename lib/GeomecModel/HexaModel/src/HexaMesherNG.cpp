#include "HexaMesherNG.h"

#include "IProgressBase.h"
#include "GlobalMessage.h"

#include "HexaMesh.h"
#include "HexaHorizon.h"
#include "HexaFormation.h"
#include "HexaEntryTypes.h"
#include "HexaBoundary.h"
#include "hexameshregion.h"
#include "HexaModel.h"
#include "InterfaceElement.h"
#include "BaseEntryTypes.h"
#include "Global.h"
#include "IPlatform.h"
#include "ElementGroup.h"
#include "Hexahedron.h"
#include "ISupport.h"
#include "ModifiedHint.h"
#include "PressureSupportNode.h"
#include "RotatedSystem.h"
#include "Triangle.h"



#include <algorithm>

void print_tyings(std::map<int, std::pair<int, int> >& tyings)
{
  FILE *fp = fopen("E:\\Temp\\_ty_0.txt", "a");
  fprintf(fp, "----\n");
  int i = 0;
  for (std::map<int, std::pair<int, int> >::iterator it = tyings.begin(); it != tyings.end(); ++it)
  {
    fprintf(fp, "%d:\t%d -> (%d, %d)\n", i++, it->first, it->second.first, it->second.second);
  }

  fprintf(fp, "----\n");
  fclose(fp);
}


CHexaMesherNG::CHexaMesherNG(CHexaMesh& mesh)
  : IHexaMesher(mesh)
  , m_metaMesh(mesh)
  , m_bCanMesh(true)
{
}

CHexaMesherNG::~CHexaMesherNG()
{
}

int CHexaMesherNG::MesherType() const
{
  return NEXT_GEN;
}

bool CHexaMesherNG::CreateMesh(IProgressBase& progress)
{
  if (!ValidRegions())
  {
    _m()->msg("Invalid mesh regions. Check your input.");
    return false;
  }

  if (ProblematicRegions())
  {
    _m()->msg("Mesh regions with special refinements extend to the border of the main mesh and the tyings will clash with supports. Check your input.");
    return false;
  }

  m_metaMesh.clear();
  m_metaMesh.CreateHexaMetaMesh(progress);

  //print_tyings(m_metaMesh.mpCreatedTyings);
  m_bCanMesh = false;

  return true;
}

bool CHexaMesherNG::ProblematicRegions()
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(model.GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();

  CHexaMainMeshRegion *pMainRegion = 0;
  std::set<CHexaSubMeshRegion *> stSubRegions;

  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); ++itMeshRegion)
  {
    if (dynamic_cast<CHexaMainMeshRegion*>(*itMeshRegion))
      pMainRegion = static_cast<CHexaMainMeshRegion *>(*itMeshRegion);
    else if (dynamic_cast<CHexaSubMeshRegion *>(*itMeshRegion) && !static_cast<CHexaSubMeshRegion *>(*itMeshRegion)->FullModel())
      stSubRegions.insert(static_cast<CHexaSubMeshRegion *>(*itMeshRegion));
  }

  if (!pMainRegion)
    return true; // shouldn't happen

  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  geo::CPoint ptMeshMin = rs.LocalMin(pMainRegion->Hull());
  geo::CPoint ptMeshMax = rs.LocalMax(pMainRegion->Hull());

  for (std::set<CHexaSubMeshRegion *>::iterator itSubMeshRegion = stSubRegions.begin(); itSubMeshRegion != stSubRegions.end(); ++itSubMeshRegion)
  {
    geo::CPoint ptRegionMin = rs.LocalMin((*itSubMeshRegion)->Hull());
    geo::CPoint ptRegionMax = rs.LocalMax((*itSubMeshRegion)->Hull());
    bool bInside = (ptRegionMin.X() > ptMeshMin.X() && ptRegionMin.Y() > ptMeshMin.Y() && ptRegionMax.X() < ptMeshMax.X() && ptRegionMax.Y() < ptMeshMax.Y());

    if (!bInside)
      return true;
  }

  return false;
}

void CHexaMesherNG::CreateTyings(IProgressBase& progress)
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis);

  std::vector<geo::CTying::TMasterDef> vcMasters(3);

  for (hmm::TTyingMap::iterator it = m_metaMesh.CreatedTyings().begin(); it != m_metaMesh.CreatedTyings().end(); ++it)
  {
    std::vector<hmm::TTyingInfo>& v = it->second;
    int i = 0;
    switch (v.size())
    {
    case 6:
      Mesh().m_mesh.CreateTying(it->first, rsNormal[v[i].axis], geo::CTying::TR,
        v[i].master, rsNormal[v[i].axis], geo::CTying::TR, v[i].distance,
        v[i + 1].master, rsNormal[v[i + 1].axis], geo::CTying::TR, v[i + 1].distance);
       i += 2;
        /* Fall-through */
    case 4:
      Mesh().m_mesh.CreateTying(it->first, rsNormal[v[i].axis], geo::CTying::TR,
        v[i].master, rsNormal[v[i].axis], geo::CTying::TR, v[i].distance,
        v[i + 1].master, rsNormal[v[i + 1].axis], geo::CTying::TR, v[i + 1].distance);
      i += 2;
      Mesh().m_mesh.CreateTying(it->first, rsNormal[v[i].axis], geo::CTying::TR,
        v[i].master, rsNormal[v[i].axis], geo::CTying::TR, v[i].distance,
        v[i + 1].master, rsNormal[v[i + 1].axis], geo::CTying::TR, v[i + 1].distance);
      break;
    case 9:
      for (int j = 0; j < 3; ++j)
      {
        int axisIndex = v[i].axis;

        for (int k = 0; k < 3; ++k)
        {
          vcMasters[k].direction = &rsNormal[v[i].axis];
          vcMasters[k].node = v[i].master;
          vcMasters[k].type = geo::CTying::TR;
          vcMasters[k].factor = v[i].distance;
          ++i;
        }

        Mesh().m_mesh.CreateTying(it->first, rsNormal[axisIndex], geo::CTying::TR, vcMasters);
      }
    }
  }
}

bool CHexaMesherNG::CanCreateMesh()
{
  return m_bCanMesh;
}

void CHexaMesherNG::Invalidate()
{
  m_metaMesh.clear();
  m_bCanMesh = true;
}

void CHexaMesherNG::LoadStream(CArchiveInterface& stream, CStreamVersion& version, IProgressBase& progress)
{
  int size;
  stream >> size;

  hmm::TTyingMap::iterator it = m_metaMesh.CreatedTyings().end();

  for (int i = 0; i < size; ++i)
  {
    int slave, info_size;
    stream >> slave;
    stream >> info_size;
    std::vector<hmm::TTyingInfo> v(info_size);
    for (int j = 0; j < info_size; ++j)
    {
      stream >> v[j].axis;
      stream >> v[j].master;
      stream >> v[j].distance;
    }
    it = m_metaMesh.CreatedTyings().insert(it, std::make_pair(slave, v));
  }

  if (version > CStreamVersion(4, 1, 61))
  {
    int flag;
    stream >> flag;
    m_bCanMesh = flag != 0;
  }
}

void CHexaMesherNG::SaveStream(CArchiveInterface& stream, IProgressBase& progress)
{
  stream << (int)m_metaMesh.CreatedTyings().size();

  for (hmm::TTyingMap::iterator it = m_metaMesh.CreatedTyings().begin(); it != m_metaMesh.CreatedTyings().end(); ++it)
  {
    stream << it->first;
    stream << (int)it->second.size();
    for (std::vector<hmm::TTyingInfo>::iterator info = it->second.begin(); info != it->second.end(); ++info)
    {
      stream << info->axis;
      stream << info->master;
      stream << info->distance;
    }
  }

  stream << (int)m_bCanMesh;
}

long CHexaMesherNG::SavedItems() const
{
  return const_cast<hmm::CHexaMetaMesh&>(m_metaMesh).CreatedTyings().size();
}

