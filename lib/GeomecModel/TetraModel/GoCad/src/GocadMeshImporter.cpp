#ifdef SKUA_NEW

#include "GocadMeshImporter.h"

#include "TetMeshBase.h"

#include "TetraModel.h"
#include "TetraMesh.h"
#include "TetraFormation.h"
#include "TetraSubHorizon.h"
#include "TetraSubBoundary.h"
#include "TetraEntryTypes.h"

#include "Surface.h"
#include "SurfaceDesc.h"
#include "Tetrahedron.h"
#include "TetSurface.h"
#include "TiedBodyTetSurface.h"
#include "BodyTriangle.h"
#include "TiedPointBodyTriangle.h"

#include "IProgressFactory.h"
#include "Global.h"
#include "IProgressBase.h"

#include "SKUAParseData.h"
#include "DSFIntermediateSurfaceGenerator.h"

#include <algorithm>
#include <cstring>


#include <QString>

void lg(const QString& st)
{
#if 0
  FILE *fp = fopen("C:\\Local\\new_log.txt", "a");
  fprintf(fp, st.toStdString().c_str());
  fprintf(fp, "\n");
  fclose(fp);
#endif
}


geo::CPoint Node2Point(const CGocadData::CNode & node)
{
  return geo::CPoint(node.X(), node.Y(), node.Z());
}



CGocadMeshImporter::CGocadMeshImporter(const gm_skua::SKUAParseData *solid, CTetraMesh& tetramesh)
  : m_inputSolids(1, solid)
  , m_solid(solid)
  , m_tetramesh(tetramesh)
{
}

CGocadMeshImporter::CGocadMeshImporter(const std::vector<const gm_skua::SKUAParseData *>& solids, CTetraMesh& tetramesh)
  : m_inputSolids(solids)
  , m_solid(solids.empty() ? nullptr : solids.front())
  , m_tetramesh(tetramesh)
{
  // TODO: if we have more than one input solid, we merge them; routine to be added to SKUAConnector library
}

CGocadMeshImporter::~CGocadMeshImporter()
{
}

bool CGocadMeshImporter::Import()
{
  if (!m_solid)
  return true;

  CTetraSubHorizon* pTop = new CTetraSubHorizon("Top Horizon", FALSE, Model());
  CTetraSubHorizon* pBottom = new CTetraSubHorizon("Bottom Horizon", FALSE, Model());

  CTetraSubBoundary& boundary = ((CTetraModel&)Model()).SubBoundary();
  boundary.TopHorizon().LinkTo(*pTop);
  boundary.BottomHorizon().LinkTo(*pBottom);

  std::auto_ptr <IProgressBase> progress(_g->prog()->create(eProgress::Dual, "Import Gocad mesh", true, 6 ));

  try
  {
  std::vector<geo::CSurfaceDesc *> surface_descriptions;
  surface_descriptions.reserve(100);

  std::vector<CSurfaceBase *> mesh_surfaces;

  std::vector<int> re_index_tetras(m_solid->tetras.size());

  if (!CreateNodes(*progress))
      return false;

  if (!CreateTetrahedrons(*progress, re_index_tetras))
      return false;

  if (!CreateVolumes(*progress, re_index_tetras))
      return false;

  if (!CreateFormations(*progress))
      return false;

  if (!CreateSurfaces(*progress, surface_descriptions, mesh_surfaces, re_index_tetras))
      return false;

  if (!CreateInterfaceElements(*progress, surface_descriptions, re_index_tetras))
      return false;

  if (!CreateIntermediateSurfaces(*progress, surface_descriptions))
      return false;
  }
  catch (CProgressCancel* e)
  {
  delete e;
  return false;
  }

  return true;
}

CTetraModel& CGocadMeshImporter::Model()
{
  return static_cast<CTetraModel&>(m_tetramesh.Model());
}

const CTetraModel& CGocadMeshImporter::Model() const
{
  return static_cast<const CTetraModel&>(m_tetramesh.Model());
}

const geo::CTetMeshBase& CGocadMeshImporter::TetMesh() const
{
  return static_cast<geo::CTetMeshBase&>(m_tetramesh.Mesh());
}

geo::CTetMeshBase& CGocadMeshImporter::TetMesh()
{
  return static_cast<geo::CTetMeshBase&>(m_tetramesh.Mesh());
}

bool CGocadMeshImporter::CreateNodes(IProgressBase& progress)
{
  size_t size_vertices = m_solid->vertices.size();

  progress.NextJob("Create nodes");
  progress.AddSteps(size_vertices);

  geo::CPoint point;
 
  for (size_t i = 0; i < size_vertices; ++i)
  {
  const gm_skua::SKUAParseData::Vertex& vertex = m_solid->get_vertex(i);

  point.Set(vertex.coordinate[1], vertex.coordinate[0], vertex.coordinate[2]);
  TetMesh().RegisterNodeInMainMesh(point, vertex.elements);

  progress.Step();
  }

  return true;
}

bool CGocadMeshImporter::CreateTetrahedrons(IProgressBase& progress, std::vector<int>& re_index_tetras)
{
  std::vector<int> nodes(4);
  const std::vector<gm_skua::SKUAParseData::Tetra>& tetras = m_solid->tetras;
  const std::vector<int>& volume_types = m_solid->volume_types;

  progress.NextJob("Create tetrahedrons");
  progress.AddSteps(tetras.size());

  for (size_t i = 0; i < tetras.size(); ++i)
  {
  int lastTetra = -1;
  const gm_skua::SKUAParseData::Tetra& tetra = tetras[i];
  if (canHandleVolume(volume_types[tetra.model_region]))
  {
      for (int j = 0; j < 4; ++j)
    nodes[j] = tetra.node[j];
      lastTetra = TetMesh().RegisterTetrahedronInMainMesh(nodes);
  }

  re_index_tetras[i] = lastTetra;

  progress.Step();
  }

  return true;
}

bool CGocadMeshImporter::CreateVolumes(IProgressBase& progress, const std::vector<int>& re_index_tetras)
{
  const std::vector<int>& volume_types = m_solid->volume_types;
  const std::vector<std::vector<int> >& volumes = m_solid->volumes;

  progress.NextJob("Create volumes");
  progress.AddSteps(volumes.size());

  for (size_t i = 0; i < volumes.size(); ++i)
  {
  if (canHandleVolume(volume_types[i]))
  {
      std::vector<int> volume;
      volume.reserve(volumes[i].size());

      for (size_t j = 0; j < volumes[i].size(); ++j)
      {
    int new_index = re_index_tetras[volumes[i][j]];
    if (new_index >= 0)
          volume.emplace_back(new_index);
      }
      TetMesh().RegisterTetVolume(volume);
  }

  progress.Step();
  }

  return true;
}

bool CGocadMeshImporter::CreateFormations(IProgressBase& progress)
{
  bool haveTetraRegions = !m_solid->tetra_region_names.empty();

  progress.NextJob("Create formations");

  if (haveTetraRegions)
  {
  progress.AddSteps(m_solid->tetra_region_names.size());

  if (!_createFormationsFromTetraRegions(progress))
      return false;
  }
  else
  {
  progress.AddSteps(m_solid->volumes.size());

  if (!_createFormationsFromVolumes(progress))
      return false;
  }

  SortFormations(progress);

  return true;
}

void CGocadMeshImporter::SortFormations(IProgressBase& progress)
{
  CTetraFormationEntry *tetraFormationEntry = static_cast<CTetraFormationEntry *>(Model().GraphEntry(MD_TETRA_FORMATION));
  CTetraFormationEntry::TNodeSet stNodes = tetraFormationEntry->EntryNodes();

  progress.NextJob("Sort formations");
  progress.AddSteps(stNodes.size() * 2);

  // Min() and Max() of CTetraFormation use DisplayList, and that is not setup yet, so we do our own gathering of min_z and max_z
  // If this is too slow, we may do something clever by keeping track of these values during import/parsing the file
  struct TetraFormationWrapper
  {
  CTetraFormation *formation;
  double min_z;
  double max_z;

  TetraFormationWrapper(CTetraFormation *formation)
      : formation(formation)
      , min_z(DBL_MAX)
      , max_z(-DBL_MAX)
  {
      for (int i = 0; i < formation->ElementSetSize(); ++i)
      {
    for (int j = 0; j < formation->ElementSet(i).ElementSet().ElementSize(); ++j)
    {
          const geo::IElement& elt = formation->ElementSet(i).ElementSet().Element(j);
          double elt_min_z = elt.Min().Z();
          double elt_max_z = elt.Max().Z();

          min_z = std::min(min_z, elt_min_z);
          max_z = std::max(max_z, elt_max_z);
    }
      }
  }
  };

  std::vector<TetraFormationWrapper *> formations;
  formations.reserve(stNodes.size());

  for (CTetraFormationEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  formations.push_back(new TetraFormationWrapper(*it));
  progress.Step(1);
  }

  std::sort(formations.begin(), formations.end(),
  [](const TetraFormationWrapper *lhs, const TetraFormationWrapper *rhs) -> bool
      {
    if (lhs->min_z == rhs->min_z)
    {
          if (lhs->max_z == rhs->max_z)
      return lhs->formation->Name() < rhs->formation->Name();
          else
      return lhs->max_z > rhs->max_z; // Careful, if same min_z but different max_z we sort with largest formation height first
    }
    return lhs->min_z < rhs->min_z;
      });

  unsigned int order = 0;
  for (std::vector<TetraFormationWrapper *>::iterator it = formations.begin(); it != formations.end(); ++it)
  {
  (*it)->formation->Order(order++);
  delete *it;
  progress.Step(1);
  }
}

bool CGocadMeshImporter::CreateSurfaces(IProgressBase& progress, std::vector<geo::CSurfaceDesc *>& surface_descriptions, std::vector<CSurfaceBase *>& mesh_surfaces, const std::vector<int>& re_index_tetras)
{
  CSurfaceEntry& surface_entry = static_cast<CSurfaceEntry&>(*Model().GraphEntry(MD_BASE_SURFACE));

  size_t size_surfaces = m_solid->surfaces.size();

  progress.NextJob("Create surfaces");
  progress.AddSteps(2 * size_surfaces);

  std::vector<std::string> mesh_surface_names;
  std::vector<std::map<size_t, std::vector<const geo::IFace *> > > surface2bodies;
  mesh_surfaces.reserve(size_surfaces);
  mesh_surface_names.reserve(size_surfaces);

  if (!_createDataSurfaces(progress, surface_descriptions, re_index_tetras, mesh_surfaces, mesh_surface_names, surface2bodies))
  return false;

  if (!_createModelSurfaces(progress, surface_descriptions, mesh_surfaces, mesh_surface_names, surface2bodies))
  return false;

  return true;
}

bool CGocadMeshImporter::CreateInterfaceElements(IProgressBase & progress, std::vector<geo::CSurfaceDesc *>& surface_descriptions, const std::vector<int>& re_index_tetras)
{
  const std::vector<std::vector<gm_skua::SKUAParseData::Triangle> >& surfaces = m_solid->surfaces;
  const std::vector<std::vector<gm_skua::SKUAParseData::BodyTriangle> >& surfaces_minus = m_solid->surfaces_minus;
  const std::vector<std::vector<gm_skua::SKUAParseData::BodyTriangle> >& surfaces_plus = m_solid->surfaces_plus;
  const std::vector<int>& surface_types = m_solid->surface_types;

  progress.NextJob("Create interface elements");
  progress.AddSteps(surfaces.size());

  for (size_t i = 0; i < surfaces.size(); ++i)
  {
  if (surface_types[i] == gm_skua::SKUAParseData::SurfaceType::SurfFault)
  {
      for (size_t j = 0; j < surfaces[i].size(); ++j)
      {
    const gm_skua::SKUAParseData::Triangle& triangle = surfaces[i][j];

    if (triangle.minus < 0 || triangle.plus < 0)
          continue;

    const gm_skua::SKUAParseData::BodyTriangle& minus = surfaces_minus[i][triangle.minus];
    const gm_skua::SKUAParseData::BodyTriangle& plus = surfaces_plus[i][triangle.plus];

    int tetra_minus = re_index_tetras[minus.tetra];
    int tetra_plus = re_index_tetras[plus.tetra];

    if (tetra_minus < 0 || tetra_plus < 0)
          continue;

    const geo::IFace *frontFace = &static_cast<const geo::CTetrahedron&>(TetMesh().Element(tetra_minus)).Face(minus.opposite);
    const geo::IFace *backFace = &static_cast<const geo::CTetrahedron&>(TetMesh().Element(tetra_plus)).Face(plus.opposite);

    TetMesh().AddInterfaceElement(*surface_descriptions[i], frontFace, backFace);
      }
  }

  progress.Step();
  }

  return true;
}

bool CGocadMeshImporter::CreateIntermediateSurfaces(IProgressBase & progress, const std::vector<geo::CSurfaceDesc *> surface_descriptions)
{
  typedef std::map<gm_skua::SKUAParseData::String, std::pair<int, int> > TDSFFaultPairMap;
  TDSFFaultPairMap dsf_fault_pair_map;

  const std::vector<gm_skua::SKUAParseData::String>& surface_names = m_solid->surface_names;
  const std::vector<int>& surface_types = m_solid->surface_types;

  for (int i = 0; i < surface_types.size(); ++i)
  {
  if (surface_types[i] == gm_skua::SKUAParseData::SurfaceType::SurfDSFFault)
  {
      if (surface_names[i].ends_with("_minus")) // remove last 11 characters
      {
    gm_skua::SKUAParseData::String fault_base(surface_names[i], 6);
    TDSFFaultPairMap::iterator result = dsf_fault_pair_map.insert(std::make_pair(fault_base, std::pair<int, int>(-1, -1))).first;
    result->second.first = i;
      }
      else // ends with "fault_plus" (remove last 10 characters)
      {
    gm_skua::SKUAParseData::String fault_base(surface_names[i], 5);
    TDSFFaultPairMap::iterator result = dsf_fault_pair_map.insert(std::make_pair(fault_base, std::pair<int, int>(-1, -1))).first;
    result->second.second = i;
      }
  }
  }

  if (dsf_fault_pair_map.empty())
  return true;

  progress.NextJob("Generate intermediate fault surfaces");
  progress.AddSteps(dsf_fault_pair_map.size());

  for (TDSFFaultPairMap::const_iterator it = dsf_fault_pair_map.begin(); it != dsf_fault_pair_map.end(); ++it)
  {
  geo::CSurfaceDesc& surf_desc_0 = *surface_descriptions[it->second.first];
  geo::CSurfaceDesc& surf_desc_1 = *surface_descriptions[it->second.second];

  std::vector<const geo::IFace *> surf_0; surf_0.reserve(100);
  std::vector<const geo::IFace *> surf_1; surf_1.reserve(100);

  for (int i = 0; i < surf_desc_0.TetSurfaceSize(); ++i)
      for (int j = 0; j < surf_desc_0.TetSurface(i).FaceSize(); ++j)
    surf_0.push_back(&surf_desc_0.TetSurface(i).Face(j));

  for (int i = 0; i < surf_desc_1.TetSurfaceSize(); ++i)
      for (int j = 0; j < surf_desc_1.TetSurface(i).FaceSize(); ++j)
    surf_1.push_back(&surf_desc_1.TetSurface(i).Face(j));

  gm_dsf::DSFIntermediateSurfaceGenerator gen(m_tetramesh);

  if (!gen.GenerateIntermediateSurface(it->first.data, surf_0, surf_1, Model().DSFIntermediateSurfaceType() > 0))
      return false;

  progress.Step();
  }

  return true;
}


bool CGocadMeshImporter::_createFormationsFromTetraRegions(IProgressBase& progress)
{
  const std::vector<gm_skua::SKUAParseData::String>& formation_names = m_solid->tetra_region_names;
  const std::vector<gm_skua::SKUAParseData::String>& volume_names = m_solid->model_region_names;
  const std::vector<int>& volume_types = m_solid->volume_types;
  const std::vector<std::vector<int> >& volumes = m_solid->volumes;
  const std::vector<gm_skua::SKUAParseData::Tetra>& tetras = m_solid->tetras;

  std::vector<CTetraFormation *> formations;
  formations.reserve(formation_names.size());

  for (size_t i = 0; i < formation_names.size(); ++i)
  {
  formations.push_back(new CTetraFormation(formation_names[i].data, Model()));
  }

  size_t mesh_volume = 0;
  for (size_t i = 0; i < volume_names.size(); ++i)
  {
  if (canHandleVolume(volume_types[i]))
  {
      const gm_skua::SKUAParseData::Tetra& tetra = tetras[volumes[i][0]];

      CTetraFormation *formation = formations[tetra.tetra_region];

      const geo::CBodyGroup& volume = static_cast<const geo::CBodyGroup&>(TetMesh().Volume(mesh_volume));
      CTetraFormationVolume* formation_volume = new CTetraFormationVolume(*formation, const_cast<geo::CBodyGroup&>(volume));

      formation_volume->Name(volume_names[i].data);

      formation_volume->reParent(formation);

      ++mesh_volume;
  }

  progress.Step();
  }

  return true;
}

bool CGocadMeshImporter::_createFormationsFromVolumes(IProgressBase& progress)
{
  const std::vector<gm_skua::SKUAParseData::String>& volume_names = m_solid->model_region_names;
  const std::vector<int>& volume_types = m_solid->volume_types;
  const std::vector<std::vector<int> >& volumes = m_solid->volumes;

  size_t volume_index = 0;
  for (size_t i = 0; i < volume_names.size(); ++i)
  {
  if (canHandleVolume(volume_types[i]))
  {
      CTetraFormation * formation = new CTetraFormation(volume_names[i].data, Model());

      const geo::CBodyGroup& volume = static_cast<const geo::CBodyGroup&>(TetMesh().Volume(volume_index));
      CTetraFormationVolume* formation_volume = new CTetraFormationVolume(*formation, const_cast<geo::CBodyGroup&>(volume));

      formation_volume->reParent(formation);

      ++volume_index;
  }

  progress.Step();
  }

  return true;
}

bool CGocadMeshImporter::_createDataSurfaces(IProgressBase& progress, std::vector<geo::CSurfaceDesc *>& surface_descriptions, const std::vector<int>& re_index_tetras,
  std::vector<CSurfaceBase *>& mesh_surfaces, std::vector<std::string>& mesh_surface_names, std::vector<std::map<size_t, std::vector<const geo::IFace *> > >& surface2bodies)
{
  CSurfaceEntry& surface_entry = static_cast<CSurfaceEntry&>(*Model().GraphEntry(MD_BASE_SURFACE));

  const std::vector<gm_skua::SKUAParseData::String>& surface_names = m_solid->surface_names;
  const std::vector<std::vector<gm_skua::SKUAParseData::Triangle> >& surfaces = m_solid->surfaces;
  const std::vector<std::vector<gm_skua::SKUAParseData::BodyTriangle> >& surfaces_minus = m_solid->surfaces_minus;
  const std::vector<std::vector<gm_skua::SKUAParseData::BodyTriangle> >& surfaces_plus = m_solid->surfaces_plus;
  const std::vector<int>& surface_types = m_solid->surface_types;
  const std::vector<gm_skua::SKUAParseData::Tetra>& tetras = m_solid->tetras;

  const std::vector<int>& volume_types = m_solid->volume_types;
  std::vector<int> re_index_volumes(volume_types.size());

  int volume_index = 0;
  for (size_t i = 0; i < volume_types.size(); ++i)
  re_index_volumes[i] = canHandleVolume(volume_types[i]) ? volume_index++ : -1;

  for (size_t i = 0; i < surfaces.size(); ++i)
  {
  geo::CSurface geo_surface;

  surface2bodies.emplace_back(std::map<size_t, std::vector<const geo::IFace *> >());

  geo_surface.reserveFaceSize(surfaces[i].size());

  for (size_t j = 0; j < surfaces[i].size(); ++j)
  {
      const gm_skua::SKUAParseData::Triangle& triangle = surfaces[i][j];

      const geo::IFace *face = nullptr;

      if (triangle.minus >= 0)
      {
    const gm_skua::SKUAParseData::BodyTriangle& minus = surfaces_minus[i][triangle.minus];

    int tetra_minus = re_index_tetras[minus.tetra];
    int volume_minus = re_index_volumes[tetras[minus.tetra].model_region];

    if (tetra_minus >= 0)
    {
          face = &static_cast<const geo::CTetrahedron&>(TetMesh().Element(tetra_minus)).Face(minus.opposite);

          std::map<size_t, std::vector<const geo::IFace *> >::iterator faces = surface2bodies.back().insert(std::make_pair(volume_minus, std::vector<const geo::IFace *>())).first;

          if (face)
      faces->second.push_back(face);
    }
      }
      if (triangle.plus >= 0)
      {
    const gm_skua::SKUAParseData::BodyTriangle& plus = surfaces_plus[i][triangle.plus];

    int tetra_plus = re_index_tetras[plus.tetra];
    int volume_plus = re_index_volumes[tetras[plus.tetra].model_region];

    if (tetra_plus >= 0)
    {
          const geo::IFace *backface = &static_cast<const geo::CTetrahedron&>(TetMesh().Element(tetra_plus)).Face(plus.opposite);

          std::map<size_t, std::vector<const geo::IFace *> >::iterator faces = surface2bodies.back().insert(std::make_pair(volume_plus, std::vector<const geo::IFace *>())).first;
          if (!face && backface)
          {
      faces->second.push_back(backface);
      face = backface;
          }
    }
      }

      if (face)
    geo_surface.AddFace(*face);
  }

  if (geo_surface.FaceSize() > 0)
  {
      CSurfaceBase *mesh_surface = surface_entry.InsertSurface(surface_names[i].data, geo_surface);

      lg(QString("CSurfaceBase %1 with %2 triangles").arg(surface_names[i].data).arg(mesh_surface->Surface().FaceSize()));
      mesh_surfaces.emplace_back(mesh_surface);
      mesh_surface_names.emplace_back(surface_names[i].data);
  }
  else surface2bodies.resize(surface2bodies.size() - 1);

  progress.Step();
  }

  return true;
}

bool CGocadMeshImporter::_createModelSurfaces(IProgressBase& progress, std::vector<geo::CSurfaceDesc *>& surface_descriptions,
  std::vector<CSurfaceBase *>& mesh_surfaces, std::vector<std::string>& mesh_surface_names, std::vector<std::map<size_t, std::vector<const geo::IFace *> > >& surface2bodies)
{
  const std::vector<int>& surface_types = m_solid->surface_types;

  for (size_t i = 0; i < surface_types.size(); ++i)
  {
  bool is_fault = false;
  bool is_dsf = false;

  switch (surface_types[i])
  {
  case gm_skua::SKUAParseData::SurfaceType::SurfBoundary:
      Model().SubBoundary().LinkTo(*mesh_surfaces[i]);
      m_tetramesh.m_vcSurfaceSource.push_back(&Model().SubBoundary());
      break;
  case gm_skua::SKUAParseData::SurfaceType::SurfTop:
  {
      CTetraSubHorizon* horizon = Model().SubBoundary().TopHorizon().Horizon();
      horizon->LinkTo(*mesh_surfaces[i]);
      horizon->Name(mesh_surfaces[i]->Name());
      m_tetramesh.m_vcSurfaceSource.push_back(mesh_surfaces[i]);
  }
  break;
  case gm_skua::SKUAParseData::SurfaceType::SurfBottom:
  {
      CTetraSubHorizon* horizon = Model().SubBoundary().BottomHorizon().Horizon();
      horizon->LinkTo(*mesh_surfaces[i]);
      horizon->Name(mesh_surfaces[i]->Name());
      m_tetramesh.m_vcSurfaceSource.push_back(mesh_surfaces[i]);
  }
  break;
  case gm_skua::SKUAParseData::SurfaceType::SurfDSFFault:
      is_dsf = true;
  case gm_skua::SKUAParseData::SurfaceType::SurfFault:
      is_fault = true;
  default: // horizons
      (new CTetraSubHorizon(mesh_surfaces[i]->Name(), is_fault, Model()))->LinkTo(*mesh_surfaces[i]);
      m_tetramesh.m_vcSurfaceSource.push_back(mesh_surfaces[i]);
      break;
  }

  geo::CSurfaceDesc *surface_desc = new geo::CSurfaceDesc(TetMesh(), mesh_surfaces[i]->Surface(), mesh_surface_names[i], is_fault);
  surface_descriptions.push_back(surface_desc);

  if (is_dsf)
  {
      for (std::map<size_t, std::vector<const geo::IFace *> >::iterator it = surface2bodies[i].begin(); it != surface2bodies[i].end(); ++it)
      {
    geo::CTiedBodyTetSurface *tetsurf = new geo::CTiedBodyTetSurface(*surface_desc, TetMesh().Volume(it->first));
    tetsurf->reserveFaceSize(it->second.size());

    for (size_t j = 0; j < it->second.size(); ++j)
    {
          const geo::CBodyTriangle *body_triangle = static_cast<const geo::CBodyTriangle *>(it->second[j]);
          tetsurf->AddFace(*new geo::CTiedPointBodyTriangle(const_cast<geo::IBody&>(*body_triangle->Parent()), body_triangle->Index()));
    }

    lg(QString("CTetSurface %1 with %2 triangles").arg(tetsurf->SurfaceDesc().Name().c_str()).arg(tetsurf->FaceSize()));
      }
  }
  else
  {
      for (std::map<size_t, std::vector<const geo::IFace *> >::iterator it = surface2bodies[i].begin(); it != surface2bodies[i].end(); ++it)
      {
    geo::CTetSurface *tetsurf = new geo::CTetSurface(*surface_desc, TetMesh().Volume(it->first));
    tetsurf->reserveFaceSize(it->second.size());

    for (size_t j = 0; j < it->second.size(); ++j)
    {
          tetsurf->AddFace(*it->second[j]);
    }

    lg(QString("CTetSurface %1 with %2 triangles").arg(tetsurf->SurfaceDesc().Name().c_str()).arg(tetsurf->FaceSize()));
      }
  }

  progress.Step();
  }

  return true;
}


bool CGocadMeshImporter::canHandleVolume(int type)
{
  return type == gm_skua::SKUAParseData::VolumeType::VolNormal;
}


#else
// GocadMeshImporter.cpp: implementation of the CGocadMeshImporter class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "GocadMeshImporter.h"

#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraFormation.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
#include "InterfaceElement.h"
#include "IProgressFactory.h"
#include "Global.h"
#include "CProgressBase.h"
#include "IPoint.h"
#include "INode.h"
#include "TetraSubHorizon.h"
#include "ITetrahedron.h"
#include "Surface.h"
#include "TriaIntersecBox.h"
#include "TiedBodyTriangle.h"
#include "RotatedSystem.h"
#include "BoundaryBase.h"
#include "TetraSurface.h"
#include "TSSurface.h"
#include "TSSurfaceProgress.h"
#include "Point.h"

#include "DSFIntermediateSurfaceGenerator.h"
#include "TiedBodyTetSurface.h"
#include "BodyTriangle.h"
#include "TiedPointBodyTriangle.h"


///// CGocadMeshImporter::CGocadSurface

namespace
{

int ONE_FACE = 1;

} // anonymous namespace


geo::CPoint Node2Point(const CGocadData::CNode & node)
{
  return geo::CPoint(node.X(), node.Y(), node.Z());
}


CGocadMeshImporter::CGocadSurface::CGocadSurface(const CGocadData::CSurface& gocadsurface)
: m_surface(gocadsurface),
  m_bFault(false)
{
  int i;
  for(i = 0; i < gocadsurface.TFaceSize(); ++i)
  m_vcTFaces.push_back(new CGocadMeshImporter::CGocadTFace(gocadsurface.TFace(i)));
}

CGocadMeshImporter::CGocadSurface::~CGocadSurface()
{
  for(size_t i = 0; i < m_vcTFaces.size(); ++i)
  delete m_vcTFaces[i];
}

const CGocadData::CSurface& CGocadMeshImporter::CGocadSurface::Surface() const
{
  return m_surface;
}

int CGocadMeshImporter::CGocadSurface::TFaceSize() const
{
  return m_vcTFaces.size();
}

const CGocadMeshImporter::CGocadTFace& CGocadMeshImporter::CGocadSurface::TFace(int i) const
{
  assert(i >= 0 && i < m_vcTFaces.size());
  return *m_vcTFaces[i];
}

CGocadMeshImporter::CGocadTFace& CGocadMeshImporter::CGocadSurface::TFace(int i)
{
  assert(i >= 0 && i < m_vcTFaces.size());
  return *m_vcTFaces[i];
}

bool CGocadMeshImporter::CGocadSurface::IsFault() const
{
  return m_bFault;
}

void CGocadMeshImporter::CGocadSurface::MarkAsFault()
{
  m_bFault = true;
}

///// CGocadMeshImporter::CGocadTFace

CGocadMeshImporter::CGocadTFace::CGocadTFace(const CGocadData::CTFace& tface)
: m_tface(tface),
  m_pFrontGroup(0),
  m_pBackGroup(0),
  m_pSurface(0)
{
}

const geo::CBodyGroup* CGocadMeshImporter::CGocadTFace::FrontGroup() const
{
  return m_pFrontGroup;
}

const geo::CBodyGroup* CGocadMeshImporter::CGocadTFace::BackGroup() const
{
  return m_pBackGroup;
}

int CGocadMeshImporter::CGocadTFace::FacePairSize() const
{
  return m_vcFacePairs.size();
}

const CGocadMeshImporter::CGocadTFace::TFacePair& CGocadMeshImporter::CGocadTFace::FacePair(int i) const
{
  assert(i >= 0 && i < m_vcFacePairs.size());
  return m_vcFacePairs[i];
}

// Goal: Fill 'm_vcFacePairs'
bool CGocadMeshImporter::CGocadTFace::CollectFacesSimple(const TPositionFacesMap& mpPositionFaces, bool bTwoSeparateBodies)
{
  int num_tri = m_tface.TriangleSize();
  for(size_t i = 0; i < num_tri; ++i)
  {
  TPositionFacesMap::const_iterator it = mpPositionFaces.find(CPositionTriangle(m_tface.Triangle(i)));
  assert(it != mpPositionFaces.end());

  if(it != mpPositionFaces.end())
  {
      assert(it->second.size() == 1 || it->second.size() == 2);
      TFaceBodyPair prFaceBody1((geo::IFace*)0, (geo::CBodyGroup*)0);
      TFaceBodyPair prFaceBody2((geo::IFace*)0, (geo::CBodyGroup*)0);

      TFaceBodyPairSet::const_iterator itFaces;
      for(itFaces = it->second.begin(); itFaces != it->second.end(); ++itFaces)
      {
    assert(itFaces->first != 0 && itFaces->second != 0);
    if(prFaceBody1.first == 0 && prFaceBody1.second == 0)
          prFaceBody1 = *itFaces;
    else if(prFaceBody2.first == 0 && prFaceBody2.second == 0)
          prFaceBody2 = *itFaces;
    else
          assert(false); // too many
      }

      TFacePair prFaces((geo::IFace*)0, (geo::IFace*)0);

      if (bTwoSeparateBodies)
      {
    if(m_pFrontGroup != 0 && prFaceBody2.second == m_pFrontGroup)
    {
          // swap
          TFaceBodyPair prTmp = prFaceBody2;
          prFaceBody2 = prFaceBody1;
          prFaceBody1 = prTmp;
    }
      }
      else
      {
    if (prFaceBody2.second != 0)
    {
          // check for swap

      geo::CVector Nfault = TriangleNormal(m_tface.Triangle(i));
      geo::CVector Nbody1 = prFaceBody1.first->Normal();
      geo::CVector Nbody2 = prFaceBody2.first->Normal();

          if (Nfault == Nbody2)
          {
      TFaceBodyPair prTmp = prFaceBody2;
      prFaceBody2 = prFaceBody1;
      prFaceBody1 = prTmp;
          }
    }
      }

      assert(m_pFrontGroup == 0 || prFaceBody1.second == m_pFrontGroup);
      m_pFrontGroup = prFaceBody1.second;
      prFaces.first = prFaceBody1.first;

      if(prFaceBody2.first && prFaceBody2.second) // if we have a second
      {
    assert(m_pBackGroup == 0 || prFaceBody2.second == m_pBackGroup);
    m_pBackGroup = prFaceBody2.second;
    prFaces.second = prFaceBody2.first;
      }

      assert(m_pFrontGroup != 0);
      assert(prFaces.first != 0);
      assert((m_pBackGroup == 0 && prFaces.second == 0) || (m_pBackGroup != 0 && prFaces.second != 0));

      m_vcFacePairs.push_back(prFaces);
  }
  }

  return true;
}

// Goal: Fill 'm_vcFacePairs'
bool CGocadMeshImporter::CGocadTFace::CollectFaces( const TPositionFacesMap& mpPositionFaces )
{
  // 1. Determine how many bodies we have
  int nNrOfBodies = 0;
  size_t nNrOfTriangles = m_tface.TriangleSize();

  bool bTwoSeparateBodies = false;
  const geo::CBodyGroup *pBodyGroup[] = { 0, 0 };

  if( nNrOfTriangles > 0 )
  {
  const CGocadData::CTriangle& tri = m_tface.Triangle(0);

  TPositionFacesMap::const_iterator it = mpPositionFaces.find( CPositionTriangle( tri ) );
  
  assert(it != mpPositionFaces.end());
  	assert(it->second.size() == 1 || it->second.size() == 2);

  nNrOfBodies = it->second.size();

  if( nNrOfBodies == 2 )
  {
      TFaceBodyPairSet::const_iterator itSet = it->second.begin();
      
    for (int j = 0; j < nNrOfBodies; ++j, ++itSet)
    pBodyGroup[j] = itSet->second;

    assert(pBodyGroup[0] != 0);
      assert(pBodyGroup[1] != 0);
      
    bTwoSeparateBodies = pBodyGroup[0] != pBodyGroup[1];
  }
  }

  for( size_t i = 1; i < nNrOfTriangles; ++i )
  {
  const CGocadData::CTriangle& tri = m_tface.Triangle( i );
  TPositionFacesMap::const_iterator it = mpPositionFaces.find(CPositionTriangle(m_tface.Triangle(i)));

  assert(it != mpPositionFaces.end());
  assert(it->second.size() == 1 || it->second.size() == 2);
  assert(nNrOfBodies == it->second.size());

  if (bTwoSeparateBodies)
  {
      if (nNrOfBodies == 2)
      {
    TFaceBodyPairSet::const_iterator itSet = it->second.begin();

    int first = -1;
    if (itSet->second == pBodyGroup[0])
          first = 0;
    else if (itSet->second == pBodyGroup[1])
          first = 1;
    assert(first >= 0);

    ++itSet;

    int second = -1;
    if (itSet->second == pBodyGroup[0])
          second = 0;
    else if (itSet->second == pBodyGroup[1])
          second = 1;
    assert(second >= 0);

    bTwoSeparateBodies = first + second == 1;
    assert(bTwoSeparateBodies);
      }
      else
      {
    bTwoSeparateBodies = false; // this shouldn't happen:
    assert(false);
      }
  }
  }

  // 2. Simple case: 1 body or 2 separate bodies, simple gathering, using old routine
  //    Added: when we only have one triangle

  if( bTwoSeparateBodies || nNrOfBodies == 1 || nNrOfTriangles == 1 )
  return CollectFacesSimple(mpPositionFaces, bTwoSeparateBodies);
  
  // 3. Case of 2 bodies. Note that these "bodies" may be one body existing on both sides of the fault
  assert(nNrOfBodies == 2);

  // 3.a Determine outer edge of Fault

  typedef std::pair<geo::CPoint, geo::CPoint> TEdge;
  typedef std::vector<const CGocadData::CTriangle *> TTriangleVec;
  typedef std::map<TEdge, TTriangleVec> TEdgeTrianglesMap;
  typedef std::pair<TEdgeTrianglesMap::iterator, bool> TEdgeTrianglesMapResult;
  typedef std::set<TEdge> TFaultEdges;

  TEdgeTrianglesMap EdgeTrianglesMap;
  TFaultEdges FaultEdges;

  for(size_t i = 0; i < nNrOfTriangles; ++i)
  {
  CGocadData::CTriangle triangle = m_tface.Triangle(i);

    for (int j = 0; j < 3; ++j)
    {
      TEdge edge = std::make_pair(Node2Point(triangle.Node(j)), Node2Point(triangle.Node((j+1) % 3)));
      TEdge rvrs = std::make_pair(Node2Point(triangle.Node((j+1) % 3)), Node2Point(triangle.Node(j)));

      TEdgeTrianglesMap::iterator it = EdgeTrianglesMap.find(edge);
      if (it == EdgeTrianglesMap.end())
        it = EdgeTrianglesMap.find(rvrs);

      if (it == EdgeTrianglesMap.end())
      {
        TEdgeTrianglesMapResult result = EdgeTrianglesMap.insert(std::make_pair(edge, TTriangleVec()));
        it = result.first;
      }
      it->second.push_back(&triangle);
    }
  }

  for (TEdgeTrianglesMap::iterator it = EdgeTrianglesMap.begin(); it != EdgeTrianglesMap.end(); ++it)
  {
    if (it->second.size() == 1)
      FaultEdges.insert(it->first);
  }

  // TODO: check that FaultEdges contains one continuous path
  
  // 3.b Construct connection map of PointIndex edges to body faces, excluding any edges on the Fault edge

  typedef std::pair<int, int> TPointEdge;
  typedef std::vector<const geo::IFace *> TConnectVector;
  typedef std::map<TPointEdge, TConnectVector> TConnectMap;
  typedef std::pair<TConnectMap::iterator, bool> TConnectMapResult;

  TConnectMap connectMap;

  bool bFoundHorizonEdge = false;

  for(size_t i = 0; i < nNrOfTriangles; ++i)
  {
    const CGocadData::CTriangle& tri = m_tface.Triangle(i);
    TPositionFacesMap::const_iterator it = mpPositionFaces.find(CPositionTriangle( tri ));

  if (it == mpPositionFaces.end())
  {
      return false;
  }

    for (TFaceBodyPairSet::const_iterator itFaces = it->second.begin(); itFaces != it->second.end(); ++itFaces)
    {
      int n = itFaces->first->NrOfPoints();
      for (int j = 0; j < n; ++j)
      {
        geo::CPoint p0 = itFaces->first->Point(j);
        geo::CPoint p1 = itFaces->first->Point((j+1) % n);

        TEdge edge = std::make_pair(p0, p1);
        TEdge rvrs = std::make_pair(p1, p0);

        if (FaultEdges.find(edge) == FaultEdges.end() && FaultEdges.find(rvrs) == FaultEdges.end())
        {
          int pIndex_0 = itFaces->first->PointIndex(j);
          int pIndex_1 = itFaces->first->PointIndex((j+1) % n);

          TPointEdge pEdge = std::make_pair(pIndex_0, pIndex_1);
          TPointEdge pRvrs = std::make_pair(pIndex_1, pIndex_0);

          TConnectMap::iterator itConnect = connectMap.find(pEdge);
          if (itConnect == connectMap.end())
            itConnect = connectMap.find(pRvrs);

          if (itConnect == connectMap.end())
          {
            TConnectMapResult result = connectMap.insert(std::make_pair(pEdge, TConnectVector()));
            itConnect = result.first;
          }
          itConnect->second.push_back(itFaces->first);

          if (itConnect->second.size() > 2)
            bFoundHorizonEdge = true;
        }
      }
    }
  }

  // 3.c Horizons (can) have two PointIndex-identical triangles with different BodyGroups attached; process with the old method

  if (bFoundHorizonEdge)
  {
    //Printer::instance()->debug("Horizon edge");
    return CollectFacesSimple(mpPositionFaces, bTwoSeparateBodies);
  }

  // 3.d Color the two surfaces in connectMap 

  typedef std::map<const geo::IFace *, int> TColorMap;
  typedef std::pair<TColorMap::iterator, bool> TColorMapResult;

  enum { COLOR_FRONT = 1, COLOR_BACK = 2};

  TColorMap colorMap;

  for (int color = COLOR_FRONT; color <= COLOR_BACK; ++color)
  {
  TConnectMap::iterator painter;
  std::vector<const geo::IFace *> stack;

    painter = connectMap.begin();

  bool first = false;
  while (!first)
  {
    assert(painter != connectMap.end());

    if (colorMap.find(painter->second[0]) == colorMap.end())
      first = true;
    else
      ++painter;
  }

    assert(painter != connectMap.end());

  for (size_t j = 0; j < painter->second.size(); ++j)
    stack.push_back(painter->second[j]);

  while (stack.size() > 0)
  {
    const geo::IFace *face = stack.back();
    stack.pop_back();

    TColorMapResult resColor = colorMap.insert(std::make_pair(face, color));

      assert(resColor.second || resColor.first->second == color);

    if (resColor.second)
    {
      int n = face->NrOfPoints();
      for (int i = 0; i < n; ++i)
      {
          int pIndex_0 = face->PointIndex(i);
          int pIndex_1 = face->PointIndex((i+1) % n);

        TPointEdge pEdge = std::make_pair(pIndex_0, pIndex_1);
        TPointEdge pRvrs = std::make_pair(pIndex_1, pIndex_0);

        painter = connectMap.find(pEdge);
        if (painter == connectMap.end())
            painter = connectMap.find(pRvrs);

        if (painter != connectMap.end())
        {
          for (size_t j = 0; j < painter->second.size(); ++j)
            stack.push_back(painter->second[j]);
        }
      }
    }
  }
  }

  // 3.e Check for completeness: all faces should have one color

  assert(colorMap.size() == 2 * nNrOfTriangles);

#if defined(_DEBUG)
  int NrOfColorFront = 0;
  int NrOfColorBack  = 0;

  for (TColorMap::iterator it = colorMap.begin(); it != colorMap.end(); ++it)
  {
    if (it->second == COLOR_FRONT)
      ++NrOfColorFront;
    if (it->second == COLOR_BACK)
      ++NrOfColorBack;
  }

  assert(NrOfColorFront + NrOfColorBack == 2 * nNrOfTriangles);
  assert(NrOfColorFront == NrOfColorBack);
#endif

  // 3.f Gather all face pairs in the right orientation (TODO: remove the old code fragments)

  for(size_t i = 0; i < m_tface.TriangleSize(); ++i)
  {
  TPositionFacesMap::const_iterator it = mpPositionFaces.find(CPositionTriangle(m_tface.Triangle(i)));
  assert(it != mpPositionFaces.end());

  if(it != mpPositionFaces.end())
  {
      assert(it->second.size() == 1 || it->second.size() == 2);
      TFaceBodyPair prFaceBody1((geo::IFace*)0, (geo::CBodyGroup*)0);
      TFaceBodyPair prFaceBody2((geo::IFace*)0, (geo::CBodyGroup*)0);

      TFaceBodyPairSet::const_iterator itFaces;
      for(itFaces = it->second.begin(); itFaces != it->second.end(); ++itFaces)
      {
    assert(itFaces->first != 0 && itFaces->second != 0);
    if(prFaceBody1.first == 0 && prFaceBody1.second == 0)
          prFaceBody1 = *itFaces;
    else if(prFaceBody2.first == 0 && prFaceBody2.second == 0)
          prFaceBody2 = *itFaces;
    else
          assert(false); // too many
      }

      TFacePair prFaces((geo::IFace*)0, (geo::IFace*)0);

    TColorMap::iterator color = colorMap.find(prFaceBody1.first);
      if(color->second == COLOR_BACK)
      {
    // swap
    TFaceBodyPair prTmp = prFaceBody2;
    prFaceBody2 = prFaceBody1;
    prFaceBody1 = prTmp;
      }

      assert(m_pFrontGroup == 0 || prFaceBody1.second == m_pFrontGroup);
      m_pFrontGroup = prFaceBody1.second;
      prFaces.first = prFaceBody1.first;

      if(prFaceBody2.first && prFaceBody2.second) // if we have a second
      {
    assert(m_pBackGroup == 0 || prFaceBody2.second == m_pBackGroup);
    m_pBackGroup = prFaceBody2.second;
    prFaces.second = prFaceBody2.first;
      }

      assert(m_pFrontGroup != 0);
      assert(prFaces.first != 0);
      assert((m_pBackGroup == 0 && prFaces.second == 0) || (m_pBackGroup != 0 && prFaces.second != 0));

      m_vcFacePairs.push_back(prFaces);
  }
  }

  return true;
}

void CGocadMeshImporter::CGocadTFace::AttachSurface(CSurfaceBase& surface)
{
  assert(m_pSurface == 0);
  m_pSurface = &surface;
}

CSurfaceBase* CGocadMeshImporter::CGocadTFace::AttachedSurface()
{
  return m_pSurface;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGocadMeshImporter::CGocadMeshImporter(const CGocadData::CTSolid& tsolid, CTetraMesh& tetramesh)
: m_vcTSolids(1, &tsolid),
  m_tetramesh(tetramesh)
{
}

CGocadMeshImporter::CGocadMeshImporter(const std::vector<const CGocadData::CTSolid*>& vcTSolids, CTetraMesh& tetramesh)
: m_vcTSolids(vcTSolids),
  m_tetramesh(tetramesh)
{
}

CGocadMeshImporter::~CGocadMeshImporter()
{
  DestroyGocadSurfaces();
}

CTetraModel& CGocadMeshImporter::Model()
{
  return static_cast<CTetraModel&>(m_tetramesh.Model());
}

const CTetraModel& CGocadMeshImporter::Model() const
{
  return static_cast<const CTetraModel&>(m_tetramesh.Model());
}

geo::CTetMeshBase& CGocadMeshImporter::TetMesh()
{
  return static_cast<geo::CTetMeshBase&>(m_tetramesh.Mesh());
}

const geo::CTetMeshBase& CGocadMeshImporter::TetMesh() const
{
  return static_cast<const geo::CTetMeshBase&>(m_tetramesh.Mesh());
}

void CGocadMeshImporter::set_formations()
{
  std::vector<CTetraFormation*> vcFormations;

  if (m_vcTSolids.size() == 1 && !m_vcTSolids[0]->TetraRegions().empty())
  {
  const std::vector<QString>& regions = m_vcTSolids[0]->TetraRegions();
  vcFormations.reserve(regions.size());

  for (size_t i = 0; i < regions.size(); ++i)
  {
      vcFormations.push_back(new CTetraFormation(regions[i], Model()));
  }
  }

   for(int iVolume = 0; iVolume < TetMesh().NrOfVolumes(); ++iVolume)
  {
      //const geo::CBodyGroup& volume = static_cast<const geo::CBodyGroup&>(TetMesh().Volume(iVolume));

      // get the right solid and the right volume in it
      int iSolid;
      int iSolidVol = iVolume;
      for(iSolid = 0; iSolid < m_vcTSolids.size(); ++iSolid)
      {
    if(iSolidVol < m_vcTSolids[iSolid]->VolumeSize())
          break;

    iSolidVol -= m_vcTSolids[iSolid]->VolumeSize();
      }

      assert(iSolid < m_vcTSolids.size());
      assert(iSolidVol < m_vcTSolids[iSolid]->VolumeSize());

      CTetraFormation* pForm;
      if(m_vcTSolids.size() == 1)
      {
    if (vcFormations.empty())
    {
          // generate formations for all volumes
          pForm = new CTetraFormation(m_vcTSolids[iSolid]->Volume(iSolidVol).Name(), Model());
    }
    else
    {
          const std::map<CGocadData::CTVolume *, int>& mp = m_vcTSolids[iSolid]->TetraRegionMap();

          const std::map<CGocadData::CTVolume *, int>::const_iterator it = mp.find(const_cast<CGocadData::CTVolume *>(&m_vcTSolids[iSolid]->Volume(iSolidVol)));

          if (it != mp.end() && it->second < (int)vcFormations.size())
          {
      pForm = vcFormations[it->second];
          }
          else // error, but let's not break on it
          {
      pForm = new CTetraFormation(m_vcTSolids[iSolid]->Volume(iSolidVol).Name(), Model());
          }
    }
      }
      else
      {
    // generate formations for all solids, assign volumes
    if(iSolid < vcFormations.size())
    {
          pForm = vcFormations[iSolid];
    }
    else
    {
          assert( iSolid == vcFormations.size() );

          pForm = new CTetraFormation( m_vcTSolids[ iSolid ]->Name(), Model());
          vcFormations.push_back( pForm ); // INSERT FORMATION !!!
    }
      }

    const geo::CBodyGroup& volume = static_cast<const geo::CBodyGroup&>(TetMesh().Volume(iVolume));
      CTetraFormationVolume* pVol = new CTetraFormationVolume(*pForm, const_cast<geo::CBodyGroup&>(volume));
      pVol->reParent(pForm);
  if (m_vcTSolids[iSolid]->TetraRegionMap().size() == 0)
      pVol->Name(QString("%1 %2").arg(pForm->Name()).arg(pForm->VolumeSize()));
  else
      pVol->Name(m_vcTSolids[iSolid]->Volume(iSolidVol).Name());
  }
}

void CGocadMeshImporter::set_tetras( int iSolid, int iVolume, TVertex2NodeIndexMap& mpVertex2NodeIndex, std::vector< int >& vol_elems_v, CProgressBase& progdlg )
{
  const CGocadData::CTSolid* solid = m_vcTSolids[ iSolid ];
  const CGocadData::CTVolume& tvolume = solid->Volume(iVolume);

  if (tvolume.DoubleSidedFaultVolume())
    return;

  //std::vector< int > vcVolumeElements( tvolume.TetraSize() );

  int num_tetras = tvolume.TetraSize();
  for( int iTetra = 0; iTetra < num_tetras; iTetra++ )
  {
    const CGocadData::CTetra& tetra = tvolume.Tetra(iTetra);
    std::vector<int> vcTetra(4);
  for( int iNode = 0; iNode < 4; ++iNode )
  {
      const CGocadData::CNode& node = tetra.Node(iNode);
      const CGocadData::CVertex* pVertex = 0;

      switch (node.Type())
      {
      case CGocadData::IS_VERTEX:			pVertex = static_cast<const CGocadData::CVertex *>(&node);					break;
      case CGocadData::IS_ATOM:			pVertex = &static_cast<const CGocadData::CAtom&>(node).Vertex();			break;
      case CGocadData::IS_SHAREDVERTEX:	pVertex = &static_cast<const CGocadData::CSharedVertex&>(node).Vertex();	break;
      }

      // MCR 2018-02-09; why this verification? is it really needed? 
      assert(pVertex != 0);
      TVertex2NodeIndexMap::iterator it = mpVertex2NodeIndex.find(pVertex);
      assert(it != mpVertex2NodeIndex.end());

      vcTetra[iNode] = it->second;
    }

    bool res = geo::ITetrahedron::CheckOrientation(Node2Point(tetra.Node(0)), Node2Point(tetra.Node(1)), Node2Point(tetra.Node(2)), Node2Point(tetra.Node(3)));
    if( !res )
    {
      // swap nodes 3 and 4
      int tmp = vcTetra[3];
      vcTetra[3] = vcTetra[2];
      vcTetra[2] = tmp;
    }

    int iElementIndex = TetMesh().RegisterTetrahedronInMainMesh( vcTetra ); // INSERT TETRA !!!

    vol_elems_v.push_back( iElementIndex );

    try
    {
      progdlg.Step();
    }
    catch( CProgressCancel* )
    {
      throw;
    }
  }

  // MCR: moved to another part of the code
  //int volume_idx = TetMesh().RegisterTetVolume( vcVolumeElements );// ATENTTION !!! : all tetras in one volume (bodygroup) 
  //IT_Printer::numbers( vcVolumeElements ); //FIXME: singleton, etc.
}

int CGocadMeshImporter::set_points( int iSolid, int iVolume, TVertex2NodeIndexMap& mpVertex2NodeIndex, CProgressBase& progdlg )
{
  const CGocadData::CTSolid* solid = m_vcTSolids[ iSolid ];
  const CGocadData::CTVolume& tvolume = solid->Volume(iVolume);

  int num_points = tvolume.VertexSize();
  for( int iVertex = 0; iVertex < num_points; ++iVertex)
  {
    const CGocadData::CVertex& vertex = tvolume.Vertex(iVertex);
    geo::CPoint point = Node2Point( vertex );
    int iNodeIndex = TetMesh().RegisterNodeInMainMesh( point ); // INSERT POINT !!!

    VERIFY( mpVertex2NodeIndex.insert( TVertex2NodeIndexMap::value_type( &vertex, iNodeIndex ) ).second ); // MCR ???????

    try
    {
      progdlg.Step();
    }
    catch( CProgressCancel* )
    {
      throw;
    }
  }
  return num_points;
}

bool CGocadMeshImporter::Import()
{
  // create bottom and top horizons
  CTetraSubHorizon* pTop = new CTetraSubHorizon("Top Horizon", FALSE, Model());
  CTetraSubHorizon* pBottom = new CTetraSubHorizon("Bottom Horizon", FALSE, Model());

  CTetraSubBoundary& boundary = ((CTetraModel&)Model()).SubBoundary();
  boundary.TopHorizon().LinkTo(*pTop);
  boundary.BottomHorizon().LinkTo(*pBottom);

  // insert all unique nodes (read: the vertices (VRTX))

  TVertex2NodeIndexMap mpVertex2NodeIndex; // from VRTX object to mesh node index

  std::auto_ptr <CProgressBase> progdlg( _g->prog()->create( eProgress::Dual, 4, "Import Gocad mesh", true));

  try
  {
    // #1: insert nodes and elements

    progdlg->NextJob("Inserting nodes and elements");
    progdlg->AddSteps( get_steps( PointsPlusTetras ));

    int num_solids = m_vcTSolids.size();
    for( int iSolid = 0; iSolid < num_solids; iSolid++ )
    {
      const CGocadData::CTSolid* solid = m_vcTSolids[ iSolid ];
      int num_vols = solid->VolumeSize();
      for (int iVolume = 0; iVolume < num_vols; ++iVolume)
      {
    int num_points = set_points(iSolid, iVolume, mpVertex2NodeIndex, *progdlg);
      }
      for( int iVolume = 0; iVolume < num_vols; ++iVolume )
      {
        std::vector< int > vol_elems_v;
        set_tetras( iSolid, iVolume, mpVertex2NodeIndex, vol_elems_v, *progdlg ); // Fills 'model->m_je.m_tetras_m'

        if( !solid->Volume(iVolume).DoubleSidedFaultVolume()) // TODO: 'volume_idx' ????
          int volume_idx = TetMesh().RegisterTetVolume( vol_elems_v );// ATENTTION !!! : all tetras in one volume (bodygroup) 
      }

    }
    //assert(TetMesh().NrOfVolumes() == m_tsolid.VolumeSize());

    bool res = set_surfaces( *progdlg ); // Fills 'm_vcGocadSurfaces'
    if( res )
    {
      res = create_gm_objs( *progdlg ); // THE CORE PROCESS !!!!!
      DestroyGocadSurfaces(); // Deletes 'm_vcGocadSurfaces'
      if( !res )
        return false;
    }
    else
    {
      delete pTop;
      delete pBottom;
      return false;
    }
    // (4) create formations
    set_formations(); // old implementation ( not valid for je)
  }
  catch( CProgressCancel* e )
  {
    delete e;
    delete pTop;
    delete pBottom;

    return false;
  }

  return true;
}

int CGocadMeshImporter::get_steps( eProgress type_ )
{
  int nTotalSteps = 0;
  switch( type_ )
  {
    case Surface:
    {
      for(int iSolid = 0; iSolid < m_vcTSolids.size(); ++iSolid)
      {
        const CGocadData::CTSolid* solid = m_vcTSolids[ iSolid ];
        for(int i = 0; i < solid->SurfaceSize(); ++i)
          nTotalSteps += solid->Surface(i).TFaceSize();
      }
      break;
    }
    case ConnectivityMap:
    {
      for( int iVolume = 0; iVolume < TetMesh().NrOfVolumes(); ++iVolume)
        nTotalSteps += TetMesh().Volume(iVolume).BodySize();
    }
    case PointsPlusTetras:
    {
      for(int iSolid = 0; iSolid < m_vcTSolids.size(); ++iSolid)
      {
        const CGocadData::CTSolid* solid = m_vcTSolids[ iSolid ];
        for( int iVolume = 0; iVolume < solid->VolumeSize(); ++iVolume)
        {
          const CGocadData::CTVolume& volume = solid->Volume(iVolume);
          nTotalSteps +=	( volume.VertexSize() + volume.TetraSize() );
        }
      }
    }

    default:
      break;
  }
  return nTotalSteps;
}

void CGocadMeshImporter::DestroyGocadSurfaces()
{
  // destroy the gocad surfaces
  for(size_t i = 0; i < m_vcGocadSurfaces.size(); ++i)
  delete m_vcGocadSurfaces[i];

  m_vcGocadSurfaces.clear();
}

CGocadMeshImporter::TPositionFacesMap CGocadMeshImporter::map_position_faces( TPositionTriangleSet& stGocadSurfaceTriangles, CProgressBase& progdlg )
{
  TPositionFacesMap mpPositionFaces;

  int num_vols = TetMesh().NrOfVolumes();
  for( int i = 0; i < num_vols; ++i )
  {
    const geo::CBodyGroup& vol = TetMesh().Volume( i ); 
    int num_bodies = vol.BodySize();
    for(int j = 0; j < num_bodies; ++j)
    {
      const geo::IBody& body = vol.Body( j );
      int num_faces = body.NrOfFaces();
      for( int k = 0; k < num_faces; ++k)
      {
        const geo::IFace& face = body.Face(k);
        if(stGocadSurfaceTriangles.find(CPositionTriangle(face)) != stGocadSurfaceTriangles.end())
        {
          TPositionFacesMap::iterator it = mpPositionFaces.insert( TPositionFacesMap::value_type( CPositionTriangle( face ), TFaceBodyPairSet() ) ).first;
          it->second.insert( std::make_pair( &face, &TetMesh().Volume(i) ) );
        }
        try
        {
          progdlg.Step();
        }
        catch (CProgressCancel*)
        {
          throw;
        }
      }
    }
  }
  return mpPositionFaces;
}

CGocadMeshImporter::TPositionTriangleSet CGocadMeshImporter::gocad_sur_tri_set()
{
  TPositionTriangleSet stGocadSurfaceTriangles;
  for( int iSolid = 0; iSolid < m_vcTSolids.size(); ++iSolid )
  {
    const CGocadData::CTSolid* solid = m_vcTSolids[ iSolid ];
    for( int  i = 0; i < solid->SurfaceSize(); ++i )
    {
      const CGocadData::CSurface& sur = solid->Surface( i );
      add_to_gocad_sur_tri_set( sur, stGocadSurfaceTriangles );
    }
  }
  return stGocadSurfaceTriangles;
}

// For '.ts' file
void CGocadMeshImporter::add_to_gocad_sur_tri_set( const CGocadData::CSurface& sur, TPositionTriangleSet& stGocadSurfaceTriangles )
{
  int num_tfaces = sur.TFaceSize();
  for( int i = 0; i < num_tfaces; i++ )
  {
    const CGocadData::CTFace& tface = sur.TFace( i );
    int num_tri = tface.TriangleSize();
    for( int j = 0; j < num_tri; j++ )
    {
      const CGocadData::CTriangle& tri = tface.Triangle( j );

      if( !&tri.Node( 0 ) || !&tri.Node( 1 ) || !&tri.Node( 2 ) )
      {
        //Printer::instance()->error("add_to_gocad_sur_tri_set > tface: %d, tri: %d, node: %d", i, j, !&tri.Node( 0 ) ? 0 : !&tri.Node( 1 ) ? 1 : 2 );
      }
      else
        stGocadSurfaceTriangles.insert( CPositionTriangle( tri ) );
    }

  }
}


bool CGocadMeshImporter::add_surface( const CGocadData::CSurface& gocadsurface, TPositionFacesMap& mpPositionFaces, CProgressBase& progdlg )
{
  CGocadSurface* pGocadSurface = new CGocadSurface(gocadsurface);

  assert(pGocadSurface->TFaceSize() == gocadsurface.TFaceSize());

  m_vcGocadSurfaces.push_back(pGocadSurface);

  int num_tfaces = pGocadSurface->TFaceSize();
  //Printer::instance()->debug("add_surface : num_tfaces: %d", num_tfaces);
  for( int i = 0; i < num_tfaces; i++ )
  {
    CGocadTFace& tface = pGocadSurface->TFace( i );
    bool res = tface.CollectFaces( mpPositionFaces );
    if( !res )
    {
      //Printer::instance()->error("add_surface > CollectFaces : ERROR" );
      DestroyGocadSurfaces();
      _m()->msg("Inconsistency found in the SURFACEs area of the input");
      return false;
    }

    bool res1 = tface.FrontGroup() == tface.BackGroup();
    bool res2 = pGocadSurface->Surface().DoubleSidedFaultSurface();

    if(res1 || res2)
    {
      pGocadSurface->MarkAsFault();
      //Printer::instance()->debug("Is FAULT");
    }
    try
    {
      progdlg.Step();
    }
    catch (CProgressCancel*)
    {
      throw;
    }
  }
  return true;
}

bool CGocadMeshImporter::set_surfaces( CProgressBase& progdlg )
{
  try
  {
  // collect a set containing the positions of all gocad surface triangles
  TPositionTriangleSet stGocadSurfaceTriangles = gocad_sur_tri_set();

  // #2: ref counts
  progdlg.NextJob("Building connectivity map");
  progdlg.AddSteps( get_steps( ConnectivityMap ) );

  // build a map from positions to sets of pairs of faces and the volumes they belong to
  TPositionFacesMap mpPositionFaces = map_position_faces( stGocadSurfaceTriangles, progdlg );

  // #3: ref counts
  progdlg.NextJob("Collecting surface faces");
  progdlg.AddSteps( get_steps( Surface ) );

  int num_solid = m_vcTSolids.size();
  for( int iSolid = 0; iSolid < num_solid; iSolid++ )
  {
    const CGocadData::CTSolid* solid = m_vcTSolids[ iSolid ];
    int num_sur = solid->SurfaceSize();
    for( int i = 0; i < num_sur; i++ )
    {
      const CGocadData::CSurface& sur = solid->Surface( i );
      add_surface( sur, mpPositionFaces, progdlg );
    }
  }
  }
  catch(CProgressCancel*)
  {
  DestroyGocadSurfaces();
  throw;
  }
  return true;
}

namespace
{
  QString message_01( std::vector<const geo::IFace *>& missing, const QString& gocad_sur_name )
  {
    const geo::IFace& face = *missing.front();

    int dec = 3;

    return QString("Fault '%1' (surface) has %2 faces without bodygroup\nExample:\nnode 1: (%3, %4, %5)\nnode 2: (%6, %7, %8)\nnode 3: (%9, %10, %11)")

      .arg( gocad_sur_name )
      .arg( missing.size() )
      
      .arg(QString::number( face.Point(0).X(), 'f', dec ) )
      .arg(QString::number( face.Point(0).Y(), 'f', dec ) )
      .arg(QString::number( face.Point(0).Z(), 'f', dec ) )

      .arg(QString::number( face.Point(1).X(), 'f', dec ) )
      .arg(QString::number( face.Point(1).Y(), 'f', dec ) )
      .arg(QString::number( face.Point(1).Z(), 'f', dec ) )

      .arg(QString::number( face.Point(2).X(), 'f', dec ) )
      .arg(QString::number( face.Point(2).Y(), 'f', dec ) )
      .arg(QString::number( face.Point(2).Z(), 'f', dec ) );
  }
}


bool CGocadMeshImporter::create_gm_objs_01( bool& bHaveDoubleSidedFaults, 
                      TAverageDepthMap& mpAverageDepth, 
                      CProgressBase& progdlg )
{
  CSurfaceEntry& surface_entry = (CSurfaceEntry&)*Model().GraphEntry(MD_BASE_SURFACE);


  int iFaultSurface = 0;
  int iSurface = 0;

  for(int i = 0; i < m_vcGocadSurfaces.size(); ++i)
  {
  CGocadSurface& gocadsurface = *m_vcGocadSurfaces[i];
  int num_tface = gocadsurface.TFaceSize();
  for(int j = 0; j < num_tface; ++j)
  {
      QString strName;
      assert(gocadsurface.TFace(j).FacePairSize() > 0);
      geo::CSurface tmpGeoSurf;
      double dAverageZ = 0;
    int num_tface_pairs = gocadsurface.TFace(j).FacePairSize();
      for(int k = 0; k < num_tface_pairs; ++k)
      {
    const CGocadTFace::TFacePair& prFaces = gocadsurface.TFace(j).FacePair(k);
    assert(prFaces.first != 0);
    tmpGeoSurf.AddFace(*prFaces.first);
    dAverageZ += prFaces.first->MidPoint().Z();
      }

      if(gocadsurface.IsFault()) // (1) FAULT
      {
    // check the back faces
    if (!gocadsurface.Surface().DoubleSidedFaultSurface())
    {
          std::vector<const geo::IFace *> missing;
          for( int k = 0; k < gocadsurface.TFace(j).FacePairSize(); ++k)
          {
      const CGocadTFace::TFacePair& prFaces = gocadsurface.TFace(j).FacePair(k);
      if (prFaces.second == 0)
      {
              missing.push_back(prFaces.first);
      }
          }
          if (!missing.empty())
          {
        QString s = message_01( missing, gocadsurface.Surface().Name() );
        //Printer::instance()->debug( "%s", s.toStdString().c_str() );
      _m()->msg(  s );
      return false;
          }
    }
    else
          bHaveDoubleSidedFaults = true;

    //strName = QString("Gocad fault surface %1").arg(++iFaultSurface);
    strName = QString("Gocad fault surface %1").arg(++iFaultSurface);
      }
      else // (2) NO FAULT
      {
    strName = QString("Gocad surface %1").arg(++iSurface);
    if(gocadsurface.TFace(j).FacePairSize() > 0)
    {
          dAverageZ /= gocadsurface.TFace(j).FacePairSize();
          mpAverageDepth.insert(TAverageDepthMap::value_type(dAverageZ, i));
    }
      }

      if (!gocadsurface.Surface().Name().isEmpty())
      {
      
    if( num_tface > 1 )
          strName = QString("%1 %2").arg(gocadsurface.Surface().Name()).arg(j);
    else
          strName = gocadsurface.Surface().Name();
      }

      assert(tmpGeoSurf.FaceSize() > 0);
      CSurfaceBase* pSurfaceBase = surface_entry.InsertSurface(strName, tmpGeoSurf);
      assert(gocadsurface.TFace(j).AttachedSurface() == 0);
      gocadsurface.TFace(j).AttachSurface(*pSurfaceBase); // GOCAD TFACE -> GM SURFACE
  } // for : num_tface

  // FAULT
  if( gocadsurface.IsFault() && (gocadsurface.TFaceSize() > ONE_FACE))
  {
      CNamedSurfaces faces;

      for (int face = 0; face < gocadsurface.TFaceSize(); ++face)
      {
    faces.push_back(STNamedSurface(
          gocadsurface.TFace(face).AttachedSurface()->Name().toStdString(),
          &(gocadsurface.TFace(face).AttachedSurface()->Surface())));
      }

      surface_entry.MergeSurfaces(faces);
  }

  try
  {
      progdlg.Step();
  }

  catch (CProgressCancel*)
  {
      throw;
  }
  }
  if(mpAverageDepth.size() < 2)
  {
  _m()->msg("Not enough surfaces detected to build a model");
  return false;
  }
  return true;

}

void CGocadMeshImporter::create_gm_objs_02( int iTopSurface, 
                      TSurfaceSourceMap& mpSurfaceSource, 
                      TBodyGroup2SurfaceMap& mpBodyGroup2Surface, 
                      CProgressBase& progdlg ) // top horizon
{
  CTetraSubHorizon* pTopHor = Model().SubBoundary().TopHorizon().Horizon();
  bool nameTopHorizonIsSet = false;

  assert(pTopHor);

  for(int i = 0; i < m_vcGocadSurfaces[ iTopSurface ]->TFaceSize(); ++i)
  {
  CSurfaceBase* pSurfaceBase = m_vcGocadSurfaces[iTopSurface]->TFace(i).AttachedSurface();

  if (!nameTopHorizonIsSet && !pSurfaceBase->Name().isEmpty())
  {
      pTopHor->Name(pSurfaceBase->Name());
      nameTopHorizonIsSet = true;
  }

  pTopHor->LinkTo( *pSurfaceBase ); // LINK

  mpSurfaceSource.insert(TSurfaceSourceMap::value_type(&m_vcGocadSurfaces[iTopSurface]->TFace(i), pSurfaceBase));

  TBodyGroup2SurfaceMap::iterator it = mpBodyGroup2Surface.insert( TBodyGroup2SurfaceMap::value_type(	m_vcGocadSurfaces[iTopSurface]->TFace(i).FrontGroup(), TSurfaceVec() ) ).first;
  it->second.push_back(TSurfaceDef(m_vcGocadSurfaces[iTopSurface]->TFace(i), true, false));
  }

}


void CGocadMeshImporter::create_gm_objs_03( int iBotSurface, 
                      TSurfaceSourceMap& mpSurfaceSource, 
                      TBodyGroup2SurfaceMap& mpBodyGroup2Surface, 
                      CProgressBase& progdlg ) // bottom horizon
{
  bool nameBottomHorizonIsSet = false;

  CTetraSubHorizon* pBotHor = Model().SubBoundary().BottomHorizon().Horizon();

  assert(pBotHor);

  for(int i = 0; i < m_vcGocadSurfaces[iBotSurface]->TFaceSize(); ++i)
  {
  CSurfaceBase* pSurfaceBase = m_vcGocadSurfaces[iBotSurface]->TFace(i).AttachedSurface();

  if (!nameBottomHorizonIsSet && !pSurfaceBase->Name().isEmpty())
  {
      pBotHor->Name(pSurfaceBase->Name());
      nameBottomHorizonIsSet = true;
  }

  pBotHor->LinkTo(*pSurfaceBase); // LINK

  mpSurfaceSource.insert( TSurfaceSourceMap::value_type( &m_vcGocadSurfaces[ iBotSurface ]->TFace( i ), pSurfaceBase) );

  TBodyGroup2SurfaceMap::iterator it = mpBodyGroup2Surface.insert( TBodyGroup2SurfaceMap::value_type( m_vcGocadSurfaces[ iBotSurface ]->TFace( i ).FrontGroup(), TSurfaceVec() ) ).first;
  it->second.push_back( TSurfaceDef( m_vcGocadSurfaces[iBotSurface]->TFace(i), true, false) );
  }
}

void CGocadMeshImporter::create_gm_objs_04( TSurfaceSourceMap& mpSurfaceSource, 
                      TBodyGroup2SurfaceMap& mpBodyGroup2Surface, 
                      int iTopSurface,
                      int iBotSurface,
                      CProgressBase& progdlg )
{
  int iFault = 0;
  int iHorizon = 0;

  for(int i = 0; i < m_vcGocadSurfaces.size(); ++i)
  {
  CGocadSurface& gocadsurface = *m_vcGocadSurfaces[i];
  //------------------------------------------------------ FAULTS
  if(gocadsurface.IsFault())
  {
      // create the fault object and attach the surface(s)
      QString strName;
      strName = QString("Fault %1").arg(++iFault);

      if (!gocadsurface.Surface().Name().isEmpty())
      {
    strName = gocadsurface.Surface().Name();
      }

      CTetraSubHorizon* pFault = new CTetraSubHorizon(strName, TRUE, Model());
      for(int j = 0; j < gocadsurface.TFaceSize(); ++j)
      {
    assert(gocadsurface.TFace(j).AttachedSurface() != 0);
    assert(gocadsurface.TFace(j).FrontGroup() != 0);
    assert(gocadsurface.TFace(j).BackGroup() != 0 || gocadsurface.Surface().DoubleSidedFaultSurface());

    CSurfaceBase* pSurfaceBase = gocadsurface.TFace(j).AttachedSurface();
    //Printer::instance()->debug("GM OBJS : Fault");
    pFault->LinkTo(*pSurfaceBase);  // LINK
    mpSurfaceSource.insert(TSurfaceSourceMap::value_type(&gocadsurface.TFace(j), pSurfaceBase));

    TBodyGroup2SurfaceMap::iterator it;
    it = mpBodyGroup2Surface.insert(TBodyGroup2SurfaceMap::value_type(gocadsurface.TFace(j).FrontGroup(), TSurfaceVec())).first;
    it->second.push_back(TSurfaceDef(gocadsurface.TFace(j), true, true));

    if (!gocadsurface.Surface().DoubleSidedFaultSurface())
    {
          it = mpBodyGroup2Surface.insert(TBodyGroup2SurfaceMap::value_type(gocadsurface.TFace(j).BackGroup(), TSurfaceVec())).first;
          it->second.push_back(TSurfaceDef(gocadsurface.TFace(j), false, true));
    }
      }
  }
  //------------------------------------------------------ OTHERS THAN FAULTS
  else
  {
      if( i != iTopSurface && i != iBotSurface) // these two have already been processed
      {
    for(int j = 0; j < gocadsurface.TFaceSize(); ++j)
    {
          assert(gocadsurface.TFace(j).AttachedSurface() != 0);
          assert(gocadsurface.TFace(j).FrontGroup() != 0);

          CTetraSubHorizon* pHor = 0;

          CSurfaceBase* pSurfaceBase = gocadsurface.TFace(j).AttachedSurface();
          TBodyGroup2SurfaceMap::iterator it1 = mpBodyGroup2Surface.insert( TBodyGroup2SurfaceMap::value_type( gocadsurface.TFace(j).FrontGroup(), TSurfaceVec() ) ).first;
          it1->second.push_back(TSurfaceDef( gocadsurface.TFace( j ), true, false) );

      // horizon
          if(gocadsurface.TFace(j).BackGroup() != 0)
          {
      if(!pHor)
      {
              QString strName = QString("Horizon %1").arg(++iHorizon);

              if (!gocadsurface.Surface().Name().isEmpty())
        strName = gocadsurface.Surface().Name();

              pHor = new CTetraSubHorizon(strName, FALSE, Model());
      }
      //Printer::instance()->debug("GM OBJS : Other horizon");
      pHor->LinkTo(*pSurfaceBase); // LINK
      mpSurfaceSource.insert(TSurfaceSourceMap::value_type(&gocadsurface.TFace(j), pSurfaceBase));

      TBodyGroup2SurfaceMap::iterator it2 = mpBodyGroup2Surface.insert( TBodyGroup2SurfaceMap::value_type( gocadsurface.TFace(j).BackGroup(), TSurfaceVec() ) ).first;
      it2->second.push_back(TSurfaceDef(gocadsurface.TFace(j), false, false));
          }
      // side surface
          else
          {
      //Printer::instance()->debug("GM OBJS : Boundary horizon");
       CTetraSubBoundary& boundary  = Model().SubBoundary();
      boundary.LinkTo(*pSurfaceBase); // LINK
      // CTetraMesh::m_vcSurfaceSource expects a pointer to the boundary for boundary surfaces
      mpSurfaceSource.insert(TSurfaceSourceMap::value_type(&gocadsurface.TFace(j), &boundary));
          }
    }
      }
  }

  try
  {
      progdlg.Step();
  }

  catch (CProgressCancel*)
  {
      throw;
  }
  }
}

void CGocadMeshImporter::create_gm_objs_05( TSurfaceSourceMap& mpSurfaceSource, 
                      TBodyGroup2SurfaceMap& mpBodyGroup2Surface, 
                      int bHaveDoubleSidedFaults,
                      CProgressBase& progdlg )
{
  TSurface2SurfaceDescMap mpSurface2SurfaceDesc;

    TBodyGroup2SurfaceMap::iterator it;
  for(it = mpBodyGroup2Surface.begin(); it != mpBodyGroup2Surface.end(); ++it)
  {
  geo::CBodyGroup& vol = const_cast<geo::CBodyGroup&>(*it->first);

  for(size_t i = 0; i < it->second.size(); ++i)
  {
      CGocadTFace& tface = it->second[i].m_tface;
      bool bFrontGroup = it->second[i].m_bFrontGroup;
      bool bFault = it->second[i].m_bFault;

      assert(tface.AttachedSurface() != 0);
      CSurfaceBase& surfacebase = *tface.AttachedSurface();

      assert(mpSurfaceSource.find(&tface) != mpSurfaceSource.end());

      geo::CSurfaceDesc& surfacedesc = GetSurfaceDescriptor(surfacebase, mpSurface2SurfaceDesc, bFault, *mpSurfaceSource[&tface]);

      geo::CTetSurface* pTetSurface = new geo::CTetSurface(surfacedesc, vol);
      for(int j = 0; j < tface.FacePairSize(); ++j)
      {
    const geo::IFace* pFace;
    if(bFrontGroup)
    {
          assert(&vol == tface.FrontGroup());
          pFace = tface.FacePair(j).first;
    }
    else
    {
          assert(&vol == tface.BackGroup());
          pFace = tface.FacePair(j).second;
    }

    assert(pFace != 0);
    //Printer::instance()->debug("GM OBJS : Add face to surface...");
    pTetSurface->AddFace(*pFace); // -------------------- ADD FACE TO SURFACE
      }
  }
  }
  assert(m_tetramesh.m_vcSurfaceSource.size() == m_tetramesh.m_pMesh->NrOfSurfaces());

  if (bHaveDoubleSidedFaults)
  CreateIntermediateSurfaces(progdlg, mpSurface2SurfaceDesc);
  else
  CreateInterfaceElements(progdlg, mpSurface2SurfaceDesc);
}


// input : m_vcSurfaceSource
bool CGocadMeshImporter::create_gm_objs( CProgressBase& progdlg )
{
  m_tetramesh.m_vcSurfaceSource.clear();

  bool bHaveDoubleSidedFaults = false;

  TAverageDepthMap mpAverageDepth;

  int nTotalSteps = 3 * m_vcGocadSurfaces.size();
  
  progdlg.NextJob("Creating objects");
  progdlg.AddSteps(nTotalSteps);

  bool res = create_gm_objs_01( bHaveDoubleSidedFaults, mpAverageDepth, progdlg );

  // map surfaces and whether they are faults to the body groups

  TBodyGroup2SurfaceMap mpBodyGroup2Surface;
  TSurfaceSourceMap mpSurfaceSource;

  // determine and create top and bottom horizons
  // top and bottom horizon objects have already been created

  int iTopSurface = mpAverageDepth.begin()->second;
  create_gm_objs_02( iTopSurface, mpSurfaceSource, mpBodyGroup2Surface, progdlg ); // top horizon

  int iBotSurface = mpAverageDepth.rbegin()->second;
  create_gm_objs_03( iBotSurface, mpSurfaceSource, mpBodyGroup2Surface, progdlg ); // bottom horizon
  
  create_gm_objs_04( mpSurfaceSource, mpBodyGroup2Surface, iTopSurface, iBotSurface, progdlg );

  // create the tetra surfaces
  create_gm_objs_05( mpSurfaceSource, mpBodyGroup2Surface, bHaveDoubleSidedFaults, progdlg );

  
#if 0
  // create the fault interface elements
  for(size_t i = 0; i < m_vcGocadSurfaces.size(); ++i)
  {
  CGocadSurface& gocadsurface = *m_vcGocadSurfaces[i];
  if(gocadsurface.IsFault())
  {
      int j;
      for(j = 0; j < gocadsurface.TFaceSize(); ++j)
      {
    CGocadTFace& tface = gocadsurface.TFace(j);
    assert(tface.AttachedSurface() != 0);
    TSurface2SurfaceDescMap::iterator itsd = mpSurface2SurfaceDesc.find(tface.AttachedSurface());
    assert(itsd != mpSurface2SurfaceDesc.end());
    geo::CSurfaceDesc& surfdesc = *itsd->second;

    if(tface.FrontGroup() != 0 && tface.BackGroup() != 0)
    {
          int k;
          for(k = 0; k < tface.FacePairSize(); ++k)
          {
      const CGocadTFace::TFacePair& facepair = tface.FacePair(k);
      const geo::IFace* face1 = facepair.first;
      const geo::IFace* face2 = facepair.second;
      assert(face1 != face2);

      TetMesh().AddInterfaceElement(surfdesc, face1, face2);
          }
    }
      }
  }

  try
  {
      progdlg.Step();
      ++nStep;
  }

  catch (CProgressCancel*)
  {
      throw;
  }
  }
#endif

  return true;
}

void CGocadMeshImporter::CreateInterfaceElements(IProgressBase& progdlg, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc)
{
  // create the fault interface elements
  for (size_t i = 0; i < m_vcGocadSurfaces.size(); ++i)
  {
  CGocadSurface& gocadsurface = *m_vcGocadSurfaces[i];
  if (gocadsurface.IsFault())
  {
      assert(!gocadsurface.Surface().DoubleSidedFaultSurface());
      int j;
      for (j = 0; j < gocadsurface.TFaceSize(); ++j)
      {
    CGocadTFace& tface = gocadsurface.TFace(j);
    assert(tface.AttachedSurface() != 0);
    TSurface2SurfaceDescMap::iterator itsd = mpSurface2SurfaceDesc.find(tface.AttachedSurface());
    assert(itsd != mpSurface2SurfaceDesc.end());
    geo::CSurfaceDesc& surfdesc = *itsd->second;

    if (tface.FrontGroup() != 0 && tface.BackGroup() != 0)
    {
          int k;
          for (k = 0; k < tface.FacePairSize(); ++k)
          {
      const CGocadTFace::TFacePair& facepair = tface.FacePair(k);
      const geo::IFace* face1 = facepair.first;
      const geo::IFace* face2 = facepair.second;
      assert(face1 != face2);

      TetMesh().AddInterfaceElement(surfdesc, face1, face2);
          }
    }
      }
  }

  try
  {
      progdlg.Step();
  }

  catch (CProgressCancel*)
  {
      throw;
  }
  }
}

#if 0
double CGocadMeshImporter::TriangleInfo::SquaredDistance(const geo::IPoint& p)
{
  return (p.X() - x) * (p.X() - x) + (p.Y() - y) * (p.Y() - y) + (p.Z() - z) * (p.Z() - z);
}

// Determines whether a point lies more or less inside a triangle
// Coming from SKUA/Gocad, this can be a bit imprecise
// First we detect if the point is inside a circle of 1.3 * radius
// Then we move the point SignedDistance closer along the normal, and 20% closer to the midpoint

double CGocadMeshImporter::TriangleInfo::Contains(const geo::IPoint& p)
{
  if (SquaredDistance(p) < r2)
  {
  geo::CVector normal = triangle->Normal();
  double signedDist = triangle->SignedDistance(p, true);

  geo::CPoint trans(p.X() - signedDist * normal.X(), p.Y() - signedDist * normal.Y(), p.Z() - signedDist * normal.Z());

  geo::CVector v(trans, triangle->MidPoint());
  v = v * 0.2;

  trans.X(trans.X() + v.X());
  trans.Y(trans.Y() + v.Y());
  trans.Z(trans.Z() + v.Z());

  // see IFace's Contains
  double angle = 0;

  for (int i = 0; i < 3; ++i)
  {
      geo::CVector v1(trans, triangle->Point(i));
      geo::CVector v2(trans, triangle->Point((i + 1) % 3));

      angle += v1.AngleRad(v2);
  }

  angle = fabs(fabs(angle) - TWOPI);

  return angle < 0.1;
  }
  return false;
}

CGocadMeshImporter::TriangleInfo::TriangleInfo(const geo::CBodyTriangle *triangle) : triangle(triangle), x(triangle->MidPoint().X()), y(triangle->MidPoint().Y()), z(triangle->MidPoint().Z()), r2(0)
{
  for (int i = 0; i < 3; ++i)
  {
  double r2candidate = SquaredDistance(triangle->Point(i)) * 1.3;

  if (r2candidate > r2)
      r2 = r2candidate;
  }
}


void CGocadMeshImporter::getTyingInformation(TNodeInfoMap& mpNodeInfo, CGocadSurface& minusGocadsurface, CGocadSurface& plusGocadsurface, double eps)
{
  double min_eps = -eps;
  double max_eps = 1 + eps;

  // retrieve triangles from plus surface
  std::vector<TriangleInfo> vcTriangles;

  for (int j = 0; j < plusGocadsurface.TFaceSize(); ++j)
  {
  CGocadTFace& tface = plusGocadsurface.TFace(j);

  for (int k = 0; k < tface.FacePairSize(); ++k)
  {
      const CGocadTFace::TFacePair& pair = tface.FacePair(k);

      const geo::CBodyTriangle *face = static_cast<const geo::CBodyTriangle *>(pair.first);

      vcTriangles.push_back(TriangleInfo(face));
  }
  }

  // find unique nodes in minus surface; turn them into copies and find the corresponding plus triangles

  for (int j = 0; j < minusGocadsurface.TFaceSize(); ++j)
  {
  CGocadTFace& tface = minusGocadsurface.TFace(j);

  for (int k = 0; k < tface.FacePairSize(); ++k)
  {
      const CGocadTFace::TFacePair& facepair = tface.FacePair(k);
      const geo::IFace* face1 = facepair.first;

      assert(!facepair.second);

      for (int m = 0; m < face1->NrOfPoints(); ++m)
      {
    const geo::IPoint& point = face1->Point(m);

    std::pair<TNodeInfoMap::iterator, bool> retval = mpNodeInfo.insert(std::make_pair(face1->PointIndex(m), TNodeInfo()));
    if (retval.second)
    {
          // copy node
          //retval.first->second.first = TetMesh().RegisterNode(point, false);

          // find triangle
          for (int n = 0; n < vcTriangles.size(); ++n)
          {
      if (vcTriangles[n].Contains(point))
      {
              retval.first->second.second = vcTriangles[n].triangle;
              std::vector<double> isoPt = vcTriangles[n].triangle->WorldToIso(point);
              if (isoPt[0] < max_eps && isoPt[0] > min_eps && isoPt[1] < max_eps && isoPt[1] > min_eps)
              {
        retval.first->second.second = vcTriangles[n].triangle;
        break;
              }
      }
          }

          assert(retval.first->second.second);
    }
      }
  }
  }
}

void CGocadMeshImporter::getTyingInformation(TNodeInfoMap& mpNodeInfo, CSurfaceBase& fault, CGocadSurface& plusGocadsurface, double eps)
{
  double min_eps = -eps;
  double max_eps = 1 + eps;

  // retrieve triangles from plus surface
  std::vector<TriangleInfo> vcTriangles;

  for (int j = 0; j < plusGocadsurface.TFaceSize(); ++j)
  {
  CGocadTFace& tface = plusGocadsurface.TFace(j);

  for (int k = 0; k < tface.FacePairSize(); ++k)
  {
      const CGocadTFace::TFacePair& pair = tface.FacePair(k);

      const geo::CBodyTriangle *face = static_cast<const geo::CBodyTriangle *>(pair.first);

      vcTriangles.push_back(TriangleInfo(face));
  }
  }

  // find unique nodes in minus surface; turn them into copies and find the corresponding plus triangles

  for (int j = 0; j < fault.Surface().FaceSize(); ++j)
  {
  const geo::IFace* face1 = &fault.Surface().Face(j);

  for (int m = 0; m < face1->NrOfPoints(); ++m)
  {
      const geo::IPoint& point = face1->Point(m);

      std::pair<TNodeInfoMap::iterator, bool> retval = mpNodeInfo.insert(std::make_pair(face1->PointIndex(m), TNodeInfo()));
      if (retval.second)
      {
    // copy node
    //retval.first->second.first = TetMesh().RegisterNode(point, false);

    // find triangle
    for (int n = 0; n < vcTriangles.size(); ++n)
    {
          if (vcTriangles[n].Contains(point))
          {
      retval.first->second.second = vcTriangles[n].triangle;
      std::vector<double> isoPt = vcTriangles[n].triangle->WorldToIso(point);
      if (isoPt[0] < max_eps && isoPt[0] > min_eps && isoPt[1] < max_eps && isoPt[1] > min_eps)
      {
              retval.first->second.second = vcTriangles[n].triangle;
              break;
      }
          }
    }

    assert(retval.first->second.second);
      }
  }
  }
}

void CGocadMeshImporter::duplicateNodes(TNodeInfoMap& mpNodeInfo, const geo::IElementSet& set)
{
  geo::CTetMeshBase& mesh = TetMesh();

  for (TNodeInfoMap::iterator it = mpNodeInfo.begin(); it != mpNodeInfo.end(); ++it)
  {
  it->second.first = mesh.RegisterNode(set.Point(it->first), false);
  }
}
#endif

CGocadMeshImporter::CGocadSurface *CGocadMeshImporter::findOppositeFault(CGocadSurface& fault)
{
  assert(fault.IsFault());
  assert(fault.Surface().DoubleSidedFaultSurface());

  QString otherName;

  if (fault.Surface().Name().endsWith("_minus", Qt::CaseInsensitive))
  otherName = fault.Surface().Name().left(fault.Surface().Name().length() - 6) + "_plus";
  else
  otherName = fault.Surface().Name().left(fault.Surface().Name().length() - 5) + "_minus";

  CGocadSurface *otherGocadsurface = 0;
  for (int j = 0; j < m_vcGocadSurfaces.size(); ++j)
  {
  if (m_vcGocadSurfaces[j]->Surface().Name() == otherName)
  {
      otherGocadsurface = m_vcGocadSurfaces[j];
      break;
  }
  }

  assert(otherGocadsurface);

  return otherGocadsurface;
}

#if 0
void CGocadMeshImporter::createIFElements(TNodeInfoMap& mpNodeInfo, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc, CGocadSurface& fault)
{
  std::vector<int> nodes(3);
  std::vector<const geo::CBodyTriangle *> triangles(3);

  for (int j = 0; j < fault.TFaceSize(); ++j)
  {
  CGocadTFace& tface = fault.TFace(j);
  CSurfaceBase *surf = tface.AttachedSurface();
  assert(surf != 0);
  TSurface2SurfaceDescMap::iterator itsd = mpSurface2SurfaceDesc.find(surf);
  assert(itsd != mpSurface2SurfaceDesc.end());
  geo::CSurfaceDesc& surfdesc = *itsd->second;

  if (tface.FrontGroup() != 0)
  {
      for (int k = 0; k < tface.FacePairSize(); ++k)
      {
    const CGocadTFace::TFacePair& facepair = tface.FacePair(k);
    const geo::IFace* face1 = facepair.first;
    const geo::IFace* face2 = facepair.second;

    assert(!face2);
    for (int m = 0; m < face1->NrOfPoints(); ++m)
    {
          nodes[m] = mpNodeInfo[face1->PointIndex(m)].first;
          triangles[m] = mpNodeInfo[face1->PointIndex(m)].second;
    }

    const_cast<CGocadTFace::TFacePair&>(facepair).second = face2 = new geo::CTiedBodyTriangle(TetMesh(), nodes, triangles);

    TetMesh().AddInterfaceElement(surfdesc, face1, face2);
      }
  }
  }
}

void CGocadMeshImporter::createIFElements(TNodeInfoMap& mpMinus, TNodeInfoMap& mpPlus, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc, CSurfaceBase& fault)
{
  std::vector<int> nodes(3);
  std::vector<const geo::CBodyTriangle *> triangles(3);

  TSurface2SurfaceDescMap::iterator itsd = mpSurface2SurfaceDesc.find(&fault);
  assert(itsd != mpSurface2SurfaceDesc.end());
  geo::CSurfaceDesc& surfdesc = *itsd->second;

  for (int i = 0; i < fault.Surface().FaceSize(); ++i)
  {
  const geo::IFace& face = fault.Surface().Face(i);

  for (int j = 0; j < face.NrOfPoints(); ++j)
  {
      nodes[j] = mpMinus[face.PointIndex(j)].first;
      triangles[j] = mpMinus[face.PointIndex(j)].second;
  }

  const geo::IFace *face1 = new geo::CTiedBodyTriangle(TetMesh(), nodes, triangles);
      
  for (int j = 0; j < face.NrOfPoints(); ++j)
  {
      nodes[j] = mpPlus[face.PointIndex(j)].first;
      triangles[j] = mpPlus[face.PointIndex(j)].second;
  }

  const geo::IFace *face2 = new geo::CTiedBodyTriangle(TetMesh(), nodes, triangles);

  TetMesh().AddInterfaceElement(surfdesc, face1, face2);
  }
}

void CGocadMeshImporter::createTyings(TNodeInfoMap& mpNodeInfo)
{
  CRotatedSystem rs(Model().Boundary().BoxAzimuth(), Model().Boundary().Box().MidPoint());

  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same

  for (TNodeInfoMap::iterator it = mpNodeInfo.begin(); it != mpNodeInfo.end(); ++it)
  {
  int nIndex = it->second.first;
  const geo::IPoint& point = TetMesh().Node(nIndex);
  const geo::CBodyTriangle *triangle = it->second.second;

  geo::CTriangle localTriangle(rs.ToLocal(triangle->Point(0)), rs.ToLocal(triangle->Point(1)), rs.ToLocal(triangle->Point(2)));

  std::vector<double> isoPt = localTriangle.WorldToIso(rs.ToLocal(point));
  isoPt.push_back(1 - isoPt[0] - isoPt[1]);

  std::vector<geo::CTying::TMasterDef> vcMasters(3);

  for (int axisIndex = 0; axisIndex < 3; ++axisIndex) // three axes
  {
      for (int pointIndex = 0; pointIndex < 3; ++pointIndex) // isoparametric factor for three points
      {
    vcMasters[pointIndex].direction = &rsNormal[axisIndex];
    vcMasters[pointIndex].node = triangle->PointIndex(pointIndex);
    vcMasters[pointIndex].type = geo::CTying::TR;
    vcMasters[pointIndex].factor = isoPt[pointIndex];
      }
      m_tetramesh.Mesh().CreateTying(nIndex, rsNormal[axisIndex], geo::CTying::TR, vcMasters);
  }
  }
}

#if 0
void CGocadMeshImporter::CreateInterfaceElementsDSF(IProgressBase& progdlg, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc)
{
  CSurfaceEntry& surface_entry = (CSurfaceEntry&)*Model().GraphEntry(MD_BASE_SURFACE);

  // create the fault interface elements
  for (size_t i = 0; i < m_vcGocadSurfaces.size(); ++i)
  {
  CGocadSurface& minusGocadSurface = *m_vcGocadSurfaces[i];
  if (minusGocadSurface.IsFault())
  {
      assert(minusGocadSurface.Surface().DoubleSidedFaultSurface());

      // we handle the minus surface (front)
      if (minusGocadSurface.Surface().Name().endsWith("_minus", Qt::CaseInsensitive))
      {
    TNodeInfoMap mpNodeInfo;

    CGocadSurface *plusGocadSurface = findOppositeFault(minusGocadSurface);

    assert(plusGocadSurface);

    getTyingInformation(mpNodeInfo, minusGocadSurface, *plusGocadSurface);

    duplicateNodes(mpNodeInfo, TetMesh());

    createIFElements(mpNodeInfo, mpSurface2SurfaceDesc, minusGocadSurface);

    createTyings(mpNodeInfo);
      }
  }

  try
  {
      progdlg.Step();
  }

  catch (CProgressCancel*)
  {
      throw;
  }
  }
}
#endif

void CGocadMeshImporter::getIntermediateSurfaceInfo(std::vector<geo::CPoint>& points, std::vector<std::vector<int> >& triangles, TNodeInfoMap& mpNodeInfo, CGocadSurface& surface)
{
  std::map<int, int> mpPoints;

  for (int j = 0; j < surface.TFaceSize(); ++j)
  {
  CGocadTFace& tface = surface.TFace(j);

  if (tface.FrontGroup() != 0)
  {
      for (int k = 0; k < tface.FacePairSize(); ++k)
      {
    const CGocadTFace::TFacePair& facepair = tface.FacePair(k);
    const geo::IFace* face1 = facepair.first;
    const geo::IFace* face2 = facepair.second;

    assert(!face2);

    std::vector<int> t(3);

    for (int m = 0; m < face1->NrOfPoints(); ++m)
    {
          std::map<int, int>::iterator it = mpPoints.find(face1->PointIndex(m));
          if (it == mpPoints.end())
          {
      geo::CPoint p = face1->Point(m);
      const geo::CBodyTriangle *triangle = mpNodeInfo[face1->PointIndex(m)].second;

      geo::CPlane plane(triangle->Line(0).First(), triangle->Normal());

      geo::CPoint p1 = plane.Project(p);

      mpPoints[face1->PointIndex(m)] = points.size();

      t[m] = (int)points.size();

      points.push_back((p + p1) / 2);
          }
          else
      t[m] = it->second;
    }
    triangles.push_back(t);
      }
  }
  }

}
#endif

void CGocadMeshImporter::CreateIntermediateSurfaces(IProgressBase& progdlg, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc)
{
  for (size_t i = 0; i < m_vcGocadSurfaces.size(); ++i)
  {
  CGocadSurface& minusGocadSurface = *m_vcGocadSurfaces[i];
  if (minusGocadSurface.IsFault())
  {
      assert(minusGocadSurface.Surface().DoubleSidedFaultSurface());

      if (minusGocadSurface.Surface().Name().endsWith("_minus", Qt::CaseInsensitive))
      {
    QString name = minusGocadSurface.Surface().Name().left(minusGocadSurface.Surface().Name().length() - 6);

    CGocadSurface *plusGocadSurface = findOppositeFault(minusGocadSurface);

    assert(plusGocadSurface);

    gm_dsf::DSFIntermediateSurfaceGenerator gen(m_tetramesh);

    gm_dsf::DSFIntermediateSurfaceGenerator::TSurface fault_minus;
    gm_dsf::DSFIntermediateSurfaceGenerator::TSurface fault_plus;

    for (int j = 0; j < minusGocadSurface.TFaceSize(); ++j)
    {
          for (int k = 0; k < minusGocadSurface.TFace(j).FacePairSize(); ++k)
          {
      fault_minus.push_back(minusGocadSurface.TFace(j).FacePair(k).first);
          }
    }

    for (int j = 0; j < plusGocadSurface->TFaceSize(); ++j)
    {
          for (int k = 0; k < plusGocadSurface->TFace(j).FacePairSize(); ++k)
          {
      fault_plus.push_back(plusGocadSurface->TFace(j).FacePair(k).first);
          }
    }

    gen.GenerateIntermediateSurface(name, fault_minus, fault_plus);

#if 0

    TNodeInfoMap mpMinus, mpPlus;

    // Minus fault
    std::vector<geo::CPoint> vcPointsMinus;
    std::vector<std::vector<int> > vcTrianglesMinus;

    getTyingInformation(mpMinus, minusGocadSurface, *plusGocadSurface);
    getIntermediateSurfaceInfo(vcPointsMinus, vcTrianglesMinus, mpMinus, minusGocadSurface);

    // Plus fault
    std::vector<geo::CPoint> vcPointsPlus;
    std::vector<std::vector<int> > vcTrianglesPlus;
    
    getTyingInformation(mpPlus, *plusGocadSurface, minusGocadSurface);
    getIntermediateSurfaceInfo(vcPointsPlus, vcTrianglesPlus, mpPlus, *plusGocadSurface);

    // Select plus or minus based on size: the larger one is the outersurface in a curved fault in our examples; THIS IS NOT A GOOD TEST!
    std::vector<geo::CPoint> *pvcPoints;
    std::vector<std::vector<int> > *pvcTriangles;

    if (vcTrianglesPlus.size() > vcTrianglesMinus.size())
    {
          pvcPoints = &vcPointsMinus;
          pvcTriangles = &vcTrianglesMinus;
    }
    else
    {
          pvcPoints = &vcPointsPlus;
          pvcTriangles = &vcTrianglesPlus;
    }

    CTSSurfaceProgress SurfaceProgress;
    CTSSurface tempMinus(*pvcPoints, *pvcTriangles, SurfaceProgress);

    CSurfaceBase *pSurfaceBase = new CTetraSurface(name, *new geo::CTriSurface(tempMinus), Model());

    CTetraSubHorizon *pHor = new CTetraSubHorizon(name, true, Model());

    pHor->LinkTo(*pSurfaceBase);

    geo::CSurfaceDesc& surfDesc = GetSurfaceDescriptor(*pSurfaceBase, mpSurface2SurfaceDesc, true, *pSurfaceBase);

    mpSurface2SurfaceDesc[pSurfaceBase] = &surfDesc;

    // TODO: move this stuff (and the meshing above) to its own place, as we may want to redo in the GUI on this or imported surface
    mpMinus.clear();
    mpPlus.clear();

    getTyingInformation(mpMinus, *pSurfaceBase, minusGocadSurface);
    getTyingInformation(mpPlus, *pSurfaceBase, *plusGocadSurface);

    duplicateNodes(mpMinus, pSurfaceBase->Surface());
    duplicateNodes(mpPlus, pSurfaceBase->Surface());

    createIFElements(mpMinus, mpPlus, mpSurface2SurfaceDesc, *pSurfaceBase);

    createTyings(mpMinus);
    createTyings(mpPlus);
#endif
      }
  }
  }
}

geo::CSurfaceDesc& CGocadMeshImporter::GetSurfaceDescriptor(const CSurfaceBase& surfacebase, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc, bool bSlip, COpenGLNode& surfacesource)
{
  geo::CSurfaceDesc* pSurfaceDesc;

  TSurface2SurfaceDescMap::iterator it = mpSurface2SurfaceDesc.find(&surfacebase);
  if(it != mpSurface2SurfaceDesc.end())
  {
  pSurfaceDesc = it->second;
  }
  else
  {
  pSurfaceDesc = new geo::CSurfaceDesc(TetMesh(), surfacebase.Surface(), std::string(), bSlip);
  mpSurface2SurfaceDesc.insert(TSurface2SurfaceDescMap::value_type(&surfacebase, pSurfaceDesc));
  m_tetramesh.m_vcSurfaceSource.push_back(&surfacesource);
  }

  return *pSurfaceDesc;
}

geo::CVector TriangleNormal(const CGocadData::CTriangle & triangle)
{
  const geo::CPoint p0 = Node2Point(triangle.Node(0));
  const geo::CPoint p1 = Node2Point(triangle.Node(1));
  const geo::CPoint p2 = Node2Point(triangle.Node(2));
  const geo::CVector dir1 = geo::CVector(p2 - p0);
  const geo::CVector dir2 = geo::CVector(p2 - p1);
  geo::CVector pointBasedNormal = dir1.CrossProduct(dir2);
  if (!pointBasedNormal.isNullVector())
  pointBasedNormal = pointBasedNormal.UnitVector();
  return pointBasedNormal;
}

#endif
