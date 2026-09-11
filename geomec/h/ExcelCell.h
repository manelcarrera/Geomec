#ifndef _ExcelCell_h_
#define _ExcelCell_h_

class _Worksheet;

#include "excel8.h"

typedef std::vector<VARIANT> TContentVec;

class CExcelCell
{
private:
  const TContentVec* m_pvcReadContents;
  mutable TContentVec* m_pvcWriteContents;
  long m_nRowLength;
  int m_row;
  int m_column;
  int m_row_size;
  bool m_bWriteMode;

  typedef std::pair<int, int> prCell;
  typedef std::map<prCell, VARIANT> TWriteBuffer;
  TWriteBuffer m_buffer;

private:
  std::pair<const TContentVec*, int> GetWriteBufferAndRowLength() const;

public:
  // reading
  CExcelCell(const TContentVec& vcReadContents, long nRowLength, int nRow, int nCol);
  CExcelCell(const TContentVec& vcReadContents, long nRowLength, int nRow, int nCol, int nRowSize);

  VARIANT getValue() const;
  bool isEmpty() const;
  bool isString() const;
  bool isDouble() const;
  QString getString() const;
  CString GetString() const;
  geo::CValue getDouble() const;

  // writing
  CExcelCell(int nRow, int nCol);

  void setValue(VARIANT varValue);
  void writeString(const QString& sValue);
  void WriteString(const CString& sValue);
  void WriteStringID(unsigned int uValue);
  void writeDouble(const double& dValue);

  void WriteToSheet(_Worksheet ws) const;

  // navigation
  bool nextRow();
  bool prevRow();
  bool nextColumn();
  int column() const;
  int row() const;
  int columnSize() const;
  int rowSize() const;

  // error messages
  void throwMessage(const QString& sMessage) const;
  void ThrowMessage(const CString& sMessage) const;
  void ThrowMessageID(unsigned int uMessage) const;
};

#endif  // _ExcelCell_h_
