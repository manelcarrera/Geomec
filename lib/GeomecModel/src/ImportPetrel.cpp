#include "ImportPetrel.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "LengthQuantity.h"
#include "NewWellPathInput.h"
#include "Point.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QStringList>
#include <QVector>
#include <assert.h>
#include <ios>
#include <string>

#define NUMBEROFPROGRESSSTEPS 100

const size_t CImportPetrel::m_bufferSize = 1000;

CImportPetrel::CImportPetrel(const QString &strFileName, CNewWellPathInput *wellpathInput, IProgressBase &progress,
                             const CQuantity::UNIT &defaultLateralUnit, const CQuantity::UNIT &defaultDepthUnit)
    : m_cBuffer(new char[m_bufferSize + 1]), m_lineCounter(0), m_strFileName(strFileName),
      m_wellpathInput(wellpathInput), m_message(QString("Petrel Import (%1):\n").arg(m_strFileName)),
      m_stream(strFileName.toStdString().c_str(), std::ios::in),
      m_coordUnit(defaultLateralUnit == CQuantity::FIELD_UNIT ? e_field_unit : e_si_unit),
      m_depthUnit(defaultDepthUnit == CQuantity::FIELD_UNIT ? e_field_unit : e_si_unit), m_x_column(-1), m_y_column(-1),
      m_z_column(-1), m_column_count(-1), m_warning(false), m_progress(progress), m_filesize(0),
      m_currentProgressStep(0), m_onlyReadHeaders(false) {}

CImportPetrel::CImportPetrel(const QString &strFileName, IProgressBase &progress,
                             const CQuantity::UNIT &defaultLateralUnit, const CQuantity::UNIT &defaultDepthUnit)
    : m_cBuffer(new char[m_bufferSize + 1]), m_lineCounter(0), m_strFileName(strFileName), m_wellpathInput(0),
      m_message(QString("Petrel Import (%1):\n").arg(m_strFileName)),
      m_stream(strFileName.toStdString().c_str(), std::ios::in),
      m_coordUnit(defaultLateralUnit == CQuantity::FIELD_UNIT ? e_field_unit : e_si_unit),
      m_depthUnit(defaultDepthUnit == CQuantity::FIELD_UNIT ? e_field_unit : e_si_unit), m_x_column(-1), m_y_column(-1),
      m_z_column(-1), m_column_count(-1), m_warning(false), m_progress(progress), m_filesize(0),
      m_currentProgressStep(0), m_onlyReadHeaders(true) {}

CImportPetrel::~CImportPetrel() { delete[] m_cBuffer; }

bool CImportPetrel::Warning() {
  bool retval = false;

  if (m_coordUnit == e_unknown) {
    m_message += QString("- SI units used for coordinates\n");
    retval = true;
  }

  if (m_depthUnit == e_unknown) {
    m_message += QString("- SI units used for depth\n");
    retval = true;
  }

  if (m_warning)
    retval = true;

  return retval;
}

bool CImportPetrel::Import() {
  if (!m_stream) {
    m_message += QString("- Unable to open file\n");
    return false;
  }
  SetProgressSteps();

  try {
    if (!ReadHeaders())
      return false;
    if (!m_onlyReadHeaders && !ReadTable())
      return false;
  } catch (CProgressCancel *p) {
    delete p;
    m_message += QString("- Cancelled\n");
    return false;
  }

  return true;
}

CImportPetrel::t_unit CImportPetrel::getLateralUnit() const { return m_coordUnit; }

CImportPetrel::t_unit CImportPetrel::getDepthUnit() const { return m_depthUnit; }

namespace {

const QString PETREL_HEADER = QObject::tr("# WELL TRACE FROM PETREL");

} // anonymous namespace

bool CImportPetrel::hasPetrelHeader(char *const buffer) {
  return (std::string(buffer).find(PETREL_HEADER.toStdString()) != std::string::npos);
}

// private

// Call this once before reading commences.
//
void CImportPetrel::SetProgressSteps() {
  assert(m_stream);

  m_stream.seekg(0, std::ios::end);
  m_filesize = m_stream.tellg();
  m_stream.seekg(0, std::ios::beg);
  m_progress.AddSteps(NUMBEROFPROGRESSSTEPS);
}

void CImportPetrel::ProgressStep() {
  if (m_stream.tellg() >= ((double)m_currentProgressStep) / NUMBEROFPROGRESSSTEPS * m_filesize) {
    ++m_currentProgressStep;
    m_progress.Step();
  }
}

bool CImportPetrel::ReadIdentifier() {
  m_stream.getline(m_cBuffer, m_bufferSize);
  ProgressStep();
  ++m_lineCounter;
  if (m_stream.rdstate() != std::ios::goodbit) {
    m_message += QString("- Cannot read first header line (line %1)\n").arg(QString::number(m_lineCounter));

    return false;
  }
  if (!CImportPetrel::hasPetrelHeader(m_cBuffer)) {
    m_message += QString("- Incorrect first header line (line %1)\n").arg(QString::number(m_lineCounter));
    return false;
  }

  return true;
}

bool CImportPetrel::ReadHeaders() {
  if (!ReadIdentifier())
    return false;

  bool bDone = false;
  do {
    m_stream.getline(m_cBuffer, m_bufferSize);
    ProgressStep();
    ++m_lineCounter;

    if (m_stream.rdstate() != std::ios::goodbit) {
      m_message += QString("- Failed reading headers (line %1)\n").arg(QString::number(m_lineCounter));
      return false;
    }

    if ((m_cBuffer[0] != '#' && CheckColumnNumbers()) || m_lineCounter >= 100)
      break;

    if (m_cBuffer[0] == '#') {
      CheckCoordUnit("m", e_si_unit) || CheckCoordUnit("ft", e_field_unit) || CheckDepthUnit("m-UNITS", e_si_unit) ||
          CheckDepthUnit("ft-UNITS", e_field_unit);
    } else if (!CheckColumnNumbers()) {
      if (!CheckColumnNames())
        return false;
    } else
      bDone = true;
  } while (!bDone);

  if (!CheckColumnNumbers()) {
    m_message += QString("- Giving up finding X Y Z column names (line %1)\n").arg(QString::number(m_lineCounter));
    return false;
  }

  return true;
}

bool CImportPetrel::CheckColumnNames() {
  if (m_cBuffer[0] == '#') {
    return true; // nothing found, but not an error
  }

  if (!FindColumnNumbers()) {
    m_message += QString("- Some X Y Z column names missing (line %1)\n").arg(QString::number(m_lineCounter));
    return false;
  }

  return true;
}

bool CImportPetrel::FindColumnNumbers() {
  // all column numbers must be found simultaneously
  //
  m_column_count = m_x_column = m_y_column = m_z_column = -1;

  QStringList row = QString(m_cBuffer).split(QRegExp("\\s+"), QString::SkipEmptyParts);

  for (size_t ii = 0; ii < (size_t)(row.size()); ++ii) {
    bool bOk = true;
    if (row.at(ii).toDouble(&bOk), bOk) {
      m_message += QString("- Number found in header (line %1)\n").arg(QString::number(m_lineCounter));

      return false;
    }
  }

  for (size_t ii = 0; ii < (size_t)(row.size()); ++ii) {
    if (row.at(ii) == "X")
      m_x_column = ii;
    else if (row.at(ii) == "Y")
      m_y_column = ii;
    else if (row.at(ii) == "Z")
      m_z_column = ii;
  }
  m_column_count = row.size();
  return CheckColumnNumbers();
}

bool CImportPetrel::CheckColumnNumbers() { return m_x_column >= 0 && m_y_column >= 0 && m_z_column >= 0; }

bool CImportPetrel::_CheckUnit(const QString &s_unit, const CImportPetrel::t_unit /*unit*/
                               ,
                               const QString &s_regex) {
  QString str_regex = QString(s_regex).arg(s_unit);
  QRegExp regex(str_regex);
  regex.setCaseSensitivity(Qt::CaseInsensitive);
  regex.setMinimal(true);
  QRegExpValidator validator;
  validator.setRegExp(regex);

  int dummy;
  QString buffer = QString(m_cBuffer);
  if (validator.validate(buffer, dummy) == QValidator::Acceptable) {
    return true;
  }
  return false;
}

bool CImportPetrel::CheckCoordUnit(const QString &s_unit, const CImportPetrel::t_unit unit) {
  if (_CheckUnit(s_unit, unit, QString("^#\\s*WELL\\s*HEAD\\s+X-COORDINATE:[^(]*\\(%1\\).*"))) {
    m_coordUnit = unit;
    return true;
  }
  return false;
}

bool CImportPetrel::CheckDepthUnit(const QString &s_unit, const CImportPetrel::t_unit unit) {
  if (_CheckUnit(s_unit, unit, QString("^#\\s*DEPTH\\s*\\(\\s*Z,\\s*TVD\\s*\\)\\s*GIVEN\\s+IN\\s+%1"))) {
    m_depthUnit = unit;
    return true;
  }
  return false;
}

bool CImportPetrel::ReadTable() {
  QVector<geo::CPoint> vcTable;

  do // first line is already in m_cBuffer
  {
    if (m_stream.rdstate() != std::ios::goodbit) {
      m_message += QString("- Failed reading table (line %1)\n").arg(QString::number(m_lineCounter));
      return false;
    }

    if (m_cBuffer[0] == '#')
      continue;

    QStringList row = QString(m_cBuffer).split(QRegExp("\\s+"), QString::SkipEmptyParts);

    if (m_column_count != row.size()) {
      m_message +=
          QString("- Skipped row with %1 values, expected %2 (line %3)\n")
              .arg(QString::number(row.size()), QString::number(m_column_count), QString::number(m_lineCounter));
      m_warning = true;
      continue;
    }

    bool bOk = true;
    double x = row.at(m_x_column).toDouble(&bOk);
    if (!bOk) {
      m_message += QString("- Skipped row with invalid number '%1' for X (line %2)\n")
                       .arg(row.at(m_x_column), QString::number(m_lineCounter));
      m_warning = true;
      continue;
    }

    double y = row.at(m_y_column).toDouble(&bOk);
    if (!bOk) {
      m_message += QString("- Skipped row with invalid number '%1' for Y (line %2)\n")
                       .arg(row.at(m_y_column), QString::number(m_lineCounter));
      m_warning = true;
      continue;
    }

    double z = row.at(m_z_column).toDouble(&bOk);
    if (!bOk) {
      m_message += QString("- Skipped row with invalid number '%1' for Z (line %2)\n")
                       .arg(row.at(m_z_column), QString::number(m_lineCounter));
      m_warning = true;
      continue;
    }

    // Petrel X = Geomec Y (Easting)
    // Petrel Y = Geomec X (Northing)
    //
    vcTable.push_back(geo::CPoint(y, x, z));

    ++m_lineCounter;
  } while (m_stream.getline(m_cBuffer, m_bufferSize), ProgressStep(), ++m_lineCounter,
           !(m_stream.rdstate() & std::ios::eofbit));

  if (vcTable.size() < 2) {
    m_message += QString("- Not enough points in wellpath (line %1)\n").arg(QString::number(m_lineCounter));
    return false;
  }

  // Check if Z is decreasing.
  // In that case create Depth from Z by Multiplying by -1.
  // adapted: waij TFS 79058
  //
  bool bReverse = false;

  for (int ii = 1; ii < vcTable.size(); ++ii) {
    if (vcTable[ii - 1].Z() != vcTable[ii].Z()) {
      bReverse = vcTable[ii - 1].Z() > vcTable[ii].Z();
      break;
    }
  }

  if (bReverse) {
    for (int ii = 0; ii < vcTable.size(); ++ii) {
      vcTable[ii].Z(-vcTable[ii].Z());
    }
  }

  // convert X,Y from 'ft' to 'm'
  //
  if (m_coordUnit == e_field_unit) {
    for (int ii = 0; ii < vcTable.size(); ++ii) {
      vcTable[ii].X(CLengthQuantity().Convert(vcTable[ii].X(), CDoubleQuantity::SI_UNIT, CDoubleQuantity::FIELD_UNIT));
      vcTable[ii].Y(CLengthQuantity().Convert(vcTable[ii].Y(), CDoubleQuantity::SI_UNIT, CDoubleQuantity::FIELD_UNIT));
    }
  }

  // convert Z from 'ft-UNITS' to 'm-UNITS'
  //
  if (m_depthUnit == e_field_unit) {
    for (int ii = 0; ii < vcTable.size(); ++ii) {
      vcTable[ii].Z(CLengthQuantity().Convert(vcTable[ii].Z(), CDoubleQuantity::SI_UNIT, CDoubleQuantity::FIELD_UNIT));
    }
  }

  m_wellpathInput->InitFromPointArray(vcTable);

  return true;
}

const QString &CImportPetrel::Message() const { return m_message; }
