#ifndef _ConfigFileBase
#define _ConfigFileBase

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>

//void exitWithError(const std::string &error);

class Convert
{
public:
	template <typename T>
	static std::string T_to_string(T const &val) 
	{
		std::ostringstream ostr;
		ostr << val;

		return ostr.str();
	}
		
	template <typename T>
	static T string_to_T(std::string const &val) 
	{
		std::istringstream istr(val);
		T returnVal;
		if (!(istr >> returnVal))
		{
			//exitWithError("CFG: Not a valid " + (std::string)typeid(T).name() + " received!\n");
		}
		return returnVal;
	}

	template <>
	static std::string string_to_T(std::string const &val)
	{
		return val;
	}
};

class ConfigFile
{
public:

	static const std::string CONFIG_FILE;

protected:

	static const char COMMENT_DELIMITER;
	static const char KEY_VALUE_DELIMITER;

protected:

	std::map<std::string, std::string> contents;
	static std::string m_name;

	void removeComment(std::string &line) const;
	bool onlyWhitespace(const std::string &line) const;
	bool validLine(const std::string &line) const;
	void extractKey(std::string &key, size_t const &sepPos, const std::string &line) const;
	void extractValue(std::string &value, size_t const &sepPos, const std::string &line) const;
	void extractContents(const std::string &line); 
	void parseLine(const std::string &line, size_t const lineNo);
	
	virtual void ExtractKeys();
	virtual void exitWithError(const std::string &error);


	ConfigFile(const std::string &name = m_name);

public:

	static void file( std::string name ){ m_name = name; }
	static std::string file(){ return m_name; }

	bool keyExists(const std::string &key) const;

	template <typename ValueType>
	ValueType getValueOfKey(const std::string &key, ValueType const &defaultValue = ValueType()) const
	{
		if (!keyExists(key))
			return defaultValue;

		return Convert::string_to_T<ValueType>(contents.find(key)->second);
	}
};

#endif //_ConfigFileBase