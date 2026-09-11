#ifndef _cora_ParseCSV_h_
#define _cora_ParseCSV_h_

#include <fstream>
#include <vector>

#include <QString>

namespace cora {

class CParseCSV {
public:
  static const char DELIMITER = ',';
  static const char QUOTE = '"';
  static const char NEWLINE = '\n';

  CParseCSV(std::ifstream &csvFile, char delimiter = DELIMITER, char quote = QUOTE, char newline = NEWLINE);

  std::vector<QString> readLine() const;

private:
  CParseCSV(const CParseCSV &rhs);
  CParseCSV &operator=(CParseCSV rhs);

  std::ifstream &m_csvFile;
  char m_delimiter;
  char m_quote;
  char m_newline;
};

} // namespace cora

#endif // _cora_ParseCSV_h_
