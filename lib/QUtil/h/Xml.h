#pragma once

#include <string>
#include <map>
#include <vector>

class CXml
{
	typedef std::map< std::string, std::string > TKeyValueM;
	typedef std::vector< std::string > TKeysV;
	typedef std::vector< std::string > TValuesV;

	TKeysV m_keys_v;
	TKeysV m_optional_keys_v;

	TKeyValueM m_map;
	std::string m_file;

public:

	static const std::string NOT_FOUND;

	enum eKeySensitive
	{
		Yes,
		No
	};

private:

	bool parse(bool allow_debug = true);
	bool is_key( std::string key );

	void replace_wild_cards();

public:

	CXml( std::string file, TKeysV keys, TKeysV optional_keys = TKeysV() );
	bool verify();
	bool exist( std::string key );
	std::string value( std::string key );
	bool set( std::string key, std::string value );
	bool is( std::string key, std::string value, eKeySensitive key_sensitive = No );
	bool in( std::string key, TValuesV values_v, eKeySensitive key_sensitive = No );

	//static
	static void toUpper( std::string& str );
	static void toUpper( TKeysV& str_v );

	static std::string replace( const std::string k, const std::string v, const std::string value );

	static void update_DEPRECATED( std::string file, std::string node, std::string value );
	static void update( std::string file, std::string node, std::string value );

	static std::string get( std::string file, std::string key );

};