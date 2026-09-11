#ifndef _GVT_SEISMIC_GRID_DEFINITION_H_
#define _GVT_SEISMIC_GRID_DEFINITION_H_

#include "GridDefinition.h"
#include "StorageNode.h"

#include <QString>

class CModelBase;

class CGVTSeismicGridDefinition {
  QString m_KeyFile; // path to a possible keyfile
  QString m_Name;
  double m_Depth;

  CGridDefinition m_Grid;

  friend class CGVTSettings;
  CGVTSeismicGridDefinition();
  CGVTSeismicGridDefinition(const QString &name);
  CGVTSeismicGridDefinition(const CGVTSeismicGridDefinition &rhs);

public:
  const QString &KeyFile() const;
  void KeyFile(const QString &path);

  const QString &Name() const;
  void Name(const QString &name);

  double Depth() const;
  void Depth(double depth);

  double TrackSpacing() const;
  void TrackSpacing(double trackspacing);

  double BinSpacing() const;
  void BinSpacing(double binspacing);

  double Azimuth() const;
  void Azimuth(double azimuth);

  double CornerNorthing() const;
  void CornerNorthing(double cornernorthing);

  double CornerEasting() const;
  void CornerEasting(double cornereasting);

  int DeltaBin() const;
  void DeltaBin(int deltabin);

  int DeltaTrack() const;
  void DeltaTrack(int deltatrack);

  int FirstBin() const;
  void FirstBin(int firstbin);

  int FirstTrack() const;
  void FirstTrack(int firsttrack);

  int NumBin() const;
  void NumBin(int numbin);

  int NumTrack() const;
  void NumTrack(int numtrack);

  bool GridIsValid() const;

  void GenerateGrid(CPointSet &grid);

  typedef CStorageNode::TSTREAM TSTREAM;
  typedef CStorageNode::TPROGRESS TPROGRESS;
  void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  bool FindBinTrack(double Northing, double Easting, int &nBin, int &nTrack);

  bool Import(const QString &fileName, CModelBase &model, bool bNoData = false);
};

#endif