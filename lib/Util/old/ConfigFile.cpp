#include "ConfigFile.h"
#include "Printer.h"

#include <string>

#include <Windows.h> //FIXME: just for Windows

#include "Shlobj.h"

const char ConfigFile::COMMENT_DELIMITER	= '#';
const char ConfigFile::KEY_VALUE_DELIMITER	= '=';

void ConfigFile::exitWithError(const std::string &error) 
{
  //Printer::instance()->debug( (char*)error.c_str() );

  std::cout << error;
  std::cin.ignore();
  std::cin.get();

  exit(EXIT_FAILURE);
}
//-----------------------------------------------------
/*const std::string ConfigFile::APPDATA		= std::string( getenv("APPDATA") );
const std::string ConfigFile::BASE_PATH		= APPDATA + "\\Geomec\\plugins\\Inversion tool";
const std::string ConfigFile::CONFIG_FILE	= BASE_PATH + "\\inversion_tool.cfg";
const std::string ConfigFile::IT_FLAG		= APPDATA + "\\it";*/

const std::string ConfigFile::CONFIG_FILE	= "config.cfg";
std::string ConfigFile::m_name = CONFIG_FILE;

void ConfigFile::removeComment(std::string &line) const
{
  if (line.find( COMMENT_DELIMITER ) != line.npos)
    line.erase(line.find( COMMENT_DELIMITER ));
}

bool ConfigFile::onlyWhitespace(const std::string &line) const
{
  return (line.find_first_not_of(' ') == line.npos);
}
bool ConfigFile::validLine(const std::string &line) const
{
  std::string temp = line;
  temp.erase(0, temp.find_first_not_of("\t "));
  if (temp[0] == KEY_VALUE_DELIMITER)
    return false;

  for (size_t i = temp.find( KEY_VALUE_DELIMITER ) + 1; i < temp.length(); i++)
    if (temp[i] != ' ')
      return true;

  return false;
}

void ConfigFile::extractKey(std::string &key, size_t const &sepPos, const std::string &line) const
{
  key = line.substr(0, sepPos);
  if (key.find('\t') != line.npos || key.find(' ') != line.npos)
    key.erase(key.find_first_of("\t "));
}
void ConfigFile::extractValue(std::string &value, size_t const &sepPos, const std::string &line) const
{
  value = line.substr(sepPos + 1);
  value.erase(0, value.find_first_not_of("\t "));
  value.erase(value.find_last_not_of("\t ") + 1);
}

void ConfigFile::extractContents(const std::string &line) 
{
  std::string temp = line;
  temp.erase(0, temp.find_first_not_of("\t "));
  size_t sepPos = temp.find('=');

  std::string key, value;
  extractKey(key, sepPos, temp);
  extractValue(value, sepPos, temp);

  if (!keyExists(key))
    contents.insert(std::pair<std::string, std::string>(key, value));
  else
  {
    contents.insert(std::pair<std::string, std::string>(key, value));
    exitWithError("CFG: Can only have unique key names!\n");
    //Printer::instance()->debug("Config file ERROR: key %s defined twice", key.c_str() );
  }
}

void ConfigFile::parseLine(const std::string &line, size_t const lineNo)
{
  //Printer::instance()->debug("line: %s", line.c_str());
  if (line.find('=') == line.npos)
    exitWithError("CFG: Couldn't find separator on line: " + Convert::T_to_string(lineNo) + "\n");

  if (!validLine(line))
    exitWithError("CFG: Bad format for line: " + Convert::T_to_string(lineNo) + "\n");
    //Printer::instance()->debug("Config file ERROR: Bad format for line %d", lineNo );

  extractContents(line);
}

void ConfigFile::ExtractKeys()
{
  std::ifstream file;
  file.open(m_name.c_str());
  if (!file)
    exitWithError("CFG: File " + m_name + " couldn't be found!\n");

  std::string line;
  size_t lineNo = 0;
  while (std::getline(file, line))
  {
    lineNo++;
    std::string temp = line;

    if (temp.empty())
      continue;

    removeComment(temp);
    if (onlyWhitespace(temp))
      continue;

    parseLine(temp, lineNo);
  }

  file.close();
}

ConfigFile::ConfigFile(const std::string &name)
{
  this->m_name = name;
  //ExtractKeys();
}

bool ConfigFile::keyExists(const std::string &key) const{ return contents.find(key) != contents.end(); }


/*int main()
{
  ConfigFile cfg("config.cfg");

  bool exists = cfg.keyExists("car");
  std::cout << "car key: " << std::boolalpha << exists << "\n";
  exists = cfg.keyExists("fruits");
  std::cout << "fruits key: " << exists << "\n";

  std::string someValue = cfg.getValueOfKey<std::string>("mykey", "Unknown");
  std::cout << "value of key mykey: " << someValue << "\n";
  std::string carValue = cfg.getValueOfKey<std::string>("car");
  std::cout << "value of key car: " << carValue << "\n";
  double doubleVal = cfg.getValueOfKey<double>("double");
  std::cout << "value of key double: " << doubleVal << "\n\n";

  std::cin.get();
  return 0;
}*/