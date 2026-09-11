#include "GVTVtFile.h"

#include "GlobalMessage.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "NodalValueSet.h"
#include "ValueTypeFactory.h"
#include "BaseEntryTypes.h"

#include <cassert>


// defined as 4-byte int, but just because ints are 32 bits now, doesn't mean they stay that way forever
#define VT_WORD_SIZE  4


////////////////////////////////////////////////////////////////////////////////
// Big endian fest
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <intrin.h>
#endif
#include <inttypes.h>


char readChar(std::ifstream& stream)
{
  char data;
  stream.read(&data, 1);
  return data;
}

short readShort(std::ifstream& stream)
{
  uint16_t data;
  stream.read((char *)&data, 2);
#ifdef _WIN32
  return _byteswap_ushort(data);
#else
  return (data >> 8) | ((data & 0xff) << 8);
#endif
}

int readInt(std::ifstream& stream)
{
  uint32_t data;
  stream.read((char *)&data, 4);
#ifdef _WIN32
  return _byteswap_ulong(data);
#else
  return __builtin_bswap32(data);
#endif
}

float readFloat(std::ifstream& stream)
{
  union {
  uint32_t input;
  float output;
  } data;
  stream.read((char *)&data, 4);
#ifdef _WIN32
  data.input = _byteswap_ulong(data.input);
#else
  data.input = __builtin_bswap32(data.input);
#endif
  return data.output;
}

double readDouble(std::ifstream& stream)
{
  union {
  uint64_t input;
  double output;
  } data;
  stream.read((char *)&data.input, 8);
#ifdef _WIN32
  data.input = _byteswap_uint64(data.input);
#else
  data.input = __builtin_bswap64(data.input);
#endif
  return data.output;
}




////////////////////////////////////////////////////////////////////////////////
// CGVTVtFileHeader
////////////////////////////////////////////////////////////////////////////////

double CGVTVtFileHeader::TimeDepthConversion()
{
  if (revision_number >= 6)
  {
  switch (new_time_depth)
  {
  case 1:
      return 0.001;
  case 2:
      return 0.1;
  case 3:
      return 0.01;
  case 4:
      return 0.000001;
  }
  }
  switch (time_depth)
  {
  case 0:
  return 0.001;
  case 1:
  return 0.0001;
  case 2:
  return 0.00001;
  case 3:
  return 0.000001;
  }

  return -1;
}



////////////////////////////////////////////////////////////////////////////////
// CGVTVtFileCheck
////////////////////////////////////////////////////////////////////////////////

bool CGVTVtFileCheck::RowsIsTracks()
{
  return track_direction == 'H';
}

int CGVTVtFileCheck::NumRows()
{
  return RowsIsTracks() ? num_tracks : num_bins;
}

int CGVTVtFileCheck::FirstRow()
{
  return RowsIsTracks() ? first_track : first_bin;
}

int CGVTVtFileCheck::DeltaRow()
{
  return RowsIsTracks() ? delta_track : delta_bin;
}

int CGVTVtFileCheck::NumCols()
{
  return RowsIsTracks() ? num_bins : num_tracks;
}

int CGVTVtFileCheck::FirstCol()
{
  return RowsIsTracks() ? first_bin : first_track;
}

int CGVTVtFileCheck::DeltaCol()
{
  return RowsIsTracks() ? delta_bin : delta_track;
}

double CGVTVtFileCheck::CornerNorthing()
{
  return geo_y;
}

double CGVTVtFileCheck::DeltaNorthing()
{
  return delta_y;
}

double CGVTVtFileCheck::CornerEasting()
{
  return geo_x;
}

double CGVTVtFileCheck::DeltaEasting()
{
  return delta_x;
}

double CGVTVtFileCheck::Azimuth()
{
  return skew_angle; // degrees?
}

int CGVTVtFileCheck::MinZ()
{
  return zero_time; // unit?
}

int CGVTVtFileCheck::MaxZ()
{
  return zero_time + num_samples * digi; // unit?
}



////////////////////////////////////////////////////////////////////////////////
// CGVTVtFile
////////////////////////////////////////////////////////////////////////////////

bool CGVTVtFile::IsBinaryVtFile(const QString& fileName)
{
  bool retval = false;
  std::ifstream is(fileName.toStdString().c_str(), std::ios::in | std::ios::binary);

  if (is.is_open())
  {
  int t;
  t = readInt(is);
  t = readInt(is);

  if (t == MARKER)
      retval = true;

  is.close();
  }

  return retval;
}




CGVTVtFile::CGVTVtFile(CModelBase& model)
  : m_model(model)
  , m_pPointSet(0)
  , m_emptyTraces(0)
{
}

CGVTVtFile::~CGVTVtFile()
{
  delete m_pPointSet;
}

CGVTVtFileHeader& CGVTVtFile::getHeaderInfo()
{
  return m_header;
}

CGVTVtFileCheck& CGVTVtFile::getCheckInfo()
{
  return m_check;
}

bool CGVTVtFile::Import(const QString& fileName, IProgressBase& prog, bool bNoData)
{
  m_ifs.open(fileName.toStdString().c_str(), std::ios::in | std::ios::binary);

  if (m_ifs.is_open())
  {
  m_ifs.seekg(0, std::ios::end);
  m_uiRealFileSize = m_ifs.tellg();
  m_ifs.seekg(0, std::ios::beg);

  if (!ReadSection1())
      return false;

  if (!ReadArraySize(2, 100))
      return false;

  if (!ReadSection3())
      return false;

  if (!ReadArraySize(4, 200))
      return false;

  if (!ReadSection5())
      return false;

  if (bNoData)
  {
      m_ifs.close();
      return true; // we're done
  }

  if (!ReadArraySize(6, m_check.num_bins * m_check.num_tracks))
      return false;

  QString pointsetName = fileName;
  int pos0 = fileName.lastIndexOf('/');
  int pos1 = fileName.lastIndexOf('\\');
  int pos = pos0 < pos1 ? pos1 : pos0;

  if (pos > 0)
      pointsetName = fileName.right(fileName.length() - pos - 1);

  m_pPointSet = new CPointSet(pointsetName.toStdString().c_str(), m_model, 1, IPointSet::DIM_3D);
  m_pPointSet->pointSetType(IPointSet::TIME_DEPTH);
  const CValueTypeFactory *factory = CValueTypeFactory::instance();
  m_pPointSet->NodalValueSet(3).LinkTo(factory->BuildValueType(*m_pPointSet, IDT_VALUETYPE_DELTAT, "Timeshifts")->Component());

  if (!ReadSection7(prog))
      return false;

  TPointSetEntry& psentry = (TPointSetEntry&)(*m_model.GraphEntry(MD_BASE_POINTSET));
  psentry.ConnectItem(*m_pPointSet);

  m_pPointSet = 0;

  m_ifs.close(); // we expect to be deconstructed soon, but it can't hurt
  }

  if (m_emptyTraces || !m_incompleteTraces.empty())
  {
  QString msg = "Warning: ";
  QString del = "found ";
  if (m_emptyTraces)
  {
      msg += del + QString("%1 empty trace(s) (size 0)").arg(m_emptyTraces);
      del = ", and ";
  }
  if (!m_incompleteTraces.empty())
  {
      msg += del + QString("%1 incomplete trace(s) (extending past the end of the file) at address").arg(m_incompleteTraces.size());
      if (m_incompleteTraces.size() > 1)
    msg += "es";
      msg += ": ";

      int i = 1;
      std::set<int>::iterator it = m_incompleteTraces.begin();
      msg += QString::number(*it++);
      for (; it != m_incompleteTraces.end() && i < 20; ++it, ++i)
    msg += ", " + QString::number(*it);
      if (m_incompleteTraces.size() > i)
    msg += ", ...";
  }
  _m()->msg(msg);
  }

  return true;
}


bool CGVTVtFile::Export(const QString& fileName)
{
  _m()->msg("Export of binary .vt files is not supported yet.");
  return false;
}


bool CGVTVtFile::ReadArraySize(int section, unsigned int expect)
{
  int marker = readInt(m_ifs);

  if (marker != MARKER)
  {
  _m()->msg(QString("Error while reading section %1: marker not found").arg(section));
  return false;
  }

  m_uiArraySize = readInt(m_ifs);

  if (m_uiArraySize != expect)
  {
  _m()->msg(QString("Error while reading section %1: expected size %2, found %3").arg(section).arg(expect).arg(m_uiArraySize));
  return false;
  }

  return true;
}


bool CGVTVtFile::ReadSection1()
{
  m_uiFileSize = readInt(m_ifs);

  if (m_uiFileSize == 0xffffffff)
  {
  _m()->msg(QString("Error while reading section 1: files > 16Gb not supported yet"));
  return false;
  }

  if (m_uiFileSize * VT_WORD_SIZE != m_uiRealFileSize)
  {
  _m()->msg(QString("Warning while reading section 1: real file size is %1, but in file it says %2 (* 4 = %3)").arg(m_uiRealFileSize).arg(m_uiFileSize).arg(m_uiFileSize * VT_WORD_SIZE));
  }

  return true;
}


bool CGVTVtFile::ReadSection3()
{
  m_header.format_version = readInt(m_ifs); // 1
  m_header.parameter_address = readInt(m_ifs); // 104
  m_header.bits_per_sample = readInt(m_ifs); // 8/16/32

  if (m_header.bits_per_sample != 8 && m_header.bits_per_sample != 16 && m_header.bits_per_sample != 32)
  {
  _m()->msg(QString("Error while reading section 3: bits per sample is %1, but only 8, 16, and 32 are supported").arg(m_header.bits_per_sample));
  return false;
  }

  m_header.amplitude_levels = readInt(m_ifs); // 255/65535/0
  m_header.minimum_clip_amplitude = readInt(m_ifs);
  m_header.maximum_clip_amplitude = readInt(m_ifs);
  m_header.trace_matrix_address = readInt(m_ifs); // 306
  m_header.inline_crossline = readInt(m_ifs); // I/C
  m_header.track_direction = readInt(m_ifs); // A/D
  m_header.bin_direction = readInt(m_ifs); // A/D
  m_header.revision_number = readInt(m_ifs); // 5
  m_header.time_depth = readInt(m_ifs); //
  m_header.history_header_address = readInt(m_ifs); // 0
  m_header.data_type = readInt(m_ifs); // 0/1/2
  m_header.data_subtype = readInt(m_ifs); // 0/1/2
  m_header.use_NULL = readInt(m_ifs);  // 0/1
  m_header.NULL_value = readFloat(m_ifs);
  m_header.datum_value = readFloat(m_ifs);
  m_header.datum_units = readInt(m_ifs);
  m_header.correction_velocity = readFloat(m_ifs);
  m_header.domain = readInt(m_ifs);
  m_header.new_time_depth = readInt(m_ifs);

  if (m_header.TimeDepthConversion() < 0)
  {
  QString msg = "Error while reading section 3: time/depth unit";
  if (m_header.revision_number >= 6)
      msg += QString("s %1 and %2 (new) are").arg(m_header.time_depth).arg(m_header.new_time_depth);
  else
      msg += QString(" %1 is").arg(m_header.time_depth);
  msg += " not supported";
  _m()->msg(msg);
  return false;
  }

  for (unsigned int i = sizeof(CGVTVtFileHeader) / VT_WORD_SIZE; i < m_uiArraySize; ++i)
  readInt(m_ifs);

  return true;
}


bool CGVTVtFile::ReadSection5()
{
  for (unsigned int i = 0; i < 60; ++i)
  m_check.name[i] = readInt(m_ifs);

  m_check.track_direction = readInt(m_ifs);
  m_check.num_tracks = readInt(m_ifs);
  m_check.num_bins = readInt(m_ifs);
  m_check.first_track = readInt(m_ifs);
  m_check.first_bin = readInt(m_ifs);
  m_check.delta_track = readInt(m_ifs);
  m_check.delta_bin = readInt(m_ifs);

  for (int i = 0; i < 6; ++i)
  m_check.subpoint_code[i] = readInt(m_ifs);

  m_check.zero_time = readInt(m_ifs);
  m_check.digi = readInt(m_ifs);
  m_check.num_samples = readInt(m_ifs);
  m_check.epirus_code = readInt(m_ifs);
  m_check.epsg_code = readInt(m_ifs);
  m_check.xform_towgs84 = readInt(m_ifs);
  m_check.pad = readInt(m_ifs);
  m_check.geo_x = readDouble(m_ifs);
  m_check.geo_y = readDouble(m_ifs);
  m_check.delta_x = readDouble(m_ifs);
  m_check.delta_y = readDouble(m_ifs);
  m_check.skew_angle = readDouble(m_ifs);

  for (unsigned int i = 90; i < m_uiArraySize; ++i)
  readInt(m_ifs);

  return true;
}



bool CGVTVtFile::ReadSection7(IProgressBase& prog)
{

  size_t valid_address_min = m_ifs.tellg() / VT_WORD_SIZE;
  size_t valid_address_max = m_uiFileSize - 1; // this would only be valid if that position contains 0 (empty trace)

  int rows = m_check.NumRows();
  int corner_row = m_check.FirstRow();
  int delta_row = m_check.DeltaRow();

  int cols = m_check.NumCols();
  int corner_col = m_check.FirstCol();
  int delta_col = m_check.DeltaCol();

  double corner_easting = m_check.CornerEasting();
  double delta_easting = m_check.DeltaEasting();

  double corner_northing = m_check.CornerNorthing();
  double delta_northing = m_check.DeltaNorthing();

  double min_z = m_check.MinZ();
  double delta_z = m_check.digi;

  double azimuth = m_check.Azimuth();

  double conversion = m_header.TimeDepthConversion();

  double y, x;


  geo::CPoint point;

  prog.AddSteps(rows * cols);

  for (int row = 0; row < rows; ++row)
  {
  y = (corner_row + row * delta_row) * delta_easting;

  for (int col = 0; col < cols; ++col)
  {

      int address = readInt(m_ifs);
      
      if (address == 0)
    continue;

      if (address < valid_address_min || address > valid_address_max)
      {
    // msg
    return false;
      }
      
      x = (corner_col + col * delta_col) * delta_northing;

      point.Set(x, y, 0);
      point.Rotate(geo::CVector::Zaxis, azimuth);

      point.Set(point.X() + corner_northing, point.Y() + corner_easting, 0);

      if (!ReadTrace(address, point, min_z, delta_z, conversion))
      {
    // error msg
    return false;
      }

      prog.Step();
  }
  }

  return true;
}

bool CGVTVtFile::ReadTrace(int address, geo::CPoint& point, double min_z, double delta_z, double conversion) // address is in 4 byte int offsets (from beginning of file)
{
  size_t currentPos = m_ifs.tellg();

  m_ifs.seekg((address - 1) * VT_WORD_SIZE, std::ios::beg);

  int marker = readInt(m_ifs);

  if (marker != MARKER)
  {
  _m()->msg(QString("Error while reading section 8: marker not found at address %1 (-1)").arg(address));
  return false;
  }

  int size = readInt(m_ifs);

  if (size <= 0) // This shouldn't happen, but I know these people!
  {
  ++m_emptyTraces;
  }
  else
  {

  std::vector<double> data(4, 0);

  data[0] = point.X();
  data[1] = point.Y();

  size_t maxPos = m_uiFileSize * VT_WORD_SIZE - m_header.bits_per_sample / 8; // apparently we have 'empty' traces that aren't completely written

  switch (m_header.bits_per_sample)
  {
  case 8:
      for (int i = 0; i < size; ++i)
      {
    if (m_ifs.tellg() <= maxPos)
    {
          data[2] = min_z + i * delta_z;
          data[3] = readChar(m_ifs) * conversion;

          m_pPointSet->PushBack(data);
    }
    else
          m_incompleteTraces.insert(address);
      }
      break;

  case 16:
      for (int i = 0; i < size && m_ifs.tellg() <= maxPos; ++i)
      {
    if (m_ifs.tellg() <= maxPos)
    {
          data[2] = min_z + i * delta_z;
          data[3] = readShort(m_ifs) * conversion;

          m_pPointSet->PushBack(data);
    }
    else
          m_incompleteTraces.insert(address);
      }
      break;

  case 32:
      for (int i = 0; i < size && m_ifs.tellg() <= maxPos; ++i)
      {
    if (m_ifs.tellg() <= maxPos)
    {
          data[2] = min_z + i * delta_z;
          data[3] = readFloat(m_ifs) * conversion;

          m_pPointSet->PushBack(data);
    }
    else
          m_incompleteTraces.insert(address);
      }
      break;

  default:
      assert(false);
      return false;
  }

  }

  m_ifs.seekg(currentPos, std::ios::beg);

  return true;
}

