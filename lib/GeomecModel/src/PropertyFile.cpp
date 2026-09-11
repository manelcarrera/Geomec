// PropertyFile.cpp: implementation of the CPropertyFile class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include "BaseEntryTypes.h"
#include "ExportFormat.h"
#include "ModelBase.h"
#include "NewWellPathInput.h"
#include "NodalValueSet.h"
#include "PropertyFile.h"

#include "GeoSurface.h"
#include "HotSpot.h"
#include "ImportFormat.h"
#include "MaterialResultTree.h"
#include "Result.h"
#include "Surface.h"

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CElementSetFile::CElementSetFile(CModelBase &model)
    : m_pNodalExportFormat(0), m_pElementSetExportFormat(0), m_bCoorUnitSet(false), m_pElementSet(0), m_pPointSet(0),
      m_pNodalFormat(), m_pElementFormat(), m_pResultFormat(0), m_pWellPathInputFormat(0), m_bWriteHeader(true),
      m_model(model), m_strExportDelimiter("\t"), m_created_objects(0) {
  // Define comment
  DefineComment("*");
  DefineComment("#");

  // Define trailer
  DefineToken(*(new THeaderToken(*this, "SURFACE", &CElementSetFile::OnSurface)));
  DefineToken(*(new THeaderToken(*this, "ELEMENTSET", &CElementSetFile::OnElementSet)));
  DefineToken(*(new THeaderToken(*this, "POINTSET", &CElementSetFile::OnPointSet)));
  DefineToken(*(new THeaderToken(*this, "POINTSSET", &CElementSetFile::OnPointSet)));
  DefineToken(*(new THeaderToken(*this, "POINT", &CElementSetFile::OnPoint_Set)));
  DefineToken(*(new THeaderToken(*this, "PROPERTY", &CElementSetFile::OnProperty)));
  DefineToken(*(new THeaderToken(*this, "ELEMENTS", &CElementSetFile::OnElementTrailer)));
  DefineToken(*(new THeaderToken(*this, "DEPL.STAGES", &CElementSetFile::OnDepletionTimes)));
  DefineToken(*(new THeaderToken(*this, "UNIT", &CElementSetFile::OnUnit)));
  DefineToken(*(new THeaderToken(*this, "UNITS", &CElementSetFile::OnUnit)));
  DefineToken(*(new THeaderToken(*this, "COOR", &CElementSetFile::OnCoor)));
  DefineToken(*(new THeaderToken(*this, "POINTS", &CElementSetFile::OnPoints)));
  DefineToken(*(new THeaderToken(*this, "ELEMENT", &CElementSetFile::OnElement)));
  DefineToken(*(new THeaderToken(*this, "END", &CElementSetFile::OnEnd)));
}

CElementSetFile::CElementSetFile(CModelBase &model, CGraphNode::TNodeVec &created_objects)
    : m_pNodalExportFormat(0), m_pElementSetExportFormat(0), m_bCoorUnitSet(false), m_pElementSet(0), m_pPointSet(0),
      m_pNodalFormat(), m_pElementFormat(), m_pResultFormat(0), m_pWellPathInputFormat(0), m_bWriteHeader(true),
      m_model(model), m_strExportDelimiter("\t"), m_created_objects(&created_objects) {
  // Define comment
  DefineComment("*");
  DefineComment("#");

  // Define trailer
  DefineToken(*(new THeaderToken(*this, "SURFACE", &CElementSetFile::OnSurface)));
  DefineToken(*(new THeaderToken(*this, "ELEMENTSET", &CElementSetFile::OnElementSet)));
  DefineToken(*(new THeaderToken(*this, "POINTSET", &CElementSetFile::OnPointSet)));
  DefineToken(*(new THeaderToken(*this, "POINTSSET", &CElementSetFile::OnPointSet)));
  DefineToken(*(new THeaderToken(*this, "POINT", &CElementSetFile::OnPoint_Set)));
  DefineToken(*(new THeaderToken(*this, "PROPERTY", &CElementSetFile::OnProperty)));
  DefineToken(*(new THeaderToken(*this, "ELEMENTS", &CElementSetFile::OnElementTrailer)));
  DefineToken(*(new THeaderToken(*this, "DEPL.STAGES", &CElementSetFile::OnDepletionTimes)));
  DefineToken(*(new THeaderToken(*this, "UNIT", &CElementSetFile::OnUnit)));
  DefineToken(*(new THeaderToken(*this, "UNITS", &CElementSetFile::OnUnit)));
  DefineToken(*(new THeaderToken(*this, "COOR", &CElementSetFile::OnCoor)));
  DefineToken(*(new THeaderToken(*this, "POINTS", &CElementSetFile::OnPoints)));
  DefineToken(*(new THeaderToken(*this, "ELEMENT", &CElementSetFile::OnElement)));
  DefineToken(*(new THeaderToken(*this, "END", &CElementSetFile::OnEnd)));
}

CElementSetFile::CElementSetFile(CResultImportFormat &result_format)
    : m_pNodalExportFormat(0), m_pElementSetExportFormat(0), m_bCoorUnitSet(false), m_pElementSet(0), m_pPointSet(0),
      m_pNodalFormat(), m_pElementFormat(), m_pResultFormat(&result_format), m_pWellPathInputFormat(0),
      m_bWriteHeader(true), m_model(result_format.Model()), m_strExportDelimiter("\t"), m_created_objects(0) {
  // Define comment
  DefineComment("*");
  DefineComment("#");

  DefineToken(*(new THeaderToken(*this, "RESULTS", &CElementSetFile::OnResults)));
  DefineToken(*(new THeaderToken(*this, "UNIT", &CElementSetFile::OnUnit)));
  DefineToken(*(new THeaderToken(*this, "UNITS", &CElementSetFile::OnUnit)));
  DefineToken(*(new THeaderToken(*this, "END", &CElementSetFile::OnEnd)));
}

CElementSetFile::CElementSetFile(const IElementSetExportFormat &format)
    : m_pNodalExportFormat(&format.NodalExportFormat()), m_pElementSetExportFormat(&format), m_bCoorUnitSet(false),
      m_pElementSet(0), m_pPointSet(0), m_pNodalFormat(), m_pResultFormat(0), m_pWellPathInputFormat(0),
      m_bWriteHeader(true), m_model(const_cast<CModelBase &>(format.Model())), m_strExportDelimiter("\t"),
      m_created_objects(0) {
  dynamic_cast<CElementSetExportFormat *>(const_cast<IElementSetExportFormat *>(m_pElementSetExportFormat))
      ->sortNodeVector();
}

CElementSetFile::CElementSetFile(const CNodalExportFormat &format)
    : m_pNodalExportFormat(&format), m_pElementSetExportFormat(0), m_bCoorUnitSet(false), m_pElementSet(0),
      m_pPointSet(0), m_pNodalFormat(), m_pResultFormat(0), m_pWellPathInputFormat(0), m_bWriteHeader(true),
      m_model(const_cast<CModelBase &>(format.Model())), m_strExportDelimiter("\t"), m_created_objects(0) {}

CElementSetFile::CElementSetFile(CWellPathInputImportFormat &well_format)
    : m_pNodalExportFormat(0), m_pElementSetExportFormat(0), m_bCoorUnitSet(false), m_pElementSet(0), m_pPointSet(0),
      m_pNodalFormat(), m_pElementFormat(), m_pResultFormat(0), m_pWellPathInputFormat(&well_format),
      m_bWriteHeader(true), m_model((CModelBase &)well_format.NewWellPathInput()->Model()), m_strExportDelimiter("\t"),
      m_created_objects(0) {
  // Define comment
  DefineComment("*");
  DefineComment("#");

  // Define trailer
  DefineToken(*(new THeaderToken(*this, "POINTSET", &CElementSetFile::OnPointSet)));
  DefineToken(*(new THeaderToken(*this, "POINTSSET", &CElementSetFile::OnPointSet)));
  DefineToken(*(new THeaderToken(*this, "POINT", &CElementSetFile::OnPoint_Set)));
  DefineToken(*(new THeaderToken(*this, "UNIT", &CElementSetFile::OnUnit)));
  DefineToken(*(new THeaderToken(*this, "UNITS", &CElementSetFile::OnUnit)));
  DefineToken(*(new THeaderToken(*this, "COOR", &CElementSetFile::OnCoor)));
  DefineToken(*(new THeaderToken(*this, "DEPL.STAGES", &CElementSetFile::OnDepletionTimes)));
  DefineToken(*(new THeaderToken(*this, "END", &CElementSetFile::OnEnd)));
}

CElementSetFile::~CElementSetFile() {}

bool CElementSetFile::IsElementSet() const { return m_pElementSet != 0; }

const IElementSet &CElementSetFile::ElementSet() const {
  assert(m_pElementSet);
  return *m_pElementSet;
}

IElementSet &CElementSetFile::ElementSet() {
  assert(m_pElementSet);
  return *m_pElementSet;
}

bool CElementSetFile::IsPointSet() const { return m_pPointSet != 0; }

const IPointSet &CElementSetFile::PointSet() const {
  if (m_pElementSet)
    return *m_pElementSet;
  assert(m_pPointSet);
  return *m_pPointSet;
}

IPointSet &CElementSetFile::PointSet() {
  if (m_pElementSet)
    return *m_pElementSet;
  assert(m_pPointSet);
  return *m_pPointSet;
}

CElementSet *CElementSetFile::OnCreateElementSet(const QString &sName, CFemAppModel &model,
                                                 CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit) const {
  return new CElementSet(sName, model, coordinate_unit, value_unit);
}

CPointSet *CElementSetFile::OnCreatePointSet(const QString &sName, CFemAppModel &model, int nValueSetSize,
                                             IPointSet::DIMENSION dim, bool bCreateValueSetForCoordinates) const {
  return new CPointSet(sName, model, nValueSetSize, dim, bCreateValueSetForCoordinates);
}

QString CElementSetFile::ReadName(TInputStream &stream) {
  QString sRet;

  stream.eatwhite();
  if (stream.peek() == '"') {
    stream.get();
    char ch = stream.get();
    while (ch != '"' && ch != '\n' && ch != 0) {
      sRet.append(ch);
      ch = stream.get();
    };
    if (ch != '"')
      throw CReadException("String is not terminated with \" on the same line.");
  } else {
    stream >> sRet;
  }
  return sRet;
}

void CElementSetFile::WriteName(const QString &sName, TOutputStream &stream) {
  QString sValue = sName.trimmed();
  if (sValue.indexOf(' ') > -1) {
    stream << '"' << sValue << '"';
  } else {
    stream << sName;
  }
}

bool CElementSetFile::OnElementSet(TInputStream & /*stream*/, const QString & /*sToken*/) {
  assert(!m_pResultFormat);
  // Create an element set
  if (m_pElementSet || m_pPointSet)
    throw CReadException("Keyword ELEMENTSET not expected");

  QString Name = FileName();
  Name.remove(Name.length() - 4, 4);

  CElementSet *pElementSet = OnCreateElementSet(Name, m_model, CQuantity::SI_UNIT, CQuantity::SI_UNIT);
  if (!pElementSet)
    throw CReadException("Keyword ELEMENTSET not expected");

  m_pElementSet = pElementSet;
  m_pNodalFormat = QSharedPointer<CNodalImportFormat>(new CNodalImportFormat(*pElementSet));

  return true;
}

bool CElementSetFile::OnSurface(TInputStream & /*stream*/, const QString & /*sToken*/) {
  assert(!m_pResultFormat);
  if (!m_model.GraphEntry(MD_BASE_SURFACE))
    throw CReadException("Surfaces not supported in this model type.");

  // Create an element set
  if (m_pElementSet || m_pPointSet)
    throw CReadException("Keyword SURFACE not expected");

  QString Name = FileName();
  Name.remove(Name.length() - 4, 4);

  // Create an empty surface
  geo::CSurface surface;
  CSurfaceEntry &entry = (CSurfaceEntry &)(*m_model.GraphEntry(MD_BASE_SURFACE));
  m_pElementSet = entry.InsertSurface(Name, surface);

  m_pNodalFormat = QSharedPointer<CNodalImportFormat>(new CNodalImportFormat(*m_pElementSet));

  return true;
}

bool CElementSetFile::OnProperty(TInputStream &stream, const QString & /*sToken*/) {
  if ((m_pNodalFormat || m_pElementFormat) && dynamic_cast<CSurfaceBase *>(m_pElementSet) == 0) {
    IImportFormat::TStringVec vcString;
    int nLineNumber = stream.line_number();
    do {
      vcString.push_back(ReadName(stream));
      stream.eatwhite();
    } while (nLineNumber == stream.line_number());
    CurrentFormat().CreateValueType(vcString);
  } else {
    throw CReadException("Keyword PROPERTY not expected");
  }

  return true;
}

bool CElementSetFile::OnResults(TInputStream & /*stream*/, const QString & /*sToken*/) {
  assert(m_pResultFormat);
  return true;
}

bool CElementSetFile::OnPointSet(TInputStream & /*stream*/, const QString &sToken) {
  if (m_pWellPathInputFormat)
    return true;

  assert(!m_pResultFormat);
  if (m_pPointSet || m_pElementSet)
    throw CReadException(QString("Keyword %1 not expected").arg(sToken));

  QString Name = FileName();
  Name.remove(Name.length() - 4, 4);
  CPointSet *pPointSet = OnCreatePointSet(Name, m_model, 0, IPointSet::DIM_2D, false);
  if (!pPointSet)
    throw CReadException(QString("Keyword %1 not expected").arg(sToken));

  m_pNodalFormat = QSharedPointer<CNodalImportFormat>(new CNodalImportFormat(*pPointSet));
  assert(!pPointSet->Coordinates().Component(0).Defined() && !pPointSet->Coordinates().Component(1).Defined() &&
         !pPointSet->Coordinates().Component(2).Defined());
  m_pPointSet = pPointSet;

  return true;
}

bool CElementSetFile::OnPoint_Set(TInputStream &stream, const QString &sToken) {
  QString sSet;
  stream >> sSet;
  if (sSet.toUpper() != "SET")
    throw CReadException(QString("'POINT' must be followed by 'SET', encountered '%1'").arg(sSet));
  return OnPointSet(stream, sToken + " " + sSet);
}

IImportFormat &CElementSetFile::CurrentFormat() {
  if (m_pWellPathInputFormat)
    return *m_pWellPathInputFormat;
  if (m_pElementFormat)
    return *m_pElementFormat;
  if (m_pResultFormat)
    return *m_pResultFormat;
  assert(m_pNodalFormat);
  return *m_pNodalFormat;
}

bool CElementSetFile::OnDepletionTimes(TInputStream &stream, const QString & /*sToken*/) {
  QString strDep;
  int month, year;

  do {
    stream >> strDep;
    stream >> month;
    stream >> year;

    CurrentFormat().AddTime(strDep.toStdString(), year, month);

    stream.eatwhite();
  } while (stream.peek() == 'D');

  return true;
}

bool CElementSetFile::OnElementTrailer(TInputStream & /*stream*/, const QString & /*sToken*/) {
  if (m_pElementSet && m_pNodalFormat && (m_pElementFormat == 0)) {
    m_pElementFormat = QSharedPointer<CElementImportFormat>(new CElementImportFormat(*m_pNodalFormat));

    // if depletion times were present, they are currently only known in the nodal format,
    // make sure the elementformat also knows about them:
    std::map<std::string, CGeomecTime>::const_iterator it;
    for (it = m_pNodalFormat->Times().begin(); it != m_pNodalFormat->Times().end(); it++) {
      m_pElementFormat->AddTime(it->first, it->second.GetYear(), it->second.GetMonth());
    }
  } else
    throw CReadException("\"ELEMENTS\" keyword not expected");

  return true;
}

bool CElementSetFile::OnEnd(TInputStream & /*stream*/, const QString & /*sToken*/) { return false; }

// wjrx mantis 3099
bool CElementSetFile::OnCoor(TInputStream &stream, const QString & /*sToken*/) {
  stream.eatwhite();
  if (stream.peek() == '=')
    stream.get();

  QString sUnit;
  stream >> sUnit;

  // wjrx mantis 3336
  m_bCoorUnitSet = true;

  if (m_pWellPathInputFormat)
    return true;

  if (!m_pPointSet && !m_pElementSet && !m_pResultFormat) {
    throw CReadException("COOR keyword not expected");
  }

  if (sUnit.toUpper() == "FIELD") {
    if (!m_pResultFormat) {
      PointSet().CoordinateUnit(CQuantity::FIELD_UNIT);
    }
  } else if (sUnit.toUpper() == "SI") {
    if (!m_pResultFormat) {
      PointSet().CoordinateUnit(CQuantity::SI_UNIT);
    }
  } else
    throw CReadException("Illegal expression after keyword \"COOR\"");

  return true;
}

bool CElementSetFile::OnUnit(TInputStream &stream, const QString & /*sToken*/) {
  stream.eatwhite();
  if (stream.peek() == '=')
    stream.get();

  QString sUnit;
  stream >> sUnit;

  if (m_pWellPathInputFormat)
    return m_pWellPathInputFormat->OnUnit(sUnit);

  if (!m_pPointSet && !m_pElementSet && !m_pResultFormat) {
    throw CReadException("Unit keyword not expected");
  }

  if (sUnit.toUpper() == "FIELD") {
    if (m_pResultFormat)
      m_pResultFormat->Unit(CQuantity::FIELD_UNIT);
    else {
      // wjrx mantis 3099
      // PointSet().CoordinateUnit(CQuantity::FIELD_UNIT);
      PointSet().ValueUnit(CQuantity::FIELD_UNIT);

      // wjrx mantis 3336
      if (!m_bCoorUnitSet)
        PointSet().CoordinateUnit(CQuantity::FIELD_UNIT);
    }
  } else if (sUnit.toUpper() == "SI") {
    if (m_pResultFormat)
      m_pResultFormat->Unit(CQuantity::SI_UNIT);
    else {
      // wjrx mantis 3099
      // PointSet().CoordinateUnit(CQuantity::SI_UNIT);
      PointSet().ValueUnit(CQuantity::SI_UNIT);

      // wjrx mantis 3336
      if (!m_bCoorUnitSet)
        PointSet().CoordinateUnit(CQuantity::SI_UNIT);
    }
  } else
    throw CReadException("Illegal expression after keyword \"UNIT\"");

  return true;
}

bool CElementSetFile::OnElement(TInputStream &stream, const QString & /*sToken*/) {
  // Check status
  if (m_pElementSet && m_pElementFormat) {
    stream.eatwhite();
    if (stream.peek() == '=')
      stream.get();
    QString sElementType;
    stream >> sElementType;
    m_pElementFormat->SetElementType(sElementType);
  } else {
    throw CReadException("Keyword \"Element\" not expected");
  }

  return true;
}

const CModelBase &CElementSetFile::Model() const { return m_model; }

CModelBase &CElementSetFile::Model() { return m_model; }

bool CElementSetFile::OnWrite(TOutputStream &stream) {
  assert(m_pNodalExportFormat);
  assert(m_pElementSet == 0); // ElementSet to export are stored in a array m_vcElementSet
  if (m_bWriteHeader) {
    stream << "*******************************************************************************\n";
    stream << "****************************** Geomec Export **********************************\n";
    stream << "*******************************************************************************\n";
    stream << "** Project : ";
    for (int i = 0; i < m_model.ProjectInfo().length(); i++) {
      stream << m_model.ProjectInfo()[i];
      if (m_model.ProjectInfo()[i] == '\n') {
        stream << "**           ";
      }
    }
    stream << "\n";
    stream << "*******************************************************************************\n";
    stream << "\n";
  }
  if (m_pNodalExportFormat->HasExportName())
    stream << "#" << m_pNodalExportFormat->ExportName() << endl << endl;

  // Write the elementset to file
  stream << m_pNodalExportFormat->ExportType() << "\n\n";

  if (!WriteUnit(stream))
    return false;

  if (!m_pElementSetExportFormat || m_pElementSetExportFormat->NodeSize() > 0) {
    if (!WriteDepStages(stream))
      return false;

    if (m_pNodalExportFormat->HotSpot()) {
      stream << "MIN_VALUE" << m_strExportDelimiter << m_pNodalExportFormat->HotSpot()->MinMaxValue().first << '\n';
      stream << "MAX_VALUE" << m_strExportDelimiter << m_pNodalExportFormat->HotSpot()->MinMaxValue().second << '\n'
             << '\n';
    }

    // wedx 05092007:
    // mantis #2469
    // remove POINTS keyword from pointset files
    if (m_pElementSetExportFormat)
      stream << "POINTS\n";

    int nPrecision = stream.realNumberPrecision();
    stream.setRealNumberPrecision(8);
    if (!WriteTable(*m_pNodalExportFormat, stream))
      return false;
    stream.setRealNumberPrecision(nPrecision);
  }

  if (m_pElementSetExportFormat) {
    assert(m_pElementSet == 0);
    assert(m_pPointSet == 0);
    assert(m_pElementSetExportFormat->ElementSize() > 0);

    if (m_pElementSetExportFormat->NodeSize() > 0) {
      // Write the header
      stream << "\nELEMENTS";

      // Determ element output
      stream << "\nELEMENT" << m_strExportDelimiter << m_pElementSetExportFormat->ElementType();
      stream << "\n";
    }

    if (!WriteTable(*m_pElementSetExportFormat, stream))
      return false;
  }

  if (m_bWriteHeader) {
    stream << "\nEND\n";
  }

  return true;
}

bool CElementSetFile::WriteUnit(TOutputStream &stream) {
  if (m_pNodalExportFormat->Unit() == CQuantity::SI_UNIT)
    stream << "UNIT" << m_strExportDelimiter << "SI\n\n";
  else
    stream << "UNIT" << m_strExportDelimiter << "FIELD\n\n";
  return true;
}

bool CElementSetFile::WriteDepStages(TOutputStream &stream) {
  stream << "DEPL.STAGES\n";
  QString dep;
  CDepletionStageEntry &entry = Model().DepletionStageEntry();

  CDepletionStageEntry::iterator it;
  int i;
  for (i = 0, it = entry.begin(); it != entry.end(); it++, i++) {
    dep = QString("D%1%2%3%4%5\n")
              .arg(i)
              .arg(m_strExportDelimiter)
              .arg(it->Time().GetMonth())
              .arg(m_strExportDelimiter)
              .arg(it->Time().GetYear());
    stream << dep;
  }

  stream << "\n";
  return true;
}

bool CElementSetFile::WriteTable(const IExportFormat &format, TOutputStream &stream) {
  for (size_t i = 0; i < format.ValueTypeSize(); i++) {
    const CValueType &value_type = format.ValueType(i);
    stream << "PROPERTY" << m_strExportDelimiter << value_type.ExportType();
    if (value_type.ComponentSize() > 1) {
      for (int j = 0; j < value_type.ComponentSize(); j++) {
        stream << m_strExportDelimiter;
        WriteName(value_type.Component(j).Name(), stream);
      }
    }
    stream << m_strExportDelimiter;
    WriteName(value_type.Name(), stream);
    stream << '\n';
    if (format.ValueTypeSize() == i + 1)
      stream << '\n';
  }

  /*
   * The assumption is that the condition below is
   * only valid when results are being exported!
   */

  if (!format.isGVT() && (format.ValueTypeSize() == 0) && (format.DataValueSize() > 0)) {
    std::vector<bool> processed(format.DataValueSize(), false);
    QString forceCacheCreation = format.StringAt(0, 0);

    for (size_t s = 0; s < format.DataValueSize(); ++s) {
      if (!processed[s]) {
        const IValueComponentBase &valueComponent = format.DataValue(s);
        const IValueComposite &valueComposite = valueComponent.Parent();
        const CValueType *valueType = dynamic_cast<const CValueType *>(&valueComposite); // GVT Output results

        try {
          const IResultComponent *resultComponent = dynamic_cast<const IResultComponent *>(&valueComponent);
          int stage = -1;

          if (resultComponent) {
            const CDepletionStage &depletionStage = resultComponent->Stage();
            stage = depletionStage.Index();
          }

          stream << "PROPERTY" << m_strExportDelimiter;

          try {
            const CMaterialResult &materialResult = dynamic_cast<const CMaterialResult &>(valueComposite);

            stream << materialResult.StoredExportLabel();
          }

          catch (...) {
            if (valueType) {
              stream << valueType->ExportType() << m_strExportDelimiter;
            } else
              WriteName(valueComposite.ExportLabel(), stream);
          }

          if (valueComposite.ComponentSize() > 1) {
            for (size_t t = 0; t < valueComposite.ComponentSize(); t++) {
              if (((s + t) < format.DataValueSize()) &&
                  dynamic_cast<const IResultComponent *>(&format.DataValue(s + t)) &&
                  (stage == static_cast<const IResultComponent &>(format.DataValue(s + t)).Stage().Index()) &&
                  (&valueComposite == &format.DataValue(s + t).Parent())) {
                stream << m_strExportDelimiter;
                WriteName(format.StringAt(0, format.getDataOffset() + s + t), stream);
                processed[s + t] = true;
              }
            }
          } else {
            stream << m_strExportDelimiter;
            WriteName(format.StringAt(0, format.getDataOffset() + s), stream);
          }

          if (valueComposite.ComponentSize() > 1) {
            stream << m_strExportDelimiter;
            WriteName(valueComposite.ExportLabel(), stream);
          }

          stream << '\n';
        }

        catch (...) {
        }

        processed[s] = true;
      }
    }

    if (format.DataValueSize() > 0) {
      stream << '\n';
    }
  }

  // we're going to filter out duplicate lines
  QString prev;

  for (int nRow = 0; nRow < format.RowSize(); nRow++) {
    QString curr;
    QTextStream currStream(&curr);

    if (format.HasRowComment(nRow)) {
      // Dirty hack! apparently in an earlier implementation there used to be a blank line
      // in between each element definition. Shell needs these blank lines <grunt> for further processing
      // of the exported file. now I just put a blank line before each comment... 13-01-2005
      currStream << '\n';
      currStream << "#" << format.RowComment(nRow) << '\n';
    }

    for (int nColumn = 0; nColumn < format.ColumnSize(); nColumn++) {
      if (nColumn != 0)
        currStream << m_strExportDelimiter;
      switch (format.OutputType(nRow, nColumn)) {
      case IExportFormat::OT_INTEGER:
        currStream << QString("%1").arg(format.IntegerAt(nRow, nColumn));
        break;
      case IExportFormat::OT_DOUBLE:
        currStream << QString("%1").arg(format.DoubleAt(nRow, nColumn), 0, 'G', 12);
        break;
      case IExportFormat::OT_STRING:
        WriteName(format.StringAt(nRow, nColumn), currStream);
        break;
      default:
        assert(false);
        break;
      }
    }
    currStream << '\n';

    if (curr != prev) {
      stream << curr;
      prev = curr;
    }

    ItemSaved();
  }

  stream << "\n";
  return true;
}

/*!
  Reads a row of values
  Note don't call this function in case of eof()
*/
void CElementSetFile::ReadRow(TInputStream &stream) {
  stream.eatwhite();

  std::vector<geo::CValue> vcRow(CurrentFormat().ColumnSize());

  // Reading the numbers while the stream is ok
  int nLine = -1;
  for (int nColumn = 0; nColumn < CurrentFormat().ColumnSize(); nColumn++) {
    // Checking
    if (nLine == -1)
      nLine = lineNr();
    if (nLine != lineNr()) {
      // Oops not the same line ...
      throw CReadException("Wrong number of values on the data line.");
    }

    geo::CValue value;
    stream >> value;

    vcRow[nColumn] = geo::CValue(value);
  }

  ItemRead();

  // Checking ending char ...
  while (stream.peek() != '\n' && stream.peek() != 0) {
    char ch = stream.get();

    if (m_pWellPathInputFormat == 0) {
      if (ch != ' ' && ch != '\t')
        throw CReadException(QString("More than %1 values on the line").arg(vcRow.size()));
    }
  };

  CurrentFormat().AppendRow(vcRow);

  stream.eatwhite(); // Go to first character of the first line
}

bool CElementSetFile::OnPoints(TInputStream & /*stream*/, const QString & /*sToken*/) { return true; }

// ##ModelId=3C74E3E7034E
bool CElementSetFile::OnParseFail(TInputStream &stream, const QString &sToken) {
  if (!m_pPointSet && !m_pElementSet && !m_pResultFormat && !m_pWellPathInputFormat) {
    ItemRead();
    return true; // ignore (mantis #3309)
  }

  // Could be the header ...
  CurrentFormat().AppendColumn(sToken);

  while (stream.peek() != '\n') {
    CurrentFormat().AppendColumn(ReadName(stream));
    while (stream.peek() == ' ' || stream.peek() == '\t')
      stream.get();
  }

  try {
    CurrentFormat().ValidateColumns();
  } catch (CReadException e) {
    throw CReadException(
        QString("Could not validate the column headers:\n(%1)\n\nFound '%2'.").arg(e.ErrorMessage()).arg(sToken));
  }

  // Read rows
  do {
    ReadRow(stream);
  } while (!isalpha(stream.peek()) && stream.peek() != '~' && !stream.eof());

  CPointSet *pPointSet = dynamic_cast<CPointSet *>(m_pPointSet);
  if (pPointSet) {
    pPointSet->Coordinates().Invalidate();
    pPointSet->Modified();
  } else {
    if (&CurrentFormat() == m_pElementFormat)
      PointSet().Modified();
  }

  // if the format is a result format, validate to make sure that
  // all elements have results assigned to the right amount of nodes.
  CurrentFormat().ValidateRows();

  return true;
}

bool CElementSetFile::IgnoreRead(TInputStream & /*stream*/, const QString & /*sToken*/) {
  if (!m_pPointSet && !m_pElementSet && !m_pResultFormat && !m_pWellPathInputFormat) {
    return true; // ignore (mantis #3309)
  }

  return false;
}

static const char *SPACE = " ";
static const int RADIX10 = 10;

bool CElementSetFile::OnRead(TInputStream &stream) {
  try {
    stream.eatwhite();
    if ((isdigit(stream.peek()) || stream.peek() == '-') && !m_pResultFormat && !m_pWellPathInputFormat) {
      // We have a column file
      std::vector<geo::CValue> vcValue;
      while (stream.peek() != '\n' && stream.peek() != 0) {
        geo::CValue value;
        stream >> value;
        vcValue.push_back(value);
        while (stream.peek() == ' ' || stream.peek() == '\t')
          stream.get();
      };

      CPointSet *pPointSet = 0;
      switch (vcValue.size()) {
      case 0:
      case 1:
        throw CReadException("At least two columns must be specified.");
        break;
      case 2:
        // Create a 2D pointset
        pPointSet = OnCreatePointSet(FileName(), m_model, vcValue.size() - 2, IPointSet::DIM_2D, true);
        break;
      default:
        // Create a 3D pointset
        IPointSet::DIMENSION dim = IPointSet::DIM_3D;
        pPointSet = OnCreatePointSet(FileName(), m_model, vcValue.size() - int(dim), dim, true);
        break;
      }

      assert(pPointSet);
      // Change x, y lines because
      // 1e column	Easting(Y)
      // 2e column	Northing(X)
      // 3e column	Depth(Z)
      pPointSet->Coordinates().Component(0).LinkTo(pPointSet->NodalValueSet(1));
      pPointSet->Coordinates().Component(1).LinkTo(pPointSet->NodalValueSet(0));
      m_pNodalFormat = QSharedPointer<CNodalImportFormat>(new CNodalImportFormat(*pPointSet));
      m_pPointSet = pPointSet;
      m_pNodalFormat->AppendRow(vcValue);
      // Reading lines ...
      do {
        ReadRow(stream);
      } while (!isalpha(stream.peek()) && !stream.eof());
      PointSet().Modified();
      return true;

    } else {
      int count = 1;

      while (!stream.eof()) {
        stream.eatwhite();

        if (stream.eof()) {
          break;
        }

        if (CTextFile::OnRead(stream)) {
          if (!m_pPointSet && !m_pElementSet && !m_pResultFormat && !m_pWellPathInputFormat) {
            continue; // ignore (mantis #3309)
          }

          stream.eatwhite();

          if ((m_pPointSet != 0) && (!stream.eof() || (count != 1))) {
            std::stringstream sstream;
            sstream << count;
            std::string result;
            sstream >> result;
            m_pPointSet->Name(m_pPointSet->Name() + SPACE + result.c_str());
            ++count;
          }

          if (m_created_objects != 0) {
            if (m_pPointSet)
              m_created_objects->push_back(m_pPointSet);
            else if (m_pElementSet)
              m_created_objects->push_back(m_pElementSet);
          }

          m_pPointSet = 0;
          m_pElementSet = 0;
          m_pNodalFormat.clear();
          m_pElementFormat.clear();
        } else {
          // \todo clean up by failure
          if (m_pPointSet) {
            delete m_pPointSet;
            m_pPointSet = 0;
          }
          if (m_pElementSet) {
            delete m_pElementSet;
            m_pElementSet = 0;
          }
          return false;
        }
      }
    }
  } catch (CReadException error) {
    if (m_pPointSet) {
      delete m_pPointSet;
      m_pPointSet = 0;
    } else if (m_pElementSet) {
      delete m_pElementSet;
      m_pElementSet = 0;
    }
    throw;
  }

  return true;
}

long CElementSetFile::SavedItems() const {
  assert(m_pElementSet == 0);
  assert(m_pPointSet == 0);
  assert(m_pNodalExportFormat);
  if (m_pElementSetExportFormat)
    return m_pElementSetExportFormat->RowSize() + m_pNodalExportFormat->RowSize();
  return m_pNodalExportFormat->RowSize();
}
/*
CElementSetFile::TAxisSystem CElementSetFile::AxisSystem() const
{
  assert(m_pNodalFormat);
  return m_pNodalFormat->AxisSystem();
}
*/

/////

CElementSetFile::CHeaderToken::CHeaderToken(CElementSetFile &instance, const QString &sToken, pfToken token)
    : CTokenTemplate<CElementSetFile>(instance, sToken, token), m_instance(instance) {}

bool CElementSetFile::CHeaderToken::Read(input_stream &stream, const QString &sToken) {
  bool bRet;
  try {
    bRet = CTokenTemplate<CElementSetFile>::Read(stream, sToken);
  } catch (CReadException e) {
    bRet = m_instance.IgnoreRead(stream, sToken);
    if (!bRet)
      throw;
  }

  m_instance.ItemRead();
  return bRet;
}
