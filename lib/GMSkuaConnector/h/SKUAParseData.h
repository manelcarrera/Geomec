#pragma once

#include "GMSkuaConnectorExport.h"
#include "SKUADefs.h"

#include <vector>
#include <cstring>
#include <algorithm>

namespace gm_skua
{

struct INTERFACE_GMSKUACONNECTOR SKUAParseData
{
  struct Vertex
  {
    int id;
    int link;
    int elements;
    double coordinate[3];

    Vertex(int id = 0, int link = -1)
      : id(id)
      , link(link)
      , elements(0)
    {
    }
  };

  struct VertexRange
  {
    int start;
    int size;
    int offset;

    VertexRange()
      : start(0)
      , size(0)
      , offset(0)
    {
    }

    VertexRange(int start, int size, int offset)
      : start(start)
      , size(size)
      , offset(offset)
    {
    }
  };

  struct Tetra
  {
    int model_region;
    int tetra_region;
    int node[4];
    int opposite_signs[4];
    int opposite_types[4];
    int opposite_faces[4];

    Tetra(int model_region = 0)
      : model_region(model_region)
      , tetra_region(0)
    {
    }
  };

  struct BodyTriangle
  {
    int tetra;
    int opposite;

    BodyTriangle() {}
    BodyTriangle(int tetra, int opposite) : tetra(tetra), opposite(opposite) {}
  };

  struct Triangle
  {
    int node[3];
    int minus;
    int plus;

    bool collapsed() const
    {
      return node[0] == node[1] || node[0] == node[2] || node[1] == node[2];
    }

    Triangle() : minus(-1), plus(-1) {}
  };

  struct Face
  {
    int id;
    int type;
    int key_vertices[3];
    int triangle;
    int size;

    Face() : id(-1), type(0), triangle(-1), size(0)
    {
      key_vertices[0] = key_vertices[1] = key_vertices[2] = -1;
    }
  };

  struct String
  {
    int id;
    char data[MAX_IDENTIFIER_SIZE];

    String()
      : id(-1)
    {
      data[0] = 0;
    }

    String(const String& rhs)
      : id(rhs.id)
    {
      std::strncpy(data, rhs.data, MAX_IDENTIFIER_SIZE - 1);
      data[MAX_IDENTIFIER_SIZE - 1] = 0;
    }

    String(const String& rhs, size_t truncate_from_end)
      : id(rhs.id)
    {
      size_t data_length = std::strlen(rhs.data);
      size_t copy_length = data_length > truncate_from_end ? data_length - truncate_from_end : 0;
      copy_length = std::min(copy_length, static_cast<size_t>(MAX_IDENTIFIER_SIZE - 1));
      std::strncpy(data, rhs.data, copy_length);
      data[copy_length] = 0;
    }


    String(int id, const char *string)
      : id(id)
    {
      std::strncpy(data, string, MAX_IDENTIFIER_SIZE - 1);
      data[MAX_IDENTIFIER_SIZE - 1] = 0;
    }

    bool operator==(const String& rhs) const
    {
      return id == rhs.id && strncmp(data, rhs.data, MAX_IDENTIFIER_SIZE - 1) == 0;
    }

    bool operator!=(const String& rhs) const
    {
      return !operator==(rhs);
    }

    bool operator<(const String& rhs) const
    {
      return std::strncmp(data, rhs.data, MAX_IDENTIFIER_SIZE - 1) < 0;
    }

    bool ends_with(const char *needle) const
    {
      size_t haystack_len = std::strlen(data);
      size_t needle_len = std::strlen(needle);

      haystack_len = std::min(haystack_len, static_cast<size_t>(MAX_IDENTIFIER_SIZE - 1));

      if (needle_len > haystack_len)
        return false;

      return std::strncmp(data + haystack_len - needle_len, needle, needle_len) == 0;
    }
  };

  struct PropertyData
  {
    size_t capacity;
    size_t size;

    std::vector<String> names;
    std::vector<int> esizes;
    std::vector<double> no_data_values;
    std::vector<String> units;

    std::vector<std::vector<double> > values;

    PropertyData(size_t capacity = 24)
      : capacity(capacity)
      , size(0)
    {
      names.reserve(capacity);
      esizes.reserve(capacity);
      no_data_values.reserve(capacity);
      units.reserve(capacity);
    }
  };

  enum VolumeType : int
  {
    VolUnknown = 0,
    VolEmpty = 1,
    VolNormal = 2,
    VolDSFVolume = 3
  };

  enum SurfaceType : int
  {
    SurfUnknown = 0,
    SurfIntrusive = 1,
    SurfUnconformity = 2,
    SurfBoundary = 3,
    SurfTop = 4,
    SurfBottom = 5,
    SurfFault = 6,
    SurfDSFFault = 7
  };

  enum SKUAType : int
  {
    Unknown = 0,
    LightTSolid = 1,
    TSolid = 2,
    TSurf = 3,
    VSet = 4
  } skua_type;

  String name;

  double coord_unit[3];

  int volume;

  std::vector<VertexRange> vertex_range;
  std::vector<Vertex> vertices;
  std::vector<Tetra> tetras;
  std::vector<std::vector<Triangle> > surfaces;
  std::vector<std::vector<Face> > surface_faces;
  std::vector<int> surface_types;

  mutable std::vector<VertexRange>::const_reverse_iterator last_range;

  PropertyData vertex_props;
  PropertyData tetra_props;

  std::vector<String> tetra_region_names;
  std::vector<String> model_region_names;
  std::vector<String> surface_names;
  std::vector<String> face_names;

  std::vector<std::vector<int> > volumes;
  std::vector<int> volume_types;

  std::vector<std::vector<BodyTriangle> > surfaces_minus;
  std::vector<std::vector<BodyTriangle> > surfaces_plus;

  std::vector<std::pair<int, int> > dsf_pairs;

  SKUAParseData(SKUAType type, size_t vertex_capacity = 10000, size_t tetra_capacity = 10000)
    : skua_type(type)
    , volume(-1)
    , last_range(vertex_range.rbegin())
    , vertex_props(vertex_capacity)
    , tetra_props(tetra_capacity)
  {
    coord_unit[0] = coord_unit[1] = coord_unit[2] = 1;

    vertex_range.reserve(1); // we expect one range of consecutive vertex ids
    vertices.reserve(vertex_capacity);
    tetras.reserve(tetra_capacity);
    surfaces.reserve(64);
    surface_faces.reserve(64);
    surface_types.reserve(64);
    tetra_region_names.reserve(64);
    model_region_names.reserve(64);
    surface_names.reserve(64);
    face_names.reserve(64);
    volumes.reserve(64);
    volume_types.reserve(64);
  }

  int find_node(int node) const;

  bool check_orientation(const Triangle& triangle) const;
  bool check_orientation(const Tetra& tetra) const;

  bool has_double_sided_faults() const;

  void add_vertex_ranges();
  const Vertex& get_vertex(size_t i) const;

  bool operator==(const SKUAParseData& rhs) const;
  SKUAParseData& operator=(const SKUAParseData& rhs);

};




}

