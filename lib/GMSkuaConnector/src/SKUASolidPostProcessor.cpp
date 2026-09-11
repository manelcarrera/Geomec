#include "SKUASolidPostProcessor.h"
#include "SKUAParseData.h"

#include <map>
#include <set>
#include <tbb/parallel_for_each.h>


namespace gm_skua
{

void SKUASolidPostProcessor::PostProcess()
{
  FixVolumeReferences();
  FixSurfaceReferences();
  FixCollectBodyTrianglesFromTetras();
  FixSurfaceBodyTriangles();
  FixSurfaceTypes();
  FixSurfaceTopBottom();
  FixVolumeTypes();
  //if (!m_data->has_double_sided_faults()) // We actually get shared vertices in models with DSF from SKUA
  FixSharedVertices();

  if (m_next)
  m_next->PostProcess();
}


namespace internal
{

// used in Post Processing to map SKUAParseData::BodyTriangle to SKUAParseData::Triangle
void bodytriangle2triangle_nolink(const std::vector<SKUAParseData::Vertex>& vertices, const std::vector<SKUAParseData::Tetra>& tetras, const SKUAParseData::BodyTriangle& bdytriangle, int(&node)[3])
{
  const SKUAParseData::Tetra& tetra = tetras[bdytriangle.tetra];

  int j = 0;
  for (int i = 0; i < 4; ++i)
  if (i != bdytriangle.opposite)
  {
      int n = tetra.node[i];
      node[j++] = n;
  }
}

void bodytriangle2triangle(const std::vector<SKUAParseData::Vertex>& vertices, const std::vector<SKUAParseData::Tetra>& tetras, const SKUAParseData::BodyTriangle& bdytriangle, int(&node)[3])
{
  const SKUAParseData::Tetra& tetra = tetras[bdytriangle.tetra];

  int j = 0;
  for (int i = 0; i < 4; ++i)
  if (i != bdytriangle.opposite)
  {
      int n = tetra.node[i];
      if (vertices[n].link >= 0) // SharedVrtx (or Atom)
    n = vertices[n].link;
      node[j++] = n;
  }
}


struct SortedTriangle
{
  int sorted_node[3];

  void sort()
  {
  std::sort(std::begin(sorted_node), std::end(sorted_node));
  }

  bool operator<(const SortedTriangle& rhs) const
  {
  if (sorted_node[0] != rhs.sorted_node[0])
      return sorted_node[0] < rhs.sorted_node[0];
  if (sorted_node[1] != rhs.sorted_node[1])
      return sorted_node[1] < rhs.sorted_node[1];
  return sorted_node[2] < rhs.sorted_node[2];
  }

  SortedTriangle(const SKUAParseData::Triangle& triangle)
  {
  memcpy(sorted_node, triangle.node, sizeof(sorted_node));
  sort();
  }
  SortedTriangle(const std::vector<SKUAParseData::Vertex>& vertices, const std::vector<SKUAParseData::Tetra>& tetras, const SKUAParseData::BodyTriangle& bdytriangle)
  {
  bodytriangle2triangle(vertices, tetras, bdytriangle, sorted_node);
  sort();
  }
};

}



// model_region_names in LightTSolid are discovered in CTETRA in arbitrary order
// and get as id the identifier index. Here we re-id them to the indices in their own vector,
// also adjusting the tetra info.
void SKUASolidPostProcessor::FixVolumeReferences()
{
  std::map<int, int> refs;

  int size = static_cast<int>(m_data->model_region_names.size());

  for (int i = 0; i < size; ++i)
  {
  refs[m_data->model_region_names[i].id] = i;
  m_data->model_region_names[i].id = i;
  }

  tbb::parallel_for_each(m_data->tetras.begin(), m_data->tetras.end(), [&](SKUAParseData::Tetra &tetra) { tetra.model_region = refs[tetra.model_region]; });

  m_data->volumes.resize(m_data->model_region_names.size());
  for (int i = 0; i < size; ++i)
  m_data->volumes[i].reserve(10000);

  tbb::parallel_for_each(m_data->model_region_names.begin(), m_data->model_region_names.end(),
  [&](SKUAParseData::String& region)
  {
  for (size_t i = 0; i < m_data->tetras.size(); ++i)
      if (m_data->tetras[i].model_region == region.id)
    m_data->volumes[region.id].emplace_back(static_cast<int>(i));
  });
}

// surface_names in LightTSolid are discovered in CTETRA in arbitrary order
// and get as id the identifier index. Here we re-id them to the indices in their own vector,
// also adjusting the tetra info.
void SKUASolidPostProcessor::FixSurfaceReferences()
{
  std::map<int, int> refs;

  refs[-1] = -1; // none maps to none
  int size = static_cast<int>(m_data->surface_names.size());

  for (int i = 0; i < size; ++i)
  {
  refs[m_data->surface_names[i].id] = i;
  m_data->surface_names[i].id = i;
  }

  tbb::parallel_for_each(m_data->tetras.begin(), m_data->tetras.end(), [&](SKUAParseData::Tetra &tetra) { for (int i = 0; i < 4; ++i) tetra.opposite_faces[i] = refs[tetra.opposite_faces[i]]; });
}

// Generate the mapping between surfaces and tetra-faces -- part 1
// Here we collect the info from the tetras and put them in surfaces_minus and surfaces_plus.
void SKUASolidPostProcessor::FixCollectBodyTrianglesFromTetras()
{
  const std::vector<SKUAParseData::Tetra>& tetras = m_data->tetras;

  int size = static_cast<int>(m_data->surface_names.size());

  m_data->surfaces_minus.resize(size);
  m_data->surfaces_plus.resize(size);

  for (int i = 0; i < size; ++i)
  {
  size_t reserve_size = m_data->surfaces[i].size();
  m_data->surfaces_minus[i].reserve(reserve_size);
  m_data->surfaces_plus[i].reserve(reserve_size);
  }

  int tetra_size = static_cast<int>(tetras.size());

  for (int i = 0; i < tetra_size; ++i)
  {
  const gm_skua::SKUAParseData::Tetra& tetra = m_data->tetras[i];

  for (int j = 0; j < 4; ++j)
  {
      if (tetra.opposite_faces[j] >= 0)
      {
    if (tetra.opposite_signs[j] < 0)
          m_data->surfaces_minus[tetra.opposite_faces[j]].emplace_back(SKUAParseData::BodyTriangle(i, j));
    else
          m_data->surfaces_plus[tetra.opposite_faces[j]].emplace_back(SKUAParseData::BodyTriangle(i, j));
      }
  }
  }
}

// Generate the mapping between surfaces and tetra-faces -- part 2
// Here we match surfaces_minus and surfaces_plus with the triangles in the surfaces.
void SKUASolidPostProcessor::FixSurfaceBodyTriangles()
{
  int size = static_cast<int>(m_data->surface_names.size());

  std::vector<std::map<internal::SortedTriangle, int> > sorted_triangles(size);

  // match collected body triangles to surfaces
  tbb::parallel_for_each(m_data->surface_names.begin(), m_data->surface_names.end(),
  [&](SKUAParseData::String& surface_name)
  {
  const std::vector<SKUAParseData::Vertex>& vertices = m_data->vertices;
  const std::vector<SKUAParseData::Tetra>& tetras = m_data->tetras;

  std::vector<SKUAParseData::Triangle>& triangles = m_data->surfaces[surface_name.id];
  std::vector<SKUAParseData::BodyTriangle>& minus = m_data->surfaces_minus[surface_name.id];
  std::vector<SKUAParseData::BodyTriangle>& plus = m_data->surfaces_plus[surface_name.id];
  std::map<internal::SortedTriangle, int>& sorted = sorted_triangles[surface_name.id];

  int triangles_size = static_cast<int>(triangles.size());
  for (int i = 0; i < triangles_size; ++i)
  {
      sorted.insert(std::make_pair(triangles[i], i));
  }

  if (minus.size() == 0 && plus.size() == 0) // if we didn't have any mapping info in the tetras (no CTETRA lines) then we create it
  {
      SKUAParseData::BodyTriangle tetra_triangle[4];
      for (int i = 0; i < tetras.size(); ++i)
      {
    for (int j = 0; j < 4; ++j)
    {
          SKUAParseData::BodyTriangle tetra_triangle(i, j);

          std::map<internal::SortedTriangle, int>::iterator it = sorted.find(internal::SortedTriangle(vertices, tetras, tetra_triangle));
          if (it != sorted.end())
          {
      if (triangles[it->second].minus < 0)
      {
              triangles[it->second].minus = (int)minus.size();
              minus.push_back(tetra_triangle);
      }
      else
      {
              triangles[it->second].plus = (int)plus.size();
              plus.push_back(tetra_triangle);
      }
          }
    }
      }
  }
  else
  {
      if (minus.size() > 0 && plus.size() > 0 && minus.size() != plus.size())
      {
    std::vector<SKUAParseData::BodyTriangle> tmp(minus.size() + plus.size());

    std::copy(minus.begin(), minus.end(), tmp.begin());
    std::copy(plus.begin(), plus.end(), tmp.begin() + minus.size());

    minus.clear();
    minus.reserve(tmp.size() >> 1);

    plus.clear();
    plus.reserve(tmp.size() >> 1);

    for (int i = 0; i < tmp.size(); ++i)
    {
          std::map<internal::SortedTriangle, int>::iterator it = sorted.find(internal::SortedTriangle(vertices, tetras, tmp[i]));
          if (it != sorted.end())
          {
      if (triangles[it->second].minus < 0)
      {
              triangles[it->second].minus = (int)minus.size();
              minus.push_back(tmp[i]);
      }
      else // if (triangles[it->second].plus < 0
      {
              triangles[it->second].plus = (int)plus.size();
              plus.push_back(tmp[i]);
      }
          }

    }
      }
      else
      {
    if (minus.size() > 0)
    {
          int size = std::min(triangles_size, (int)minus.size());
          for (int i = 0; i < size; ++i)
          {
      std::map<internal::SortedTriangle, int>::iterator it = sorted.find(internal::SortedTriangle(vertices, tetras, minus[i]));
      if (it != sorted.end())
      {
              triangles[it->second].minus = i;
      }
          }
    }
    if (plus.size() > 0)
    {
          int size = std::min(triangles_size, (int)plus.size());
          for (int i = 0; i < size; ++i)
          {
      std::map<internal::SortedTriangle, int>::iterator it = sorted.find(internal::SortedTriangle(vertices, tetras, plus[i]));
      if (it != sorted.end())
      {
              triangles[it->second].plus = i;
      }
          }
    }
      }
  }
  });
}

// We may have gotten all the surface types (from Earth)
// Or partial (boundary (faults?) from SKUA
// Or not at all.
// Primary concern is to detect faults, where we will follow Earth and the old rule that a fault has the same volume on either side of it
// We also detect real boundaries.
// TODO: Top and Bottom?
void SKUASolidPostProcessor::FixSurfaceTypes()
{
  tbb::parallel_for_each(m_data->surface_names.begin(), m_data->surface_names.end(),
  [&](SKUAParseData::String& surface_name)
  {
  const std::vector<SKUAParseData::Vertex>& vertices = m_data->vertices;
  const std::vector<SKUAParseData::Tetra>& tetras = m_data->tetras;

  std::vector<SKUAParseData::Face>& faces = m_data->surface_faces[surface_name.id];
  std::vector<SKUAParseData::Triangle>& triangles = m_data->surfaces[surface_name.id];
  std::vector<SKUAParseData::BodyTriangle>& minus = m_data->surfaces_minus[surface_name.id];
  std::vector<SKUAParseData::BodyTriangle>& plus = m_data->surfaces_plus[surface_name.id];

  int& type = m_data->surface_types[surface_name.id];

  int faces_size = static_cast<int>(faces.size());

  bool isDSF = surface_name.ends_with("_minus") || surface_name.ends_with("_plus");

  if (isDSF)
  {
      type = SKUAParseData::SurfaceType::SurfDSFFault;

      for (int i = 0; i < faces_size; ++i)
    faces[i].type = type;
  }
  else if (minus.size() == 0 || plus.size() == 0)
  {
      type = SKUAParseData::SurfaceType::SurfBoundary;

      for (int i = 0; i < faces_size; ++i)
    faces[i].type = type;
  }
  else
  {
      int triangles_size = static_cast<int>(triangles.size());

      SKUAParseData::Triangle helper_triangle_minus;
      SKUAParseData::Triangle helper_triangle_plus;

      int surf_type = -1;

      for (int i = 0; i < faces_size; ++i)
      {
    SKUAParseData::Face& face = faces[i];

    int size = face.triangle + face.size;

    for (int j = face.triangle; j < size; ++j)
    {
          SKUAParseData::Triangle& triangle = triangles[j];

          int type_minus = -1, type_plus = -1;
          int model_region_minus = -1, model_region_plus = -1;

          if (triangle.minus >= 0)
          {
      SKUAParseData::BodyTriangle& body_triangle_minus = minus[triangle.minus];
      const SKUAParseData::Tetra& tetra_minus = tetras[body_triangle_minus.tetra];
      type_minus = tetra_minus.opposite_types[body_triangle_minus.opposite];
      model_region_minus = tetra_minus.model_region;
          }

          if (triangle.plus >= 0)
          {
      SKUAParseData::BodyTriangle& body_triangle_plus = plus[triangle.plus];
      const SKUAParseData::Tetra& tetra_plus = tetras[body_triangle_plus.tetra];
      type_plus = tetra_plus.opposite_types[body_triangle_plus.opposite];
      model_region_plus = tetra_plus.model_region;
          }

          int single_type = std::max(type_minus, type_plus);

          if (single_type == -1)
      continue;

          switch (single_type)
          {
          default:
          case SKUAParseData::SurfaceType::SurfUnknown:
      break;
          case SKUAParseData::SurfaceType::SurfTop:
          case SKUAParseData::SurfaceType::SurfBottom:
          case SKUAParseData::SurfaceType::SurfIntrusive:
          case SKUAParseData::SurfaceType::SurfUnconformity:
          case SKUAParseData::SurfaceType::SurfBoundary:
          case SKUAParseData::SurfaceType::SurfFault:
          case SKUAParseData::SurfaceType::SurfDSFFault:
      face.type = single_type;
      break;
          }

          if (face.type < SKUAParseData::SurfaceType::SurfFault)
          {
      if (model_region_minus >= 0 && model_region_minus == model_region_plus)
              face.type = SKUAParseData::SurfaceType::SurfFault;
          }
    }
    if (face.type > surf_type)
          surf_type = face.type;
      }
      type = surf_type;
  }
  });
}

// Find top and bottom boundaries
void SKUASolidPostProcessor::FixSurfaceTopBottom()
{
  std::map<double, int> average_depths;

  const std::vector<SKUAParseData::Vertex>& vertices = m_data->vertices;

  std::for_each(m_data->surface_names.begin(), m_data->surface_names.end(),
  [&](SKUAParseData::String& surface_name)
  {
  const std::vector<SKUAParseData::Triangle>& triangles = m_data->surfaces[surface_name.id];
  int type = m_data->surface_types[surface_name.id];

  if (type == SKUAParseData::SurfaceType::SurfBoundary || type == SKUAParseData::SurfaceType::SurfTop)
  {
      double depth = 0;

      std::for_each(triangles.begin(), triangles.end(),
    [&](const SKUAParseData::Triangle& triangle)
      {
    for (int i = 0; i < 3; ++i)
    {
          const SKUAParseData::Vertex& vertex = vertices[triangle.node[i]].link < 0 ? vertices[triangle.node[i]] : vertices[vertices[triangle.node[i]].link];

          depth += vertex.coordinate[2];
    }
      });

      average_depths[depth / (3 * triangles.size())] = surface_name.id;
  }
  });

  if (average_depths.size() >= 2)
  {
  m_data->surface_types[average_depths.begin()->second] = SKUAParseData::SurfaceType::SurfTop;
  m_data->surface_types[average_depths.rbegin()->second] = SKUAParseData::SurfaceType::SurfBottom;
  }
}

// Set volume types: they are either normal volumes or DSF volumes, which are quasi volumes just to fill up space between DSF surfaces
void SKUASolidPostProcessor::FixVolumeTypes()
{
  const std::vector<SKUAParseData::Tetra>& tetras = m_data->tetras;
  const std::vector<SKUAParseData::String>& surface_names = m_data->surface_names;
  const std::vector<int>& surface_types = m_data->surface_types;
  const std::vector<std::vector<int> >& volumes = m_data->volumes;
  const std::vector<SKUAParseData::String>& volume_names = m_data->model_region_names;

  std::vector<int>& volume_types = m_data->volume_types;

  volume_types.resize(volumes.size(), SKUAParseData::VolumeType::VolNormal);

  typedef std::map<SKUAParseData::String, std::pair<int, int> > TDSFFaultPairMap;
  typedef std::set<std::pair<int, int> > TSDFFaultPairs;

  TDSFFaultPairMap dsf_fault_pair_map;

  for (int i = 0; i < surface_types.size(); ++i)
  {
  if (surface_types[i] == SKUAParseData::SurfaceType::SurfDSFFault)
  {
      if (surface_names[i].ends_with("_minus")) // remove last 6 characters
      {
    SKUAParseData::String fault_base(surface_names[i], 6);
    TDSFFaultPairMap::iterator result = dsf_fault_pair_map.insert(std::make_pair(fault_base, std::pair<int, int>(-1, -1))).first;
    result->second.first = i;
      }
      else // ends with "_plus" (remove last 5 characters)
      {
    SKUAParseData::String fault_base(surface_names[i], 5);
    TDSFFaultPairMap::iterator result = dsf_fault_pair_map.insert(std::make_pair(fault_base, std::pair<int, int>(-1, -1))).first;
    result->second.second = i;
      }
  }
  }

  TSDFFaultPairs dsf_fault_pairs;

  for (TDSFFaultPairMap::iterator it = dsf_fault_pair_map.begin(); it != dsf_fault_pair_map.end(); ++it)
  {
  if (it->second.first > it->second.second)
  {
      std::swap(it->second.first, it->second.second);
      if (it->second.first == -1)
    continue; // error
  }
  dsf_fault_pairs.insert(it->second);
  }

  tbb::parallel_for_each(volume_names.begin(), volume_names.end(),
  [&](const SKUAParseData::String& volume_name)
  {
  const std::vector<int>& volume = volumes[volume_name.id];

  if (volume.size() == 0)
      volume_types[volume_name.id] = SKUAParseData::VolumeType::VolEmpty;
  else if (!dsf_fault_pairs.empty())
  {
      int minus = -1;
      int plus = -1;

      if (std::all_of(volume.begin(), volume.end(),
    [&](int index)
      {
    const SKUAParseData::Tetra& tetra = tetras[index];

    for (int i = 0; i < 4; ++i)
    {
          int surf = tetra.opposite_faces[i];
          if (surf < 0 || surf == minus)
      continue;

          if (minus < 0)
          {
      minus = surf;
      continue;
          }

          if (surf == plus)
      continue;

          if (plus >= 0)
      return false;

          plus = surf;

          if (minus > plus)
      std::swap(minus, plus);

          if (dsf_fault_pairs.find(std::pair<int, int>(minus, plus)) == dsf_fault_pairs.end())
      return false;
    }

    return true;
      }))
    volume_types[volume_name.id] = SKUAParseData::VolumeType::VolDSFVolume;
  }
  });
}

// Remove shared vertices on non-faults. Also adjust nodal property values.
// Some minor issues remain, such as shared vertices on intersection lines with horizons going to edge of fault (fault edge should be non-split)
void SKUASolidPostProcessor::FixSharedVertices()
{
  std::vector<SKUAParseData::Vertex>& vertices = m_data->vertices;
  std::vector<SKUAParseData::Tetra>& tetras = m_data->tetras;
  std::vector<bool> use_link(vertices.size(), false);
  std::vector<int> dropped(vertices.size() + 1, 0);
  std::vector<int> re_index(vertices.size());
  std::vector<SKUAParseData::Vertex> new_vertices;
  new_vertices.reserve(vertices.size());

  int node[3];

  for (int i = 0; i < re_index.size(); ++i)
  re_index[i] = i;

  tbb::parallel_for_each(m_data->surface_names.begin(), m_data->surface_names.end(),
  [&](SKUAParseData::String& surface_name)
  {
  if (m_data->surface_types[surface_name.id] == SKUAParseData::SurfaceType::SurfFault)
  {
      std::vector<SKUAParseData::Triangle>& triangles = m_data->surfaces[surface_name.id];
      std::vector<SKUAParseData::BodyTriangle>& triangles_minus = m_data->surfaces_minus[surface_name.id];
      std::vector<SKUAParseData::BodyTriangle>& triangles_plus = m_data->surfaces_plus[surface_name.id];

      int triangle_size = static_cast<int>(triangles.size());

      for (int i = 0; i < triangle_size; ++i)
      {
    SKUAParseData::Triangle& triangle = triangles[i];

    if (triangle.minus >= 0)
    {
          SKUAParseData::BodyTriangle& minus = triangles_minus[triangle.minus];

          internal::bodytriangle2triangle_nolink(vertices, tetras, minus, node);

          for (int i = 0; i < 3; ++i)
      if (vertices[node[i]].link >= 0)
              use_link[node[i]] = true;
    }

    if (triangle.plus >= 0)
    {
          SKUAParseData::BodyTriangle& plus = triangles_plus[triangle.plus];

          internal::bodytriangle2triangle_nolink(vertices, tetras, plus, node);

          for (int i = 0; i < 3; ++i)
      if (vertices[node[i]].link >= 0)
              use_link[node[i]] = true;
    }

      }
  }
  });

  for (int i = 0; i < re_index.size(); ++i)
  {
  dropped[i + 1] = dropped[i];

  if (vertices[i].link < 0)
  {
      new_vertices.push_back(vertices[i]);
      re_index[i] = i - dropped[i];
  }
  else if (use_link[i])
  {
      new_vertices.push_back(vertices[i]);
      new_vertices.back().link -= dropped[vertices[i].link];
      re_index[i] = i - dropped[i];
  }
  else
  {
      ++dropped[i + 1];
      re_index[i] = vertices[i].link - dropped[vertices[i].link];
  }
  }

  tbb::parallel_for_each(tetras.begin(), tetras.end(),
  [&](SKUAParseData::Tetra& tetra)
  {
  for (int i = 0; i < 4; ++i)
      tetra.node[i] = re_index[tetra.node[i]];
  });

  std::vector<std::vector<double> >& values = m_data->vertex_props.values;

  tbb::parallel_for_each(values.begin(), values.end(),
  [&](std::vector<double>& old_prop)
  {
  std::vector<double> new_prop;
  new_prop.reserve(old_prop.size());

  for (size_t i = 0; i < new_prop.size(); ++i)
  {
      if (dropped[i] == dropped[i + 1])
    new_prop.emplace_back(new_prop[i]);
  }

  old_prop.swap(new_prop);
  });

  m_data->vertices.swap(new_vertices);
}

}
