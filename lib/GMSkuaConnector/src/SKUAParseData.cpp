#include "SKUAParseData.h"

#include <cstring>

namespace gm_skua
{

int SKUAParseData::find_node(int node) const
{
  if (node >= last_range->start + last_range->offset)
    return node - last_range->offset;

  for (std::vector<VertexRange>::const_reverse_iterator it = vertex_range.rbegin(); it != vertex_range.rend(); --it)
    if (node >= it->start + it->offset)
    {
      last_range = it;
      return node - it->offset;
    }

  return -1;
}

bool SKUAParseData::check_orientation(const Triangle& triangle) const
{
  double points[3][3];

  for (int i = 0; i < 3; ++i)
  {
    const Vertex& vertex = vertices[triangle.node[i]].link >= 0 ? vertices[vertices[triangle.node[i]].link] : vertices[triangle.node[i]];

    for (int j = 0; j < 3; ++j)
      points[i][j] = vertex.coordinate[j];
  }

  double cross[3] = { points[0][1] * points[1][2] - points[1][1] * points[0][2], points[1][0] * points[0][2] - points[0][0] * points[1][2], points[0][0] * points[1][1] - points[1][0] * points[0][1] };

  return points[2][0] * cross[0] + points[2][1] * cross[1] + points[2][2] * cross[2] > 0;
}

bool SKUAParseData::check_orientation(const Tetra& tetra) const
{
  double points[4][3];

  for (int i = 0; i < 4; ++i)
  {
    const Vertex& vertex = vertices[tetra.node[i]].link >= 0 ? vertices[vertices[tetra.node[i]].link] : vertices[tetra.node[i]];

    for (int j = 0; j < 3; ++j)
      points[i][j] = vertex.coordinate[j];
  }

  double vector_a[3] = { points[3][0] - points[2][0], points[3][1] - points[2][1], points[3][2] - points[2][2] };
  double vector_b[3] = { points[1][0] - points[2][0], points[1][1] - points[2][1], points[1][2] - points[2][2] };

  double cross[3] = { vector_a[1] * vector_b[2] - vector_a[2] * vector_b[1], vector_a[2] * vector_b[0] - vector_a[0] * vector_b[2], vector_a[0] * vector_b[1] - vector_a[1] * vector_b[0] };

  double r = 0;

  for (int i = 0; i < 3; ++i)
    r += cross[i] * points[0][i] - cross[i] * points[2][i];

  return r > 0;
}

bool SKUAParseData::has_double_sided_faults() const
{
  return std::any_of(volume_types.begin(), volume_types.end(), [](int type) { return type == VolumeType::VolDSFVolume; });
}

void SKUAParseData::add_vertex_ranges()
{
  if (vertices.empty() || !vertex_range.empty())
    return;

  for (int i = 0; i < vertices.size(); ++i)
  {
    Vertex& vertex = vertices[i];

    if (vertex_range.empty() || vertex.id - vertex_range.back().offset > vertex_range.back().size)
    {
      vertex_range.emplace_back(VertexRange(i, 1, vertex.id - i));
      last_range = vertex_range.rbegin();
    }
    else
    {
      ++vertex_range.back().size;
    }

    if (vertex.link >= 0)
      vertex.link = find_node(vertex.link);
  }
}

const SKUAParseData::Vertex & SKUAParseData::get_vertex(size_t i) const
{
  return vertices[i].link < 0 ? vertices[i] : vertices[vertices[i].link];
}

namespace internal
{

template<class T>
bool compareVector(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
  if (lhs.size() != rhs.size())
    return false;

  if (lhs.size() > 0)
  {
    if (std::memcmp(lhs.data(), rhs.data(), lhs.size() * sizeof(T)) != 0)
      return false;
  }

  return true;
}

bool compareVectorString(const std::vector<SKUAParseData::String>& lhs, const std::vector<SKUAParseData::String>& rhs)
{
  if (lhs.size() != rhs.size())
    return false;

  for (size_t i = 0; i < lhs.size(); ++i)
    if (lhs[i] != rhs[i])
      return false;

  return true;
}

template<class T>
bool compareVectorVector(const std::vector<std::vector<T> >& lhs, const std::vector<std::vector<T> >& rhs)
{
  if (lhs.size() != rhs.size())
    return false;

  for (size_t i = 0; i < lhs.size(); ++i)
    if (!compareVector(lhs[i], rhs[i]))
      return false;

  return true;
}

bool comparePropertyData(const SKUAParseData::PropertyData& lhs, const SKUAParseData::PropertyData& rhs)
{
  if (lhs.size != rhs.size)
    return false;

  if (!compareVectorString(lhs.names, rhs.names))
    return false;

  if (!compareVector(lhs.esizes, rhs.esizes))
    return false;

  if (!compareVector(lhs.no_data_values, rhs.no_data_values))
    return false;

  if (!compareVector(lhs.units, rhs.units))
    return false;

  if (!compareVectorVector(lhs.values, rhs.values))
    return false;

  return true;
}

}

  
bool SKUAParseData::operator==(const SKUAParseData& rhs) const
{
  if (skua_type != rhs.skua_type)
    return false;

  if (name != rhs.name)
    return false;

  for (int i = 0; i < 3; ++i)
    if (coord_unit[i] != rhs.coord_unit[i])
      return false;

  if (volume != rhs.volume)
    return false;

  if (!internal::compareVector(vertex_range, rhs.vertex_range))
    return false;

  if (!internal::compareVector(vertices, rhs.vertices))
    return false;

  if (!internal::compareVector(tetras, rhs.tetras))
    return false;

  if (!internal::compareVectorVector(surfaces, rhs.surfaces))
    return false;

  if (!internal::compareVectorVector(surface_faces, rhs.surface_faces))
    return false;

  if (!internal::compareVector(surface_types, rhs.surface_types))
    return false;

  if (!internal::comparePropertyData(vertex_props, rhs.vertex_props))
    return false;

  if (!internal::comparePropertyData(tetra_props, rhs.tetra_props))
    return false;

  if (!internal::compareVectorString(tetra_region_names, rhs.tetra_region_names))
    return false;

  if (!internal::compareVectorString(model_region_names, rhs.model_region_names))
    return false;

  if (!internal::compareVectorString(surface_names, rhs.surface_names))
    return false;

  if (!internal::compareVectorString(face_names, rhs.face_names))
    return false;

  if (!internal::compareVectorVector(volumes, rhs.volumes))
    return false;

  if (!internal::compareVector(volume_types, rhs.volume_types))
    return false;

  if (!internal::compareVectorVector(surfaces_minus, rhs.surfaces_minus))
    return false;

  if (!internal::compareVectorVector(surfaces_plus, rhs.surfaces_plus))
    return false;

  return true;
}


namespace internal
{

template<class T>
void assignVector(std::vector<T>& lhs, const std::vector<T>& rhs)
{
  lhs.resize(rhs.size());
  std::copy(rhs.begin(), rhs.end(), lhs.begin());
}

template<class T>
void assignVectorVector(std::vector<std::vector<T> >& lhs, const std::vector<std::vector<T> >& rhs)
{
  lhs.resize(rhs.size());

  for (size_t i = 0; i < rhs.size(); ++i)
  {
    lhs[i].resize(rhs[i].size());
    std::copy(rhs[i].begin(), rhs[i].end(), lhs[i].begin());
  }
}

void assignPropertyData(SKUAParseData::PropertyData& lhs, const SKUAParseData::PropertyData& rhs)
{
  lhs.capacity = rhs.capacity;
  lhs.size = rhs.size;

  assignVector(lhs.names, rhs.names);
  assignVector(lhs.esizes, rhs.esizes);
  assignVector(lhs.no_data_values, rhs.no_data_values);
  assignVector(lhs.units, rhs.units);
  assignVectorVector(lhs.values, rhs.values);
}

}

SKUAParseData& SKUAParseData::operator=(const SKUAParseData& rhs)
{
  skua_type = rhs.skua_type;
  name = rhs.name;

  for (int i = 0; i < 3; ++i)
    coord_unit[i] = rhs.coord_unit[i];

  volume = rhs.volume;

  internal::assignVector(vertex_range, rhs.vertex_range);
  last_range = vertex_range.rbegin();

  internal::assignVector(vertices, rhs.vertices);
  internal::assignVector(tetras, rhs.tetras);
  internal::assignVectorVector(surfaces, rhs.surfaces);
  internal::assignVectorVector(surface_faces, rhs.surface_faces);
  internal::assignVector(surface_types, rhs.surface_types);

  internal::assignPropertyData(vertex_props, rhs.vertex_props);
  internal::assignPropertyData(tetra_props, rhs.tetra_props);

  internal::assignVector(tetra_region_names, rhs.tetra_region_names);
  internal::assignVector(model_region_names, rhs.model_region_names);
  internal::assignVector(surface_names, rhs.surface_names);
  internal::assignVector(face_names, rhs.face_names);

  internal::assignVectorVector(volumes, rhs.volumes);
  internal::assignVector(volume_types, rhs.volume_types);

  internal::assignVectorVector(surfaces_minus, rhs.surfaces_minus);
  internal::assignVectorVector(surfaces_plus, rhs.surfaces_plus);

  return *this;
}

}
