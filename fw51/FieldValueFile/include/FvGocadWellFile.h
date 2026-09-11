/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
/*                                         Copyright (c) 2007 TNO DIANA BV */
#if !defined(GOCAD_WELL_FILE_INCLUDED_)
#define GOCAD_WELL_FILE_INCLUDED_

#include "FvTextFile.h"
#include "Point.h"

#include "FieldValueFileExports.h"

class FIELDVALUEFILE_EXPORT CGoCadWellFile : public CTextFile {
  typedef std::vector<geo::CPoint> TPointVec;
  typedef std::vector<double> TTMDVec;
  typedef std::pair<TPointVec, TTMDVec> TWellLocation;
  typedef std::pair<QString, TWellLocation> TWell;
  typedef std::vector<TWell> TWellVec;

  TWellVec m_vcWell;

  bool m_bHeaderRead;
  int m_iX;
  int m_iY;
  int m_iZ;
  int m_iMD;

private:
  void ReadHeader(TInputStream &stream);

public:
  CGoCadWellFile();
  virtual ~CGoCadWellFile();

  int wellSize() const;
  const QString &wellName(int nWellIndex) const;

  int pointSize(int nWellIndex) const;
  const geo::IPoint &pointAt(int nWellIndex, int nPointIndex) const;
  const double &tmdAt(int nWellIndex, int nPointIndex) const;

protected:
  virtual bool OnRead(TInputStream &stream);
};

#endif // !defined(GOCAD_WELL_FILE_INCLUDED_)
