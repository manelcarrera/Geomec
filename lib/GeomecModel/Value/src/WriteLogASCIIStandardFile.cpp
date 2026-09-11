#include <QString>
#include <QFile>
#include <QTextStream>

#include "WriteLogASCIIStandardFile.h"
#include "NewWellPath.h"

namespace
{

const QString TILDE = "~";

// Section Titles

const QString SECTION_VERSION = "Version";
const QString SECTION_WELL = "Well";
const QString SECTION_CURVE = "Curve";
const QString SECTION_PARAMETER = "Parameter";
const QString SECTION_ASCII = "ASCII";
const QString SECTION_LOG_PARAMETER = "Log_Parameter";
const QString SECTION_CORE_PARAMETER = "Core_Parameter";
const QString SECTION_INCLINOMETRY_PARAMETER = "Inclinometry_Parameter";
const QString SECTION_DRILLING_PARAMETER = "Drilling_Parameter";
const QString SECTION_TOPS_PARAMETER = "Tops_Parameter";
const QString SECTION_TEST_PARAMETER = "Test_Parameter";
const QString SECTION_LOG_DEFINITION = "Log_Definition";
const QString SECTION_CORE_DEFINITION = "Core_Definition";
const QString SECTION_INCLINOMETRY_DEFINITION = "Inclinometry_Definition";
const QString SECTION_DRILLING_DEFINITION = "Drilling_Definition";
const QString SECTION_TOPS_DEFINITION = "Tops_Definition";
const QString SECTION_TEST_DEFINITION = "Test_Definition";
const QString SECTION_LOG_DATA = "Log_Data";
const QString SECTION_CORE_DATA = "Core_Data";
const QString SECTION_INCLINOMETRY_DATA = "Inclinometry_Data";
const QString SECTION_DRILLING_DATA = "Drilling_Data";
const QString SECTION_TOPS_DATA = "Tops_Data";
const QString SECTION_TEST_DATA = "Test_Data";

// Parameter Data Section and Column Definition Channel Mnemonics

const QString MNEMONIC_VERSION_VERS = "VERS";
const QString MNEMONIC_VERSION_WRAP = "WRAP";
const QString MNEMONIC_VERSION_DLM = "DLM";

const QString MNEMONIC_WELL_STRT = "STRT";
const QString MNEMONIC_WELL_STOP = "STOP";
const QString MNEMONIC_WELL_STEP = "STEP";
const QString MNEMONIC_WELL_NULL = "NULL";
const QString MNEMONIC_WELL_COMP = "COMP";
const QString MNEMONIC_WELL_WELL = "WELL";
const QString MNEMONIC_WELL_FLD = "FLD";
const QString MNEMONIC_WELL_LOC = "LOC";
const QString MNEMONIC_WELL_STAT = "STAT";
const QString MNEMONIC_WELL_PROV = "PROV";
const QString MNEMONIC_WELL_CTRY = "CTRY";
const QString MNEMONIC_WELL_CNTY = "CNTY";
const QString MNEMONIC_WELL_API = "API";
const QString MNEMONIC_WELL_UWI = "UWI";
const QString MNEMONIC_WELL_LIC = "LIC";
const QString MNEMONIC_WELL_SRVC = "SRVC";
const QString MNEMONIC_WELL_DATE = "DATE";
const QString MNEMONIC_WELL_X = "X";
const QString MNEMONIC_WELL_Y = "Y";
const QString MNEMONIC_WELL_LATI = "LATI";
const QString MNEMONIC_WELL_LONG = "LONG";
const QString MNEMONIC_WELL_GDAT = "GDAT";
const QString MNEMONIC_WELL_HZCS = "HZCS";

const QString MNEMONIC_PARAMETER_RUN = "RUN";
const QString MNEMONIC_PARAMETER_APD = "APD";
const QString MNEMONIC_PARAMETER_DREF = "DREF";
const QString MNEMONIC_PARAMETER_EREF = "EREF";
const QString MNEMONIC_PARAMETER_PDAT = "PDAT";
const QString MNEMONIC_PARAMETER_RUNS = "RUNS";
const QString MNEMONIC_PARAMETER_RUN_DEPTH = "RUN_DEPTH";
const QString MNEMONIC_PARAMETER_RUN_DATE = "RUN_DATE";
const QString MNEMONIC_PARAMETER_NMAT_DEPTH = "NMAT_DEPTH";
const QString MNEMONIC_PARAMETER_DMAT_DEPTH = "DMAT_DEPTH";
const QString MNEMONIC_PARAMETER_SMAT_DEPTH = "SMAT_DEPTH";
const QString MNEMONIC_PARAMETER_MATR = "MATR";
const QString MNEMONIC_PARAMETER_MDEN = "MDEN";
const QString MNEMONIC_PARAMETER_DTMA = "DTMA";
const QString MNEMONIC_PARAMETER_FR_LR = "FR_LR";

const QString MNEMONIC_LOG_PARAMETER_RUN = "RUN";
const QString MNEMONIC_LOG_PARAMETER_APD = "APD";
const QString MNEMONIC_LOG_PARAMETER_DREF = "DREF";
const QString MNEMONIC_LOG_PARAMETER_EREF = "EREF";
const QString MNEMONIC_LOG_PARAMETER_PDAT = "PDAT";
const QString MNEMONIC_LOG_PARAMETER_RUNS = "RUNS";
const QString MNEMONIC_LOG_PARAMETER_RUN_DEPTH = "RUN_DEPTH";
const QString MNEMONIC_LOG_PARAMETER_RUN_DATE = "RUN_DATE";
const QString MNEMONIC_LOG_PARAMETER_NMAT_DEPTH = "NMAT_DEPTH";
const QString MNEMONIC_LOG_PARAMETER_DMAT_DEPTH = "DMAT_DEPTH";
const QString MNEMONIC_LOG_PARAMETER_SMAT_DEPTH = "SMAT_DEPTH";
const QString MNEMONIC_LOG_PARAMETER_MATR = "MATR";
const QString MNEMONIC_LOG_PARAMETER_MDEN = "MDEN";
const QString MNEMONIC_LOG_PARAMETER_DTMA = "DTMA";
const QString MNEMONIC_LOG_PARAMETER_FR_LR = "FR_LR";

const QString MNEMONIC_CORE_PARAMETER_C_SRS = "C_SRS";
const QString MNEMONIC_CORE_PARAMETER_C_TY = "C_TY";
const QString MNEMONIC_CORE_PARAMETER_C_DATE = "C_DATE";
const QString MNEMONIC_CORE_PARAMETER_C_TOP = "C_TOP";
const QString MNEMONIC_CORE_PARAMETER_C_BS = "C_BS";
const QString MNEMONIC_CORE_PARAMETER_C_RC = "C_RC";
const QString MNEMONIC_CORE_PARAMETER_C_FM = "C_FM";
const QString MNEMONIC_CORE_PARAMETER_C_DI = "C_DI";
const QString MNEMONIC_CORE_PARAMETER_C_AC = "C_AC";
const QString MNEMONIC_CORE_PARAMETER_C_AD = "C_AD";

const QString MNEMONIC_CORE_DEFINITION_CORT = "CORT";
const QString MNEMONIC_CORE_DEFINITION_CORB = "CORB";
const QString MNEMONIC_CORE_DEFINITION_CDES = "CDES";

const QString MNEMONIC_DRILLING_DEFINITION_DDEP = "DDEP";
const QString MNEMONIC_DRILLING_DEFINITION_DIST = "DIST";
const QString MNEMONIC_DRILLING_DEFINITION_HRS = "HRS";
const QString MNEMONIC_DRILLING_DEFINITION_ROP = "ROP";
const QString MNEMONIC_DRILLING_DEFINITION_WOB = "WOB";
const QString MNEMONIC_DRILLING_DEFINITION_RPM = "RPM";
const QString MNEMONIC_DRILLING_DEFINITION_TQ = "TQ";
const QString MNEMONIC_DRILLING_DEFINITION_PUMP = "PUMP";
const QString MNEMONIC_DRILLING_DEFINITION_TSPM = "TSPM";
const QString MNEMONIC_DRILLING_DEFINITION_GPM = "GPM";
const QString MNEMONIC_DRILLING_DEFINITION_ECD = "ECD";
const QString MNEMONIC_DRILLING_DEFINITION_TBR = "TBR";
const QString MNEMONIC_DRILLING_DEFINITION_RIG = "RIG";
const QString MNEMONIC_DRILLING_DEFINITION_CONTR = "CONTR";

const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_DT = "I_DT";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_CO = "I_CO";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_RF = "I_RF";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_AT = "I_AT";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_DC = "I_DC";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_KD = "I_KD";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_GD = "I_GD";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_ONS = "I_ONS";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_OEW = "I_OEW";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_CP = "I_CP";
const QString MNEMONIC_INCLINOMETRY_PARAMETER_I_CS = "I_CS";

const QString MNEMONIC_INCLINOMETRY_DEFINITION_MD = "MD";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_TVD = "TVD";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_AZIM = "AZIM";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_DEVI = "DEVI";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_RB = "RB";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_NSDR = "NSDR";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_EWDR = "EWDR";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_CLSR = "CLSR";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_TIEMD = "TIEMD";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_TIETVD = "TIETVD";
const QString MNEMONIC_INCLINOMETRY_DEFINITION_TIEDEVI = "TIEDEVI";

const QString MNEMONIC_TOPS_DEFINITION_TOPT = "TOPT";
const QString MNEMONIC_TOPS_DEFINITION_TOPB = "TOPB";
const QString MNEMONIC_TOPS_DEFINITION_TOPN = "TOPN";
const QString MNEMONIC_TOPS_DEFINITION_TOPSRC = "TOPSRC";
const QString MNEMONIC_TOPS_DEFINITION_TOPDR = "TOPDR";

const QString MNEMONIC_TEST_DEFINITION_TSTT = "TSTT";
const QString MNEMONIC_TEST_DEFINITION_TSTB = "TSTB";
const QString MNEMONIC_TEST_DEFINITION_TSTN = "TSTN";
const QString MNEMONIC_TEST_DEFINITION_DDES = "DDES";
const QString MNEMONIC_TEST_DEFINITION_ISIP = "ISIP";
const QString MNEMONIC_TEST_DEFINITION_FSIP = "FSIP";
const QString MNEMONIC_TEST_DEFINITION_RATE = "RATE";
const QString MNEMONIC_TEST_DEFINITION_BLOWD = "BLOWD";
const QString MNEMONIC_TEST_DEFINITION_TESTT = "TESTT";

//

const QString LAS_VERSION = "3.0";
const QString POINT = ".";
const QString WRAP = "NO";
const QString DELIMITING_CHARACTER = "SPACE";
const QString COMMENT_CHARACTER = ":";
const QString NULL_VALUE = "-99999";
const QString EMPTY_VALUE = "";
const QString FORMAT = "%1";
const QString HEADER_PREFIX = "Geomec";
const QString SPACE = " ";

const int WIDTH_NAME_FIELD = 20;
const int WIDTH_UNIT_FIELD = 10;
const int WIDTH_VAlUE_FIELD = 15;
const int WIDTH_OTHER_FIELDS = 0;
const int ROW_HEADER = 0;
const int COLUMN_DEPTH = 0;
const int PRE_DEFINED_COLUMNS = 7;
const int LENGTH_COLUMNS_0_4 = 5;
const int ANGLE_COLUMNS_5_6 = PRE_DEFINED_COLUMNS;

void streamLine(QTextStream& lasStream, const QString& name,
  const QString& unit, const QString& value,
  const QString& format = EMPTY_VALUE)
{
  lasStream << left << qSetFieldWidth(WIDTH_NAME_FIELD) <<
    name << qSetFieldWidth(WIDTH_UNIT_FIELD) << (POINT + unit) <<
    qSetFieldWidth(WIDTH_VAlUE_FIELD) << value <<
    qSetFieldWidth(WIDTH_OTHER_FIELDS) << COMMENT_CHARACTER << SPACE <<
    format << endl;
}

void writeSectionVersion(QFile& lasFile)
{
  QTextStream lasStream(&lasFile);

  lasStream << TILDE << SECTION_VERSION << endl;

  streamLine(lasStream, MNEMONIC_VERSION_VERS, EMPTY_VALUE, LAS_VERSION);
  streamLine(lasStream, MNEMONIC_VERSION_WRAP, EMPTY_VALUE, WRAP);
  streamLine(lasStream, MNEMONIC_VERSION_DLM, EMPTY_VALUE,
    DELIMITING_CHARACTER);
}

const QString ILLEGAL_CHARACTERS = "[.: \t{}\\[\\]]";

QString validateString(const QString& s)
{
  QString v = s;

  v.remove(QRegExp(ILLEGAL_CHARACTERS));

  return v;
}

const QString REGULAR_EXPRESSION_SCIENTIFIC = "[-+]?\\d+(.\\d+)?[eE][-+]?\\d+";
const QRegExp IS_SCIENTIFIC_NOTATION(REGULAR_EXPRESSION_SCIENTIFIC);

QString getValueString(const CNodalExportFormat& nodalExportFormat,
  int row, int column)
{
  switch (nodalExportFormat.OutputType(row, column))
  {
    case CNodalExportFormat::OT_DOUBLE:
      {
        double value = nodalExportFormat.DoubleAt(row, column);
        QString valueString = FORMAT.arg(value);

        if (IS_SCIENTIFIC_NOTATION.indexIn(valueString) >= 0)
        {
          return QString::number(value, 'E', 5);
        }
        else
        {
          return valueString;
        }
      }
    case CNodalExportFormat::OT_INTEGER:
      return FORMAT.arg(nodalExportFormat.IntegerAt(row, column));
    case CNodalExportFormat::OT_STRING:
      {
        QString string =
          validateString(nodalExportFormat.StringAt(row, column));

        if (string == getStringTableEntry(IDS_ET_NAN))
        {
          return NULL_VALUE;
        }
        else
        {
          return string;
        }
      }
    default:
      assert(false);
      return EMPTY_VALUE;
  }
}

const QString FORMAT_DOUBLE = "{F}";
const QString FORMAT_INTEGER = "{I}";
const QString FORMAT_STRING = "{S}";
const QString FORMAT_SCIENTIFIC = "{E0.00000E+00}";

QString getFormatString(const CNodalExportFormat& nodalExportFormat,
  int row, int column)
{
  switch (nodalExportFormat.OutputType(row, column))
  {
    case CNodalExportFormat::OT_DOUBLE:
      {
        QString valueString = getValueString(nodalExportFormat, row, column);

        if (IS_SCIENTIFIC_NOTATION.indexIn(valueString) >= 0)
        {
          return FORMAT_SCIENTIFIC;
        }
        else
        {
          return FORMAT_DOUBLE;
        }
      }
    case CNodalExportFormat::OT_INTEGER:
      return FORMAT_INTEGER;
    case CNodalExportFormat::OT_STRING:
      {
        QString string =
          validateString(nodalExportFormat.StringAt(row, column));

        if (string == getStringTableEntry(IDS_ET_NAN))
        {
          return FORMAT_DOUBLE;
        }
        else
        {
          return FORMAT_STRING;
        }
      }
    default:
      assert(false);
      return EMPTY_VALUE;
  }
}

double calculateSTEP(const CNodalExportFormat& nodalExportFormat)
{
  assert(nodalExportFormat.RowSize() > (ROW_HEADER + 1));

  double begin = nodalExportFormat.DoubleAt(ROW_HEADER + 1, COLUMN_DEPTH);
  double step = 0;

  for (int r = ROW_HEADER + 2; r < nodalExportFormat.RowSize(); ++r)
  {
    double end = nodalExportFormat.DoubleAt(r, COLUMN_DEPTH);

    if (step != 0)
    {
      if ((end - begin) != step)
      {
        return 0;
      }
    }
    else
    {
      step = end - begin;
    }

    begin = end;
  }

  return step;
}

QString getUnitName(const CNodalExportFormat& nodalExportFormat, int column)
{
  QString unitName;

  if (column < LENGTH_COLUMNS_0_4)
  {
    CLengthQuantity lengthQuantity;

    unitName = lengthQuantity.UnitName(nodalExportFormat.Unit()).c_str();
  }
  else if (column < ANGLE_COLUMNS_5_6)
  {
    CAngleQuantity angleQuantity;

    unitName = angleQuantity.UnitName(nodalExportFormat.Unit()).c_str();
  }
  else
  {
    unitName = nodalExportFormat.DataValue(column - PRE_DEFINED_COLUMNS).
      UnitName(nodalExportFormat.Unit());
  }

  return unitName;
}

void writeSectionWell(QFile& lasFile,
  const CNodalExportFormat& nodalExportFormat)
{
  // force the initialization of CNodalExportFormat::m_iteratorWellPath
  // before the first two 'streamLine' statements are executed

  getValueString(nodalExportFormat, ROW_HEADER, COLUMN_DEPTH);

  QTextStream lasStream(&lasFile);

  lasStream << TILDE << SECTION_WELL << endl;

  streamLine(lasStream, MNEMONIC_WELL_STRT,
    getUnitName(nodalExportFormat, COLUMN_DEPTH),
    getValueString(nodalExportFormat, ROW_HEADER + 1, COLUMN_DEPTH));
  streamLine(lasStream, MNEMONIC_WELL_STOP,
    getUnitName(nodalExportFormat, COLUMN_DEPTH),
    getValueString(nodalExportFormat, nodalExportFormat.RowSize() - 1,
      COLUMN_DEPTH));
  streamLine(lasStream, MNEMONIC_WELL_STEP,
    getUnitName(nodalExportFormat, COLUMN_DEPTH),
    FORMAT.arg(calculateSTEP(nodalExportFormat)));
  streamLine(lasStream, MNEMONIC_WELL_NULL, EMPTY_VALUE, NULL_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_COMP, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_WELL, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_FLD, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_LOC, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_STAT, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_PROV, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_CTRY, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_CNTY, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_API, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_UWI, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_LIC, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_SRVC, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_DATE, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_X, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_Y, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_LATI, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_LONG, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_GDAT, EMPTY_VALUE, EMPTY_VALUE);
  streamLine(lasStream, MNEMONIC_WELL_HZCS, EMPTY_VALUE, EMPTY_VALUE);
}

void writeSectionParameter(QFile& lasFile,
  const CNodalExportFormat& nodalExportFormat)
{
  QTextStream lasStream(&lasFile);

  lasStream << TILDE << SECTION_PARAMETER << endl;

  for (int c = 0; c < nodalExportFormat.ColumnSize(); ++c)
  {
    streamLine(lasStream,
      HEADER_PREFIX + getValueString(nodalExportFormat, ROW_HEADER, c),
      EMPTY_VALUE, EMPTY_VALUE);
  }
}

// The import functionality requires SECTION_CURVE_POSTFIX, which is not
// documented in 'LAS_3_File_Structure.pdf', but poses no threat to the
// LAS 3.0 certification with 'LAS Certify'.

const QString SECTION_CURVE_POSTFIX = "Information";

void writeSectionCurve(QFile& lasFile,
  const CNodalExportFormat& nodalExportFormat)
{
  QTextStream lasStream(&lasFile);

  lasStream << TILDE << SECTION_CURVE << SPACE << SECTION_CURVE_POSTFIX << endl;

  for (int c = 0; c < nodalExportFormat.ColumnSize(); ++c)
  {
    streamLine(lasStream, getValueString(nodalExportFormat, ROW_HEADER, c),
      getUnitName(nodalExportFormat, c), EMPTY_VALUE,
      getFormatString(nodalExportFormat, ROW_HEADER + 1, c)); // actual value
  }
}

void writeSectionASCII(QFile& lasFile,
  const CNodalExportFormat& nodalExportFormat)
{
  QTextStream lasStream(&lasFile);

  lasStream << TILDE << SECTION_ASCII << endl;

  for (int r = (ROW_HEADER + 1); r < nodalExportFormat.RowSize(); ++r)
  {
    for (int c = 0; c < nodalExportFormat.ColumnSize(); ++c)
    {
      lasStream << left <<
        getValueString(nodalExportFormat, r, c) << SPACE;
    }

    lasStream << endl;
  }
}

} // anonymous namespace

CWriteLogASCIIStandardFile::CWriteLogASCIIStandardFile(
  const CNewWellPath* newWellPath)
: m_newWellPath(newWellPath)
{
}

void CWriteLogASCIIStandardFile::writeLogASCIIStandardFile(
  const QString& fileName, const IExportFormat::TDataVec& dataVector,
  const IExportFormat::CExportArg& arguments)
{
  QFile lasFile(fileName);

  if (!lasFile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    return;
  }

  CNodalExportFormat
    nodalExportFormat(*m_newWellPath, dataVector, arguments, true);

  writeSectionVersion(lasFile);
  writeSectionWell(lasFile, nodalExportFormat);
  writeSectionParameter(lasFile, nodalExportFormat);
  writeSectionCurve(lasFile, nodalExportFormat);
  writeSectionASCII(lasFile, nodalExportFormat);
}
