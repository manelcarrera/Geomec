/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "FvInputStream.h"
#include "FvTextFile.h"
#include "Value.h"
#include <cassert>

//////////////////////////////////////////////////////////////////////
// Implementation of input_stream
//////////////////////////////////////////////////////////////////////
input_stream::input_stream(CTextFile &file) : m_file(file) {}

/*!
  Returns true when the end of file is reached
*/
bool input_stream::eof() const { return m_file.peek() == 0; }

/*!
  Returns the next character in the instream
*/
char input_stream::get() { return m_file.get(); }

/*!
  Returns the current character
*/
char input_stream::peek() { return m_file.peek(); }

/*!
  Checks or a delimiters follows. Ignoring space characters
  When the delimiters is not found an excpetion is throwned
  You cannot use this functions white space characters
*/
void input_stream::checkDelimiter(char ch) {
  assert(ch != '\t');
  assert(ch != '\r');
  assert(ch != '\n');
  assert(ch != ' ');
  assert(ch != 0);

  while (peek() != ch) {
    switch (get()) {
    case '\t':
    case '\n':
    case '\r':
    case ' ':
      break; // Ignoring
    case 0:
      throw CReadException("Unexpected end of file");
    default:
      throw CReadException(QString("Delimiter %1 expected").arg(ch));
    }
  }
  get(); // Eat the delimiter
}

/*!
  Eats white space
*/
bool input_stream::eatwhite() {
  if (eof())
    return false;

  bool bStop = false;
  while (!eof() && !bStop) {
    char ch = peek();
    switch (ch) {
    case '\n':
    case '\r':
    case '\t':
    case ' ':
      get();
      break;
    default:
      bStop = true;
      break;
    }
  };

  return bStop && !eof();
}

int input_stream::line_number() const { return m_file.lineNr(); }

/*
std::string input_stream::read_line()
{
  std::string sRet;
  if(skip_empty_lines)
  {
  if(!eatwhite())
      return sRet;
  }

  if(m_pBuffer->first->eof())
  {
  if(!next_line(skip_empty_lines))
      return sRet;
  }

  while(!m_pBuffer->first->eof())
  {
  char ch = m_pBuffer->first->get();
  sRet.append(1, ch);
  }

  next_line(false);

  return sRet;
}
*/
/*
std::vector<std::string> input_stream::read_line_segmented(bool skip_emtpy_lines)
{
  assert(false);
  return std::vector<std::string>();
}
*/
input_stream &input_stream::operator>>(bool &bValue) {
  QString sValue;
  (*this) >> sValue;
  if (sValue.toUpper() == "FALSE") {
    bValue = false;
  } else if (sValue.toUpper() == "TRUE") {
    bValue = true;
  } else {
    bool bSuccess;
    int nValue = sValue.toInt(&bSuccess);
    if (bSuccess && (nValue == 0 || nValue == 1)) {
      bValue = nValue == 1;
    } else {
      throw CReadException("Boolean value expected.");
    }
  }

  return *this;
}

input_stream &input_stream::operator>>(int &nValue) {
  QString sValue;
  (*this) >> sValue;
  bool bSuccess;
  nValue = sValue.toInt(&bSuccess);
  if (!bSuccess) {
    throw CReadException("Integer value expected.");
  }

  return *this;
}

input_stream &input_stream::operator>>(double &dValue) {
  QString sValue;
  (*this) >> sValue;
  bool bSuccess;
  dValue = sValue.toDouble(&bSuccess);
  if (!bSuccess) {
    throw CReadException(QString("Double expected instead of %1").arg(sValue));
  }

  return *this;
}

input_stream &input_stream::operator>>(QString &sValue) {
  eatwhite();
  if (eof()) {
    throw CReadException("Unexpected end of file.");
  }
  sValue = "";
  while (peek() != ' ' && peek() != '\t' && peek() != '\n' && peek() != 0) {
    sValue += get();
  }

  return *this;
}

input_stream &input_stream::operator>>(char *sValue) {
  QString value;
  (*this) >> value;
  if (value.length() == 0) {
    throw CReadException("String expected.");
  }
  strcpy(sValue, value.toLatin1());

  return *this;
}

input_stream &input_stream::operator>>(geo::CValue &value) {
  QString sValue;
  (*this) >> sValue;
  if (sValue.toUpper() == "NAN") {
    value = geo::CValue();
  } else {
    bool bSuccess;
    value = geo::CValue(sValue.toDouble(&bSuccess));
    if (!bSuccess) {
      throw CReadException(QString("Value expected instead of %1").arg(sValue));
    }
  }
  return *this;
}

input_stream &input_stream::get(char &sValue) {
  sValue = get();
  return *this;
}
