#include <cstdio>
#include "GVT123DIImport.h"
#include "FieldFactor.h"
#include <QRegExp>
#include <cassert>

CGVT123DIImport::CGVT123DIImport()
: m_fp(0),
  m_nLineNr(0),
  m_strError(""),
  m_N_BIN(std::pair<int, bool>(0, false)),
  m_N_TRACK(std::pair<int, bool>(0, false)),
  m_REAL_DELTA_X(std::pair<double, bool>(0, false)),
  m_REAL_DELTA_Y(std::pair<double, bool>(0, false)),
  m_REAL_GEO_X(std::pair<double, bool>(0, false)),
  m_REAL_GEO_Y(std::pair<double, bool>(0, false)),
  m_SKEW_ANGLE(std::pair<double, bool>(0, false)),
  m_bFeetUnit(false),
  m_DELTA_BIN_NUM(1),
  m_DELTA_TRACK_NUM(1),
  m_FIRST_BIN(0),
  m_FIRST_TRACK(0)
{
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("N_BIN", &CGVT123DIImport::ReadN_BIN));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("N_TRACK", &CGVT123DIImport::ReadN_TRACK));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("REAL_DELTA_X", &CGVT123DIImport::ReadREAL_DELTA_X));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("REAL_DELTA_Y", &CGVT123DIImport::ReadREAL_DELTA_Y));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("REAL_GEO_X", &CGVT123DIImport::ReadREAL_GEO_X));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("REAL_GEO_Y", &CGVT123DIImport::ReadREAL_GEO_Y));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("SKEW_ANGLE", &CGVT123DIImport::ReadSKEW_ANGLE));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("UNITS", &CGVT123DIImport::ReadUNITS));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("DELTA_BIN_NUM", &CGVT123DIImport::ReadDELTA_BIN_NUM));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("DELTA_TRACK_NUM", &CGVT123DIImport::ReadDELTA_TRACK_NUM));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("FIRST_BIN", &CGVT123DIImport::ReadFIRST_BIN));
  m_KeyToFunction.insert(TKeyToFuncMap::value_type("FIRST_TRACK", &CGVT123DIImport::ReadFIRST_TRACK));
}

CGVT123DIImport::~CGVT123DIImport(void)
{
}

bool CGVT123DIImport::Import(const QString& strFileName)
{
  assert(m_fp == 0);
  m_fp = fopen(strFileName.toStdString().c_str(), "r");
  if(!m_fp)
  {
  m_strError = "Unable to open file: " + strFileName;
  return false;
  }

  bool bParseOK = Parse();

  fclose(m_fp);
  m_fp = 0;

  return bParseOK;
}

bool CGVT123DIImport::Parse()
{
  assert(m_fp != 0);

  m_nLineNr = 0;
  QString strLine;

  char buf[1024];
  while(fgets(buf, 1024, m_fp))
  {
  if(buf[strlen(buf)-1] == '\n' || feof(m_fp) )
  {
      // append contents of buf to the current line
      strLine += buf;

      // update line nr
      ++m_nLineNr;
      if(m_nLineNr != 1)// first line is ignored, just contains 'MESSAGE_FILE'
      {
    // get rid of leading and trailing whitespace characters
    strLine = strLine.trimmed();

    // parse the contents of the line
    if(!ParseLine(strLine))
    {
          return false;
    }
      }

      // reset line
      strLine.clear();
  }
  }

  if(!CheckReadParameters())
  return false;

  ConvertParameters();
  return true;
}

bool CGVT123DIImport::ParseLine(const QString &strLine)
{
  QRegExp re("[ \\t]");
  QString s = strLine;
  int n = re.indexIn(s);
  
  if(n == -1)
  {
  m_strError = QString("Syntax error in line nr. %1").arg(m_nLineNr);
  return false;
  }

  s = s.left(n);

  TKeyToFuncMap::iterator it = m_KeyToFunction.find(s);

  if(it != m_KeyToFunction.end())
  {
  return ((*this).*(it->second))(strLine);
  }
  // might be a line we're just not interested in...
  return true;
}

bool CGVT123DIImport::ReadN_BIN(const QString &strLine)
{
  //e.g. N_BIN I 501
  m_N_BIN.second = true;
  return ReadIntegerValue(strLine, m_N_BIN.first);
}

bool CGVT123DIImport::ReadN_TRACK(const QString &strLine)
{
  //e.g. N_TRACK I 405
  m_N_TRACK.second = true;
  return ReadIntegerValue(strLine, m_N_TRACK.first);
}

bool CGVT123DIImport::ReadREAL_DELTA_X(const QString &strLine)
{
  //e.g. REAL_DELTA_X R 100.00000
  m_REAL_DELTA_X.second = true;
  return ReadDoubleValue(strLine, m_REAL_DELTA_X.first);
}

bool CGVT123DIImport::ReadREAL_DELTA_Y(const QString &strLine)
{
  //e.g. REAL_DELTA_Y R 100.00000
  m_REAL_DELTA_Y.second = true;
  return ReadDoubleValue(strLine, m_REAL_DELTA_Y.first);
}

bool CGVT123DIImport::ReadREAL_GEO_X(const QString &strLine)
{
  //e.g. REAL_GEO_X R 894741.31250000
  m_REAL_GEO_X.second = true;
  return ReadDoubleValue(strLine, m_REAL_GEO_X.first);
}

bool CGVT123DIImport::ReadREAL_GEO_Y(const QString &strLine)
{
  //e.g. REAL_GEO_Y R 100220439.00000
  m_REAL_GEO_Y.second = true;
  return ReadDoubleValue(strLine, m_REAL_GEO_Y.first);
}

bool CGVT123DIImport::ReadSKEW_ANGLE(const QString &strLine)
{
  //e.g. SKEW_ANGLE R 30.120800
  m_SKEW_ANGLE.second = true;
  return ReadDoubleValue(strLine, m_SKEW_ANGLE.first);
}

bool CGVT123DIImport::ReadUNITS(const QString& strLine)
{
  // allow METRES, METERS or FEET
  // assuming METERS if not encountered in the file

  QString s = strLine;
  int n = s.indexOf(" C ");

  if(n == -1)
  {
  m_strError = QString("Syntax error in line nr. %1, 'C' expected").arg(m_nLineNr);
  return false;
  }

  s = s.right(s.length() - (n + 3));
  s = s.trimmed();
  if(s == "FEET")
  m_bFeetUnit = true;
  else if(s != "METERS" && s != "METRES")
  {
  m_strError = QString("Syntax error in line nr. %1, expect 'FEET', 'METERS' or 'METRES'").arg(m_nLineNr);
  return false;
  }

  return true;
}

bool CGVT123DIImport::ReadDELTA_BIN_NUM(const QString& strLine)
{
  return ReadIntegerValue(strLine, m_DELTA_BIN_NUM);
}

bool CGVT123DIImport::ReadDELTA_TRACK_NUM(const QString& strLine)
{
  return ReadIntegerValue(strLine, m_DELTA_TRACK_NUM);
}

bool CGVT123DIImport::ReadFIRST_BIN(const QString& strLine)
{
  return ReadIntegerValue(strLine, m_FIRST_BIN);
}

bool CGVT123DIImport::ReadFIRST_TRACK(const QString& strLine)
{
  return ReadIntegerValue(strLine, m_FIRST_TRACK);
}

bool CGVT123DIImport::ReadIntegerValue(const QString &strLine, int &nVal)
{
  QString s = strLine;
  int n = s.indexOf(" I ");

  if(n == -1)
  {
  m_strError = QString("Syntax error in line nr. %1, 'I' expected").arg(m_nLineNr);
  return false;
  }

  s = s.right(s.length() - (n + 3));
  if(StringToInt(s, nVal))
  return true;

  m_strError = QString("Error in Line %1, '%2' is not an integer value").arg(m_nLineNr).arg(s);
  return false;
}

bool CGVT123DIImport::ReadDoubleValue(const QString &strLine, double &dVal)
{
  QString s = strLine;
  int n = s.indexOf(" R ");

  if(n == -1)
  {
  m_strError = QString("Syntax error in line nr. %d, 'R' expected").arg(m_nLineNr);
  return false;
  }

  s = s.right(s.length() - (n + 3));
  if(StringToDouble(s, dVal))
  return true;

  m_strError = QString("Error in Line %1, '%2' is not a real value").arg(m_nLineNr).arg(s);
  return false;
}

bool CGVT123DIImport::StringToInt(const QString& strVal, int& iVal) const
{
  bool bRet;

  iVal = strVal.toInt(&bRet);

  return bRet;
}

bool CGVT123DIImport::StringToDouble(const QString& strVal, double& dVal) const
{
  bool bRet;

  dVal = strVal.toDouble(&bRet);

  return bRet;
}

bool CGVT123DIImport::CheckReadParameters()
{
  if(!m_N_BIN.second)
  {
  m_strError = "N_BIN not found";
  return false;
  }
  
  if(!m_N_TRACK.second)
  {
  m_strError = "N_TRACK not found";
  return false;
  }
  
  if(!m_REAL_DELTA_X.second)
  {
  m_strError = "REAL_DELTA_X not found";
  return false;
  }

  if(!m_REAL_DELTA_Y.second)
  {
  m_strError = "REAL_DELTA_Y not found";
  return false;
  }

  if(!m_REAL_GEO_X.second)
  {
  m_strError = "REAL_GEO_X not found";
  return false;
  }

  if(!m_REAL_GEO_Y.second)
  {
  m_strError = "REAL_GEO_Y not found";
  return false;
  }

  if(!m_SKEW_ANGLE.second)
  {
  m_strError = "SKEW_ANGLE not found";
  return false;
  }
  return true;
}

void CGVT123DIImport::ConvertParameters()
{
  if(m_bFeetUnit)
  {
  // convert from feet to meters
  m_REAL_DELTA_X.first /= FF_FACTOR_LENGTH;
  m_REAL_DELTA_Y.first /= FF_FACTOR_LENGTH;
  m_REAL_GEO_X.first /= FF_FACTOR_LENGTH;
  m_REAL_GEO_Y.first /= FF_FACTOR_LENGTH;
  }
}
