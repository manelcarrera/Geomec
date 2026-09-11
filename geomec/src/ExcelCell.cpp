#include "StdAfx.h"

#include "ExcelCell.h"
#include "GeomecStringTable.h"
#include "FvInputStream.h"

CExcelCell::CExcelCell(const TContentVec &vcReadContents, long nRowLength, int nRow, int nCol)
: m_pvcReadContents(&vcReadContents),
  m_pvcWriteContents(0),
  m_nRowLength(nRowLength),
  m_row(nRow - 1),
  m_column(nCol - 1),
  m_row_size(vcReadContents.size() / nRowLength)
{
  assert(m_column < m_nRowLength);
  assert(m_row * m_nRowLength + m_column < m_pvcReadContents->size());
}

CExcelCell::CExcelCell(const TContentVec &vcReadContents, long nRowLength, int nRow, int nCol, int nRowSize)
: m_pvcReadContents(&vcReadContents),
  m_pvcWriteContents(0),
  m_nRowLength(nRowLength),
  m_row(nRow - 1),
  m_column(nCol - 1),
  m_row_size(nRowSize)
{
  assert(m_column < m_nRowLength);
  assert(m_row * m_nRowLength + m_column < m_pvcReadContents->size());
}

CExcelCell::CExcelCell(int nRow, int nCol)
: m_pvcReadContents(0),
  m_pvcWriteContents(0),
  m_nRowLength(0),
  m_row(nRow - 1),
  m_column(nCol - 1),
  m_row_size(0)
{
}

VARIANT CExcelCell::getValue() const
{
  assert(m_pvcReadContents); // must be in read mode

  assert(!m_pvcReadContents->empty());
  assert(m_column < m_nRowLength);
  long idx = m_row * m_nRowLength + m_column;
  assert(idx < m_pvcReadContents->size());
  return (*m_pvcReadContents)[idx];
}

void CExcelCell::setValue(VARIANT varValue)
{
  assert(!m_pvcReadContents); // must be in write mode
  m_buffer.insert(TWriteBuffer::value_type(std::make_pair(m_row, m_column), varValue));
}

/*!
  Return true when the cell is empty
*/
bool CExcelCell::isEmpty() const
{
  VARIANT val = getValue();

  return (val.vt == VT_EMPTY);
}

/*!
  Retrun true when cell type is a string
*/
bool CExcelCell::isString() const
{
  VARIANT val = getValue();

  return (val.vt == VT_BSTR);
}

/*!
  Retrun true when cell type is a double
*/
bool CExcelCell::isDouble() const
{
  VARIANT val = getValue();

  return (val.vt == VT_R8);
}

QString CExcelCell::getString() const
{
  return QString((LPCSTR)GetString());
}

CString CExcelCell::GetString() const
{
  VARIANT val = getValue();

  if(val.vt != VT_BSTR) throwMessage("String type expected.");

  CString sRet( val.bstrVal );
  sRet.TrimLeft();
  sRet.TrimRight();
  return sRet;
}

geo::CValue CExcelCell::getDouble() const
{
  VARIANT val = getValue();

  if(val.vt != VT_R8)
  {
  if ((val.vt == VT_BSTR) && wcslen(val.bstrVal) == 3)
  {
      static wchar_t nan[] = L"NAN";
      bool ok = true;

      for (size_t i = 0; i < 3; ++i)
    ok = ok && towupper(val.bstrVal[i]) == nan[i];

      if (ok)
    return geo::CValue();
  }

  throwMessage("Real type expected.");
  }

  return val.dblVal;
}

/*!
  Go to the next column on the current row
*/
bool CExcelCell::nextColumn()
{
  m_column++;
  return m_column < m_nRowLength;
}

/*!
  Go to the next row and sets the column back to the begin
*/
bool CExcelCell::nextRow()
{
  m_column = 0;
  m_row++;
  return m_row < m_row_size;
}

/*!
 Go to the previous row and sets the column back to the begin
*/
bool CExcelCell::prevRow()
{
  m_column = 0;
  m_row--;
  return m_row >= 0;
}

/*!
  Writes an QT string on the cell position
*/
void CExcelCell::writeString(const QString& sValue)
{
  WriteString( sValue.toStdString().c_str() );
}

/*!
  Writes an MFC string on the cell position
*/
void CExcelCell::WriteStringID(unsigned int uValue)
{
  CString sValue;
  sValue = getStringTableEntry(uValue);
  WriteString(sValue);
}

/*!
  Writes an MFC string on the cell position
*/
void CExcelCell::WriteString(const CString& sValue)
{
  // Initialize parameter container
  VARIANT vStringValue;
  V_VT(&vStringValue) = VT_BSTR;

  V_BSTR(&vStringValue) = sValue.AllocSysString();
  setValue(vStringValue);  
}

void CExcelCell::writeDouble(const double& dValue)
{
  // Initialize parameter container
  VARIANT vDoubleValue;
  V_VT(&vDoubleValue) = VT_R8;

  V_R8(&vDoubleValue) = (double) dValue;
  setValue(vDoubleValue);
}

std::pair<const TContentVec*, int> CExcelCell::GetWriteBufferAndRowLength() const
{
  assert(!m_pvcReadContents); // must be in write mode
  if(m_pvcWriteContents)
  delete m_pvcWriteContents;

  int nRows = 0;
  int nColumns = 0;
  for(TWriteBuffer::const_iterator it = m_buffer.begin(); it != m_buffer.end(); ++it)
  {
  nRows = max(nRows, it->first.first + 1);
  nColumns = max(nColumns, it->first.second + 1);
  }

  m_pvcWriteContents = new TContentVec(nRows * nColumns);

  for(int r = 0; r < nRows; ++r)
  {
  for(int c = 0; c < nColumns; ++c)
  {
      VARIANT varValue;
      TWriteBuffer::const_iterator it = m_buffer.find(std::make_pair(r, c));
      if(it != m_buffer.end())
    varValue = it->second;
      else
    varValue.vt = VT_EMPTY;

      (*m_pvcWriteContents)[r * nColumns + c] = varValue;
  }
  }

  return std::make_pair(m_pvcWriteContents, nColumns);
}

/*!
  Throws an exception with the cell.
*/
void CExcelCell::throwMessage(const QString& sMessage) const
{
  char c = 'A' + m_column;
  throw CReadException(QString("Cell %1%2: %3").arg(c).arg(m_row + 1).arg(sMessage));
}

/*!
  Throws an exception with the cell.
*/
void CExcelCell::ThrowMessage(const CString& sMessage) const
{
  throwMessage((LPCSTR)sMessage);
}

/*!
  Throws an exception with the cell based on a resource ID
*/
void CExcelCell::ThrowMessageID(unsigned int uMessage) const
{
  CString sMessage;
  sMessage = getStringTableEntry(uMessage);
  ThrowMessage(sMessage);
}

/*!
  Returns column position
*/
int CExcelCell::column() const
{
  return m_column + 1;
}

/*!
  Returns row position
*/
int CExcelCell::row() const
{
  return m_row + 1;
}

/*!
  Returns the size of the rows
*/
int CExcelCell::rowSize() const
{
  return m_row_size;
}

void CExcelCell::WriteToSheet(_Worksheet ws) const
{
  std::pair<const TContentVec*, int> prBuffer = GetWriteBufferAndRowLength();
  const TContentVec& vcContents = *prBuffer.first;
  int rowlength = prBuffer.second;

  int nRows = vcContents.size() / rowlength;
  int nCols = rowlength;

  SAFEARRAYBOUND rgsabound[2];
  rgsabound[0].lLbound = 1;
  rgsabound[0].cElements = nRows;
  rgsabound[1].lLbound = 1;
  rgsabound[1].cElements = nCols;
  SAFEARRAY* psa = SafeArrayCreate(VT_VARIANT, 2, rgsabound);

  LONG rgIndices[2];
  for(int r = 0; r < nRows; ++r)
  {
  rgIndices[0] = r + 1;
  for(int c = 0; c < nCols; ++c)
  {
      rgIndices[1] = c + 1;
      SafeArrayPutElement(psa, rgIndices, (void*)&vcContents[r * rowlength + c]);
  }
  }

  VARIANT varValues;
  varValues.vt = (VT_ARRAY | VT_VARIANT);
  varValues.parray = psa;

  Range cells;
  cells.AttachDispatch(ws.GetCells());

  Range cell1;
  // Initialize parameter containers
  VARIANT vRow1, vColumn1;
  V_VT(&vRow1)    = VT_I4;
  V_VT(&vColumn1) = VT_I4;

  //Write header
  V_I4(&vRow1)    = 1;
  V_I4(&vColumn1) = 1;
  VARIANT var1 = cells.GetItem(vRow1, vColumn1);
  cell1.AttachDispatch(V_DISPATCH(&var1));
  VARIANT vCell1;
  V_VT(&vCell1) = VT_DISPATCH;
  V_DISPATCH(&vCell1) = cell1.m_lpDispatch;

  Range cell2;
  // Initialize parameter containers
  VARIANT vRow2, vColumn2;
  V_VT(&vRow2)    = VT_I4;
  V_VT(&vColumn2) = VT_I4;

  //Write header
  V_I4(&vRow2)    = nRows;
  V_I4(&vColumn2) = nCols;
  VARIANT var2 = cells.GetItem(vRow2, vColumn2);
  cell2.AttachDispatch(V_DISPATCH(&var2));
  VARIANT vCell2;
  V_VT(&vCell2) = VT_DISPATCH;
  V_DISPATCH(&vCell2) = cell2.m_lpDispatch;

  Range targetcells;
  targetcells.AttachDispatch(cells.GetRange(vCell1, vCell2));

  targetcells.SetValue(varValues);
}
