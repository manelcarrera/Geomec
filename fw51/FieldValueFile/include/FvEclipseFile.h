 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// EclipseFormat.h: interface for the CEclipseFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECLIPSEFORMAT_H__3DAA6809_EE76_4E93_9F0C_26B532F758AA__INCLUDED_)
#define AFX_ECLIPSEFORMAT_H__3DAA6809_EE76_4E93_9F0C_26B532F758AA__INCLUDED_

#include "FvEclipseCell.h"
#include "FvTextFile.h"

#include "FieldValueFileExports.h"

class CEclipseReservoir;
class FIELDVALUEFILE_EXPORT CEclipseFile : public CTextFile  
{
protected:
  typedef std::vector<geo::CPoint> TRow;
  typedef std::vector<TRow> TEclipseHorizon;
private:
  std::vector<TEclipseHorizon> m_vcEclipseHorizon;

  typedef std::pair<QString, array_temp<double> > TProperty;
  std::vector<TProperty> m_vcProperty;
  bool m_bCartesian;
  int m_nDepth;
  geo::CPoint MeanPoint(int Xp, int Yp, int Zp) const;
  std::vector<geo::CPoint> AdjacentPoints(int Xp, int Yp, int Zp) const;
  void AddPoint(std::vector<geo::CPoint>& vcPoint, int Xv, int Yv, int Zv, int px, int py, int pz) const;

protected:
  std::vector<CEclipseReservoir*> m_vcReservoir;
  typedef CTokenTemplate<CEclipseFile> TEclipseToken;
public:
  // Construction / Destruction
  CEclipseFile();
  virtual ~CEclipseFile();

  CEclipseCell Cell(int x, int y, int z) const;
  bool WriteCellEnable(TOutputStream& stream);
  int SizeX() const;  // Total amount of eclipse cells in the X-direction
  int SizeY() const;  // Total amount of eclipse cells in the Y-direction
  int SizeZ() const;  // Total amount of eclipse cells in the Z-direction
  int HorizonSize() const;
  const TEclipseHorizon Horizon(size_t nHorizonIndex) const;
  TEclipseHorizon Horizon(size_t nHorizonIndex);

  // Reservoirs at a certain depth
  const CEclipseReservoir& Reservoir(int nDepth) const;
  CEclipseReservoir& Reservoir(int nDepth);

  void Destroy();

  virtual bool OnWrite(TOutputStream& stream);
  virtual long SavedItems() const;

  // Properties
  size_t PropertySize() const;
  const QString& PropertyName(size_t nPropertyIndex) const;
  const double& PropertyValue(size_t nPropertyIndex, int x, int y, int z) const;
  void PropertyValue(size_t nPropertyIndex, int x, int y, int z, const double& dValue);
  int CreateProperty(const QString& sName);
protected:
  virtual bool OnParseFail(TInputStream& stream, const QString& sToken);
  virtual bool OnEndParseSucceed();  // The parser ended with success
  virtual CEclipseReservoir& CreateReservoir(int x, int y);
  bool ReadZCoord(TInputStream& stream, const QString& sToken);
  bool ReadCellEnable(TInputStream& stream, const QString& sToken);
  bool ReadSpecGrid(TInputStream& stream, const QString& sToken);
  bool ReadCoordSys(TInputStream& stream, const QString& sToken);
  bool ReadCoordinates(TInputStream& stream, const QString& sToken);
  bool WriteSpecGrid(TOutputStream& stream);
  bool WriteCoordSys(TOutputStream& stream);
  bool WriteCoord(TOutputStream& stream);
  bool WriteZCoord(TOutputStream& stream);
  bool WriteProperties(TOutputStream& stream);
};

#endif // !defined(AFX_ECLIPSEFORMAT_H__3DAA6809_EE76_4E93_9F0C_26B532F758AA__INCLUDED_)
