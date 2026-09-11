#include "SKUAParserDataIO.h"

#include "SKUAParseData.h"
#include "StreamVersion.h"


namespace gm_skua
{

namespace internal
{

void LoadStreamString(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress, gm_skua::SKUAParseData::String& string_data)
{
  stream >> string_data.id;

  char *data = string_data.data;
  char c;
  int i = 0;

  do {
  stream >> c;
  data[i++] = c;
  } while (c && i < MAX_IDENTIFIER_SIZE);
}

void SaveStreamString(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress, const gm_skua::SKUAParseData::String& string_data)
{
  stream << string_data.id;

  const char *data = string_data.data;
  char c;
  int i = 0;

  do {
  c = data[i++];
  stream << c;
  } while (c && i < MAX_IDENTIFIER_SIZE);
}

void LoadStreamStringVector(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress, std::vector<gm_skua::SKUAParseData::String>& string_vector_data)
{
  int size;
  stream >> size;

  if (size > 0)
  {
  string_vector_data.resize(size);

  for (int i = 0; i < size; ++i)
  {
      LoadStreamString(stream, version, progress, string_vector_data[i]);
      
      progress.Step();
  }
  }
}

void SaveStreamStringVector(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress, const std::vector<gm_skua::SKUAParseData::String>& string_vector_data)
{
  int size = (int)string_vector_data.size();
  stream << size;

  for (int i = 0; i < size; ++i)
  {
  SaveStreamString(stream, progress, string_vector_data[i]);

  progress.Step();
  }
}

template<class T>
void LoadStreamVector(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress, std::vector<T>& vector_data)
{
  int size;
  stream >> size;

  if (size > 0)
  {
  vector_data.resize(size);

  char *data = (char *)vector_data.data();

  for (int i = 0; i < size; ++i)
  {
      for (int j = 0; j < sizeof(T); ++j)
    stream >> *data++;

      progress.Step();
  }
  }
}

template<class T>
void SaveStreamVector(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress, const std::vector<T>& vector_data)
{
  int size = (int)vector_data.size();
  stream << size;

  if (size > 0)
  {
  char *data = (char *)vector_data.data();

  for (int i = 0; i < size; ++i)
  {
      for (int j = 0; j < sizeof(T); ++j)
    stream << *data++;

      progress.Step();
  }
  }
}

template<class T>
void LoadStreamVectorVector(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress, std::vector<T>& vector_vector_data)
{
  int size;
  stream >> size;

  if (size > 0)
  {
  vector_vector_data.resize(size);

  for (int i = 0; i < size; ++i)
  {
      LoadStreamVector(stream, version, progress, vector_vector_data[i]);
  }
  }
}

template<class T>
void SaveStreamVectorVector(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress, const std::vector<T>& vector_vector_data)
{
  int size = (int)vector_vector_data.size();
  stream << size;

  for (int i = 0; i < size; ++i)
  {
  SaveStreamVector(stream, progress, vector_vector_data[i]);
  }
}

void LoadStreamPropertyData(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress, gm_skua::SKUAParseData::PropertyData& property_data)
{
  int capacity;
  stream >> capacity;

  property_data.capacity = capacity;

  int size;
  stream >> size;

  property_data.size = size;

  LoadStreamStringVector(stream, version, progress, property_data.names);
  LoadStreamVector(stream, version, progress, property_data.esizes);
  LoadStreamVector(stream, version, progress, property_data.no_data_values);
  LoadStreamStringVector(stream, version, progress, property_data.units);
  LoadStreamVectorVector(stream, version, progress, property_data.values);
}

void SaveStreamPropertyData(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress, const gm_skua::SKUAParseData::PropertyData& property_data)
{
  int capacity = (int)property_data.capacity;
  stream << capacity;

  int size = (int)property_data.size;
  stream << size;

  SaveStreamStringVector(stream, progress, property_data.names);
  SaveStreamVector(stream, progress, property_data.esizes);
  SaveStreamVector(stream, progress, property_data.no_data_values);
  SaveStreamStringVector(stream, progress, property_data.units);
  SaveStreamVectorVector(stream, progress, property_data.values);
}

}


void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress, struct gm_skua::SKUAParseData& data)
{
  int skua_type;
  stream >> skua_type;

  data.skua_type = (gm_skua::SKUAParseData::SKUAType)skua_type;

  internal::LoadStreamString(stream, version, progress, data.name);

  for (int i = 0; i < 3; ++i)
  stream >> data.coord_unit[i];

  stream >> data.volume;

  internal::LoadStreamVector(stream, version, progress, data.vertex_range);
  internal::LoadStreamVector(stream, version, progress, data.vertices);
  internal::LoadStreamVector(stream, version, progress, data.tetras);
  internal::LoadStreamVectorVector(stream, version, progress, data.surfaces);
  internal::LoadStreamVectorVector(stream, version, progress, data.surface_faces);
  internal::LoadStreamVector(stream, version, progress, data.surface_types);

  data.last_range = data.vertex_range.rbegin();

  internal::LoadStreamPropertyData(stream, version, progress, data.vertex_props);
  internal::LoadStreamPropertyData(stream, version, progress, data.tetra_props);

  internal::LoadStreamStringVector(stream, version, progress, data.tetra_region_names);
  internal::LoadStreamStringVector(stream, version, progress, data.model_region_names);
  internal::LoadStreamStringVector(stream, version, progress, data.surface_names);
  internal::LoadStreamStringVector(stream, version, progress, data.face_names);

  internal::LoadStreamVectorVector(stream, version, progress, data.volumes);
  internal::LoadStreamVector(stream, version, progress, data.volume_types);

  internal::LoadStreamVectorVector(stream, version, progress, data.surfaces_minus);
  internal::LoadStreamVectorVector(stream, version, progress, data.surfaces_plus);
}

void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress, const struct gm_skua::SKUAParseData& data)
{
  stream << (int)data.skua_type;

  internal::SaveStreamString(stream, progress, data.name);

  for (int i = 0; i < 3; ++i)
  stream << data.coord_unit[i];

  stream << data.volume;

  internal::SaveStreamVector(stream, progress, data.vertex_range);
  internal::SaveStreamVector(stream, progress, data.vertices);
  internal::SaveStreamVector(stream, progress, data.tetras);
  internal::SaveStreamVectorVector(stream, progress, data.surfaces);
  internal::SaveStreamVectorVector(stream, progress, data.surface_faces);
  internal::SaveStreamVector(stream, progress, data.surface_types);

  data.last_range = data.vertex_range.rbegin();

  internal::SaveStreamPropertyData(stream, progress, data.vertex_props);
  internal::SaveStreamPropertyData(stream, progress, data.tetra_props);

  internal::SaveStreamStringVector(stream, progress, data.tetra_region_names);
  internal::SaveStreamStringVector(stream, progress, data.model_region_names);
  internal::SaveStreamStringVector(stream, progress, data.surface_names);
  internal::SaveStreamStringVector(stream, progress, data.face_names);

  internal::SaveStreamVectorVector(stream, progress, data.volumes);
  internal::SaveStreamVector(stream, progress, data.volume_types);

  internal::SaveStreamVectorVector(stream, progress, data.surfaces_minus);
  internal::SaveStreamVectorVector(stream, progress, data.surfaces_plus);
}


}

