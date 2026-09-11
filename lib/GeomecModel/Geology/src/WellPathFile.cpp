// WellPathFile.cpp: implementation of the CWellPathFile class.
//
//////////////////////////////////////////////////////////////////////

#include "WellPathFile.h"
#include "LengthQuantity.h"
#include "NewWellPath.h"
#include "NewWellPathInput.h"
#include "NewWellDefinitionPointList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWellPathFile::CWellPathFile(CNewWellPathInput *pNewWellPathInput,CQuantity::UNIT unit): m_State(READ_NONE)
, m_pNewWellPathInput(pNewWellPathInput)
{
  
  m_Unit=unit;

  // Comment
  DefineComment("!");
  DefineComment("#");
  
  // Keys
  DefineToken(*(new TWellPathToken(*this, "Holename:",     &CWellPathFile::ReadIgnore)));
  DefineToken(*(new TWellPathToken(*this, "Holename",      &CWellPathFile::ReadIgnore)));
  DefineToken(*(new TWellPathToken(*this, "Status",        &CWellPathFile::ReadIgnore)));
  DefineToken(*(new TWellPathToken(*this, "Well_EPDWB",    &CWellPathFile::ReadIgnore)));
  DefineToken(*(new TWellPathToken(*this, "Wellname",      &CWellPathFile::ReadWellName)));
  DefineToken(*(new TWellPathToken(*this, "Well_Northing", &CWellPathFile::ReadNorthing)));
  DefineToken(*(new TWellPathToken(*this, "Well_Easting",  &CWellPathFile::ReadEasting)));
  DefineToken(*(new TWellPathToken(*this, "Derrick",       &CWellPathFile::ReadDerrickElev)));
}

CWellPathFile::~CWellPathFile()
{

}

bool CWellPathFile::ReadDerrickElev(TInputStream& stream, const QString& sToken)
{
  QString sElevation;
  stream >> sElevation;
  if( sElevation.toUpper() != "ELEVATION" ) 
    throw CReadException("Keyword derrick elevation expected.");
  CheckDelimiter(stream, ':');
  
  double dValue;
  stream >> dValue; // Do something with it!
  
  CLengthQuantity l;
  l.Value(dValue,m_Unit);
  m_TVD  = l.Value();
  m_State = READ_HEADER;
  ReadIgnore(stream, sToken); // ignore rest of line...
  return true;
}

bool CWellPathFile::ReadEasting(TInputStream& stream, const QString& sToken)
{
  CheckDelimiter(stream, ':');

  double dValue;
  stream >> dValue; // Do something with it!
  
  CLengthQuantity l;
  l.Value(dValue,m_Unit);
  m_Easting = l.Value();
  m_State = READ_HEADER;
  ReadIgnore(stream, sToken); // ignore rest of line...

  return true;
}

bool CWellPathFile::ReadIgnore(TInputStream& stream, const QString& /*sToken*/)
{
  while(stream.peek() != '\n' && stream.peek() != 0) stream.get();

  return true;
}

bool CWellPathFile::ReadNorthing(TInputStream& stream, const QString& sToken)
{
  CheckDelimiter(stream, ':');

  double dValue;
  stream >> dValue; // Do something with it!

  CLengthQuantity l;
  l.Value(dValue,m_Unit);
  m_Northing = l.Value();
  m_State = READ_HEADER;
  ReadIgnore(stream, sToken); // ignore rest of line...

  return true;
}

bool CWellPathFile::ReadWellName(TInputStream& stream, const QString& /*sToken*/)
{
  CheckDelimiter(stream, ':');

  QString strValue;
  
  stream >> strValue; // Do something with it!
  
  {
    m_pNewWellPathInput->Name(strValue);
  }

  m_State = READ_HEADER;
  
  return true;
}

void CWellPathFile::CheckDelimiter(TInputStream& stream, char ch)
{
  char c;
  do {
  c = stream.get();
  if( c != ' ' && c != '\t' && c != ch )
    throw CReadException(QString("Separator expected ").arg(ch));
  } while( c != ch );
}

/*
enum bool CWellPathFile::OnParseSucceed(const TInputStream& stream, const QString& sToken)
{
  StoreFilePosition();
  return CToken::OK_CONTINUE;
}
*/
void CWellPathFile::OnParseSucceed(TInputStream& stream, const QString& /*sToken*/)
{
  stream.eatwhite();
  if(!isdigit( stream.peek() )) return; 

  // Numbers seen ...
  if(m_State != READ_HEADER) {
    throw CReadException("Values expected...");
  }

  // Start reading colunmns
  QVector<double> tmd;
  QVector<double> azi;
  QVector<double> inc;

  int count=0;

   // Check of a delimiter
  stream.eatwhite();
  while( isdigit( stream.peek() ) ) {

  // Read just one line
  std::vector<double> wellptvec(6);
  for(int i = 0; i < wellptvec.size(); i++)
  {
    stream >> wellptvec[i];
  }
      
  count++;
  tmd.resize(count);
  azi.resize(count);
  inc.resize(count);

  //unit conversion
  CLengthQuantity l;
  wellptvec[0] = l.Convert(wellptvec[0],CQuantity::SI_UNIT,m_Unit); //ahd
  wellptvec[3] = l.Convert(wellptvec[3],CQuantity::SI_UNIT,m_Unit); //northing
  wellptvec[4] = l.Convert(wellptvec[4],CQuantity::SI_UNIT,m_Unit); //easting
  wellptvec[5] = l.Convert(wellptvec[5],CQuantity::SI_UNIT,m_Unit); //tvd 

  tmd[count-1]=wellptvec[0];
  inc[count-1]=wellptvec[1];
  azi[count-1]=wellptvec[2];

  ItemRead(); // Update progress bar...

  stream.eatwhite();
  }

  {
  m_pNewWellPathInput->GlobalNorthing(m_Northing);
  m_pNewWellPathInput->GlobalEasting(m_Easting);
  m_pNewWellPathInput->GlobalTVD(m_TVD);

  QVector<geo::CPoint> points;
  QVector<double>      azimuth;
  QVector<double>      inclination;
  CCommonWellPath::PartialInitFromArray_tmd_azi_inc(0, 0, 0, tmd, azi, inc, points, azimuth, inclination);

    m_pNewWellPathInput->InitFromPointArray(points, false, &azimuth, &inclination);
  }
}

