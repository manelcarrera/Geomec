 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "FvGocadWellFile.h"
#include <assert.h>

CGoCadWellFile::CGoCadWellFile()
: m_bHeaderRead(false),
  m_iX(-1),
  m_iY(-1),
  m_iZ(-1),
  m_iMD(-1)
{
}

CGoCadWellFile::~CGoCadWellFile()
{
}

/*!
  Returns the number of wells in the file
*/
int CGoCadWellFile::wellSize() const
{
  return m_vcWell.size();
}

/*!
  The name of the well based on the index
*/
const QString& CGoCadWellFile::wellName(int nWellIndex) const
{
  assert(nWellIndex >= 0);
  assert(nWellIndex < wellSize());
  return m_vcWell[nWellIndex].first;
}

/*!
  Returns the number of points based on the well index
*/
int CGoCadWellFile::pointSize(int nWellIndex) const
{
  assert(nWellIndex >= 0);
  assert(nWellIndex < wellSize());
  assert(m_vcWell[nWellIndex].second.first.size() == m_vcWell[nWellIndex].second.second.size());
  return m_vcWell[nWellIndex].second.first.size();
}

/*!
  Returns a point location for the well.
  \sa tmdAt
*/
const geo::IPoint& CGoCadWellFile::pointAt(int nWellIndex, int nPointIndex) const
{
  assert(nPointIndex >= 0);
  assert(nPointIndex < pointSize(nWellIndex));
  return m_vcWell[nWellIndex].second.first[nPointIndex];
}

/*!
  Returns a tmd for a location at the well.
*/
const double& CGoCadWellFile::tmdAt(int nWellIndex, int nPointIndex) const
{
  assert(nPointIndex >= 0);
  assert(nPointIndex < pointSize(nWellIndex));
  return m_vcWell[nWellIndex].second.second[nPointIndex];
}

/*!
  Read the header of the file
*/
void CGoCadWellFile::ReadHeader(TInputStream& stream)
{
  int iLineNr = stream.line_number();
  int i;
  for(i = 0; i < 4; ++i)
  {
  if(stream.eof())
    throw CReadException("Unexpected end of file found (incomplete header)");

  QString sTag;
  stream >> sTag;

  if(stream.line_number() != iLineNr) // end of header
    break;

  // NOTE: gocad has X == Easting and Y == Northing !!

  if     (sTag == "Y" || sTag.toUpper() == "NORTHING") {
    if(m_iX != -1)
    throw CReadException("Header tag 'Y' or 'Northing' appears twice");
    m_iX = i;
  }
  else if(sTag == "X" || sTag.toUpper() == "EASTING") {
    if(m_iY != -1)
    throw CReadException("Header tag 'X' or 'Easting' appears twice");
    m_iY = i;
  }
  else if(sTag == "Z" || sTag.toUpper() == "DEPTH") {
    if(m_iZ != -1)
    throw CReadException("Header tag 'Z' or 'Depth' appears twice");
    m_iZ = i;
  }
  else if(sTag == "MD" || sTag == "TMD" || sTag == "AHD") {
    if(m_iMD != -1)
    throw CReadException("Header tag 'MD' or 'TMD' or 'AHD' appears twice");
    m_iMD = i;
  }
  else {
    throw CReadException(QString("Invalid header tag '%1' found").arg(sTag));
  }
  }

  if(m_iX == -1)
  throw CReadException("Header tag 'Y' or 'Northing' not found");
  if(m_iY == -1)
  throw CReadException("Header tag 'X' or 'Easting' not found");
  if(m_iZ == -1)
  throw CReadException("Header tag 'Z' or 'Depth' not found");
  if(m_iMD == -1)
  throw CReadException("Header tag 'MD' or 'TMD' or 'AHD' not found");

  m_bHeaderRead = true;

  DefineComment("#");
  DefineComment("*");
}

/*!
  Read the well path file
*/
bool CGoCadWellFile::OnRead( TInputStream& stream )
{
  stream.eatwhite();
  while( !stream.eof() ) {
      if(!m_bHeaderRead) {
    // maintain compatibility with older files, allow comment character
    if(stream.peek() == '#' || stream.peek() == '*')
      stream.get();

    QString sHeaderMagic;
    stream >> sHeaderMagic;
    if(sHeaderMagic != "WELLNAME")
      throw CReadException("'WELLNAME' expected");
    ReadHeader(stream);
    } else {
    QString sWellName;
    stream >> sWellName;
    if( m_vcWell.size() > 0 ) {
      if( sWellName.toUpper() != m_vcWell[ m_vcWell.size() - 1 ].first )
      m_vcWell.push_back(TWell( sWellName.toUpper(), TWellLocation() ));
    } else m_vcWell.push_back(TWell( sWellName.toUpper(), TWellLocation() ));
    TWellLocation& location = m_vcWell[m_vcWell.size() - 1].second;
  
    double vcValue[4];
    for(int i = 0; i < 4; i++) {
      stream >> vcValue[i];
    }
    geo::CPoint point( vcValue[m_iX], vcValue[m_iY], vcValue[m_iZ] );
    location.first.push_back( point );
    location.second.push_back( vcValue[m_iMD] );
    stream.eatwhite();
    }
  };
  return true;
}

