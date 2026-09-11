#pragma once


#include <QString>
#include <fstream>
#include <set>

class CPointSet;
class CModelBase;
class IProgressBase;

namespace geo
{
class CPoint;
}


struct CGVTVtFileHeader
{
  int format_version; // 1
  int parameter_address; // 104
  int bits_per_sample; // 8/16/32
  int amplitude_levels; // 255/65535/0
  float minimum_clip_amplitude;
  float maximum_clip_amplitude;
  unsigned int trace_matrix_address; // 306
  int inline_crossline; // I/C
  int track_direction; // A/D
  int bin_direction; // A/D
  int revision_number; // 5
  int time_depth; //
  int history_header_address; // 0
  int data_type; // 0/1/2
  int data_subtype; // 0/1/2
  int use_NULL;  // 0/1
  float NULL_value;
  float datum_value;
  int datum_units;
  float correction_velocity;
  int domain;
  int new_time_depth;

  double TimeDepthConversion();
};

struct CGVTVtFileCheck
{
  int name[60];
  int track_direction;
  int num_tracks;
  int num_bins;
  int first_track;
  int first_bin;
  int delta_track;
  int delta_bin;
  int subpoint_code[6];
  int zero_time;
  int digi;
  int num_samples;
  int epirus_code;
  int epsg_code;
  int xform_towgs84;
  int pad;
  double geo_x;
  double geo_y;
  double delta_x;
  double delta_y;
  double skew_angle;

  bool RowsIsTracks();

  int NumRows();
  int FirstRow();
  int DeltaRow();

  int NumCols();
  int FirstCol();
  int DeltaCol();

  double CornerNorthing();
  double DeltaNorthing();
  
  double CornerEasting();
  double DeltaEasting();

  double Azimuth();

  int MinZ();
  int MaxZ();
};


class CGVTVtFile
{
  static const int MARKER = -54321;
public:
  static bool IsBinaryVtFile(const QString& fileName);

  CGVTVtFile(CModelBase& model);
  ~CGVTVtFile();

  bool Import(const QString& fileName, IProgressBase& prog, bool bNoData = false);
  bool Export(const QString& fileName);

  CGVTVtFileHeader& getHeaderInfo();
  CGVTVtFileCheck& getCheckInfo();

private:
  std::ifstream m_ifs;

  bool ReadArraySize(int section, unsigned int expect);
  bool ReadSection1();
  bool ReadSection3();
  bool ReadSection5();
  bool ReadSection7(IProgressBase& prog);
  bool ReadTrace(int address, geo::CPoint& point, double min_z, double delta_z, double conversion);

  size_t       m_uiRealFileSize;
  unsigned int m_uiFileSize;
  unsigned int m_uiArraySize;

  CModelBase& m_model;

  CGVTVtFileHeader m_header;
  CGVTVtFileCheck  m_check;
  CPointSet *m_pPointSet;

  int m_emptyTraces;
  std::set<int> m_incompleteTraces;
};