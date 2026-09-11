
#include "LogASCIIStandardFile.h"

#include "FieldFactor.h"
#include "GeomecStringTable.h"
#include "IObject.h"
#include "LasPointSet.h"
#include "NewWellPath.h"
#include "NodalValueSet.h"
#include "PointSet.h"
#include "StringUtils.h"
#include "ValueTypeFactory.h"
#include "ValueTypes.h"
#include "resourceIDS.h"

const int BS = 512; // 2048;

///// CLogASCIIStandardFile::CLasDataLine

CLogASCIIStandardFile::CLasDataLine::CLasDataLine(const QString &strLine, int nLineNr) {
  int iDot = strLine.indexOf('.');
  if (iDot == -1)
    throw CLogASCIIStandardFile::CException("Line %1: no '.' found in parameter data", nLineNr);

  if (iDot + 1 == strLine.length())
    throw CLogASCIIStandardFile::CException("Line %1: no data found in parameter data (line too short)", nLineNr);

  m_strMnemonic = strLine.left(iDot);
  m_strMnemonic = m_strMnemonic.trimmed();

  // find the next whitespace
  int iWSpace;
  for (iWSpace = iDot + 1; iWSpace < strLine.length() && strLine[iWSpace] != ' ' && strLine[iWSpace] != '\t'; ++iWSpace)
    ;

  if (iWSpace - iDot > 1) // found a unit
    m_strUnit = strLine.mid(iDot + 1, iWSpace - iDot - 1);

  int iColon = strLine.indexOf(':', iWSpace);
  if (iColon == -1) {
    m_strData = strLine.mid(iWSpace);
    // ... and no description
  } else {
    m_strData = strLine.mid(iWSpace, iColon - iWSpace - 1);

    if (iColon < strLine.length() - 1) {
      m_strDescription = strLine.mid(iColon + 1);
      m_strDescription = m_strDescription.trimmed();
    }
  }

  m_strData = m_strData.trimmed();
}

const QString &CLogASCIIStandardFile::CLasDataLine::Mnemonic() const { return m_strMnemonic; }

const QString &CLogASCIIStandardFile::CLasDataLine::Unit() const { return m_strUnit; }

const QString &CLogASCIIStandardFile::CLasDataLine::Data() const { return m_strData; }

const QString &CLogASCIIStandardFile::CLasDataLine::Description() const { return m_strDescription; }

///// CLogASCIIStandardFile::CCurveDataLine

CLogASCIIStandardFile::CCurveDataLine::CCurveDataLine(const QString &strLine, int nLineNr,
                                                      const CLogASCIIStandardFile &lasFile) {
  int nValues = lasFile.ColumnSize();
  m_vcValues.resize(nValues);

  char dlm = lasFile.Delimiter();
  QString sDlm(dlm); // a string with the delimiter character (so length = 1)

  int iPos = 0;
  int iCol;

  for (iCol = 0; iCol < nValues; ++iCol) {
    if (iPos >= strLine.length()) {
      // error, too few values
      QString msg;
      msg = QString("Line %1: too few values encountered (expected %2, found %3)").arg(nLineNr).arg(nValues).arg(iCol);
      throw CLogASCIIStandardFile::CException(msg);
    }

    // the remainder of the string starting at iPos
    QString s = strLine.mid(iPos);
    QString sVal = spanExcluding(s, sDlm);

    if (findOneOf(sVal, "0123456789") != -1) {
      double dVal = sVal.toDouble();
      if (!(lasFile.NullValue() == dVal))
        m_vcValues[iCol] = lasFile.ColumnDef(iCol).ConvertValue(sVal.toDouble());
    }

    iPos += sVal.length() + 1; // skip the value string and the delimiter behind it
    if (iPos < strLine.length() && dlm == ' ') {
      // subsequent spaces are considered as one (no NULL value in between)
      while (iPos < strLine.length() && strLine[iPos] == ' ')
        ++iPos;
    }
  }
}

const geo::CValue &CLogASCIIStandardFile::CCurveDataLine::Value(int nColumn) const {
  assert(nColumn >= 0 && nColumn < m_vcValues.size());
  return m_vcValues[nColumn];
}

///// CLogASCIIStandardFile::CException

CLogASCIIStandardFile::CException::CException(const QString &strError) : m_strError(strError) {}

CLogASCIIStandardFile::CException::CException(const QString &strError, int nLine) {
  m_strError = QString(strError).arg(nLine);
}

const QString &CLogASCIIStandardFile::CException::Error() const { return m_strError; }

///// CLogASCIIStandardFile::CUnitConverter

CLogASCIIStandardFile::CUnitConverter::CUnitConverter(double dFactor) : m_dFactor(dFactor) {}

CLogASCIIStandardFile::CUnitConverter::CUnitConverter(const CUnitConverter &rhs) : m_dFactor(rhs.m_dFactor) {}

CLogASCIIStandardFile::CUnitConverter::~CUnitConverter() {}

double CLogASCIIStandardFile::CUnitConverter::Factor() const { return m_dFactor; }

///// CLogASCIIStandardFile::CProportionalUnitConverter

CLogASCIIStandardFile::CProportionalUnitConverter::CProportionalUnitConverter(double dFactor)
    : CUnitConverter(dFactor) {}

CLogASCIIStandardFile::CProportionalUnitConverter::CProportionalUnitConverter(const CProportionalUnitConverter &rhs)
    : CUnitConverter(rhs) {}

CLogASCIIStandardFile::CProportionalUnitConverter *CLogASCIIStandardFile::CProportionalUnitConverter::Clone() const {
  return new CProportionalUnitConverter(*this);
}

geo::CValue CLogASCIIStandardFile::CProportionalUnitConverter::Convert(double dSourceVal) const {
  return geo::CValue(dSourceVal * Factor());
}

///// CLogASCIIStandardFile::CInverseUnitConverter

CLogASCIIStandardFile::CInverseUnitConverter::CInverseUnitConverter(double dFactor) : CUnitConverter(dFactor) {}

CLogASCIIStandardFile::CInverseUnitConverter::CInverseUnitConverter(const CInverseUnitConverter &rhs)
    : CUnitConverter(rhs) {}

CLogASCIIStandardFile::CInverseUnitConverter *CLogASCIIStandardFile::CInverseUnitConverter::Clone() const {
  return new CInverseUnitConverter(*this);
}

geo::CValue CLogASCIIStandardFile::CInverseUnitConverter::Convert(double dSourceVal) const {
  if (fabs(dSourceVal) < EPS)
    return geo::CValue();

  return geo::CValue(Factor() / dSourceVal);
}

///// CLogASCIIStandardFile::CColumnDef

CLogASCIIStandardFile::CColumnDef::CColumnDef(unsigned int uiValueType, unsigned int uiValueNameID,
                                              const QString &strMnemonic, const CUnitConverter *pUnitConverter)
    : m_uiValueType(uiValueType), m_strMnemonic(strMnemonic), m_pUnitConverter(pUnitConverter) {
  m_strValueName = getStringTableEntry(uiValueNameID);
}

CLogASCIIStandardFile::CColumnDef::CColumnDef(const CColumnDef &rhs)
    : m_uiValueType(rhs.m_uiValueType), m_strValueName(rhs.m_strValueName), m_strMnemonic(rhs.m_strMnemonic),
      m_pUnitConverter(rhs.m_pUnitConverter != 0 ? rhs.m_pUnitConverter->Clone() : 0) {}

CLogASCIIStandardFile::CColumnDef::~CColumnDef() {
  if (m_pUnitConverter)
    delete m_pUnitConverter;
}

unsigned int CLogASCIIStandardFile::CColumnDef::ValueTypeID() const { return m_uiValueType; }

const QString &CLogASCIIStandardFile::CColumnDef::ValueName() const { return m_strValueName; }

const QString &CLogASCIIStandardFile::CColumnDef::Mnemonic() const { return m_strMnemonic; }

geo::CValue CLogASCIIStandardFile::CColumnDef::ConvertValue(double dSourceVal) const {
  if (!m_pUnitConverter)
    return geo::CValue();

  return m_pUnitConverter->Convert(dSourceVal);
}

///// CLogASCIIStandardFile

CLogASCIIStandardFile::CLogASCIIStandardFile(const CNewWellPath &well)
    : m_pNewWellPath(&well), m_pFile(0), m_delimiter(' '), m_pPointSet(0) {}

CLogASCIIStandardFile::~CLogASCIIStandardFile() { Close(); }

bool CLogASCIIStandardFile::Read(const QString &strFileName) {
  assert(m_pFile == 0); // already open?

  m_pFile = fopen(strFileName.toStdString().c_str(), "r");
  if (!m_pFile)
    return false;

  bool bRet = true;

  try {
    Parse();
  } catch (CException &e) {
    m_strError = e.Error();
    bRet = false;
    if (m_pPointSet)
      delete m_pPointSet;
  }

  Close();

  return bRet;
}

void CLogASCIIStandardFile::Close() {
  if (m_pFile) {
    fclose(m_pFile);
    m_pFile = 0;
  }
}

const QString &CLogASCIIStandardFile::Error() const { return m_strError; }

const geo::CValue &CLogASCIIStandardFile::NullValue() const { return m_NullValue; }

char CLogASCIIStandardFile::Delimiter() const { return m_delimiter; }

int CLogASCIIStandardFile::ColumnSize() const { return m_vcColumns.size(); }

const CLogASCIIStandardFile::CColumnDef &CLogASCIIStandardFile::ColumnDef(int nColumn) const {
  assert(nColumn >= 0 && nColumn < m_vcColumns.size());
  return m_vcColumns[nColumn];
}

void CLogASCIIStandardFile::Parse() {
  if (!m_pFile)
    throw CException("Input file not open");

  m_nLine = 0;
  m_section = SEC_NONE;
  m_mpValueTypes.clear();
  m_vcColumns.clear();
  m_strError.clear();
  m_pPointSet = 0;
  m_iDepthColumn = -1;

  char buf[BS];
  QString strLine;
  bool bAppend = false;

  while (fgets(buf, BS, m_pFile)) {
    // replace \n and \r characters
    char *q = strchr(buf, '\n');
    if (q)
      *q = 0;
    char *r = strchr(buf, '\r');
    if (r)
      *r = 0;

    if (bAppend)
      strLine += buf;
    else
      strLine = buf;

    if (!q && !r) // no EOL character found, continue reading and append
    {
      bAppend = true;
      continue;
    }

    // EOL found, unset append mode and parse the line
    bAppend = false;

    ++m_nLine;

    const std::string stdline = strLine.toStdString();
    const char *line = stdline.c_str();
    const char *p = line;

    // skip leading whitespace
    p = SkipWhiteSpace(line);

    if (!IsComment(p) && *p != 0) // no comment or empty line
    {
      if (IsSection(p)) {
        NewSection(p);
      } else {
        switch (m_section) {
        case SEC_VERSION:
          ReadVersionData(p);
          break;
        case SEC_WELLINFO:
          ReadWellInfoData(p);
          break;
        case SEC_CURVEINFO:
          ReadCurveInfoData(p);
          break;
        case SEC_CURVE:
          ReadCurveData(p);
          break;
        case SEC_NONE:
        case SEC_UNKNOWN:
          break;
        }
      }
    }
  }
}

bool CLogASCIIStandardFile::IsComment(const char *text) const { return (text != 0 && *text == '#'); }

bool CLogASCIIStandardFile::IsSection(const char *text) const { return (text != 0 && *text == '~'); }

void CLogASCIIStandardFile::NewSection(const char *text) {
  if (text == 0)
    throw CException("NULL text encountered on line %1", m_nLine);

  const char *p = text;
  if (*p == '~')
    ++p;

  QString s = p;
  s = trimRight(s);

  if (s.left(7).compare("VERSION", Qt::CaseInsensitive) == 0)
    m_section = SEC_VERSION;
  else if (s.left(4).compare("WELL", Qt::CaseInsensitive) == 0)
    m_section = SEC_WELLINFO;
  else if (s.left(5).compare("CURVE", Qt::CaseInsensitive) == 0 &&
           s.right(11).compare("INFORMATION", Qt::CaseInsensitive) == 0)
    m_section = SEC_CURVEINFO;
  else if (s.left(5).compare("ASCII", Qt::CaseInsensitive) == 0 || s.left(5).compare("CURVE", Qt::CaseInsensitive) == 0)
    m_section = SEC_CURVE;
  else
    m_section = SEC_UNKNOWN;
}

void CLogASCIIStandardFile::ReadVersionData(const char *text) {
  if (!text)
    throw CException("NULL text encountered on line %1", m_nLine);

  CLasDataLine lasData(text, m_nLine);

  if (lasData.Mnemonic() == "VERS") {
    if (!lasData.Unit().isEmpty())
      throw CException("Line %1: VERS mnemonic should not have a unit", m_nLine);

    // check for correct version (must >= 3.0)
    double dVersion = lasData.Data().toDouble();
    if (dVersion < 3.0)
      throw CException("Line %1: only LAS version 3.0 or higher is supported", m_nLine);
  } else if (lasData.Mnemonic() == "DLM") {
    if (!lasData.Unit().isEmpty())
      throw CException("Line %1: DLM mnemonic should not have a unit", m_nLine);

    if (lasData.Data() == "SPACE")
      m_delimiter = ' ';
    else if (lasData.Data() == "COMMA")
      m_delimiter = ',';
    else if (lasData.Data() == "TAB")
      m_delimiter = '\t';
    else {
      QString s;
      s = QString("Line %1: unknown delimiter '%2' encountered").arg(m_nLine).arg(lasData.Data());
      throw CException(s);
    }
  }
}

void CLogASCIIStandardFile::ReadWellInfoData(const char *text) {
  if (!text)
    throw CException("NULL text encountered on line %1", m_nLine);

  CLasDataLine lasData(text, m_nLine);

  if (lasData.Mnemonic() == "NULL") {
    if (!lasData.Unit().isEmpty())
      throw CException("Line %1: NULL mnemonic should not have a unit", m_nLine);

    m_NullValue = lasData.Data().toDouble();
  } else if (lasData.Mnemonic() == "WELL") {
    if (!lasData.Unit().isEmpty())
      throw CException("Line %1: WELL mnemonic should not have a unit", m_nLine);

    m_strLasWellName = lasData.Data();
  }
}

void CLogASCIIStandardFile::ReadCurveInfoData(const char *text) {
  if (!text)
    throw CException("NULL text encountered on line %1", m_nLine);

  CLasDataLine lasData(text, m_nLine);
  CUnitConverter *pUnitConverter = 0;

  if (lasData.Mnemonic().compare("DEPT", Qt::CaseInsensitive) == 0 ||
      lasData.Mnemonic().compare("DEPTH", Qt::CaseInsensitive) == 0 ||
      lasData.Mnemonic().compare("MD", Qt::CaseInsensitive) == 0 ||
      lasData.Mnemonic().compare("AHD", Qt::CaseInsensitive) == 0 ||
      lasData.Mnemonic().compare("AHDKB", Qt::CaseInsensitive) == 0) // depth
  {
    if (lasData.Unit().compare("M", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.);
    else if (lasData.Unit().compare("FT", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("F", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1. / FF_FACTOR_LENGTH);
    else
      ThrowUnrecognizedUnitException(lasData, m_nLine);

    RegisterValueType(IDT_VALUETYPE_DEPTH, IDS_VALUENAME_DEPTH, lasData, m_nLine, pUnitConverter);
  } else if (lasData.Mnemonic().compare("VP", Qt::CaseInsensitive) == 0) // acoustic velocity (Vp)
  {
    if (lasData.Unit().compare("M/S", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.);
    else if (lasData.Unit().compare("FT/S", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("F/S", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1. / FF_FACTOR_LENGTH);
    else if (lasData.Unit().compare("M/US", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e6);
    else if (lasData.Unit().compare("FT/US", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("F/US", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e6 / FF_FACTOR_LENGTH);
    RegisterValueType(IDT_VALUETYPE_VELOCITYP, IDS_VALUENAME_VELOCITYP, lasData, m_nLine, pUnitConverter);
  } else if (lasData.Mnemonic().compare("DT", Qt::CaseInsensitive) == 0 ||
             lasData.Mnemonic().compare("SONIC", Qt::CaseInsensitive) == 0) // transit time (1/Vp)
  {
    if (lasData.Unit().compare("S/M", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CInverseUnitConverter(1.);
    else if (lasData.Unit().compare("S/FT", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("S/F", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CInverseUnitConverter(1. / FF_FACTOR_LENGTH);
    else if (lasData.Unit().compare("US/M", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CInverseUnitConverter(1.e6);
    else if (lasData.Unit().compare("US/FT", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("US/F", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CInverseUnitConverter(1.e6 / FF_FACTOR_LENGTH);
    RegisterValueType(IDT_VALUETYPE_VELOCITYP, IDS_VALUENAME_VELOCITYP, lasData, m_nLine, pUnitConverter);
  } else if (lasData.Mnemonic().compare("RHOB", Qt::CaseInsensitive) == 0 ||
             lasData.Mnemonic().compare("DENSITY", Qt::CaseInsensitive) == 0 ||
             lasData.Mnemonic().compare("DEN", Qt::CaseInsensitive) == 0) // density
  {
    if (lasData.Unit().compare("K/M3", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.);
    else if (lasData.Unit().compare("KG/M3", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.);
    else if (lasData.Unit().compare("K/CM3", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e6);
    else if (lasData.Unit().compare("KG/CM3", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e6);
    else if (lasData.Unit().compare("G/CM3", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("G/C3", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e3);
    else if (lasData.Unit().compare("G/M3", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e-3);
    else if (lasData.Unit().compare("PPG", Qt::CaseInsensitive) == 0) // pounds per gallon
      pUnitConverter = new CProportionalUnitConverter(1. / FF_FACTOR_DENSITY);
    RegisterValueType(IDT_VALUETYPE_RHOB, IDS_VALUENAME_DENSITY, lasData, m_nLine, pUnitConverter);
  } else if (lasData.Mnemonic().compare("NPHI", Qt::CaseInsensitive) == 0 ||
             lasData.Mnemonic().compare("POR_MID", Qt::CaseInsensitive) == 0 ||
             lasData.Mnemonic().compare("PORNET", Qt::CaseInsensitive) == 0) // porosity
  {
    RegisterValueType(IDT_VALUETYPE_POROSITY, IDS_VALUENAME_POROSITY, lasData, m_nLine,
                      new CProportionalUnitConverter(1.));
  } else if (lasData.Mnemonic().compare("YME", Qt::CaseInsensitive) == 0) // Young's Modulus
  {
    if (lasData.Unit().compare("MPA", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.);
    else if (lasData.Unit().compare("KPA", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e-3);
    else if (lasData.Unit().compare("PA", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e-6);
    else if (lasData.Unit().compare("PSI", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1. / FF_FACTOR_STRESS);
    RegisterValueType(IDT_VALUETYPE_YOUNGS_MODULUS, IDS_VALUENAME_YOUNGS_MODULUS, lasData, m_nLine, pUnitConverter);
  } else if (lasData.Mnemonic().compare("VS", Qt::CaseInsensitive) == 0) // Vs
  {
    if (lasData.Unit().compare("M/S", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.);
    else if (lasData.Unit().compare("FT/S", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("F/S", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1. / FF_FACTOR_LENGTH);
    else if (lasData.Unit().compare("M/US", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e6);
    else if (lasData.Unit().compare("FT/US", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("F/US", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CProportionalUnitConverter(1.e6 / FF_FACTOR_LENGTH);
    RegisterValueType(IDT_VALUETYPE_VELOCITYS, IDS_VALUENAME_VELOCITYS, lasData, m_nLine, pUnitConverter);
  } else if (lasData.Mnemonic().compare("SHEARSONIC", Qt::CaseInsensitive) == 0 ||
             lasData.Mnemonic().compare("DTS", Qt::CaseInsensitive) == 0 ||
             lasData.Mnemonic().compare("DTS_BTC", Qt::CaseInsensitive) == 0) // 1/Vs
  {
    if (lasData.Unit().compare("S/M", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CInverseUnitConverter(1.);
    else if (lasData.Unit().compare("S/FT", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("S/F", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CInverseUnitConverter(1. / FF_FACTOR_LENGTH);
    else if (lasData.Unit().compare("US/M", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CInverseUnitConverter(1.e6);
    else if (lasData.Unit().compare("US/FT", Qt::CaseInsensitive) == 0 ||
             lasData.Unit().compare("US/F", Qt::CaseInsensitive) == 0)
      pUnitConverter = new CInverseUnitConverter(1.e6 / FF_FACTOR_LENGTH);
    RegisterValueType(IDT_VALUETYPE_VELOCITYS, IDS_VALUENAME_VELOCITYS, lasData, m_nLine, pUnitConverter);
  } else // ignore column
  {
    RegisterValueType(0, 0, lasData, m_nLine, new CProportionalUnitConverter(1.0));
  }
}

void CLogASCIIStandardFile::ReadCurveData(const char *text) {
  assert(m_pNewWellPath != 0); // exactly one

  if (m_vcColumns.empty())
    throw CException("Line %1: curve data encountered without CURVE INFORMATION", m_nLine);

  if (m_pPointSet == 0) {
    int i;
    for (i = 0; i < m_vcColumns.size(); ++i) {
      if (m_vcColumns[i].ValueTypeID() == IDT_VALUETYPE_DEPTH) {
        m_iDepthColumn = i;
        break;
      }
    }

    if (m_iDepthColumn == -1) // not found, but we need it
      throw CException("Line %1: no depth column found. A depth column is required.", m_nLine);

    QString strPointSetName;
    {
      strPointSetName = QString("LAS '%1' on well '%2'").arg(m_strLasWellName).arg(m_pNewWellPath->Name());
      m_pPointSet = new CLasPointSet(*m_pNewWellPath, strPointSetName, m_vcColumns.size() - 1, IPointSet::DIM_3D);
    }

    const CValueTypeFactory *vtf = CValueTypeFactory::instance();

    int offset = 0;
    for (i = 0; i < m_vcColumns.size(); ++i) {
      if (m_vcColumns[i].ValueTypeID() == IDT_VALUETYPE_DEPTH)
        offset = 1;

      if (m_vcColumns[i].ValueTypeID() != IDT_VALUETYPE_DEPTH) {
        if (m_vcColumns[i].ValueTypeID()) {
          m_pPointSet->NodalValueSet(3 + i - offset).Name(m_vcColumns[i].ValueName());
          CValueType *pvt = vtf->BuildValueType(*m_pPointSet, m_vcColumns[i].ValueTypeID(), m_vcColumns[i].ValueName());
          pvt->Component().LinkTo(m_pPointSet->NodalValueSet(3 + i - offset));
        } else {
          m_pPointSet->NodalValueSet(3 + i - offset).Name(m_vcColumns[i].Mnemonic());
        }
      }
    }
  }

  CCurveDataLine curveLine(text, m_nLine, *this);

  // the TMD of this data
  const geo::CValue &valDepth = curveLine.Value(m_iDepthColumn);
  if (!valDepth.Valid())
    throw CException("Line %1: a NULL value for the depth is not allowed", m_nLine);

  {
    double dTMD = valDepth.Value();

    assert(m_pNewWellPath->WellPointSize() > 0);

    CNewWellPoint pLast = m_pNewWellPath->WellPoint(m_pNewWellPath->WellPointSize() - 1);

    if (dTMD <= pLast.TMD()) {
      // make a point out of it
      CNewWellPoint pt(*m_pNewWellPath, dTMD);
      assert(!pt.Empty());
      assert(pt.Defined());

      std::vector<geo::CValue> vcRow(m_vcColumns.size() + 2);
      vcRow[0] = pt.X();
      vcRow[1] = pt.Y();
      vcRow[2] = pt.Z();
      int iCol = 2;

      int i;
      for (i = 0; i < m_vcColumns.size(); ++i) {
        if (i != m_iDepthColumn)
          vcRow[++iCol] = curveLine.Value(i);
      }

      m_pPointSet->PushBack(vcRow);
    }
  }
}

const char *CLogASCIIStandardFile::SkipWhiteSpace(const char *text) const {
  const char *p = text;
  while (p && (*p == ' ' || *p == '\t'))
    ++p;

  return p;
}

void CLogASCIIStandardFile::RegisterValueType(unsigned int uiValueType, unsigned int uiValueNameID,
                                              const CLasDataLine &lasData, int nLineNr,
                                              CUnitConverter *pUnitConverter) {
  if (uiValueType != 0) {
    std::pair<TValueTypeMap::iterator, bool> prInsert =
        m_mpValueTypes.insert(TValueTypeMap::value_type(uiValueType, std::make_pair(lasData.Mnemonic(), nLineNr)));
    if (!prInsert.second) // already defined, throw error
    {
      if (pUnitConverter)
        delete pUnitConverter;
      QString msg;
      msg = QString("Line %1: '%2' is already defined through '%3' (line %4)")
                .arg(m_nLine)
                .arg(lasData.Mnemonic())
                .arg(prInsert.first->second.first)
                .arg(prInsert.first->second.second);
      throw CException(msg);
    }
  }

  // add it to the columns vector
  m_vcColumns.push_back(CColumnDef(uiValueType, uiValueNameID, lasData.Mnemonic(), pUnitConverter));
}

void CLogASCIIStandardFile::ThrowUnrecognizedUnitException(const CLasDataLine &lasData, int nLineNr) const {
  QString msg;
  msg = QString("Line %1: unrecognized unit '%2' for mnemonic '%3'")
            .arg(nLineNr)
            .arg(lasData.Unit())
            .arg(lasData.Mnemonic());
  throw CException(msg);
}
