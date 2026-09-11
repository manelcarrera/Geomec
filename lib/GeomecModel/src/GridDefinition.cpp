#include "GridDefinition.h"

#include "Point.h"
#include "PointSet.h"
#include "Vector.h"

CGridDefinition::CGridDefinition()
    : m_TrackSpacing(0), m_BinSpacing(0), m_Azimuth(0), m_CornerNorthing(0), m_CornerEasting(0), m_NumBin(0),
      m_NumTrack(0), m_DeltaBin(1), m_DeltaTrack(1), m_FirstBin(0), m_FirstTrack(0) {}

CGridDefinition::CGridDefinition(const double &trackspacing, const double &binspacing, const double &azimuth,
                                 const double &cornernorthing, const double &cornereasting, int numbin, int numtrack,
                                 int deltabin, int deltatrack, int firstbin, int firsttrack)
    : m_TrackSpacing(trackspacing), m_BinSpacing(binspacing), m_Azimuth(azimuth), m_CornerNorthing(cornernorthing),
      m_CornerEasting(cornereasting), m_NumBin(numbin), m_NumTrack(numtrack), m_DeltaBin(deltabin),
      m_DeltaTrack(deltatrack), m_FirstBin(firstbin), m_FirstTrack(firsttrack) {}
/*
CGridDefinition::CGridDefinition(const CGridDefinition &rhs)
:m_TrackSpacing(rhs.m_TrackSpacing),
 m_BinSpacing(rhs.m_BinSpacing),
 m_Azimuth(rhs.m_Azimuth),
 m_CornerNorthing(rhs.m_CornerNorthing),
 m_CornerEasting(rhs.m_CornerEasting)
{
}
*/
bool CGridDefinition::GridIsValid() const {
  return (m_TrackSpacing > 0 && m_BinSpacing > 0 && m_NumBin > 0 && m_NumTrack > 0 && m_DeltaBin != 0 &&
          m_DeltaTrack != 0 && m_FirstBin >= 0 && m_FirstTrack >= 0);
}

void CGridDefinition::GenerateGrid(CPointSet &grid) {
  geo::CPoint pt;
  for (int nTrack = m_FirstTrack; nTrack < m_FirstTrack + m_NumTrack && nTrack > m_FirstTrack - m_NumTrack;
       nTrack += m_DeltaTrack) {
    double y(m_CornerEasting + nTrack * m_TrackSpacing);
    for (int nBin = m_FirstBin; nBin < m_FirstBin + m_NumBin && nBin > m_FirstBin - m_NumBin; nBin += m_DeltaBin) {
      pt.Set(m_CornerNorthing + nBin * m_BinSpacing, y, 0);
      // rotate the point
      // Add the rotated point to the pointset.
      // translate point to so that the corner point is the origin:
      pt.Y(pt.Y() - m_CornerEasting);
      pt.X(pt.X() - m_CornerNorthing);
      // rotate around z-axis
      pt.Rotate(geo::CVector::Zaxis, m_Azimuth);
      // now translate back
      pt.Y(pt.Y() + m_CornerEasting);
      pt.X(pt.X() + m_CornerNorthing);

      grid.PushBack(pt, std::vector<double>());
    }
  }
}

double CGridDefinition::GetLength() const { return NumBin() * BinSpacing(); }

double CGridDefinition::GetWidth() const { return NumTrack() * TrackSpacing(); }

void CGridDefinition::SetLength(double length) {
  BinSpacing(length);
  NumBin(1);
  DeltaBin(1);
  FirstBin(0);
}

void CGridDefinition::SetWidth(double width) {
  TrackSpacing(width);
  NumTrack(1);
  DeltaTrack(1);
  FirstTrack(0);
}

int CGridDefinition::PointsEasting() const { return m_NumTrack; }

int CGridDefinition::PointsNorthing() const { return m_NumBin; }

// TODO: combine with generating the grid in the first place
bool CGridDefinition::FindBinTrack(double Northing, double Easting, int &nBin, int &nTrack) {
  geo::CPoint pt;

  for (nTrack = m_FirstTrack; nTrack < m_FirstTrack + m_NumTrack && nTrack > m_FirstTrack - m_NumTrack;
       nTrack += m_DeltaTrack) {
    double y(m_CornerEasting + nTrack * m_TrackSpacing);
    for (nBin = m_FirstBin; nBin < m_FirstBin + m_NumBin && nBin > m_FirstBin - m_NumBin; nBin += m_DeltaBin) {
      pt.Set(m_CornerNorthing + nBin * m_BinSpacing, y, 0);
      // rotate the point
      // Add the rotated point to the pointset.
      // translate point to so that the corner point is the origin:
      pt.Y(pt.Y() - m_CornerEasting);
      pt.X(pt.X() - m_CornerNorthing);
      // rotate around z-axis
      pt.Rotate(geo::CVector::Zaxis, m_Azimuth);
      // now translate back
      pt.Y(pt.Y() + m_CornerEasting);
      pt.X(pt.X() + m_CornerNorthing);

      if (std::abs(pt.X() - Northing) < 1e-4 && std::abs(pt.Y() - Easting) < 1e-4)
        return true;
    }
  }

  return false;
}
