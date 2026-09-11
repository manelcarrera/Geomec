#include "GVTSeismicGridDefinition.h"
#include "GVT123DIImport.h"
#include "GlobalMessage.h"
#include "StreamVersion.h"
#include "GVTVtFile.h"
#include "ModelBase.h"
#include "Global.h"
#include "IProgressFactory.h"
#include <memory>

CGVTSeismicGridDefinition::CGVTSeismicGridDefinition()
  : m_KeyFile("")
  , m_Name("")
  , m_Depth(0)
{
}

CGVTSeismicGridDefinition::CGVTSeismicGridDefinition(const QString &name)
  : m_KeyFile("")
  , m_Name(name)
  , m_Depth(0)
{
}

CGVTSeismicGridDefinition::CGVTSeismicGridDefinition(const CGVTSeismicGridDefinition &rhs)
  : m_KeyFile(rhs.m_KeyFile)
  , m_Name(rhs.m_Name)
  , m_Depth(rhs.m_Depth)
  , m_Grid(rhs.m_Grid)
{
}


const QString &CGVTSeismicGridDefinition::KeyFile() const
{ 
  return m_KeyFile;
}

void CGVTSeismicGridDefinition::KeyFile(const QString &path)
{ 
  m_KeyFile = path;
}


const QString &CGVTSeismicGridDefinition::Name() const
{ 
  return m_Name;
}

void CGVTSeismicGridDefinition::Name(const QString &name)
{ 
  m_Name = name;
}


double CGVTSeismicGridDefinition::Depth() const
{
  return m_Depth;
}

void CGVTSeismicGridDefinition::Depth(double depth)
{ 
  m_Depth = depth;
}


double CGVTSeismicGridDefinition::TrackSpacing() const
{ 
  return m_Grid.TrackSpacing();
}

void CGVTSeismicGridDefinition::TrackSpacing(double trackspacing)
{ 
  m_Grid.TrackSpacing(trackspacing);
}


double CGVTSeismicGridDefinition::BinSpacing() const
{ 
  return m_Grid.BinSpacing();
}

void CGVTSeismicGridDefinition::BinSpacing(double binspacing)
{ 
  m_Grid.BinSpacing(binspacing);
}


double CGVTSeismicGridDefinition::Azimuth() const
{ 
  return m_Grid.Azimuth();
}

void CGVTSeismicGridDefinition::Azimuth(double azimuth)
{ 
  m_Grid.Azimuth(azimuth);
}


double CGVTSeismicGridDefinition::CornerNorthing() const
{ 
  return m_Grid.CornerNorthing();
}

void CGVTSeismicGridDefinition::CornerNorthing(double cornernorthing)
{ 
  m_Grid.CornerNorthing(cornernorthing);
}


double CGVTSeismicGridDefinition::CornerEasting() const
{ 
  return m_Grid.CornerEasting();
}

void CGVTSeismicGridDefinition::CornerEasting(double cornereasting)
{ 
  m_Grid.CornerEasting(cornereasting);
}


int CGVTSeismicGridDefinition::DeltaBin() const
{ 
  return m_Grid.DeltaBin();
}

void CGVTSeismicGridDefinition::DeltaBin(int deltabin)
{ 
  m_Grid.DeltaBin(deltabin);
}


int CGVTSeismicGridDefinition::DeltaTrack() const
{ 
  return m_Grid.DeltaTrack();
}

void CGVTSeismicGridDefinition::DeltaTrack(int deltatrack)
{ 
  m_Grid.DeltaTrack(deltatrack);
}


int CGVTSeismicGridDefinition::FirstBin() const
{ 
  return m_Grid.FirstBin();
}

void CGVTSeismicGridDefinition::FirstBin(int firstbin)
{ 
  m_Grid.FirstBin(firstbin);
}


int CGVTSeismicGridDefinition::FirstTrack() const
{ 
  return m_Grid.FirstTrack();
}

void CGVTSeismicGridDefinition::FirstTrack(int firsttrack)
{ 
  m_Grid.FirstTrack(firsttrack);
}


int CGVTSeismicGridDefinition::NumBin() const
{ 
  return m_Grid.NumBin();
}

void CGVTSeismicGridDefinition::NumBin(int numbin)
{ 
  m_Grid.NumBin(numbin);
}


int CGVTSeismicGridDefinition::NumTrack() const
{ 
  return m_Grid.NumTrack();
}

void CGVTSeismicGridDefinition::NumTrack(int numtrack)
{ 
  m_Grid.NumTrack(numtrack);
}


bool CGVTSeismicGridDefinition::GridIsValid() const
{
  return m_Grid.GridIsValid();
}

void CGVTSeismicGridDefinition::GenerateGrid(CPointSet &grid)
{ 
  m_Grid.GenerateGrid(grid);
}


void CGVTSeismicGridDefinition::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& /*progress*/)
{
  stream >> m_KeyFile;
  stream >> m_Name;
  stream >> m_Depth;

  double length = 0, width = 0;
  if(version < CStreamVersion(4, 1, 37))
  {
    stream >> length;
    stream >> width;
  }
  else
  {
    int numbin, numtrack, deltabin, deltatrack, firstbin, firsttrack;
    stream >> numbin;
    stream >> numtrack;
    stream >> deltabin;
    stream >> deltatrack;
    stream >> firstbin;
    stream >> firsttrack;
    m_Grid.NumBin(numbin);
    m_Grid.NumTrack(numtrack);
    m_Grid.DeltaBin(deltabin != 0 ? deltabin : 1);
    m_Grid.DeltaTrack(deltatrack != 0 ? deltatrack : 1);
    m_Grid.FirstBin(firstbin);
    m_Grid.FirstTrack(firsttrack);
  }
  double azi, cnorth, ceast, track, bin;
  stream >> azi;
  stream >> cnorth;
  stream >> ceast;
  stream >> track;
  stream >> bin;
  m_Grid.Azimuth(azi);
  m_Grid.CornerNorthing(cnorth);
  m_Grid.CornerEasting(ceast);
  m_Grid.TrackSpacing(track);
  m_Grid.BinSpacing(bin);

  if(version < CStreamVersion(4, 1, 37))
  {
    m_Grid.SetLength(length);
    m_Grid.SetWidth(width);
  }
}

void CGVTSeismicGridDefinition::SaveStream(TSTREAM& stream, TPROGRESS& /*progress*/)
{
  stream << m_KeyFile;
  stream << m_Name;
  stream << m_Depth;
  // 4.1.37 {
  stream << m_Grid.NumBin();
  stream << m_Grid.NumTrack();
  stream << m_Grid.DeltaBin();
  stream << m_Grid.DeltaTrack();
  stream << m_Grid.FirstBin();
  stream << m_Grid.FirstTrack();
  // } 4.1.37
  stream << m_Grid.Azimuth();
  stream << m_Grid.CornerNorthing();
  stream << m_Grid.CornerEasting();
  stream << m_Grid.TrackSpacing();
  stream << m_Grid.BinSpacing();
}

bool CGVTSeismicGridDefinition::FindBinTrack(double Northing, double Easting, int& nBin, int& nTrack)
{
  return m_Grid.FindBinTrack(Northing, Easting, nBin, nTrack);
}


bool CGVTSeismicGridDefinition::Import(const QString& fileName, CModelBase& model, bool bNoData)
{
  if (CGVTVtFile::IsBinaryVtFile(fileName))
  {
    CGVTVtFile VtFile(model);

    try
    {
      std::auto_ptr<IProgressBase> prog(_g->prog()->create(eProgress::Geo, "Import Vt file..."));

      if (VtFile.Import(fileName, *prog, bNoData))
      {
        CGVTVtFileCheck checkInfo = VtFile.getCheckInfo();

        NumTrack(checkInfo.NumRows());
        FirstTrack(checkInfo.FirstRow());
        DeltaTrack(checkInfo.DeltaRow());

        NumBin(checkInfo.NumCols());
        FirstBin(checkInfo.FirstCol());
        DeltaBin(checkInfo.DeltaCol());

        CornerEasting(checkInfo.CornerEasting());
        TrackSpacing(checkInfo.DeltaEasting());

        CornerNorthing(checkInfo.CornerNorthing());
        BinSpacing(checkInfo.DeltaNorthing());

        //Depth(checkInfo.MaxZ());
        Depth(model.Mesh().Max().Z()); // temporarily, by Peter's request
        Azimuth(checkInfo.Azimuth());

        return true;
      }
    }
    catch (CProgressCancel* e)
    {
      delete e;
    }

    return false;
  }
  else
  {
    CGVT123DIImport import;

    if (import.Import(fileName))
    {
      CornerNorthing(import.REAL_GEO_Y());
      CornerEasting(import.REAL_GEO_X());
      Azimuth(import.SKEW_ANGLE());
      TrackSpacing(import.REAL_DELTA_X());
      BinSpacing(import.REAL_DELTA_Y());
      DeltaBin(import.DELTA_BIN_NUM());
      DeltaTrack(import.DELTA_TRACK_NUM());
      FirstBin(import.FIRST_BIN());
      FirstTrack(import.FIRST_TRACK());
      NumBin(import.N_BIN());
      NumTrack(import.N_TRACK());

      return true;
    }
    else
    {
      _m()->msg(import.ErrorMessage());

      return false;
    }
  }
}

