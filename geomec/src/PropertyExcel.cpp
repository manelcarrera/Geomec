// PropertyExcel.cpp: implementation of the CPropertyExcel class.
//
//////////////////////////////////////////////////////////////////////

#include "PropertyExcel.h"
#include "BaseEntryTypes.h"
#include "DimensionDlg.h"
#include "ExportFormat.h"
#include "FieldFactor.h"
#include "FvTextFile.h"
#include "GeoSurface.h"
#include "GlobalMessage.h"
#include "HotSpot.h"
#include "IElementSet.h"
#include "ImportFormat.h"
#include "ModelBase.h"
#include "NewWellPathInput.h"
#include "NodalValueSet.h"
#include "ProgressDlg_MFC.h"
#include "Utils.h"
#include "geomec.h"
#include "resourceIDP.h"
#include "resourceIDS.h"
#include "stdafx.h"

#include <sys/stat.h>
#include <sys/types.h>

#include "ExcelAppGuard.h"
#include "ExcelCell.h"
#include "IProgressFactory.h"
#include "Result.h"
#include "excel8.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

class CElementSetExcelFileImpl {
public:
  CElementSetExcelFileImpl(CModelBase &model, bool newWellPath = false);
  CElementSetExcelFileImpl(const CNodalExportFormat &export_format);
  CElementSetExcelFileImpl(const IElementSetExportFormat &export_format);
  ~CElementSetExcelFileImpl();

private:
  CElementSetExcelFileImpl(const CElementSetExcelFileImpl &rhs);
  CElementSetExcelFileImpl &operator=(const CElementSetExcelFileImpl &rhs);

  typedef enum _SheetType {
    ST_UNKNOWN,    // Unknown format
    ST_UNTAGGED,   // Untagged format
    ST_POINTSET,   // Point set format
    ST_ELEMENTSET, // Element set format
    ST_SURFACE,    // Surface format
    ST_MATRIX      // only one property, first row contains easting coords, first column contains northing coords
  } TSheetType;

  typedef std::vector<VARIANT> TContentVec;

  typedef struct _SheetContents {
    // row indices are 1-based, for direct use in Excel
    _Worksheet ws; // Sheet
    char *SheetName;
    TContentVec vcContents;
    long rowlength;    // length of a row in vcContents
    TSheetType type;   // Type of the format
    long typerow;      // row of POINTSET or ELEMENTSET keyword
    long dimrow;       // row of DIM keyword
    long unitrow;      // row of UNIT keyword
    long coorrow;      // row of COOR keyword wjrx mantis 3339
    long dstagerow;    // row of DEPL.STAGES keyword
    long pointsrow;    // row of POINTS keyword
    long pointhdstart; // row of point header start
    long pointhdend;   // row of point header end
    long pointsstart;  // row where points start
    long pointsend;    // row where points end
    long elementsrow;  // row of ELEMENTS keyword
    long elmtyperow;   // row of ELEMENT type keyword
    long elmhdstart;   // row of element header start
    long elmhdend;     // row of element header start
    long elmsstart;    // row where elements start
    long elmsend;      // row where elements end
    long endrow;       // row of END keyword or the end
  } TContents;

  typedef std::vector<TContents> TSheetsVector;

  void WriteUnit(CExcelCell &cell);
  void WriteDepStages(CExcelCell &cell);
  void WriteHotSpotMinMax(CExcelCell &cell);
  void WriteTable(const IExportFormat &format, CExcelCell &cell, IProgressBase &callback);

  bool ReadCellContents(_Worksheet ws, TContents &contents);
  VARIANT GetCellValue(const TContents &contents, long row, long col) const;
  void ReadContents(TContents &contents);

  bool GetSheets(CExcelAppGuard &ExcelApp, const QString &sFileName, TSheetsVector &vcValidSheets);
  void Read(TContents &contents, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects);

  void ReadElementSet(TContents &contents, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects);
  void ReadPointSet(TContents &contents, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects);
  void ReadUntagged(TContents &contents, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects);
  void ReadMatrixType(TContents &contents, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects);

  void ReadHeaders(IImportFormat &format, CExcelCell &cell);
  void ReadData(IImportFormat &format, CExcelCell &cell, IProgressBase &dlg, int dlgstart, int dlgend);
  CQuantity::UNIT UnitValue(TContents &contents);
  CQuantity::UNIT CoorValue(TContents &contents); // wjrx mantis 3339
  void ProcessEndCell(TContents &contents, CExcelCell &cell, int last_number_line, bool &reading_numbers,
                      bool &reading_dstages);

  const CNodalExportFormat *m_pNodalExportFormat;
  const IElementSetExportFormat *m_pElementSetExportFormat;
  CModelBase &m_model;
  bool m_bNewWellPath;
  CNewWellPathInput *m_pNewWellPathInput;

public:
  CNewWellPathInput *NewWellPathInput() const { return m_pNewWellPathInput; }

  bool ReadSheets(const QString &sFileName, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects);
  bool Write(const CString &sFileName, IProgressBase &callback, bool append = false);

  const CModelBase &Model() const;
  CModelBase &Model();
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CElementSetExcelFileImpl::CElementSetExcelFileImpl(CModelBase &model, bool bNewWellPath)
    : m_model(model), m_pElementSetExportFormat(0), m_pNodalExportFormat(0), m_bNewWellPath(bNewWellPath),
      m_pNewWellPathInput(0) {}

CElementSetExcelFileImpl::CElementSetExcelFileImpl(const IElementSetExportFormat &format)
    : m_model(const_cast<CModelBase &>(format.Model())), m_pElementSetExportFormat(&format),
      m_pNodalExportFormat(&format.NodalExportFormat()), m_bNewWellPath(false), m_pNewWellPathInput(0) {}

CElementSetExcelFileImpl::CElementSetExcelFileImpl(const CNodalExportFormat &format)
    : m_model(const_cast<CModelBase &>(format.Model())), m_pElementSetExportFormat(0), m_pNodalExportFormat(&format),
      m_bNewWellPath(false), m_pNewWellPathInput(0) {}

CElementSetExcelFileImpl::~CElementSetExcelFileImpl() {}

bool CElementSetExcelFileImpl::ReadSheets(const QString &sFileName, IProgressBase &dlg,
                                          CGraphNode::TNodeVec &created_objects) {
  bool AnyValidFound = false;

  try {
    CExcelAppGuard ExcelApp;

    TSheetsVector vcValidSheets;
    GetSheets(ExcelApp, sFileName, vcValidSheets);

    for (size_t i = 0; i < vcValidSheets.size(); i++) {
      try {
        CString strJob;
        strJob.Format("Reading sheet %s", vcValidSheets[i].ws.GetName());
        dlg.StatusMessage((LPCSTR)strJob);
      } catch (...) {
      }

      try {
        Read(vcValidSheets[i], dlg, created_objects);
        AnyValidFound = true;
      } catch (CReadException e) {
        if (!e.ErrorMessage().isEmpty()) {
          QString msg((LPCSTR)vcValidSheets[i].ws.GetName());
          msg += ": " + e.ErrorMessage();
          _m()->msg(msg);
        }

        return false;
      }
    }

  } catch (CExcelAppGuard::CNoExcelException &) {
    // Excel not available
    _m()->msg(IDP_EXCELNOTINSTALLED, MB_OK | MB_HELP);
    return false;
  }

  if (!AnyValidFound) {
    CString msg;
    msg.Format(IDP_NOVALIDSHEETS, sFileName);
    _m()->msg(msg);
  }

  return AnyValidFound;
}

bool CElementSetExcelFileImpl::GetSheets(CExcelAppGuard &ExcelApp, const QString &sFileName,
                                         TSheetsVector &vcValidSheets) {
  COleVariant vOpt((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

  Workbooks wbs;
  wbs.AttachDispatch(ExcelApp.excelApp().GetWorkbooks());

  LPDISPATCH lpDisp =
      wbs.Open(sFileName.toStdString().c_str(), vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt, vOpt);
  if (!lpDisp) {
    CString msg;
    msg.Format(IDP_CANNOTOPENFILE, sFileName);
    _m()->msg(msg);
    return (CGeomecDoc::FORMAT_UNKNOWN == TRUE);
  }

  _Workbook wb;
  wb.AttachDispatch(lpDisp);

  Worksheets wss;
  wss.AttachDispatch(wb.GetSheets());

  int nSheets = wss.GetCount();

  int initialSheetsSize = vcValidSheets.size();
  for (int i = 0; i < nSheets; i++) {
    // Initialise sheet ...
    vcValidSheets.push_back(TContents());
    TContents &contents = vcValidSheets[vcValidSheets.size() - 1];
    memset(&contents, 0, sizeof contents);
  }

  for (int i = 0; i < nSheets; i++) {
    TContents &contents = vcValidSheets[initialSheetsSize + i];

    contents.type = ST_UNKNOWN;
    contents.ws.AttachDispatch(wss.GetItem(COleVariant((short)(i + 1))));

    CString sName(wb.GetName());
    sName += "_";
    sName += contents.ws.GetName();
    contents.SheetName = new char[sName.GetLength()];
    strcpy(contents.SheetName, (LPCSTR)sName);

    ReadCellContents(contents.ws, contents);

    if (contents.rowlength == 0) {
      QString msg = QString("Ignored empty worksheet '%1'").arg(contents.SheetName);

      _m()->msg(msg);
      contents.type = ST_UNKNOWN;
    } else {
      // Read the contents ...

      try {
        ReadContents(contents);
      }

      catch (CReadException e) {
        // Error encounterend ...

        if (!e.ErrorMessage().isEmpty()) {
          QString msg((LPCSTR)contents.ws.GetName());

          msg += ": " + e.ErrorMessage();
          _m()->msg(msg);
        }

        contents.type = ST_UNKNOWN;
      }
    }

    // Remove is not valid...
    if (contents.type == ST_UNKNOWN || m_bNewWellPath && contents.type != ST_POINTSET) {
      vcValidSheets.erase(vcValidSheets.begin() + vcValidSheets.size() - 1);
    }
  }

  long format = wb.GetFileFormat();

  if (vcValidSheets.size() == 0)
    return CGeomecDoc::FORMAT_ERROR;

  return CGeomecDoc::FORMAT_OK;
}

void CElementSetExcelFileImpl::Read(TContents &contents, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects) {
  switch (contents.type) {
  case ST_UNTAGGED:
    ReadUntagged(contents, dlg, created_objects);
    break;
  case ST_POINTSET:
    ReadPointSet(contents, dlg, created_objects);
    break;
  case ST_ELEMENTSET:
  case ST_SURFACE:
    ReadElementSet(contents, dlg, created_objects);
    break;
  case ST_MATRIX:
    ReadMatrixType(contents, dlg, created_objects);
    break;
  default:
    // Unknown type s
    assert(false);
  }
}

/*!
  Read function for matrix type
*/
void CElementSetExcelFileImpl::ReadMatrixType(TContents &contents, IProgressBase &dlg,
                                              CGraphNode::TNodeVec &created_objects) {
  assert(contents.pointsend > 0);

  // Create a 2D pointset first
  _Workbook wb(contents.ws.GetParent());
  CString sPointSetName(wb.GetName());
  sPointSetName += "_";
  sPointSetName += contents.ws.GetName();

  CExcelCell row_cell(contents.vcContents, contents.rowlength, 1, 1);
  CPointSet *pPointSet = new CPointSet((LPCSTR)contents.SheetName, m_model, 1, IPointSet::DIM_2D, true);
  CNodalImportFormat format(*pPointSet);

  int row = 0;

  dlg.AddSteps(contents.pointsend - 1);

  while (row_cell.nextRow()) {
    // Progerss
    double progfac = double(row_cell.row() - 1) / double(contents.pointsend - 1);
    int perc = int(100 * progfac);
    dlg.Step((row_cell.row() - 1) - row);
    row = (row_cell.row() - 1);

    if (row_cell.isDouble()) {
      std::vector<geo::CValue> vcValue(3);
      vcValue[0] = row_cell.getDouble();
      CExcelCell column_cell(contents.vcContents, contents.rowlength, 1, 1);
      while (column_cell.nextColumn()) {
        if (column_cell.isDouble()) {
          vcValue[1] = column_cell.getDouble();
          CExcelCell data_cell(contents.vcContents, contents.rowlength, row_cell.row(), column_cell.column());
          vcValue[2] = data_cell.getDouble();
          format.AppendRow(vcValue);
        } else {
          if (!column_cell.isEmpty())
            column_cell.throwMessage("Unexpected data in matrix format.");
          break;
        }
      }
    } else {
      if (!row_cell.isEmpty())
        row_cell.throwMessage("Unexpected data in matrix format.");
      break;
    }
  }
}

bool CElementSetExcelFileImpl::ReadCellContents(_Worksheet ws, TContents &contents) {
  Range usedCells;
  usedCells.AttachDispatch(ws.GetUsedRange());
  VARIANT varValues = usedCells.GetValue();

  try {
    if (varValues.vt == (VT_ARRAY | VT_VARIANT)) {
      SAFEARRAY *psa = varValues.parray;
      UINT uDim = SafeArrayGetDim(psa);

      if (uDim != 2)
        throw std::exception();

      // iDim = 1: rows
      // iDim = 2: columns

      LONG lBound_row;
      LONG uBound_row;
      if (SafeArrayGetLBound(psa, 1, &lBound_row) != S_OK)
        throw std::exception();
      if (SafeArrayGetUBound(psa, 1, &uBound_row) != S_OK)
        throw std::exception();
      int nRows = uBound_row - lBound_row + 1;

      LONG lBound_col;
      LONG uBound_col;
      if (SafeArrayGetLBound(psa, 2, &lBound_col) != S_OK)
        throw std::exception();
      if (SafeArrayGetUBound(psa, 2, &uBound_col) != S_OK)
        throw std::exception();
      int nColumns = uBound_col - lBound_col + 1;

      contents.rowlength = nColumns;
      contents.vcContents.resize(nRows * nColumns);

      LONG rgIndices[2];
      for (LONG r = 0; r < nRows; ++r) {
        rgIndices[0] = r + lBound_row;
        for (LONG c = 0; c < nColumns; ++c) {
          rgIndices[1] = c + lBound_col;
          VARIANT var;
          HRESULT hr = SafeArrayGetElement(psa, rgIndices, &var);
          contents.vcContents[r * contents.rowlength + c] = var;
        }
      }
    }
  } catch (std::exception) {
    return false;
  }

  return true;
}

VARIANT CElementSetExcelFileImpl::GetCellValue(const TContents &contents, long row, long col) const {
  assert(!contents.vcContents.empty());
  assert(row < contents.rowlength);
  long idx = row * contents.rowlength + col;
  assert(idx < contents.vcContents.size());
  return contents.vcContents[idx];
}

/*!
  Reading contents of the worksheet
*/
void CElementSetExcelFileImpl::ReadContents(TContents &contents) {
  bool encounteredEND = false;

  assert(contents.type == ST_UNKNOWN);

  // Matrix format?
  CExcelCell upperleft(contents.vcContents, contents.rowlength, 1, 1);
  CExcelCell uppersecond(contents.vcContents, contents.rowlength, 1, 2);
  CExcelCell secondleft(contents.vcContents, contents.rowlength, 2, 1);
  CExcelCell secondsecond(contents.vcContents, contents.rowlength, 2, 2);

  bool reading_numbers = false;
  if ((upperleft.isEmpty() || upperleft.isString()) && uppersecond.isDouble() && secondleft.isDouble() &&
      secondsecond.isDouble()) {
    reading_numbers = true;
    contents.type = ST_MATRIX;
  }

  bool reading_dstages = false;

  // Read excell sheet
  CExcelCell cell(contents.vcContents, contents.rowlength, 1, 1);
  long last_number_line = 0;
  int iFirstEmpty = 0;

  do {
    if (cell.isEmpty()) {
      // We're register it...
      if (!iFirstEmpty)
        iFirstEmpty = cell.row();
      continue;
    }

    // reset empty index
    iFirstEmpty = 0;

    if (cell.isString()) {
      CString str = cell.GetString();

      if (str[0] == '*' || str[0] == '#')
        continue;

      if (contents.type == ST_UNTAGGED || contents.type == ST_MATRIX) {
        cell.throwMessage(QString("String \"%1\" in untagged or matrix file").arg(cell.getString()));
      }

      // wjrx mantis 3793
      if (reading_dstages && str.CompareNoCase("PROPERTY") == 0)
        reading_dstages = false;

      if (str.CompareNoCase("POINTSET") == 0) {
        // Pointset tag
        if (contents.type != ST_UNKNOWN)
          cell.ThrowMessageID(IDS_SECOND_TYPE);
        contents.type = ST_POINTSET;
        contents.typerow = cell.row();
        reading_dstages = false;
      } else if (str.CompareNoCase("ELEMENTSET") == 0) {
        // Elementset tag
        if (contents.type != ST_UNKNOWN)
          cell.ThrowMessageID(IDS_SECOND_TYPE);
        contents.type = ST_ELEMENTSET;
        contents.typerow = cell.row();
        reading_dstages = false;
      } else if (str.CompareNoCase("SURFACE") == 0) {
        // Surface tag
        if (contents.type != ST_UNKNOWN)
          cell.ThrowMessageID(IDS_SECOND_TYPE);
        contents.type = ST_SURFACE;
        contents.typerow = cell.row();
        reading_dstages = false;
      } else if (contents.type == ST_UNKNOWN) {
        // We don't expect any keyword if the type is set.
        cell.ThrowMessageID(IDS_TYPE_NOT_SET);
      } else if (str.CompareNoCase("DIM") == 0) {
        // Dimension keyword
        if (reading_numbers)
          cell.ThrowMessageID(IDS_KW_IN_NUMBERS);
        if (contents.dimrow)
          cell.ThrowMessageID(IDS_SECOND_DIM);
        if (contents.type == ST_POINTSET)
          cell.ThrowMessageID(IDS_DIMKW_IN_ELEMENTSET);
        contents.dimrow = cell.row();
        reading_dstages = false;
      } else if (str.CompareNoCase("UNIT") == 0) {
        // Unit keyword
        if (reading_numbers)
          cell.ThrowMessageID(IDS_KW_IN_NUMBERS);
        if (contents.unitrow)
          cell.ThrowMessageID(IDS_SECOND_UNIT);
        if ((contents.type == ST_ELEMENTSET || contents.type == ST_SURFACE) && contents.pointsrow)
          cell.ThrowMessageID(IDS_UNITKW_AFTER_POINTSKW);
        contents.unitrow = cell.row();
        reading_dstages = false;
      } else if (str.CompareNoCase("COOR") == 0) {
        // wjrx mantis 3339
        // COOR keyword
        if (reading_numbers)
          cell.ThrowMessageID(IDS_KW_IN_NUMBERS);
        if (contents.coorrow)
          cell.ThrowMessageID(IDS_SECOND_UNIT);
        if ((contents.type == ST_ELEMENTSET || contents.type == ST_SURFACE) && contents.pointsrow)
          cell.ThrowMessageID(IDS_UNITKW_AFTER_POINTSKW);
        contents.coorrow = cell.row();
        reading_dstages = false;
      } else if (str.CompareNoCase("DEPL.STAGES") == 0) {
        if (reading_numbers)
          cell.ThrowMessageID(IDS_KW_IN_NUMBERS);
        if (contents.dstagerow)
          cell.ThrowMessageID(IDS_SECOND_DSTAGE);
        contents.dstagerow = cell.row();
        reading_dstages = true;
      } else if (str.CompareNoCase("POINTS") == 0) {
        // Point keyword
        assert(!reading_numbers);
        if (contents.pointsrow)
          cell.ThrowMessageID(IDS_SECOND_POINTSKW);
        if (contents.type != ST_ELEMENTSET && contents.type != ST_SURFACE)
          cell.ThrowMessageID(IDS_POINTSKW_IN_POINTSET);
        contents.pointsrow = cell.row();
        reading_dstages = false;
      } else if (str.CompareNoCase("ELEMENTS") == 0) {
        if (contents.elementsrow)
          cell.ThrowMessageID(IDS_SECOND_ELEMENTSKW);
        if (contents.type != ST_ELEMENTSET && contents.type != ST_SURFACE)
          cell.ThrowMessageID(IDS_ELEMENTSKW_IN_POINTSET);
        if (!contents.pointsrow)
          cell.ThrowMessageID(IDS_ELEMENTSKW_BEFORE_POINTS);
        if (!contents.pointhdstart)
          cell.ThrowMessageID(IDS_ELEMENTSKW_BEFORE_POINTHEADERS);
        if (!contents.pointsstart)
          cell.ThrowMessageID(IDS_ELEMENTSKW_BEFORE_POINTSSTART);
        assert(!contents.pointsend);
        assert(last_number_line);
        contents.pointsend = last_number_line;
        last_number_line = 0;
        reading_numbers = false;
        contents.elementsrow = cell.row();
        reading_dstages = false;
      } else if (str.CompareNoCase("ELEMENT") == 0) {
        if (contents.elmtyperow)
          cell.ThrowMessageID(IDS_SECOND_ELEMENTTYPE);
        if (!contents.elementsrow)
          cell.ThrowMessageID(IDS_ELEMENTTYPE_BEFORE_ELEMENTSKW);
        contents.elmtyperow = cell.row();
        reading_dstages = false;
      } else if (str.CompareNoCase("END") == 0) {
        ProcessEndCell(contents, cell, last_number_line, reading_numbers, reading_dstages);
        encounteredEND = true;
        break;
      } else if (reading_dstages) {
        // skip
      } else {
        // header tags
        if (contents.type == ST_ELEMENTSET || contents.type == ST_SURFACE) {
          if (!contents.pointhdstart) {
            if (!contents.pointsrow)
              cell.ThrowMessageID(IDS_UNEXPECTED_VALUE);
            if (contents.pointhdstart)
              cell.ThrowMessageID(IDS_UNEXPECTED_VALUE);
            contents.pointhdstart = cell.row();
          } else {
            if (!contents.elmtyperow)
              cell.ThrowMessageID(IDS_UNEXPECTED_VALUE);
            if (contents.elmhdstart)
              cell.ThrowMessageID(IDS_UNEXPECTED_VALUE);
            contents.elmhdstart = cell.row();
          }
        } else {
          if (contents.pointhdstart)
            cell.ThrowMessageID(IDS_UNEXPECTED_VALUE);
          contents.pointhdstart = cell.row();
        }

        do {
          if (!cell.isEmpty()) {
            if (!cell.isString())
              cell.throwMessage("After PROPERTY a header is expected");
            if (cell.getString().toUpper() != "PROPERTY") {
              if (!contents.pointhdend)
                contents.pointhdend = cell.row() + 1;
              else
                contents.elmhdend = cell.row() + 1;
              break;
            }
          }
        } while (cell.nextRow());
      }
    } else if (cell.isDouble()) {
      if (!reading_numbers) {
        assert(last_number_line == 0);
        // Start reading numbers !!!
        if (contents.type == ST_UNKNOWN)
          contents.type = ST_UNTAGGED;

        switch (contents.type) {
        case ST_MATRIX:
        case ST_UNTAGGED:
          contents.pointsstart = cell.row();
          break;
        case ST_ELEMENTSET:
        case ST_SURFACE:
          if (!contents.pointsend) {
            // starting points read
            if (!contents.pointhdstart)
              cell.ThrowMessageID(IDS_UNEXPECTED_VALUE);
            contents.pointsstart = cell.row();
          } else {
            // starting elements read
            if (!contents.elmhdstart)
              cell.ThrowMessageID(IDS_UNEXPECTED_VALUE);
            contents.elmsstart = cell.row();
          }
          break;
        case ST_POINTSET:
          if (reading_dstages) {
            // probably overlooked the header tags, go back and retry
            reading_dstages = false;
            cell.prevRow();
            while (cell.isEmpty() && cell.row() > 1)
              cell.prevRow();
            cell.prevRow(); // to compensate for the nextRow at the end of the loop
            continue;
          }
          if (!contents.pointhdstart)
            cell.ThrowMessageID(IDS_UNEXPECTED_VALUE);
          contents.pointsstart = cell.row();
          break;
        }
        reading_numbers = true;
      }

      last_number_line = cell.row();
    } else {
      // Unknown type
      cell.ThrowMessageID(IDS_INVALID_CONTENT);
    }
  } while (cell.nextRow());

  if (!encounteredEND) {
    ProcessEndCell(contents, cell, last_number_line, reading_numbers, reading_dstages);
  }
}

void CElementSetExcelFileImpl::ProcessEndCell(TContents &contents, CExcelCell &cell, int last_number_line,
                                              bool &reading_numbers, bool &reading_dstages) {
  assert(!contents.endrow);
  if (!reading_numbers)
    cell.ThrowMessageID(IDS_UNEXPECTED_END);
  assert(last_number_line);
  if (contents.type == ST_ELEMENTSET || contents.type == ST_SURFACE) {
    if (!contents.pointsend)
      cell.ThrowMessageID(IDS_UNEXPECTED_END);
    contents.elmsend = last_number_line;
  } else {
    contents.pointsend = last_number_line;
  }

  last_number_line = 0;
  reading_numbers = false;
  reading_dstages = false;
  contents.endrow = cell.row();
}

CQuantity::UNIT CElementSetExcelFileImpl::UnitValue(TContents &contents) {
  assert(contents.unitrow);
  CExcelCell cell(contents.vcContents, contents.rowlength, contents.unitrow, 2);

  CString str = cell.GetString();
  if (str.CompareNoCase("SI") == 0)
    return CQuantity::SI_UNIT;
  else if (str.CompareNoCase("FIELD") == 0)
    return CQuantity::FIELD_UNIT;
  else
    cell.ThrowMessageID(IDP_EXCELILLEGALUNITVALUE);
  return CQuantity::FIELD_UNIT;
}

// wjrx mantis 3339
//
CQuantity::UNIT CElementSetExcelFileImpl::CoorValue(TContents &contents) {
  ASSERT(contents.coorrow);
  CExcelCell cell(contents.vcContents, contents.rowlength, contents.coorrow, 2);

  CString str = cell.GetString();
  if (str.CompareNoCase("SI") == 0)
    return CQuantity::SI_UNIT;
  else if (str.CompareNoCase("FIELD") == 0)
    return CQuantity::FIELD_UNIT;
  else
    cell.ThrowMessageID(IDP_EXCELILLEGALUNITVALUE);
  return CQuantity::FIELD_UNIT;
}

void CElementSetExcelFileImpl::WriteUnit(CExcelCell &cell) {
  assert(m_pNodalExportFormat);
  cell.writeString("UNIT");
  cell.nextColumn();

  if (m_pNodalExportFormat->Unit() == CQuantity::SI_UNIT)
    cell.WriteString("SI");
  else
    cell.WriteString("FIELD");
  cell.nextRow();
}

/*!
  Read the element set file format
*/
void CElementSetExcelFileImpl::ReadElementSet(TContents &contents, IProgressBase &dlg,
                                              CGraphNode::TNodeVec &created_objects) {
  assert(contents.type == ST_ELEMENTSET || contents.type == ST_SURFACE);

  // determine unit
  CQuantity::UNIT valueUnit = CQuantity::SI_UNIT;
  if (contents.unitrow)
    valueUnit = UnitValue(contents);

  // wjrx mantis 3339
  CQuantity::UNIT coorUnit = valueUnit;
  if (contents.coorrow)
    coorUnit = CoorValue(contents);

  CExcelCell cell(contents.vcContents, contents.rowlength, 1, 1);
  IElementSet *pElementSet = 0;
  if (contents.type == ST_SURFACE) {
    geo::CSurface surface;
    CSurfaceEntry &entry = (CSurfaceEntry &)*m_model.GraphEntry(MD_BASE_SURFACE);
    pElementSet = entry.InsertSurface((LPCSTR)contents.SheetName, surface);
  } else {
    // determine element type
    pElementSet = new CElementSet((LPCSTR)contents.SheetName, m_model, coorUnit, valueUnit);
  }

  try {
    // Read node section
    CNodalImportFormat nodal_format(*pElementSet);
    assert(contents.pointhdstart);
    assert(contents.pointhdend);
    CExcelCell header_cell1(contents.vcContents, contents.rowlength, contents.pointhdstart, 1, contents.pointhdend);
    ReadHeaders(nodal_format, header_cell1);
    CExcelCell data_cell1(contents.vcContents, contents.rowlength, contents.pointsstart, 1, contents.pointsend);
    ReadData(nodal_format, data_cell1, dlg, 0, 50);

    // Read element section
    CElementImportFormat element_format(nodal_format);
    assert(contents.elmhdstart);
    assert(contents.elmhdend);
    CExcelCell cell_element(contents.vcContents, contents.rowlength, contents.elmtyperow, 2);
    try {
      element_format.SetElementType((LPCSTR)cell_element.GetString());
    } catch (CReadException e) {
      cell_element.throwMessage(e.ErrorMessage());
    }
    CExcelCell header_cell2(contents.vcContents, contents.rowlength, contents.elmhdstart, 1, contents.elmhdend);
    ReadHeaders(element_format, header_cell2);
    CExcelCell data_cell2(contents.vcContents, contents.rowlength, contents.elmsstart, 1, contents.elmsend);
    ReadData(element_format, data_cell2, dlg, 51, 100);
    created_objects.push_back(pElementSet);
  } catch (CReadException e) {
    // Delete the elment set
    delete pElementSet;
    throw CReadException(e.ErrorMessage());
  }
}

void CElementSetExcelFileImpl::ReadUntagged(TContents &contents, IProgressBase &dlg,
                                            CGraphNode::TNodeVec &created_objects) {
  assert(contents.pointsstart);
  assert(contents.pointsend);

  // Skip white
  CExcelCell cell(contents.vcContents, contents.rowlength, contents.pointsstart, 1, contents.pointsend);
  while (cell.isEmpty()) {
    cell.nextRow();
  }

  // Determ size of column
  int nColumn = 0;
  while (cell.isDouble()) {
    nColumn++;
    if (!cell.nextColumn())
      break;
  };

  // Create a pointset
  CPointSet *pPointSet = 0;
  if (nColumn == 3) {
    pPointSet = new CPointSet((LPCSTR)contents.SheetName, m_model, nColumn - 2, IPointSet::DIM_2D, true);
  } else if (nColumn > 3) {
    pPointSet = new CPointSet((LPCSTR)contents.SheetName, m_model, nColumn - 3, IPointSet::DIM_3D, true);
  }

  if (pPointSet) {
    // In the constructor column 1 = x and column 2 = y. We swap this
    pPointSet->Coordinates().Component(0).LinkTo(pPointSet->NodalValueSet(1));
    pPointSet->Coordinates().Component(1).LinkTo(pPointSet->NodalValueSet(0));
    CNodalImportFormat format(*pPointSet);
    ReadData(format, cell, dlg, 0, 100);
    created_objects.push_back(pPointSet);
  } else {
    cell.throwMessage("Not enough columns for point set. Supply at least 3 columns.");
  }
}

void CElementSetExcelFileImpl::ReadPointSet(TContents &contents, IProgressBase &dlg,
                                            CGraphNode::TNodeVec &created_objects) {
  assert(contents.type == ST_POINTSET);
  assert(contents.pointsstart);
  assert(contents.pointsend);

  // Begin of points
  CExcelCell cell(contents.vcContents, contents.rowlength, contents.pointsstart, 1, contents.pointsend);

  // determine unit
  CQuantity::UNIT valueUnit = CQuantity::SI_UNIT;
  if (contents.unitrow)
    valueUnit = UnitValue(contents);

  // wjrx mantis 3339
  CQuantity::UNIT coorUnit = valueUnit;
  if (contents.coorrow)
    coorUnit = CoorValue(contents);

  if (!m_bNewWellPath) {
    CPointSet *pPointSet = new CPointSet((LPCSTR)contents.SheetName, m_model, 0, IPointSet::DIM_2D, false);
    pPointSet->CoordinateUnit(coorUnit);
    pPointSet->ValueUnit(valueUnit);

    try {
      // Read node section
      CNodalImportFormat nodal_format(*pPointSet);
      assert(contents.pointhdstart);
      assert(contents.pointhdend);
      CExcelCell header_cell(contents.vcContents, contents.rowlength, contents.pointhdstart, 1, contents.pointhdend);
      ReadHeaders(nodal_format, header_cell);
      ReadData(nodal_format, cell, dlg, 0, 100);
      created_objects.push_back(pPointSet);
    } catch (CReadException e) {
      // Delete the elment set
      delete pPointSet;
      throw CReadException(e.ErrorMessage());
    }
  } else {
    m_pNewWellPathInput = new CNewWellPathInput((LPCSTR)contents.SheetName, m_model);

    try {
      // Read node section
      CWellPathInputImportFormat well_format(m_pNewWellPathInput);
      VERIFY(well_format.OnUnit(valueUnit == CQuantity::FIELD_UNIT ? "FIELD" : "SI"));
      assert(contents.pointhdstart);
      assert(contents.pointhdend);
      CExcelCell header_cell(contents.vcContents, contents.rowlength, contents.pointhdstart, 1, contents.pointhdend);
      ReadHeaders(well_format, header_cell);
      ReadData(well_format, cell, dlg, 0, 100);
      created_objects.push_back(m_pNewWellPathInput);
    } catch (CReadException e) {
      // Delete the elment set
      if (m_pNewWellPathInput) {
        delete m_pNewWellPathInput;
        m_pNewWellPathInput = 0;
      }
      throw CReadException(e.ErrorMessage());
    }
  }
}

void CElementSetExcelFileImpl::ReadHeaders(IImportFormat &format, CExcelCell &cell) {
  do {
    if (!cell.isEmpty()) {
      assert(cell.isString());
      if (cell.GetString().CompareNoCase("PROPERTY") == 0) {
        IImportFormat::TStringVec vcString;
        while (cell.nextColumn()) {
          if (cell.isEmpty())
            break;
          vcString.push_back((LPCSTR)cell.GetString());
        };
        try {
          format.CreateValueType(vcString);
        } catch (CReadException e) {
          cell.throwMessage(e.ErrorMessage());
        }
      } else
        break;
    }
  } while (cell.nextRow());

  do {
    if (cell.isEmpty())
      break;

    if (!cell.isString())
      cell.ThrowMessageID(IDP_EXCELINVALIDHEADER);

    // append this header
    try {
      format.AppendColumn((LPCSTR)cell.GetString());
    } catch (CReadException e) {
      cell.throwMessage(e.ErrorMessage());
    }
  } while (cell.nextColumn());

  // Validate header
  try {
    format.ValidateColumns();
  } catch (CReadException e) {
    cell.throwMessage(e.ErrorMessage());
  }
}

/*!
  Reads the data section of an elementset file
*/
void CElementSetExcelFileImpl::ReadData(IImportFormat &format, CExcelCell &cell, IProgressBase &dlg, int dlgstart,
                                        int dlgend) {
  int startrow = cell.row();
  assert(startrow && cell.rowSize());
  assert(dlgstart >= 0 && dlgstart < dlgend);
  assert(dlgend > dlgstart && dlgend <= 100);

  int row = 0;

  dlg.AddSteps(cell.rowSize() - startrow);

  do {
    double progfac = double(cell.row() - startrow) / double(cell.rowSize() - startrow);
    int perc = int((dlgend - dlgstart) * progfac + dlgstart);

    try {
      dlg.Step((cell.row() - startrow) - row);
      row = cell.row() - startrow;
    }

    catch (CProgressCancel *c) {
      delete c;
      throw CReadException(QString());
    }

    // check if this row is not empty or comment
    if (cell.isEmpty())
      continue;
    if (cell.isString()) {
      CString str = cell.GetString();
      if (str[0] == '#' || str[0] == '*')
        continue;
    }

    std::vector<geo::CValue> values(format.ColumnSize());
    for (long col = 0; col < format.ColumnSize(); col++) {
      values[col] = cell.getDouble();
      cell.nextColumn();
    }

    try {
      format.AppendRow(values);
    } catch (CReadException e) {
      cell.throwMessage(e.ErrorMessage());
    }
  } while (cell.nextRow());
  format.ValidateRows();
}

bool CElementSetExcelFileImpl::Write(const CString &sFileName, IProgressBase &callback, bool append) {
  struct _stat statbuf;
  bool existing = (_stat(sFileName, &statbuf) == 0);
  if (existing && !append) {
    if (unlink(sFileName) != 0) {
      CString msg;
      msg.Format(IDP_CANNOTOVERWRITE, sFileName);
      _m()->msg(msg);
      return false;
    }
  }

  assert(m_pNodalExportFormat);
  // Set progress indicator
  callback.AddSteps(m_pNodalExportFormat->RowSize());
  if (m_pElementSetExportFormat)
    callback.AddSteps(m_pElementSetExportFormat->RowSize());

  callback.StatusMessage(std::string("Writing " + std::string(LPCSTR(sFileName))).c_str());

  try {
    CExcelAppGuard ExcelApp;

    // Optional parameters which are not passed
    // can be passed a VARIANT containing VT_ERROR/DISP_E_PARAMNOTFOUND.
    VARIANT vNotPassed;
    V_VT(&vNotPassed) = VT_ERROR;
    V_ERROR(&vNotPassed) = DISP_E_PARAMNOTFOUND;

    Workbooks wbs;
    wbs.AttachDispatch(ExcelApp.excelApp().GetWorkbooks());

    _Workbook wb;
    _Worksheet ws;
    if (existing && append) {

      LPDISPATCH lpDisp = wbs.Open(sFileName, vNotPassed, vNotPassed, vNotPassed, vNotPassed, vNotPassed, vNotPassed,
                                   vNotPassed, vNotPassed, vNotPassed, vNotPassed, vNotPassed, vNotPassed);
      if (!lpDisp) {
        CString msg;
        msg.Format(IDP_CANNOTOPENFILE, sFileName);
        _m()->msg(msg);
        return (CGeomecDoc::FORMAT_UNKNOWN == TRUE);
      }

      wb.AttachDispatch(lpDisp);
      Worksheets wss;
      wss.AttachDispatch(wb.GetSheets());
      // wss.AttachDispatch(wb.GetWorksheets());
      VARIANT vLastSheet;
      vLastSheet.pdispVal = wss.GetItem(COleVariant((short)wss.GetCount()));
      vLastSheet.vt = VT_DISPATCH;
      ws.AttachDispatch(wss.Add(vNotPassed, vLastSheet, vNotPassed, vNotPassed));
    } else {
      // Create a workbook with one sheet to write to
      VARIANT vTemplate;
      V_VT(&vTemplate) = VT_I2;
      V_I2(&vTemplate) = -4167; // xlWBATWorksheet (new workbook with one sheet)
      wb.AttachDispatch(wbs.Add(vTemplate));
      Worksheets wss;
      wss.AttachDispatch(wb.GetSheets());
      // wss.AttachDispatch(wb.GetWorksheets());
      ws.AttachDispatch(wss.GetItem(COleVariant((short)1)));
    }
    if (m_pNodalExportFormat->HasExportName()) {
      QString inputName = m_pNodalExportFormat->ExportName();
      QString outputName;

      int len = inputName.length();
      for (int i = 0; i < len && outputName.length() < 31; ++i) {
        QChar c = inputName.at(i);
        if (c != '\\' && c != '/' && c != '?' && c != '*' && c != '[' && c != ']')
          outputName.append(c);
      }

      if (inputName != outputName)
        _m()->msg(QString("Changed sheet name '%1' to '%2'").arg(inputName).arg(outputName));

      ws.SetName(outputName.toStdString().c_str());
    }
    // We have an empty sheet

    // write data to cells
    CExcelCell cell(1, 1);
    cell.writeString(m_pNodalExportFormat->ExportType());
    cell.nextRow();
    cell.nextRow();

    WriteUnit(cell);
    cell.nextRow();

    if (!m_pElementSetExportFormat || m_pElementSetExportFormat->NodeSize() > 0) {
      WriteDepStages(cell);
      cell.nextRow();
    }

    if (m_pNodalExportFormat->HotSpot()) {
      WriteHotSpotMinMax(cell);
      cell.nextRow();
    }

    if (m_pElementSetExportFormat) {
      cell.WriteString("POINTS");
      cell.nextRow();
    }

    WriteTable(*m_pNodalExportFormat, cell, callback);
    cell.nextRow();
    if (m_pElementSetExportFormat) {
      cell.nextRow();
      // Write the header
      cell.WriteString("ELEMENTS");
      cell.nextRow();

      // Determ element output
      cell.WriteString("ELEMENT");
      cell.nextColumn();
      cell.writeString(m_pElementSetExportFormat->ElementType());
      cell.nextRow();

      WriteTable(*m_pElementSetExportFormat, cell, callback);
    }

    cell.WriteString("END");

    cell.WriteToSheet(ws);

    // Save the sheet
    VARIANT vFileName, vFormat, vNULL, vFALSE;

    V_VT(&vFileName) = VT_BSTR;
    V_BSTR(&vFileName) = sFileName.AllocSysString();

    V_VT(&vFormat) = VT_I2;
    V_I2(&vFormat) = -4143; // xlNormal

    V_VT(&vNULL) = VT_BSTR;
    V_BSTR(&vNULL) = NULL;

    V_VT(&vFALSE) = VT_BOOL;
    V_BOOL(&vFALSE) = FALSE;

    if (!append)
      wb.SaveAs(vFileName, vNotPassed, vNotPassed, vNotPassed, vNotPassed, vNotPassed, 0, vNotPassed, vNotPassed,
                vNotPassed, vNotPassed);
    else
      wb.Save();
    SysFreeString(V_BSTR(&vFileName));
    wb.Close(vFALSE, vFileName, vNotPassed);
    wbs.Close();

    ws.ReleaseDispatch();
  } catch (CExcelAppGuard::CNoExcelException &) {
    // Excel not available
    _m()->msg(IDP_EXCELNOTINSTALLED, MB_OK | MB_HELP);
    return false;
  }

  return true;
}

void CElementSetExcelFileImpl::WriteTable(const IExportFormat &format, CExcelCell &cell, IProgressBase &callback) {
  for (size_t i = 0; i < format.ValueTypeSize(); i++) {
    const CValueType &value_type = format.ValueType(i);
    cell.WriteString("PROPERTY");
    cell.nextColumn();
    cell.WriteString(value_type.ExportType().toStdString().c_str());
    cell.nextColumn();
    if (value_type.ComponentSize() > 1) {
      for (int j = 0; j < value_type.ComponentSize(); j++) {
        cell.WriteString(value_type.Component(j).Name().toStdString().c_str());
        cell.nextColumn();
      }
    }
    cell.WriteString(value_type.Name().toStdString().c_str());
    cell.nextRow();
    if (format.ValueTypeSize() == i + 1)
      cell.nextRow();
  }

  /*
   * The assumption is that the condition below is
   * only valid when results are being exported!
   */

  if ((format.ValueTypeSize() == 0) && (format.DataValueSize() > 0)) {
    std::vector<bool> processed(format.DataValueSize(), false);
    QString forceCacheCreation = format.StringAt(0, 0);

    for (size_t s = 0; s < format.DataValueSize(); ++s) {
      if (!processed[s]) {
        const IValueComponentBase &valueComponent = format.DataValue(s);
        const IValueComposite &valueComposite = valueComponent.Parent();

        try {
          const IResultComponent &resultComponent = dynamic_cast<const IResultComponent &>(valueComponent);
          const CDepletionStage &depletionStage = resultComponent.Stage();
          int stage = depletionStage.Index();

          cell.WriteString("PROPERTY");
          cell.nextColumn();
          cell.WriteString(valueComposite.ExportLabel().toStdString().c_str());

          if (valueComposite.ComponentSize() > 1) {
            size_t size = valueComposite.ComponentSize();
            if (valueComposite.Component(size - 1).Type() != IValueComponentBase::SCALAR)
              --size;

            for (size_t t = 0; t < size; t++) {
              if (((s + t) < format.DataValueSize()) &&
                  (stage == static_cast<const IResultComponent &>(format.DataValue(s + t)).Stage().Index()) &&
                  (&valueComposite == &format.DataValue(s + t).Parent())) {
                cell.nextColumn();
                cell.WriteString(format.StringAt(0, format.getDataOffset() + s + t).toStdString().c_str());
                processed[s + t] = true;
              }
            }
          } else {
            cell.nextColumn();
            cell.WriteString(format.StringAt(0, format.getDataOffset() + s).toStdString().c_str());
          }

          if (valueComposite.ComponentSize() > 1) {
            cell.nextColumn();
            cell.WriteString(valueComposite.ExportLabel().toStdString().c_str());
          }

          cell.nextRow();
        }

        catch (...) {
        }

        processed[s] = true;
      }
    }

    if (format.DataValueSize() > 0) {
      cell.nextRow();
    }
  }

  for (int n = 0; n < format.RowSize(); n++) {
    if (format.HasRowComment(n)) {
      cell.writeString(QString("# ") + format.RowComment(n));
      cell.nextRow();
    }
    for (int nColumn = 0; nColumn < format.ColumnSize(); nColumn++) {
      switch (format.OutputType(n, nColumn)) {
      case IExportFormat::OT_INTEGER:
        cell.writeDouble(format.IntegerAt(n, nColumn));
        break;
      case IExportFormat::OT_DOUBLE:
        cell.writeDouble(format.DoubleAt(n, nColumn));
        break;
      case IExportFormat::OT_STRING:
        cell.writeString(format.StringAt(n, nColumn));
        break;
      default:
        assert(false);
        break;
      }
      cell.nextColumn();
    }
    cell.nextRow();

    callback.Step();
  }
}

void CElementSetExcelFileImpl::WriteDepStages(CExcelCell &cell) {
  cell.WriteString("DEPL.STAGES");
  cell.nextRow();

  CDepletionStageEntry &entry = Model().DepletionStageEntry();

  CDepletionStageEntry::iterator it;
  int i;
  for (i = 0, it = entry.begin(); it != entry.end(); ++it, ++i) {
    CString str;
    str.Format("D%d", i);
    cell.WriteString(str);
    cell.nextColumn();
    str.Format("%d", it->Time().GetMonth());
    cell.WriteString(str);
    cell.nextColumn();
    str.Format("%d", it->Time().GetYear());
    cell.WriteString(str);
    cell.nextRow();
  }
}

void CElementSetExcelFileImpl::WriteHotSpotMinMax(CExcelCell &cell) {
  assert(m_pNodalExportFormat->HotSpot());
  cell.nextColumn();
  cell.WriteStringID(IDS_ET_HOTSPOT_MIN_VAL);
  cell.nextColumn();
  cell.writeDouble(m_pNodalExportFormat->HotSpot()->MinMaxValue().first);
  cell.nextRow();
  cell.nextColumn();
  cell.WriteStringID(IDS_ET_HOTSPOT_MAX_VAL);
  cell.nextColumn();
  cell.writeDouble(m_pNodalExportFormat->HotSpot()->MinMaxValue().second);
  cell.nextRow();
}

/*
CElementSetExcelFileImpl::TAxisSystem CElementSetExcelFileImpl::AxisSystem() const
{
  assert(m_pNodalFormat);
  return m_pNodalFormat->AxisSystem();
}
*/
const CModelBase &CElementSetExcelFileImpl::Model() const { return m_model; }

CModelBase &CElementSetExcelFileImpl::Model() { return m_model; }

// CElementSetExcelFile

CElementSetExcelFile::CElementSetExcelFile(CModelBase &model, bool bNewWellPath)
    : m_elementSetExcelFileImpl(new CElementSetExcelFileImpl(model, bNewWellPath)) {}

CElementSetExcelFile::CElementSetExcelFile(const CNodalExportFormat &export_format)
    : m_elementSetExcelFileImpl(new CElementSetExcelFileImpl(export_format)) {}

CElementSetExcelFile::CElementSetExcelFile(const IElementSetExportFormat &export_format)
    : m_elementSetExcelFileImpl(new CElementSetExcelFileImpl(export_format)) {}

CElementSetExcelFile::~CElementSetExcelFile() { delete m_elementSetExcelFileImpl; }

CNewWellPathInput *CElementSetExcelFile::NewWellPathInput() const {
  return m_elementSetExcelFileImpl->NewWellPathInput();
}

bool CElementSetExcelFile::ReadSheets(const QString &sFileName, IProgressBase &dlg,
                                      CGraphNode::TNodeVec &created_objects) {
  return m_elementSetExcelFileImpl->ReadSheets(sFileName, dlg, created_objects);
}

bool CElementSetExcelFile::Write(const QString &sFileName, IProgressBase &callback, bool append) {
  return m_elementSetExcelFileImpl->Write(sFileName.toStdString().c_str(), callback, append);
}

const CModelBase &CElementSetExcelFile::Model() const { return m_elementSetExcelFileImpl->Model(); }

CModelBase &CElementSetExcelFile::Model() { return m_elementSetExcelFileImpl->Model(); }
