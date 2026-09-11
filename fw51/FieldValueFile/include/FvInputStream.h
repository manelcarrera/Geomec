 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// InputStream.h: interface for the CInputStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTSTREAM_H__5DCD7AFC_FC05_40EE_81DD_88507E05F3AC__INCLUDED_)
#define AFX_INPUTSTREAM_H__5DCD7AFC_FC05_40EE_81DD_88507E05F3AC__INCLUDED_

#include <qstring.h>

#include "FieldValueFileExports.h"

namespace geo {
  class CValue;
}

class CTextFile;

class FIELDVALUEFILE_EXPORT CReadException
{
  QString m_message;
public:
  CReadException(const QString& message): m_message(message) {}
  const QString& ErrorMessage() const { return m_message; }
};

// Inputstream which can also handle unix
class FIELDVALUEFILE_EXPORT input_stream
{
  CTextFile& m_file;
public:
  input_stream(CTextFile& file);

  bool eof() const;
  bool eatwhite();

  void checkDelimiter(char ch);

  char get();
  input_stream& get(char& ch);
  char peek();

  int line_number() const;

  // Read functions
  input_stream& operator>>(bool& bValue);
  input_stream& operator>>(int& nValue);
  input_stream& operator>>(double& dValue);
  input_stream& operator>>(QString& sValue);
  input_stream& operator>>(char* pValue);
  input_stream& operator>>(geo::CValue& value);
};

#endif // !defined(AFX_INPUTSTREAM_H__5DCD7AFC_FC05_40EE_81DD_88507E05F3AC__INCLUDED_)
