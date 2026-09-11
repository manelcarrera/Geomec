#if !defined(__GRID_DEFINITION_H__)
#define __GRID_DEFINITION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPointSet;

// use this class if you want to be able to generate a grid of points in two dimensions
class CGridDefinition
{
  double m_TrackSpacing;   // space in between points in width direction
  double m_BinSpacing;     // space in between points in length direction
  double m_Azimuth;        // angle anywhere from -360 to 360
  double m_CornerNorthing; // northing of corner
  double m_CornerEasting;  // easting of corner
  int m_NumBin;            // number of bins (X/Northing dir/length)    Note: PointsNorthing/Easting used to return the wrong values; as far as I can tell, this has been a bug from the beginning
  int m_NumTrack;          // number of tracks (Y/Easting dir/width)
  int m_DeltaBin;          // the number of 'bins' to skip for each point, can be negative
  int m_DeltaTrack;        // the number of 'tracks' to skip for each point, can be negative
  int m_FirstBin;          // the index of the first bin (0-based)
  int m_FirstTrack;        // the index of the first track (0-based)

public: 
  CGridDefinition();
  CGridDefinition(const double &trackspacing,
                  const double &binspacing,
                  const double &azimuth,
                  const double &cornernorthing,
                  const double &cornereasting,
                  int numbin,
                  int numtrack,
                  int deltabin,
                  int deltatrack,
                  int firstbin,
                  int firsttrack);
//  CGridDefinition(const CGridDefinition &rhs);

  bool GridIsValid() const;

  // This generates the grid and adds the points to 'grid'
  void GenerateGrid(CPointSet &grid);

  const double &TrackSpacing() const { return m_TrackSpacing; }
  void TrackSpacing(const double &trackspacing) { m_TrackSpacing = trackspacing; }

  const double &BinSpacing() const { return m_BinSpacing; }
  void BinSpacing(const double &binspacing) { m_BinSpacing = binspacing; }

  const double &Azimuth() const { return m_Azimuth; }
  void Azimuth(const double &azimuth) { m_Azimuth = azimuth; }

  const double &CornerNorthing() const { return m_CornerNorthing; }
  void CornerNorthing(const double &cornernorthing) { m_CornerNorthing = cornernorthing; }

  const double &CornerEasting() const { return m_CornerEasting; }
  void CornerEasting(const double &cornereasting) { m_CornerEasting = cornereasting; }

  int NumBin() const { return m_NumBin; }
  void NumBin(int numbin) { m_NumBin = numbin; }

  int NumTrack() const { return m_NumTrack; }
  void NumTrack(int numtrack) { m_NumTrack = numtrack; }

  int DeltaBin() const { return m_DeltaBin; }
  void DeltaBin(int deltabin) { m_DeltaBin = deltabin; }

  int DeltaTrack() const { return m_DeltaTrack; }
  void DeltaTrack(int deltatrack) { m_DeltaTrack = deltatrack; }

  int FirstBin() const { return m_FirstBin; }
  void FirstBin(int firstbin) { m_FirstBin = firstbin; }

  int FirstTrack() const { return m_FirstTrack; }
  void FirstTrack(int firsttrack) { m_FirstTrack = firsttrack; }

  double GetLength() const;
  double GetWidth() const;

  // These function will set number of bins and tracks to 1,
  // delta track/bin to 1,
  // first track/bin to 0
  // and bin spacing to length or track spacing to width resp.
  void SetLength(double length);
  void SetWidth(double width);

  int PointsEasting() const;
  int PointsNorthing() const;

  bool FindBinTrack(double Northing, double Easting, int& nBin, int& nTrack);
};
#endif