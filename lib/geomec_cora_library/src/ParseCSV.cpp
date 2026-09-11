#include "ParseCSV.h"

namespace cora
{

CParseCSV::CParseCSV(std::ifstream& csvFile, char delimiter, char quote,
  char newline)
: m_csvFile(csvFile)
, m_delimiter(delimiter)
, m_quote(quote)
, m_newline(newline)
{
}

std::vector <QString> CParseCSV::readLine() const
{
  std::vector <QString> line;
  std::string string;
  std::ifstream::int_type character = m_csvFile.get();
  bool inQuote = false;

  while (m_csvFile.good() && ((character != m_newline) || inQuote))
  {
  if ((character == m_delimiter) && !inQuote)
  {
      line.push_back(string.c_str());
      string.clear();
  }
  else
  {
      if (character == m_quote)
      {
    inQuote = !inQuote;
      }

      string += character;
  }

  character = m_csvFile.get();
  }

  if (!string.empty())
  {
  line.push_back(string.c_str());
  }

  return line;
}

} // namespace cora
